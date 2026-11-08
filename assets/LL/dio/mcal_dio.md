# MCAL DIO (AUTOSAR DIO Driver) - How It Works

## Role

Dio reads and writes the level of pins that Port has set up as DIO
(`PORT_PIN_MODE_DIO`). It does no muxing and no direction handling.

```
Application / CDD / Spi  ->  Dio_WriteChannel(DioConf_DioChannel_LED_Onboard, STD_HIGH)
                                   |
                              SIO set / clear / toggle registers (Pico SDK hardware_gpio)
```

## Channels, ports, groups

RP2040 has one GPIO bank, so there is one DIO port (`DioConf_DioPort_GPIO0`,
32 bit wide, GP0 = bit 0). A channel ID is `(port << 5) | pin`, which for
port 0 is the GPIO number (`DIO_CHANNEL(port, pin)`).

Configured in `mcal_dio_cfg.c` / `mcal_dio_cfg.h`:

- **Valid mask** of the port: the channels Dio may touch. Anything else is
  `DIO_E_PARAM_INVALID_CHANNEL_ID` and never reaches the hardware.
- **Channel names**: `DioConf_DioChannel_SPI_CS` (GP5, SPI chip select),
  `..._TestPin` (GP20), `..._LED_Onboard` (GP25).
- **Channel groups**: `mask` + `offset` of pins that are read / written
  together. Reads and writes are right-aligned: bit 0 is the lowest group pin.
  Groups are referenced through the configured pointer, for example
  `DioConf_DioChannelGroup_TestPin`; any other pointer is
  `DIO_E_PARAM_INVALID_GROUP`. The shipped group is one bit on the test pin;
  widen `mask` / `offset` for several pins (example in `mcal_dio_cfg.c`).

To add a channel: set the pin up in Port (DIO mode), add its bit to the valid
mask, add a `DioConf_DioChannel_*` name.

## Services

| Service | Behaviour |
|---|---|
| `Dio_ReadChannel` | level of the pad (inputs and outputs alike) |
| `Dio_WriteChannel` | sets the level; no effect on an input channel |
| `Dio_FlipChannel` | inverts an output and returns the new level; an input is left alone and its pad level is returned |
| `Dio_ReadPort` | pad levels of all configured channels, others read 0 |
| `Dio_WritePort` | writes all configured output channels, inputs skipped |
| `Dio_MaskedWritePort` | like WritePort, only the bits in `Mask` |
| `Dio_ReadChannelGroup` | `(port & mask) >> offset` |
| `Dio_WriteChannelGroup` | `(level << offset) & mask`, inputs skipped, level bits above the group width are dropped |
| `Dio_GetVersionInfo` | version info, `DIO_E_PARAM_POINTER` for NULL |

Writes are filtered through the SIO output enable register, so a pin that is
an input at that moment is never changed.

## Concurrency (RP2040 dual core)

No locks. Channel writes use the SIO set / clear aliases, flips the toggle
alias, and port / masked / group writes use `gpio_put_masked()`, which moves
all its bits with one store to the toggle alias, so the bits change together
and other pins are unaffected. Any service may run on either core at any
time. Two cores writing the same pin at once is last-writer-wins.

`DIO_DEV_ERROR_DETECT == STD_OFF` removes the channel / port / group checks
(hot path), as the SWS intends; do that only once the configuration is
settled.

## Not Implemented

- Pin interrupts / edge detection (not part of Dio).
- More than one DIO port (the RP2040 GPIO bank is one 32 bit port; the
  build refuses `DIO_MAX_PORT != 1`).
