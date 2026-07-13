# Target wiring for KIT_PSE84_AI.

# UART(5) is on P17_1/P17_0.
uart_loopback_args = ()
uart_loopback_kwargs = {"tx": "P17_1", "rx": "P17_0"}
