/**
 * AUTOSAR DIO Driver for RP2040, built on the Pico SDK hardware_gpio.
 *
 * Reads use gpio_get_all() (the pad level, so input and output channels read
 * the same way). Writes use the SDK's atomic SIO operations:
 *   channel write  gpio_put()          SIO set / clear alias
 *   channel flip   gpio_xor_mask()     SIO toggle alias
 *   port / group   gpio_put_masked()   one store to the SIO toggle alias
 * Nothing needs a lock and any service may run on either core at any time.
 * A masked, port or group write moves all its bits with a single store.
 *
 * Writes are filtered through the SIO output enable register, so a channel
 * configured as input is never changed (SWS_Dio_00028). Direction and pin
 * muxing are owned by Port.
 */

#include "mcal_dio.h"
#include "hardware/gpio.h"
#include "hardware/structs/sio.h"

#if (DIO_MAX_PORT != 1u)
#error "RP2040 has a single GPIO bank: DIO_MAX_PORT must be 1"
#endif

/* ---------------------------------- */
/* Development error handling */
#if (DIO_DEV_ERROR_DETECT == STD_ON)

#define DIO_REPORT(ApiId, ErrorId)  DIO_DET_REPORT_ERROR((ApiId), (ErrorId))

static boolean Dio_ChannelOk(uint8 sid, Dio_ChannelType ch)
{
    boolean ok = FALSE;

    if ((DIO_CHANNEL_PORT(ch) < DIO_MAX_PORT) &&
        ((Dio_PortCfg[DIO_CHANNEL_PORT(ch)].valid_mask & DIO_CHANNEL_BIT(ch)) != 0u))
    {
        ok = TRUE;
    }
    else
    {
        DIO_REPORT(sid, DIO_E_PARAM_INVALID_CHANNEL_ID);
    }
    return ok;
}

static boolean Dio_PortOk(uint8 sid, Dio_PortType port)
{
    boolean ok = FALSE;

    if (port < DIO_MAX_PORT)
    {
        ok = TRUE;
    }
    else
    {
        DIO_REPORT(sid, DIO_E_PARAM_INVALID_PORT_ID);
    }
    return ok;
}

/* The pointer must be one of the configured groups */
static boolean Dio_GroupOk(uint8 sid, const Dio_ChannelGroupType *group)
{
    boolean ok = FALSE;
    uint32  i;

    for (i = 0u; (i < DIO_MAX_CHANNEL_GROUP) && (ok == FALSE); i++)
    {
        if (group == &Dio_ChannelGroupCfg[i])
        {
            ok = TRUE;
        }
    }
    if (ok == FALSE)
    {
        DIO_REPORT(sid, DIO_E_PARAM_INVALID_GROUP);
    }
    return ok;
}

#else /* checks removed */

#define DIO_REPORT(ApiId, ErrorId)   ((void)0)
#define Dio_ChannelOk(sid, ch)       ((boolean)TRUE)
#define Dio_PortOk(sid, port)        ((boolean)TRUE)
#define Dio_GroupOk(sid, group)      ((boolean)TRUE)

#endif /* DIO_DEV_ERROR_DETECT */

/* ---------------------------------- */
/* Hardware access helpers */

/* Pins currently driven by SIO (output enable register) */
static inline uint32 Dio_HwOutputs(void)
{
    return (uint32)sio_hw->gpio_oe;
}

/* ---------------------------------- */
/* Channel services */

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    Dio_LevelType level = (Dio_LevelType)STD_LOW;

    if (Dio_ChannelOk(DIO_SID_READCHANNEL, ChannelId) == TRUE)
    {
        level = ((gpio_get_all() & DIO_CHANNEL_BIT(ChannelId)) != 0u)
                    ? (Dio_LevelType)STD_HIGH : (Dio_LevelType)STD_LOW;
    }

    return level;
}

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
    if (Dio_ChannelOk(DIO_SID_WRITECHANNEL, ChannelId) == TRUE)
    {
        if ((Dio_HwOutputs() & DIO_CHANNEL_BIT(ChannelId)) != 0u)
        {
            gpio_put((uint)DIO_CHANNEL_PIN(ChannelId), (Level != STD_LOW));
        }
    }
}

#if (DIO_FLIP_CHANNEL_API == STD_ON)

Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
    Dio_LevelType level = (Dio_LevelType)STD_LOW;

    if (Dio_ChannelOk(DIO_SID_FLIPCHANNEL, ChannelId) == TRUE)
    {
        const uint32 bit = DIO_CHANNEL_BIT(ChannelId);

        if ((Dio_HwOutputs() & bit) != 0u)
        {
            gpio_xor_mask(bit);
            level = gpio_get_out_level((uint)DIO_CHANNEL_PIN(ChannelId))
                        ? (Dio_LevelType)STD_HIGH : (Dio_LevelType)STD_LOW;
        }
        else
        {
            /* Input channel: nothing written, report what the pin shows */
            level = ((gpio_get_all() & bit) != 0u)
                        ? (Dio_LevelType)STD_HIGH : (Dio_LevelType)STD_LOW;
        }
    }

    return level;
}

#endif /* DIO_FLIP_CHANNEL_API */

/* ---------------------------------- */
/* Port services */

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    Dio_PortLevelType level = 0u;

    if (Dio_PortOk(DIO_SID_READPORT, PortId) == TRUE)
    {
        level = gpio_get_all() & Dio_PortCfg[PortId].valid_mask;
    }

    return level;
}

void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
    if (Dio_PortOk(DIO_SID_WRITEPORT, PortId) == TRUE)
    {
        gpio_put_masked(Dio_PortCfg[PortId].valid_mask & Dio_HwOutputs(), Level);
    }
}

#if (DIO_MASKED_WRITE_PORT_API == STD_ON)

void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask)
{
    if (Dio_PortOk(DIO_SID_MASKEDWRITEPORT, PortId) == TRUE)
    {
        gpio_put_masked(Mask & Dio_PortCfg[PortId].valid_mask & Dio_HwOutputs(), Level);
    }
}

#endif /* DIO_MASKED_WRITE_PORT_API */

/* ---------------------------------- */
/* Channel group services */

Dio_PortLevelType Dio_ReadChannelGroup(const Dio_ChannelGroupType *ChannelGroupIdPtr)
{
    Dio_PortLevelType level = 0u;

    if (Dio_GroupOk(DIO_SID_READCHANNELGROUP, ChannelGroupIdPtr) == TRUE)
    {
        level = (gpio_get_all() & ChannelGroupIdPtr->mask) >> ChannelGroupIdPtr->offset;
    }

    return level;
}

void Dio_WriteChannelGroup(const Dio_ChannelGroupType *ChannelGroupIdPtr, Dio_PortLevelType Level)
{
    if (Dio_GroupOk(DIO_SID_WRITECHANNELGROUP, ChannelGroupIdPtr) == TRUE)
    {
        const Dio_PortLevelType mask = ChannelGroupIdPtr->mask &
                                       Dio_PortCfg[ChannelGroupIdPtr->port].valid_mask &
                                       Dio_HwOutputs();

        gpio_put_masked(mask, Level << ChannelGroupIdPtr->offset);
    }
}

/* ---------------------------------- */
#if (DIO_VERSION_INFO_API == STD_ON)

void Dio_GetVersionInfo(Std_VersionInfoType *VersionInfo)
{
    if (VersionInfo == NULL)
    {
        DIO_REPORT(DIO_SID_GETVERSIONINFO, DIO_E_PARAM_POINTER);
    }
    else
    {
        VersionInfo->vendorID         = DIO_VENDOR_ID;
        VersionInfo->moduleID         = DIO_MODULE_ID;
        VersionInfo->sw_major_version = DIO_SW_MAJOR_VERSION;
        VersionInfo->sw_minor_version = DIO_SW_MINOR_VERSION;
        VersionInfo->sw_patch_version = DIO_SW_PATCH_VERSION;
    }
}

#endif /* DIO_VERSION_INFO_API */
