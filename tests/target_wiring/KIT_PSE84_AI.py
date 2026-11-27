# Target wiring for KIT_PSE84_AI.

# UART(10) is on P16_1/P16_0.
uart_loopback_args = ()
uart_loopback_kwargs = {"tx": "P16_1", "rx": "P16_0"}

i2c_loopback_controller_kwargs = {"scl": "P17_2", "sda": "P17_3"}
i2c_loopback_target_args = (5,)  # on pins P17_0/P17_1
i2c_loopback_target_kwargs = {}
