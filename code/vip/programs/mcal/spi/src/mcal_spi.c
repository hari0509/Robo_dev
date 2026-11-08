#include "mcal_spi.h"
#include "mcal_dio.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/critical_section.h"
#include "pico/time.h"

#if (SPI_LEVEL_DELIVERED > 2u)
#error "SPI_LEVEL_DELIVERED must be 0, 1 or 2"
#endif

/* ---------------------------------- */
/* Private types */

/* Run-time state of a channel */
typedef struct
{
    const Spi_DataBufferType *eb_src;    /* EB transmit data, NULL = default */
    Spi_DataBufferType       *eb_dst;    /* EB receive data, NULL = discard  */
    Spi_NumberOfDataType      eb_length;
    boolean                   eb_ready;  /* Spi_SetupEB() has been called    */
    uint8                     lock_count;/* pending sequences using channel  */
} Spi_ChannelRtType;

/* Run-time state of a sequence */
typedef struct
{
    volatile Spi_SeqResultType result;
    uint8   next_job;     /* index in job_list of the next job to start */
    boolean is_async;
    boolean running;      /* a job of this sequence is on the wire      */
    boolean cancel_req;   /* Spi_Cancel() arrived while a job was running */
    uint32  order;        /* acceptance order, FIFO among equal priority */
} Spi_SeqRtType;

/* Run-time state of a HW unit */
typedef struct
{
    volatile boolean busy;
    boolean fmt_valid;    /* width/cpol/cpha below are what PL022 holds */
    uint8   width;
    uint8   cpol;
    uint8   cpha;
    uint32  baud_hz;      /* last baud rate requested from the SDK      */
} Spi_HwRtType;

/* ---------------------------------- */
/* Private constants */
#define SPI_DUMMY_CHUNK         (16u)   /* frames per dummy-clocking burst   */
#define SPI_MAX_JOB_PRIORITY    (3u)

/* ---------------------------------- */
/* Global variables */
static const Spi_ConfigType *Spi_CfgPtr = NULL;
static boolean Spi_Initialized = FALSE;
static critical_section_t    Spi_CritSec;
static uint8        Spi_PendingSeqs = 0u;
static uint32                Spi_OrderCounter = 0u;

static Spi_ChannelRtType     Spi_ChannelRt[SPI_MAX_CHANNEL];
static volatile Spi_JobResultType Spi_JobResult[SPI_MAX_JOB];
static Spi_SeqRtType         Spi_SeqRt[SPI_MAX_SEQUENCE];
static Spi_HwRtType          Spi_HwRt[SPI_MAX_HWUNIT];

/* ---------------------------------- */
/* Development error handling */
#if (SPI_DEV_ERROR_DETECT == STD_ON)
#define SPI_REPORT(ApiId, ErrorId)  SPI_DET_REPORT_ERROR((ApiId), (ErrorId))
#else
#define SPI_REPORT(ApiId, ErrorId)  ((void)0)
#endif

/* initialization check API */
static uint8 Spi_CheckInit(void)
{
    return (Spi_Initialized == TRUE) ? SPI_E_NO_ERROR : SPI_E_UNINIT;
}

static uint8 Spi_CheckChannel(Spi_ChannelType Channel)
{
    uint8 err = SPI_E_NO_ERROR;
#if (SPI_DEV_ERROR_DETECT == STD_ON)
    err = Spi_CheckInit();
    if ((err == SPI_E_NO_ERROR) && (Channel >= SPI_MAX_CHANNEL))
    {
        err = SPI_E_PARAM_CHANNEL;
    }
#else
    (void)Channel;
#endif
    return err;
}

static uint8 Spi_CheckJob(Spi_JobType Job)
{
    uint8 err = SPI_E_NO_ERROR;
#if (SPI_DEV_ERROR_DETECT == STD_ON)
    err = Spi_CheckInit();
    if ((err == SPI_E_NO_ERROR) && (Job >= SPI_MAX_JOB))
    {
        err = SPI_E_PARAM_JOB;
    }
#else
    (void)Job;
#endif
    return err;
}

static uint8 Spi_CheckSeq(Spi_SequenceType Sequence)
{
    uint8 err = SPI_E_NO_ERROR;
#if (SPI_DEV_ERROR_DETECT == STD_ON)
    err = Spi_CheckInit();
    if ((err == SPI_E_NO_ERROR) && (Sequence >= SPI_MAX_SEQUENCE))
    {
        err = SPI_E_PARAM_SEQ;
    }
#else
    (void)Sequence;
#endif
    return err;
}

#if (SPI_HW_STATUS_API == STD_ON)
static uint8 Spi_CheckUnit(Spi_HWUnitType HWUnit)
{
    uint8 err = SPI_E_NO_ERROR;
#if (SPI_DEV_ERROR_DETECT == STD_ON)
    err = Spi_CheckInit();
    if ((err == SPI_E_NO_ERROR) && (HWUnit >= SPI_MAX_HWUNIT))
    {
        err = SPI_E_PARAM_UNIT;
    }
#else
    (void)HWUnit;
#endif
    return err;
}
#endif

#if (SPI_DEV_ERROR_DETECT == STD_ON)
/* GPIO n carries SPI instance (n >> 3) & 1 and role n & 3 (RX/CSn/SCK/TX) */
static boolean Spi_PinFits(uint8 pin, uint8 instance, uint8 role)
{
    return ((pin < NUM_BANK0_GPIOS) &&
            (((pin >> 3u) & 1u) == instance) &&
            ((pin & 3u) == role)) ? TRUE : FALSE;
}

/* A chip select that Dio would silently ignore (channel not configured, or
 * the pin not a DIO output) leaves the device unselected on every transfer,
 * so it is refused here, like a SPI pin that Port has not muxed. */
static boolean Spi_CsUsable(Dio_ChannelType cs)
{
    boolean ok = TRUE;

    if (cs != SPI_CS_NOT_USED)
    {
        ok = ((DIO_CHANNEL_PORT(cs) < DIO_MAX_PORT) &&
              ((Dio_PortCfg[DIO_CHANNEL_PORT(cs)].valid_mask & DIO_CHANNEL_BIT(cs)) != 0u) &&
              (gpio_get_function((uint)DIO_CHANNEL_PIN(cs)) == GPIO_FUNC_SIO) &&
              gpio_is_dir_out((uint)DIO_CHANNEL_PIN(cs))) ? TRUE : FALSE;
    }
    return ok;
}

/* Rejects a configuration that cannot work before touching any hardware */
static uint8 Spi_ValidateConfig(const Spi_ConfigType *cfg)
{
    uint8  err = SPI_E_NO_ERROR;
    uint32 i;
    uint32 k;

    if ((cfg == NULL) || (cfg->hw_units == NULL) || (cfg->channels == NULL) ||
        (cfg->jobs == NULL) || (cfg->sequences == NULL))
    {
        err = SPI_E_PARAM_POINTER;
    }

    for (i = 0u; (err == SPI_E_NO_ERROR) && (i < SPI_MAX_HWUNIT); i++)
    {
        const Spi_HwUnitConfigType *hw = &cfg->hw_units[i];

        if (hw->hw_instance > SPI_MAX_HWUNIT)
        {
            err = SPI_E_PARAM_UNIT;
            break;
        }
    }

    for (i = 0u; (err == SPI_E_NO_ERROR) && (i < SPI_MAX_CHANNEL); i++)
    {
        const Spi_ChannelConfigType *ch = &cfg->channels[i];

        if ((ch->ib_tx_buffer == NULL) || (ch->ib_rx_buffer == NULL))
        {
            err = SPI_E_PARAM_CHANNEL;
            break;
        }
    }

    return err;
}
#endif /* SPI_DEV_ERROR_DETECT */

/* ---------------------------------- */
/* Hardware access helpers */
static inline spi_inst_t *Spi_Inst(Spi_HWUnitType unit)
{
    return (Spi_CfgPtr->hw_units[unit].hw_instance == 1u) ? spi1 : spi0;
}

static inline void Spi_Lock(void)
{
    critical_section_enter_blocking(&Spi_CritSec);
}

static inline void Spi_Unlock(void)
{
    critical_section_exit(&Spi_CritSec);
}

static void Spi_CsDrive(const Spi_ExternalDeviceConfigType *dev, boolean active)
{
    if (dev->cs_channel != SPI_CS_NOT_USED)
    {
        boolean level_high = (dev->cs_polarity == SPI_CS_POLARITY_HIGH) ? active
                                                                        : (boolean)(active ^ 1u);
        Dio_WriteChannel(dev->cs_channel,
                         (level_high != FALSE) ? (Dio_LevelType)STD_HIGH : (Dio_LevelType)STD_LOW);
    }
}

/* Program baud rate and frame format only when they differ from what the
 * PL022 already holds - back-to-back jobs on one device skip the re-config. */
static void Spi_ApplyFormat(Spi_HWUnitType unit,
                            const Spi_ExternalDeviceConfigType *dev,
                            uint8 width)
{
    Spi_HwRtType *hw   = &Spi_HwRt[unit];
    spi_inst_t   *inst = Spi_Inst(unit);
    uint8 cpol = (dev->clk_idle == SPI_CLK_IDLE_HIGH) ? 1u : 0u;
    uint8 cpha = (dev->shift_edge == SPI_EDGE_LEADING) ? 1u : 0u;

    if (hw->baud_hz != dev->baudrate_hz)
    {
        (void)spi_set_baudrate(inst, dev->baudrate_hz);
        hw->baud_hz = dev->baudrate_hz;
    }

    if ((hw->fmt_valid == FALSE) || (hw->width != width) ||
        (hw->cpol != cpol) || (hw->cpha != cpha))
    {
        spi_set_format(inst, width,
                       (cpol != 0u) ? SPI_CPOL_1 : SPI_CPOL_0,
                       (cpha != 0u) ? SPI_CPHA_1 : SPI_CPHA_0,
                       SPI_MSB_FIRST);
        hw->width     = width;
        hw->cpol      = cpol;
        hw->cpha      = cpha;
        hw->fmt_valid = TRUE;
    }
}

/* Clock out `length` frames of `value` and discard what comes back */
static void Spi_ClockDummy(spi_inst_t *inst, Spi_DataBufferType value, Spi_NumberOfDataType length)
{
    Spi_DataBufferType   scratch[SPI_DUMMY_CHUNK];
    Spi_NumberOfDataType remaining = length;
    uint32 i;
    uint32 chunk;

    for (i = 0u; i < SPI_DUMMY_CHUNK; i++)
    {
        scratch[i] = value;
    }
    while (remaining > 0u)
    {
        chunk = (remaining > SPI_DUMMY_CHUNK) ? SPI_DUMMY_CHUNK : remaining;
        (void)spi_write_blocking(inst, scratch, chunk);
        remaining = (Spi_NumberOfDataType)(remaining - chunk);
    }
}

/* Move one channel's data through the SPI unit */
static void Spi_TransferChannel(spi_inst_t *inst,
                                const Spi_ChannelConfigType *ch,
                                const Spi_ChannelRtType *rt)
{
    const Spi_DataBufferType *src;
    Spi_DataBufferType       *dst;
    Spi_NumberOfDataType      len;

    if (ch->channel_type == SPI_CHANNEL_EB)
    {
        src = rt->eb_src;
        dst = rt->eb_dst;
        len = rt->eb_length;
    }
    else
    {
        src = ch->ib_tx_buffer;
        dst = ch->ib_rx_buffer;
        len = ch->max_length;
    }

    if ((src != NULL) && (dst != NULL))
    {
        (void)spi_write_read_blocking(inst, src, dst, len);
    }
    else if (src != NULL)
    {
        (void)spi_write_blocking(inst, src, len);
    }
    else if (dst != NULL)
    {
        (void)spi_read_blocking(inst, ch->default_data, dst, len);
    }
    else
    {
        Spi_ClockDummy(inst, ch->default_data, len);
    }
}

/* Execute one job: CS active, all its channels in order, CS inactive.
 * E_NOT_OK when the PL022 reports an RX FIFO overrun, the only error the
 * hardware exposes. */
static Std_ReturnType Spi_RunJob(Spi_JobType job)
{
    Std_ReturnType ret = E_OK;
    const Spi_JobConfigType            *jcfg = &Spi_CfgPtr->jobs[job];
    const Spi_ExternalDeviceConfigType *dev  = jcfg->device;
    spi_inst_t *inst = Spi_Inst(dev->hw_unit);
    uint32 i;

    spi_get_hw(inst)->icr = SPI_SSPICR_RORIC_BITS;

    /* Program the first channel's format while CS is still inactive, so the
     * device never sees reconfiguration and time_cs2clk is CS-to-first-clock */
    Spi_ApplyFormat(dev->hw_unit, dev,
                    Spi_CfgPtr->channels[jcfg->channel_list[0]].data_width);

    Spi_CsDrive(dev, TRUE);
    if (dev->time_cs2clk_us != 0u)
    {
        busy_wait_us_32(dev->time_cs2clk_us);
    }

    for (i = 0u; i < jcfg->channel_count; i++)
    {
        const Spi_ChannelType        ch_id = jcfg->channel_list[i];
        const Spi_ChannelConfigType *ch    = &Spi_CfgPtr->channels[ch_id];

        /* No-op for the first channel; later channels may change the width */
        Spi_ApplyFormat(dev->hw_unit, dev, ch->data_width);
        Spi_TransferChannel(inst, ch, &Spi_ChannelRt[ch_id]);
    }

    while (spi_is_busy(inst))
    {
        tight_loop_contents();
    }

    if (dev->time_clk2cs_us != 0u)
    {
        busy_wait_us_32(dev->time_clk2cs_us);
    }
    Spi_CsDrive(dev, FALSE);

    if ((spi_get_hw(inst)->ris & SPI_SSPRIS_RORRIS_BITS) != 0u)
    {
        spi_get_hw(inst)->icr = SPI_SSPICR_RORIC_BITS;
        ret = E_NOT_OK;
    }

    return ret;
}

/* ---------------------------------- */
/* Sequence bookkeeping - the caller holds the lock for all of these */

#if ((SPI_LEVEL_DELIVERED == 0u) || (SPI_LEVEL_DELIVERED == 2u))
/* TRUE when every channel and HW unit of the sequence is free (sync use) */
static boolean Spi_SeqResourcesFree(Spi_SequenceType seq)
{
    const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[seq];
    boolean free_ = TRUE;
    uint32 j;
    uint32 c;

    for (j = 0u; (j < scfg->job_count) && (free_ == TRUE); j++)
    {
        const Spi_JobConfigType *jcfg = &Spi_CfgPtr->jobs[scfg->job_list[j]];

        if (Spi_HwRt[jcfg->device->hw_unit].busy != FALSE)
        {
            free_ = FALSE;
        }
        for (c = 0u; (c < jcfg->channel_count) && (free_ == TRUE); c++)
        {
            if (Spi_ChannelRt[jcfg->channel_list[c]].lock_count != 0u)
            {
                free_ = FALSE;
            }
        }
    }
    return free_;
}

#endif /* sync only helper */

/* TRUE when every EB channel the sequence touches has been set up */
static boolean Spi_SeqBuffersReady(Spi_SequenceType seq)
{
    const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[seq];
    boolean ready = TRUE;
    uint32 j;
    uint32 c;

    for (j = 0u; (j < scfg->job_count) && (ready == TRUE); j++)
    {
        const Spi_JobConfigType *jcfg = &Spi_CfgPtr->jobs[scfg->job_list[j]];

        for (c = 0u; (c < jcfg->channel_count) && (ready == TRUE); c++)
        {
            const Spi_ChannelType id = jcfg->channel_list[c];

            if ((Spi_CfgPtr->channels[id].channel_type == SPI_CHANNEL_EB) &&
                (Spi_ChannelRt[id].eb_ready == FALSE))
            {
                ready = FALSE;
            }
        }
    }
    return ready;
}

#if ((SPI_LEVEL_DELIVERED == 0u) || (SPI_LEVEL_DELIVERED == 2u))
static void Spi_SeqSetHwBusy(Spi_SequenceType seq, boolean busy)
{
    const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[seq];
    uint32 j;

    for (j = 0u; j < scfg->job_count; j++)
    {
        Spi_HwRt[Spi_CfgPtr->jobs[scfg->job_list[j]].device->hw_unit].busy = busy;
    }
}

#endif /* sync only helper */

/* Queue the sequence: jobs QUEUED, channels locked against buffer changes */
static void Spi_SeqAccept(Spi_SequenceType seq, boolean is_async)
{
    const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[seq];
    Spi_SeqRtType *rt = &Spi_SeqRt[seq];
    uint32 j;
    uint32 c;

    for (j = 0u; j < scfg->job_count; j++)
    {
        const Spi_JobType        job  = scfg->job_list[j];
        const Spi_JobConfigType *jcfg = &Spi_CfgPtr->jobs[job];

        Spi_JobResult[job] = SPI_JOB_QUEUED;
        for (c = 0u; c < jcfg->channel_count; c++)
        {
            Spi_ChannelRt[jcfg->channel_list[c]].lock_count++;
        }
    }

    rt->next_job   = 0u;
    rt->is_async   = is_async;
    rt->running    = FALSE;
    rt->cancel_req = FALSE;
    Spi_OrderCounter++;
    rt->order      = Spi_OrderCounter;
    rt->result     = SPI_SEQ_PENDING;
    Spi_PendingSeqs++;
}

/* End the sequence: jobs that never ran fail unless the sequence completed,
 * channels are unlocked, final result becomes visible. */
static void Spi_SeqRelease(Spi_SequenceType seq, Spi_SeqResultType final_result)
{
    const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[seq];
    Spi_SeqRtType *rt = &Spi_SeqRt[seq];
    uint32 j;
    uint32 c;

    for (j = 0u; j < scfg->job_count; j++)
    {
        const Spi_JobType        job  = scfg->job_list[j];
        const Spi_JobConfigType *jcfg = &Spi_CfgPtr->jobs[job];

        if ((j >= rt->next_job) && (final_result != SPI_SEQ_OK))
        {
            Spi_JobResult[job] = SPI_JOB_FAILED;
        }
        for (c = 0u; c < jcfg->channel_count; c++)
        {
            Spi_ChannelRt[jcfg->channel_list[c]].lock_count--;
        }
    }

    rt->running = FALSE;
    rt->result  = final_result;
    Spi_PendingSeqs--;
}

/* ---------------------------------- */
/* Initialization */

static void Spi_ResetRuntime(void)
{
    uint32 i;
    uint32 k;

    for (i = 0u; i < SPI_MAX_CHANNEL; i++)
    {
        const Spi_ChannelConfigType *ch = &Spi_CfgPtr->channels[i];

        Spi_ChannelRt[i].eb_src     = NULL;
        Spi_ChannelRt[i].eb_dst     = NULL;
        Spi_ChannelRt[i].eb_length  = 0u;
        Spi_ChannelRt[i].eb_ready   = FALSE;
        Spi_ChannelRt[i].lock_count = 0u;

        if (ch->channel_type == SPI_CHANNEL_IB)
        {
            for (k = 0u; k < ch->max_length; k++)
            {
                ch->ib_tx_buffer[k] = ch->default_data;
                ch->ib_rx_buffer[k] = 0u;
            }
        }
    }

    for (i = 0u; i < SPI_MAX_JOB; i++)
    {
        Spi_JobResult[i] = SPI_JOB_OK;
    }

    for (i = 0u; i < SPI_MAX_SEQUENCE; i++)
    {
        Spi_SeqRt[i].result     = SPI_SEQ_OK;
        Spi_SeqRt[i].next_job   = 0u;
        Spi_SeqRt[i].is_async   = FALSE;
        Spi_SeqRt[i].running    = FALSE;
        Spi_SeqRt[i].cancel_req = FALSE;
        Spi_SeqRt[i].order      = 0u;
    }

    Spi_PendingSeqs = 0u;
    Spi_OrderCounter = 0u;
}

static void Spi_HwInit(void)
{
    uint32 i;

    for (i = 0u; i < SPI_MAX_HWUNIT; i++)
    {
        spi_inst_t *inst = Spi_Inst((Spi_HWUnitType)i);

        /* spi_init() leaves the unit in 8 bit, mode 0. Pin muxing is Port's job. */
        (void)spi_init(inst, SPI_HW_INIT_BAUDRATE_HZ);

        Spi_HwRt[i].busy      = FALSE;
        Spi_HwRt[i].fmt_valid = TRUE;
        Spi_HwRt[i].width     = 8u;
        Spi_HwRt[i].cpol      = 0u;
        Spi_HwRt[i].cpha      = 0u;
        Spi_HwRt[i].baud_hz   = SPI_HW_INIT_BAUDRATE_HZ;
    }

    /* Chip selects are already inactive from Port_Init(); drive them again so
     * a DIO level changed in between cannot leave a device selected */
    for (i = 0u; i < SPI_MAX_JOB; i++)
    {
        Spi_CsDrive(Spi_CfgPtr->jobs[i].device, FALSE);
    }
}

static void Spi_HwDeInit(void)
{
    uint32 i;

    /* Leave every device deselected; the pins themselves stay with Port */
    for (i = 0u; i < SPI_MAX_JOB; i++)
    {
        Spi_CsDrive(Spi_CfgPtr->jobs[i].device, FALSE);
    }

    for (i = 0u; i < SPI_MAX_HWUNIT; i++)
    {
        spi_deinit(Spi_Inst((Spi_HWUnitType)i));
    }
}

/* initialization API */
void Spi_Init(const Spi_ConfigType *ConfigPtr)
{
    
#if (SPI_DEV_ERROR_DETECT == STD_ON)
    uint8 err = SPI_E_NO_ERROR;

    if (Spi_Initialized != FALSE)
    {
        err = SPI_E_ALREADY_INITIALIZED;
    }
    else
    {
        err = Spi_ValidateConfig(ConfigPtr);
    }

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_INIT, err);
    }
    else
#endif
    {
        Spi_CfgPtr = ConfigPtr;
        critical_section_init(&Spi_CritSec);
        Spi_ResetRuntime();
        Spi_HwInit();
        Spi_Initialized = TRUE;
    }
}

Std_ReturnType Spi_DeInit(void)
{
    Std_ReturnType ret = E_NOT_OK;
    boolean idle = TRUE;

#if (SPI_DEV_ERROR_DETECT == STD_ON)
    uint8 err = Spi_CheckInit();
    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_DEINIT, err);
    }
    else
#endif
    {
        Spi_Lock();
        for (uint8 i = 0u; i < SPI_MAX_HWUNIT; i++)
        {
            if (Spi_HwRt[i].busy != FALSE)
            {
                idle = FALSE;
            }
        }
        if ((Spi_PendingSeqs == 0u) && (idle == TRUE))
        {
            Spi_Initialized = FALSE;
            ret = E_OK;
        }
        Spi_Unlock();

        if (ret == E_OK)
        {
            Spi_HwDeInit();
            critical_section_deinit(&Spi_CritSec);
            Spi_CfgPtr = NULL;
        }
    }

    return ret;
}

/* ---------------------------------- */
/* Internal buffer channels */
#if ((SPI_CHANNEL_BUFFERS_ALLOWED == 0u) || (SPI_CHANNEL_BUFFERS_ALLOWED == 2u))

Std_ReturnType Spi_WriteIB(Spi_ChannelType Channel, const Spi_DataBufferType *DataBufferPtr)
{
    Std_ReturnType ret = E_NOT_OK;
    uint8 err = Spi_CheckChannel(Channel);

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_WRITEIB, err);
    }
    else
    {
        const Spi_ChannelConfigType *ch = &Spi_CfgPtr->channels[Channel];
        Spi_NumberOfDataType k;

        Spi_Lock();
        if ((ch->channel_type == SPI_CHANNEL_IB) && (Spi_ChannelRt[Channel].lock_count == 0u))
        {
            /* NULL pointer selects the configured default data */
            for (k = 0u; k < ch->max_length; k++)
            {
                ch->ib_tx_buffer[k] = (DataBufferPtr != NULL) ? DataBufferPtr[k] : ch->default_data;
            }
            ret = E_OK;
        }
        Spi_Unlock();
    }

    return ret;
}

Std_ReturnType Spi_ReadIB(Spi_ChannelType Channel, Spi_DataBufferType *DataBufferPointer)
{
    Std_ReturnType ret = E_NOT_OK;
    uint8 err = Spi_CheckChannel(Channel);

#if (SPI_DEV_ERROR_DETECT == STD_ON)
    if ((err == SPI_E_NO_ERROR) && (DataBufferPointer == NULL))
    {
        err = SPI_E_PARAM_POINTER;
    }
#endif

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_READIB, err);
    }
    else
    {
        const Spi_ChannelConfigType *ch = &Spi_CfgPtr->channels[Channel];
        Spi_NumberOfDataType k;

        Spi_Lock();
        if ((ch->channel_type == SPI_CHANNEL_IB) && (Spi_ChannelRt[Channel].lock_count == 0u))
        {
            for (k = 0u; k < ch->max_length; k++)
            {
                DataBufferPointer[k] = ch->ib_rx_buffer[k];
            }
            ret = E_OK;
        }
        Spi_Unlock();
    }

    return ret;
}

#endif /* IB channels */

/* ---------------------------------- */
/* External buffer channels */
#if ((SPI_CHANNEL_BUFFERS_ALLOWED == 1u) || (SPI_CHANNEL_BUFFERS_ALLOWED == 2u))

Std_ReturnType Spi_SetupEB(Spi_ChannelType Channel,
                           const Spi_DataBufferType *SrcDataBufferPtr,
                           Spi_DataBufferType *DesDataBufferPtr,
                           Spi_NumberOfDataType Length)
{
    Std_ReturnType ret = E_NOT_OK;

#if (SPI_DEV_ERROR_DETECT == STD_ON)
    uint8 err = Spi_CheckChannel(Channel);
    if ((err == SPI_E_NO_ERROR) &&
        ((Length == 0u) || (Length > Spi_CfgPtr->channels[Channel].max_length)))
    {
        err = SPI_E_PARAM_LENGTH;
    }
#endif

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_SETUPEB, err);
    }
    else
    {
        Spi_Lock();
        if ((Spi_CfgPtr->channels[Channel].channel_type == SPI_CHANNEL_EB) &&
            (Spi_ChannelRt[Channel].lock_count == 0u))
        {
            Spi_ChannelRt[Channel].eb_src    = SrcDataBufferPtr;
            Spi_ChannelRt[Channel].eb_dst    = DesDataBufferPtr;
            Spi_ChannelRt[Channel].eb_length = Length;
            Spi_ChannelRt[Channel].eb_ready  = TRUE;
            ret = E_OK;
        }
        Spi_Unlock();
    }

    return ret;
}

#endif /* EB channels */

/* ---------------------------------- */
/* Synchronous transmission */
#if ((SPI_LEVEL_DELIVERED == 0u) || (SPI_LEVEL_DELIVERED == 2u))

Std_ReturnType Spi_SyncTransmit(Spi_SequenceType Sequence)
{
    Std_ReturnType ret = E_NOT_OK;
    boolean accepted = FALSE;

#if (SPI_DEV_ERROR_DETECT == STD_ON)
    uint8 err = Spi_CheckSeq(Sequence);
    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_SYNCTRANSMIT, err);
    }
    else
#endif
    {
        /* Claim every channel and HW unit of the sequence */
        Spi_Lock();
        if (Spi_SeqRt[Sequence].result == SPI_SEQ_PENDING)
        {
            err = SPI_E_SEQ_PENDING;
        }
        else if (Spi_SeqResourcesFree(Sequence) == FALSE)
        {
            err = SPI_E_SEQ_IN_PROCESS;
        }
        else if (Spi_SeqBuffersReady(Sequence) == FALSE)
        {
            /* EB data never set up: rejected without a Det report */
        }
        else
        {
            Spi_SeqAccept(Sequence, FALSE);
            Spi_SeqSetHwBusy(Sequence, TRUE);
            accepted = TRUE;
        }
        Spi_Unlock();

        if (err != SPI_E_NO_ERROR)
        {
            SPI_REPORT(SPI_SID_SYNCTRANSMIT, err);
        }
        else if (accepted == TRUE)
        {
            const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[Sequence];
            Spi_SeqResultType final_result = SPI_SEQ_OK;
            uint32 j;

            for (j = 0u; (j < scfg->job_count) && (final_result == SPI_SEQ_OK); j++)
            {
                const Spi_JobType job = scfg->job_list[j];

                Spi_JobResult[job] = SPI_JOB_PENDING;
                if (Spi_RunJob(job) == E_OK)
                {
                    Spi_JobResult[job] = SPI_JOB_OK;
                }
                else
                {
                    Spi_JobResult[job] = SPI_JOB_FAILED;
                    final_result = SPI_SEQ_FAILED;
                }
                Spi_SeqRt[Sequence].next_job = (uint8)(j + 1u);
            }

            Spi_Lock();
            Spi_SeqSetHwBusy(Sequence, FALSE);
            Spi_SeqRelease(Sequence, final_result);
            Spi_Unlock();

            ret = (final_result == SPI_SEQ_OK) ? E_OK : E_NOT_OK;
        }
        else
        {
            /* Not accepted and no Det error: buffers not ready */
        }
    }

    return ret;
}

#endif /* sync */

/* ---------------------------------- */
/* Asynchronous transmission (polling mode) */
#if ((SPI_LEVEL_DELIVERED == 1u) || (SPI_LEVEL_DELIVERED == 2u))

Std_ReturnType Spi_AsyncTransmit(Spi_SequenceType Sequence)
{
    Std_ReturnType ret = E_NOT_OK;
    uint8 err = Spi_CheckSeq(Sequence);

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_ASYNCTRANSMIT, err);
    }
    else
    {
        Spi_Lock();
        if (Spi_SeqRt[Sequence].result == SPI_SEQ_PENDING)
        {
            err = SPI_E_SEQ_PENDING;
        }
        else if (Spi_SeqBuffersReady(Sequence) == TRUE)
        {
            Spi_SeqAccept(Sequence, TRUE);
            ret = E_OK;
        }
        else
        {
            /* EB data never set up: rejected without a Det report */
        }
        Spi_Unlock();

        if (err != SPI_E_NO_ERROR)
        {
            SPI_REPORT(SPI_SID_ASYNCTRANSMIT, err);
        }
    }

    return ret;
}

Std_ReturnType Spi_SetAsyncMode(Spi_AsyncModeType Mode)
{
    Std_ReturnType ret = E_NOT_OK;
    uint8 err = Spi_CheckInit();

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_SETASYNCMODE, err);
    }
    else if (Mode == SPI_POLLING_MODE)
    {
        ret = E_OK;   /* polling is the only mode this driver implements */
    }
    else
    {
        /* SPI_INTERRUPT_MODE is not supported */
    }

    return ret;
}

/* Best queued job for `unit`: highest job priority, FIFO within a priority.
 * Caller holds the lock. */
static boolean Spi_PickNext(Spi_HWUnitType unit, Spi_SequenceType *seq_out)
{
    boolean found = FALSE;
    uint8   best_prio = 0u;
    uint32  best_order = 0u;
    uint32  s;

    for (s = 0u; s < SPI_MAX_SEQUENCE; s++)
    {
        const Spi_SeqRtType *rt = &Spi_SeqRt[s];

        if ((rt->result == SPI_SEQ_PENDING) && (rt->is_async == TRUE) && (rt->running == FALSE))
        {
            const Spi_JobConfigType *jcfg =
                &Spi_CfgPtr->jobs[Spi_CfgPtr->sequences[s].job_list[rt->next_job]];

            if (jcfg->device->hw_unit == unit)
            {
                if ((found == FALSE) || (jcfg->priority > best_prio) ||
                    ((jcfg->priority == best_prio) && (rt->order < best_order)))
                {
                    found      = TRUE;
                    best_prio  = jcfg->priority;
                    best_order = rt->order;
                    *seq_out   = (Spi_SequenceType)s;
                }
            }
        }
    }
    return found;
}

static void Spi_HandleUnit(Spi_HWUnitType unit)
{
    Spi_SequenceType seq = 0u;
    boolean have_job = FALSE;
    Spi_JobType job = 0u;

    Spi_Lock();
    if ((Spi_HwRt[unit].busy == FALSE) && (Spi_PickNext(unit, &seq) == TRUE))
    {
        job = Spi_CfgPtr->sequences[seq].job_list[Spi_SeqRt[seq].next_job];
        Spi_HwRt[unit].busy   = TRUE;
        Spi_SeqRt[seq].running = TRUE;
        Spi_JobResult[job]    = SPI_JOB_PENDING;
        have_job = TRUE;
    }
    Spi_Unlock();

    if (have_job == TRUE)
    {
        const Spi_SequenceConfigType *scfg = &Spi_CfgPtr->sequences[seq];
        Std_ReturnType rc = Spi_RunJob(job);
        Spi_SeqResultType final_result = SPI_SEQ_PENDING;
        boolean seq_done = FALSE;

        Spi_Lock();
        Spi_HwRt[unit].busy = FALSE;
        Spi_SeqRt[seq].next_job++;
        Spi_JobResult[job] = (rc == E_OK) ? SPI_JOB_OK : SPI_JOB_FAILED;

        if (rc != E_OK)
        {
            final_result = SPI_SEQ_FAILED;
            seq_done = TRUE;
        }
        else if (Spi_SeqRt[seq].cancel_req == TRUE)
        {
            final_result = SPI_SEQ_CANCELED;
            seq_done = TRUE;
        }
        else if (Spi_SeqRt[seq].next_job >= scfg->job_count)
        {
            final_result = SPI_SEQ_OK;
            seq_done = TRUE;
        }
        else
        {
            Spi_SeqRt[seq].running = FALSE;   /* next job is eligible again */
        }

        if (seq_done == TRUE)
        {
            Spi_SeqRelease(seq, final_result);
        }
        Spi_Unlock();

        /* Notifications run outside the lock so they may call the driver */
        if (Spi_CfgPtr->jobs[job].end_notification != NULL)
        {
            Spi_CfgPtr->jobs[job].end_notification();
        }
        if ((seq_done == TRUE) && (final_result != SPI_SEQ_CANCELED) &&
            (scfg->end_notification != NULL))
        {
            scfg->end_notification();
        }
    }
}

void Spi_MainFunction_Handling(void)
{
    uint32 unit;

    /* Silent before Spi_Init(): the task may be scheduled earlier */
    if (Spi_Initialized != FALSE)
    {
        for (unit = 0u; unit < SPI_MAX_HWUNIT; unit++)
        {
            Spi_HandleUnit((Spi_HWUnitType)unit);
        }
    }
}

#if (SPI_CANCEL_API == STD_ON)
void Spi_Cancel(Spi_SequenceType Sequence)
{
    uint8 err = Spi_CheckSeq(Sequence);

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_CANCEL, err);
    }
    else
    {
        Spi_Lock();
        if ((Spi_SeqRt[Sequence].result == SPI_SEQ_PENDING) && (Spi_SeqRt[Sequence].is_async == TRUE))
        {
            if (Spi_SeqRt[Sequence].running == TRUE)
            {
                /* The job on the wire finishes first, then the sequence ends */
                Spi_SeqRt[Sequence].cancel_req = TRUE;
            }
            else
            {
                Spi_SeqRelease(Sequence, SPI_SEQ_CANCELED);
            }
        }
        Spi_Unlock();
    }
}
#endif /* SPI_CANCEL_API */

#endif /* async */

/* ---------------------------------- */
/* Status */

Spi_StatusType Spi_GetStatus(void)
{
    Spi_StatusType status = SPI_UNINIT;

    if (Spi_Initialized != FALSE)
    {
        status = (Spi_PendingSeqs != 0u) ? SPI_BUSY : SPI_IDLE;
    }
    return status;
}

Spi_JobResultType Spi_GetJobResult(Spi_JobType Job)
{
    Spi_JobResultType result = SPI_JOB_FAILED;
    uint8 err = Spi_CheckJob(Job);

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_GETJOBRESULT, err);
    }
    else
    {
        result = Spi_JobResult[Job];
    }
    return result;
}

Spi_SeqResultType Spi_GetSequenceResult(Spi_SequenceType Sequence)
{
    Spi_SeqResultType result = SPI_SEQ_FAILED;
    uint8 err = Spi_CheckSeq(Sequence);

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_GETSEQUENCERESULT, err);
    }
    else
    {
        result = Spi_SeqRt[Sequence].result;
    }
    return result;
}

#if (SPI_HW_STATUS_API == STD_ON)
Spi_StatusType Spi_GetHWUnitStatus(Spi_HWUnitType HWUnit)
{
    Spi_StatusType status = SPI_UNINIT;
    uint8 err = Spi_CheckUnit(HWUnit);

    if (err != SPI_E_NO_ERROR)
    {
        SPI_REPORT(SPI_SID_GETHWUNITSTATUS, err);
    }
    else
    {
        status = (Spi_HwRt[HWUnit].busy != FALSE) ? SPI_BUSY : SPI_IDLE;
    }
    return status;
}
#endif

#if (SPI_VERSION_INFO_API == STD_ON)
void Spi_GetVersionInfo(Std_VersionInfoType *VersionInfo)
{
    if (VersionInfo == NULL)
    {
        SPI_REPORT(SPI_SID_GETVERSIONINFO, SPI_E_PARAM_POINTER);
    }
    else
    {
        VersionInfo->vendorID         = SPI_VENDOR_ID;
        VersionInfo->moduleID         = SPI_MODULE_ID;
        VersionInfo->sw_major_version = SPI_SW_MAJOR_VERSION;
        VersionInfo->sw_minor_version = SPI_SW_MINOR_VERSION;
        VersionInfo->sw_patch_version = SPI_SW_PATCH_VERSION;
    }
}
#endif
