from machine import Pin

# Freescale/NXP FRDM-K64F board
# Blue LED on port B, pin 21
LED = Pin(("GPIO_1", 21), Pin.OUT)
