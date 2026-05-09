# Target wiring for general alif board.
#
# Connect:
# - UART1 TX and RX, usually P0_5 and P0_4

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(0,)]

pwm_loopback_pins = [("P0_4", "P0_5")]

i2c_args_controller = {"scl": "P1_1", "sda": "P1_0"}
i2c_args_target = (0,)  # on pins P0_3/P0_2
