# Target wiring for zephyr rpi_pico board.
#
# Connect:
# - GPIO0 to GPIO1
# - GPIO4 to GPIO6 and GPIO5 to GPIO7

from machine import Pin

uart_loopback_args = ("uart0",)
uart_loopback_kwargs = {}

spi_standalone_args_list = [("spi0",)]

# for machine.Pin
pwm_loopback_pins = [(("gpio0", 0), ("gpio0", 1))]

i2c_args_controller = {"scl": Pin(("gpio0", 5)), "sda": Pin(("gpio0", 4))}
i2c_args_target = ("i2c1",)  # on gpio7/gpio6
