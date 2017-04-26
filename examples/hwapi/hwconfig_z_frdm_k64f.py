from machine import Pin, Signal

# Freescale/NXP FRDM-K64F board
# Blue LED on port B, pin 21
LED = Signal(Pin(("GPIO_1", 21), Pin.OUT))
