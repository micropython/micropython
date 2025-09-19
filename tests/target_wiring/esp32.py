# Target wiring for general esp32 board.
#
# Connect:
# - GPIO4 to GPIO5
# - GPIO12 to GPIO13

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": 4, "rx": 5}

encoder_loopback_id = 0
encoder_loopback_out_pins = (4, 12)
encoder_loopback_in_pins = (5, 13)
