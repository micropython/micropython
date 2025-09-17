# Target wiring for general esp32 board.
#
# Connect:
# - GPIO4 to GPIO5

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": 4, "rx": 5}
