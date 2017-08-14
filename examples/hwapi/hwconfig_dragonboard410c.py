from machine import Pin

# 96Boards/Qualcomm DragonBoard 410c
# By default, on-board LEDs are controlled by kernel LED driver.
# To make corresponding pins be available as normal GPIO,
# corresponding driver needs to be unbound first (as root):
# echo -n "soc:leds" >/sys/class/leds/apq8016-sbc:green:user1/device/driver/unbind
# Note that application also either should be run as root, or
# /sys/class/gpio ownership needs to be changed.

# User LED 1 on gpio21
LED = Pin(21, Pin.OUT)
