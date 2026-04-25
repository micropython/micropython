# Target wiring for general rp2 board.
#
# Connect:
# - GPIO0 to GPIO1
# - GPIO4 to GPIO6 and GPIO5 to GPIO7

uart_loopback_args = (0,)
uart_loopback_kwargs = {"tx": "GPIO0", "rx": "GPIO1"}

spi_standalone_args_list = [(0,), (1,)]

pwm_loopback_pins = [("GPIO0", "GPIO1")]

i2c_args_controller = {"scl": 5, "sda": 4}
i2c_args_target = (1,)  # on pins 7/6
