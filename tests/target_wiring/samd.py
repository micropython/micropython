# Target wiring for general samd board.
#
# Connect:
# - D0 to D1

uart_loopback_args = ()
uart_loopback_kwargs = {"tx": "D1", "rx": "D0"}
