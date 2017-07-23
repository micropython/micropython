from machine import Pin, Signal

# 96Boards/Qualcomm DragonBoard 410c
#
# By default, on-board LEDs are controlled by kernel LED driver.
# To make corresponding pins be available as normal GPIO,
# corresponding driver needs to be unbound first (as root):
# echo -n "soc:leds" >/sys/class/leds/apq8016-sbc:green:user1/device/driver/unbind
# Note that application also either should be run as root, or
# /sys/class/gpio ownership needs to be changed.
# Likewise, onboard buttons are controlled by gpio_keys driver.
# To release corresponding GPIOs:
# echo -n "gpio_keys" >/sys/class/input/input1/device/driver/unbind

# User LED 1 on gpio21
LED = Signal(Pin(21, Pin.OUT))

# User LED 2 on gpio120
LED2 = Signal(Pin(120, Pin.OUT))

# Button S3 on gpio107
BUTTON = Pin(107, Pin.IN)
