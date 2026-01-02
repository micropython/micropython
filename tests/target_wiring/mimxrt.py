# Target wiring for general mimxrt board.
#
# Connect:
# - UART1 TX and RX, usually D0 and D1

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [()]

encoder_loopback_id = 0
encoder_loopback_out_pins = ("D0", "D2")
encoder_loopback_in_pins = ("D1", "D3")
