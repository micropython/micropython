# Target wiring for non-PyBoard stm32 boards
#
# See PYBx.py for PyBoards
#
# Connect:
# - D0 to D1 (Arduino labels)

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

pwm_loopback_pins = [("D0", "D1")]

i2c_loopback_controller_kwargs = {"scl": "X1", "sda": "X2"}
i2c_loopback_target_args = ("X",)
i2c_loopback_target_kwargs = {}
