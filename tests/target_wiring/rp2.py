# Target wiring for general rp2 board.
#
# Connect:
# - GPIO0 to GPIO1

uart_loopback_args = (0,)
uart_loopback_kwargs = {"tx": "GPIO0", "rx": "GPIO1"}
