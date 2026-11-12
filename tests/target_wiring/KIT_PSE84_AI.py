# Target wiring for KIT_PSE84_AI.
#
# Connect:
# - P16_0 to P16_1 (header pins 6 and 5)
# - P17_0 to P17_1 (header pins 17 and 18, both have 4.7k external pull-ups)

pin_loopback_pins = [("P16_0", "P16_1")]

# UART(5) is on P17_1/P17_0.
uart_loopback_args = ()
uart_loopback_kwargs = {"tx": "P17_1", "rx": "P17_0"}
