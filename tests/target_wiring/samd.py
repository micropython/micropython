# Target wiring for general samd board.
#
# Connect:
# - D0 to D1

uart_loopback_args = ()
uart_loopback_kwargs = {"tx": "D1", "rx": "D0"}

spi_standalone_args_list = [()]

pwm_loopback_pins = [("D0", "D1")]

i2c_loopback_controller_kwargs = {"scl": "D5", "sda": "D1"}
i2c_loopback_target_args = ()
i2c_loopback_target_kwargs = {}
