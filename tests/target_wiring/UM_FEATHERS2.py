# Target wiring for UM_FEATHERS2.
#
# Connect:
# - GPIO5 to GPIO6

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": 6, "rx": 5}
