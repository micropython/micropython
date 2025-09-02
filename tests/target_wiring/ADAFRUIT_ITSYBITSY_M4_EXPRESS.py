# Target wiring for ADAFRUIT_ITSYBITSY_M4_EXPRESS.
#
# Connect:
# - D0 to D1

uart_loopback_args = 3
uart_loopback_kwargs = {"tx": "D1", "rx": "D0"}
