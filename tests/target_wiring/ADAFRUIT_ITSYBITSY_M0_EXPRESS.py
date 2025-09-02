# Target wiring for ADAFRUIT_ITSYBITSY_M0_EXPRESS.
#
# Connect:
# - D0 to D1

uart_loopback_args = (0,)
uart_loopback_kwargs = {"tx": "D1", "rx": "D0"}
