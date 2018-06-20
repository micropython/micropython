# CLICKER2

![clicker2](./docs/clicker2.png)

CLICKER2 is a ```STM32F407VGT6``` based board that features two module connectors, two switches, two LEDs, and a single cell Li-Ion battery charger.

More information: [mikroe clicker2 website](http://www.mikroe.com/stm32/clicker-2/)


## Board Specific Changes

Added directories:

- `stm32/boards/CLICKER2/` with all board specific files

- `stm32/boards/CLICKER2/docs/` with this README note

- `stm32/boards/CLICKER2/modules/` with a board support package that could be frozen


## micropython stm port notes

- In the `stm32/flashbdev.c` there is an extra segment of Flash memory available. This has been changed in a configurable item `#define MICROPY_HW_FLASH_EXTRA (1)` in the `mpconfigboard.h` and replacing `#if (0)` by `#if defined(MICROPY_HW_FLASH_EXTRA)`.

## Board IO Pins

| Board Label | CPU Pin | Description |
| ---- | ---- | ---- |
| LD1 | PE12 | LED 1 (left), active low |
| LD2 | PE15 | LED 2 (right), active low |
| SW | PE0 | PYB switch, active low |
| T2 | PE0 | left switch, active low |
| T3 | PA10 | right switch, active low |
| SENSEL | PB12 | select the battery measurement circuit, active low |
| VSENSE | PC5  | the battery voltage ADC channel, scaled 1/3 of battery voltage |
| FAULT | PC6 | status of the `FAULT` pin, active low |
| BATSTAT | PD4 | status of the `CHARGE` pin, active low |

## Board Support Package
A small BSP package is placed in `stm32/boards/CLICKER2/modules/` and can be used like:

```
import bsp
clicker2 = bsp.BSP()
```
Some code to exercise the functions is executed through `execfile('bsp.py')`.

The functions are:

### ReadBatteryVoltage()
The battery voltage will return the voltage measured at the battery using a voltage divider.

```
batval = clicker2.ReadBatteryVoltage()
print("Battery voltage is: {:4.2f}.".format(batval*9.9/4096))
```

### ReadChargerStatus()
This reads the two status pins `/CHARGE` and `/FAULT` of the power supply and returns them.

```
charge, fault = clicker2.ReadChargerStatus()
```

### IsChargerActive()
Returns ```True``` if charging.

```
charge = clicker2.IsChargerActive()
```

### IsFaultActive()
Returns ```True``` if power supply generates a fault signal.

```
charge = clicker2.IsFaultActive()
```

### IsButtonxPressed()
Return `True` if button is pressed.

```
SW1 = clicker2.IsButton1Pressed()
SW2 = clicker2.IsButton2Pressed()
```
