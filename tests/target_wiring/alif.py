# Target wiring for general alif board.
#
# Connect:
# - UART1 TX and RX, usually P0_5 and P0_4

import sys

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(0,)]

pwm_loopback_pins = [("P0_4", "P0_5")]

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

i2c_loopback_controller_kwargs = {"scl": "P1_1", "sda": "P1_0"}
i2c_loopback_target_args = (0,)  # on pins P0_3/P0_2
i2c_loopback_target_kwargs = {}
