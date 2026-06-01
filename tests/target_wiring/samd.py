# Target wiring for general samd board.
#
# Connect:
# - D0 to D1

pin_loopback_pins = [("D0", "D1")]
pwm_loopback_pins = [("D0", "D1")]

uart_loopback_args = ()
uart_loopback_kwargs = {"tx": "D1", "rx": "D0"}

spi_standalone_args_list = [()]
