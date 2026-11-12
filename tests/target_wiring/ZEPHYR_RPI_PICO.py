# Target wiring for zephyr rpi_pico board.
#
# Connect:
# - GPIO0 to GPIO1
# - GPIO4 to GPIO6 and GPIO5 to GPIO7

from machine import Pin

gp0 = ("gpio0", 0)
gp1 = ("gpio0", 1)
gp4 = ("gpio0", 4)
gp5 = ("gpio0", 5)
gp6 = ("gpio0", 6)
gp7 = ("gpio0", 7)

pin_loopback_pins = [(gp0, gp1), (gp4, gp6), (gp5, gp7)]
pwm_loopback_pins = [(gp0, gp1)]

uart_loopback_args = ("uart0",)
uart_loopback_kwargs = {}

spi_standalone_args_list = [("spi0",)]

i2c_args_controller = {"scl": Pin(gp5), "sda": Pin(gp4)}
i2c_args_target = ("i2c1",)  # on gpio7/gpio6
