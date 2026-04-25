# Target wiring for PYBV10, PYBV11, PYBLITEV10, PYBD_SF2, PYBD_SF3, PYBD_SF6.
#
# Connect:
# - X1 to X2
# - X7 to X9
# - X8 to X10

# Test all connected pin pairs.
pin_loopback_pins = [("X1", "X2"), ("X7", "X9"), ("X8", "X10")]

# Test all connected pin pairs to test multiple different TIMs.
pwm_loopback_pins = pin_loopback_pins

# UART("XA") is on X1/X2  (usually UART(4) on PA0/PA1).
uart_loopback_args = ("XA",)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,), (2,)]

# CAN args assume no connection for single device tests.
can_args = (1,)
can_kwargs = {}
