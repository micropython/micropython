# Target wiring for EK_RA6M2.
#
# Connect:
# - P601 to P602

pin_loopback_pins = [("P601", "P602")]

# UART(9) is on P602/P601.
uart_loopback_args = (9,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(0,)]
