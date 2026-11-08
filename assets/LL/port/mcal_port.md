# MCAL PORT (AUTOSAR Port Driver) - How It Works

## Role

Port owns everything static about a pin: which function it is muxed to, the
pad electrics, and the initial level and direction. Reading or writing the
pin level at run time belongs to Dio. Peripheral drivers (Spi) do not mux
their own pins; they expect Port to have done it.

```
EcuM Driver_init0():  Port_Init(&Port_Config)  ->  Spi_Init()  ->  ...
                      pins, pads, levels           uses SPI pins + Dio CS
```

## Configuration

One `Port_PinConfigType` per pin in `mcal_port_cfg.c`; pins that are not in
the table are never touched.

| Field | Meaning |
|---|---|
| `pin` | GPIO number 0..29 (`PortConf_PortPin_*` names in `mcal_port_cfg.h`) |
| `mode` | `PORT_PIN_MODE_DIO / ADC / SPI / UART / I2C / PWM / PIO0 / PIO1 / CLOCK / USB` |
| `direction` | `PORT_PIN_IN / OUT`, applied while the pin is in DIO mode |
| `direction_changeable` | allow `Port_SetPinDirection()` at run time |
| `mode_changeable` | allow `Port_SetPinMode()` at run time |
| `level` | initial output level (DIO output pins only) |
| `pull` | none / up / down / bus keeper (both resistors) |
| `drive` | 2 / 4 / 8 / 12 mA |
| `slew_fast` | `FALSE` = slew rate limited |
| `schmitt` | input hysteresis |

Mode to RP2040 function select: DIO = SIO, SPI/UART/I2C/PWM/PIO0/PIO1 = the
matching peripheral function, CLOCK = GPCK, USB, ADC = NULL function with an
analog pad (no pulls, digital input off; GP26..GP29 only).

Default table: GP2/3/4 SPI0, GP5 DIO output HIGH (SPI chip select), GP20 DIO
output LOW (self-test pin), GP25 DIO output LOW (on-board LED).

To add a pin: add a table entry, a `PortConf_PortPin_*` name, and, if software
drives it, a Dio channel (see `assets/LL/dio/mcal_dio.md`).

## Port_Init()

1. Validates the whole table before touching hardware: pin in range, no
   duplicates, mode / pull / drive / level in range, ADC only on GP26..GP29.
   A bad table reports `PORT_E_INIT_FAILED` and configures nothing.
2. Per pin: pad (pulls, drive, slew, hysteresis), then for a DIO output the
   level is written and the direction set, and only then the function select
   hands the pad to SIO. A chip select therefore goes straight to its idle
   level; `gpio_init()` is deliberately not used because it drives LOW first.
3. A second call reports `PORT_E_ALREADY_INITIALIZED`.

## Run-time services

- `Port_SetPinDirection(pin, dir)`: only `direction_changeable` pins
  (`PORT_E_DIRECTION_UNCHANGEABLE` otherwise). The direction is stored for
  every mode and applied to hardware while the pin is in DIO mode.
- `Port_RefreshPortDirection()`: re-applies the configured direction of every
  pin that is not `direction_changeable` and is in DIO mode.
- `Port_SetPinMode(pin, mode)`: only `mode_changeable` pins. Switching back
  to DIO keeps the output latch and applies the stored direction.
- `Port_GetVersionInfo()`.

Errors (`PORT_E_*`) go through `PORT_DET_REPORT_ERROR`, a no-op until a Det
module exists. Range and state checks stay active with
`PORT_DEV_ERROR_DETECT == STD_OFF`; only the reporting goes away.

## Concurrency (RP2040 dual core)

Every pad and SIO write here is a single atomic register operation, so calls
for different pins are safe from both cores. Two cores changing the same pin
at once is last-writer-wins. `Port_Init()` has to finish before the scheduler
starts.

## Not Implemented

- Checking that a peripheral mode is electrically available on the chosen
  pin (for example that GP2 really is SPI0 SCK); the table is trusted, only
  ADC is range checked, and Spi validates its own pins.
- Pin interrupts / edge detection (not part of Port).
- Peripheral pin direction: the peripheral drives its pad itself.
