# MCAL SPI (AUTOSAR SPI Handler/Driver) - How It Works

## Architecture Overview

```
┌───────────────────────────────────────────┐
│  Spi_Config (mcal_spi_cfg.c)               │
│  HW units -> Channels -> Jobs -> Sequences │
└─────────────────────┬─────────────────────┘
                      ▼
┌───────────────────────────────────────────┐
│  Spi driver (mcal_spi.c)                   │
│  - Sync:  Spi_SyncTransmit()               │
│  - Async: Spi_AsyncTransmit() queues,      │
│           Spi_MainFunction_Handling() runs │
│  - results, locking, Det checks            │
└─────────────────────┬─────────────────────┘
                      ▼
┌───────────────────────────────────────────┐
│  Pico SDK hardware_spi / hardware_gpio     │
│  spi_write_read_blocking, spi_set_format…  │
└───────────────────────────────────────────┘
```

## Configuration Model

| SWS element | Config type | Meaning |
|---|---|---|
| SpiHwUnit | `Spi_HwUnitConfigType` | SPI0 / SPI1 and its SCK, MOSI, MISO pins |
| SpiExternalDevice | `Spi_ExternalDeviceConfigType` | baud, clock idle level, shift edge, software CS pin + polarity, CS timing |
| SpiChannel | `Spi_ChannelConfigType` | IB or EB, data width (4..8 bit), default data, max length |
| SpiJob | `Spi_JobConfigType` | ordered channel list, one device, priority 0..3 (3 highest), end notification |
| SpiSequence | `Spi_SequenceConfigType` | ordered job list, end notification |

Everything lives in `mcal_spi_cfg.c` (tables) and `mcal_spi_cfg.h`
(`SPI_MAX_*` counts, `SpiConf_*` symbolic IDs, pre-compile switches).
To add a device: add an `Spi_ExternalDeviceConfigType`, a channel, a job
that points to both, bump the `SPI_MAX_*` counts and add the symbolic IDs.

Default setup: SPI0 on GP2 (SCK) / GP3 (MOSI) / GP4 (MISO), CS on GP5
driven by software through Dio (`DioConf_DioChannel_SPI_CS`), 1 MHz, mode 0,
8 bit, one EB channel.

## Pin Ownership (Port and Dio)

Spi does not touch pin muxing or the chip select pin set-up:

- **Port** muxes SCK / MOSI / MISO to `PORT_PIN_MODE_SPI` and sets the chip
  select pin up as a DIO output at its inactive level (GP5: HIGH).
- **Dio** drives the chip select (`Dio_WriteChannel`).
- `Port_Init()` must run before `Spi_Init()` (it does, in `Driver_init0()`).

`Spi_Init()` refuses to start, touching no hardware, when
- a pin does not belong to its SPI unit (GPIO n belongs to SPI
  `(n >> 3) & 1`; role is `n & 3`: RX, CSn, SCK, TX) - `SPI_E_PARAM_UNIT`
- a SPI pin is not muxed to SPI - `SPI_E_PARAM_UNIT`
- a chip select is not a configured Dio channel, or its pin is not a DIO
  output - `SPI_E_PARAM_JOB` (otherwise every transfer would run with the
  device never selected). `SPI_CS_NOT_USED` is valid for a device without CS.

Because Port already drives the chip select to its inactive level, the CS
line does not pulse low while the driver comes up. `Spi_DeInit()` leaves the
pins with Port and just deselects every device.

## Data Flow of a Job

```
Spi_RunJob(job)
  1. clear RX-overrun flag
  2. apply baud / format of the first channel   (only if it changed)
  3. CS active, wait time_cs2clk
  4. per channel: apply width if different, then
        src + dst  -> spi_write_read_blocking
        src only   -> spi_write_blocking
        dst only   -> spi_read_blocking (default data on MOSI)
        neither    -> default data clocked out in 16 frame bursts
  5. wait until the unit is idle, wait time_clk2cs
  6. CS inactive
  7. overrun flag set -> job FAILED
```

## Sync vs Async

- `Spi_SyncTransmit(seq)` claims every channel and HW unit of the sequence,
  runs all jobs in order and returns `E_OK` when the whole sequence finished.
  If another sequence holds a channel or unit it returns `E_NOT_OK`
  (`SPI_E_SEQ_IN_PROCESS`); the same sequence already pending gives
  `SPI_E_SEQ_PENDING`.
- `Spi_AsyncTransmit(seq)` only queues: sequence `PENDING`, jobs `QUEUED`,
  channels locked. `Spi_MainFunction_Handling()` then runs at most one job
  per HW unit per call. Choice per unit: highest job priority, FIFO among
  equal priority. After each job the job end notification fires, after the
  last job the sequence end notification fires (not after a cancel).
- `Spi_Cancel(seq)` ends a queued sequence at once (`SPI_SEQ_CANCELED`,
  unstarted jobs `SPI_JOB_FAILED`). A job already on the wire finishes first.
- A failed job fails its sequence; jobs that never ran are `SPI_JOB_FAILED`.

Call `Spi_MainFunction_Handling()` cyclically from the RTE. Until a BSW
task exists for it, `TestModule01_SpiTest.c` calls it from the self-test.

## Concurrency (RP2040 dual core)

State shared between callers changes inside a `critical_section_t`
(hardware spinlock, short sections only). The transfer itself runs outside
the lock; a HW unit is owned through its `busy` flag, so a second caller on
the other core is rejected instead of corrupting the transfer. A channel is
locked against `Spi_SetupEB / Spi_WriteIB / Spi_ReadIB` while a pending
sequence uses it.

## Not Implemented

- Interrupt mode (`Spi_SetAsyncMode(SPI_INTERRUPT_MODE)` returns `E_NOT_OK`)
- DataWidth above 8 bit, LSB first (the PL022 has no LSB-first mode)
- Interruptible sequences, hardware chip select
- DMA for long transfers (SDK `spi_get_dreq` is the starting point)
