# nullbits Bit-C PRO

The nullbits Bit-C PRO Board is based on the Raspberry Pi RP2040, and comes in the Arduino Pro Micro footprint.


## Board-specific modules

The `board` module contains definitions for the onboard RGB LED.

Example:

```python
>>> import board
>>> board.led_red.on()
>>> board.led_green.on()
>>> board.led_blue.on()
```
