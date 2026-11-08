#ifndef MCAL_SPI_H
#define MCAL_SPI_H

#include "mcal_spi_types.h"
#include "mcal_spi_cfg.h"

/* ---------------------------------- */
/* Service IDs */
#define SPI_SID_INIT                (0x00u)
#define SPI_SID_DEINIT              (0x01u)
#define SPI_SID_WRITEIB             (0x02u)
#define SPI_SID_ASYNCTRANSMIT       (0x03u)
#define SPI_SID_READIB              (0x04u)
#define SPI_SID_SETUPEB             (0x05u)
#define SPI_SID_GETSTATUS           (0x06u)
#define SPI_SID_GETJOBRESULT        (0x07u)
#define SPI_SID_GETSEQUENCERESULT   (0x08u)
#define SPI_SID_GETVERSIONINFO      (0x09u)
#define SPI_SID_SYNCTRANSMIT        (0x0Au)
#define SPI_SID_GETHWUNITSTATUS     (0x0Bu)
#define SPI_SID_CANCEL              (0x0Cu)
#define SPI_SID_SETASYNCMODE        (0x0Du)
#define SPI_SID_MAINFUNCTION_HANDLING (0x10u)

/* ---------------------------------- */
/* Development error codes */
#define SPI_E_NO_ERROR              (0x00u)
#define SPI_E_PARAM_CHANNEL         (0x0Au)
#define SPI_E_PARAM_JOB             (0x0Bu)
#define SPI_E_PARAM_SEQ             (0x0Cu)
#define SPI_E_PARAM_LENGTH          (0x0Du)
#define SPI_E_PARAM_UNIT            (0x0Eu)
#define SPI_E_PARAM_POINTER         (0x10u)
#define SPI_E_UNINIT                (0x1Au)
#define SPI_E_SEQ_PENDING           (0x2Au)
#define SPI_E_SEQ_IN_PROCESS        (0x3Au)
#define SPI_E_ALREADY_INITIALIZED   (0x4Au)

/* ---------------------------------- */
/* Initialization */

/* Brings up every configured HW unit, pin and chip select and resets all
 * results. Reports SPI_E_ALREADY_INITIALIZED when called twice. */
void Spi_Init(const Spi_ConfigType *ConfigPtr);

/* Releases HW units and pins. E_NOT_OK while a sequence is pending. */
Std_ReturnType Spi_DeInit(void);

/* ---------------------------------- */
/* Internal buffer (IB) channels. A channel is locked against buffer access
 * while a sequence that uses it is pending (E_NOT_OK). */
#if ((SPI_CHANNEL_BUFFERS_ALLOWED == 0u) || (SPI_CHANNEL_BUFFERS_ALLOWED == 2u))
Std_ReturnType Spi_WriteIB(Spi_ChannelType Channel, const Spi_DataBufferType *DataBufferPtr);
Std_ReturnType Spi_ReadIB(Spi_ChannelType Channel, Spi_DataBufferType *DataBufferPointer);
#endif

/* ---------------------------------- */
/* External buffer (EB) channels. SrcDataBufferPtr NULL sends the channel's
 * default data, DesDataBufferPtr NULL discards received data. */
#if ((SPI_CHANNEL_BUFFERS_ALLOWED == 1u) || (SPI_CHANNEL_BUFFERS_ALLOWED == 2u))
Std_ReturnType Spi_SetupEB(Spi_ChannelType Channel,
                           const Spi_DataBufferType *SrcDataBufferPtr,
                           Spi_DataBufferType *DesDataBufferPtr,
                           Spi_NumberOfDataType Length);
#endif

/* ---------------------------------- */
/* Transmission */
#if ((SPI_LEVEL_DELIVERED == 0u) || (SPI_LEVEL_DELIVERED == 2u))
Std_ReturnType Spi_SyncTransmit(Spi_SequenceType Sequence);
#endif

#if ((SPI_LEVEL_DELIVERED == 1u) || (SPI_LEVEL_DELIVERED == 2u))
Std_ReturnType Spi_AsyncTransmit(Spi_SequenceType Sequence);
Std_ReturnType Spi_SetAsyncMode(Spi_AsyncModeType Mode);

/* Call cyclically (e.g. from an RTE task). Runs at most one queued job per
 * HW unit per call, then raises the job/sequence end notifications. */
void Spi_MainFunction_Handling(void);
#endif

/* ---------------------------------- */
/* Status */
Spi_StatusType    Spi_GetStatus(void);
Spi_JobResultType Spi_GetJobResult(Spi_JobType Job);
Spi_SeqResultType Spi_GetSequenceResult(Spi_SequenceType Sequence);

#if (SPI_HW_STATUS_API == STD_ON)
Spi_StatusType Spi_GetHWUnitStatus(Spi_HWUnitType HWUnit);
#endif

#if (SPI_VERSION_INFO_API == STD_ON)
void Spi_GetVersionInfo(Std_VersionInfoType *VersionInfo);
#endif

#if ((SPI_CANCEL_API == STD_ON) && ((SPI_LEVEL_DELIVERED == 1u) || (SPI_LEVEL_DELIVERED == 2u)))
void Spi_Cancel(Spi_SequenceType Sequence);
#endif

#endif /* MCAL_SPI_H */