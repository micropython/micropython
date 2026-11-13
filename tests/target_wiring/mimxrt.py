# Target wiring for general mimxrt board.
#
# Connect:
# - D0 to D1 (uart1,pwm,encoder)
# - D2 to D3 (encoder)
# - D0 to A5 (i2c scl)
# - D1 to A4 (i2c sda)

import sys

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [()]

pwm_loopback_pins = [("D0", "D1")]

encoder_loopback_id = 0
encoder_loopback_out_pins = ("D0", "D2")
encoder_loopback_in_pins = ("D1", "D3")

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

i2c_loopback_controller_kwargs = {"scl": "D0", "sda": "D1"}
i2c_loopback_target_args = (0,)  # pins A5(scl)/A4(sda)
i2c_loopback_target_kwargs = {}
