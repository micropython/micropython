# Target wiring for general esp32 board.
# TODO: test on C3, C6 (irq_break may not work)
#
# Connect:
# - GPIO4 to GPIO5.

# For UART loopback tests.
uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": 4, "rx": 5}
