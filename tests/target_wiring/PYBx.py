# Target wiring for PYBV10, PYBV11, PYBLITEV10, PYBD_SF2, PYBD_SF3, PYBD_SF6.
#
# Connect:
# - X1 to X2

# UART("XA") is on X1/X2  (usually UART(4) on PA0/PA1).
uart_loopback_args = ("XA",)
uart_loopback_kwargs = {}
