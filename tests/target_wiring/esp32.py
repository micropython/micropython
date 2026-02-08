# Target wiring for general esp32 board.
#
# Connect:
# - GPIO4 to GPIO5
# - GPIO12 to GPIO13

import sys

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": 4, "rx": 5}

if "ESP32C" in sys.implementation._machine or "ESP32-C" in sys.implementation._machine:
    spi_standalone_args_list = [(1,)]
else:
    spi_standalone_args_list = [(1,), (2,)]

encoder_loopback_id = 0
encoder_loopback_out_pins = (4, 12)
encoder_loopback_in_pins = (5, 13)
