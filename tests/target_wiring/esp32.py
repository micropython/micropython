# Target wiring for general esp32 board.
#
# Connect:
# - GPIO4 to GPIO5
# - GPIO12 to GPIO13

import sys

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": 4, "rx": 5}

if "ESP32C" in sys.implementation._machine:
    spi_standalone_args_list = [(1,)]
else:
    spi_standalone_args_list = [(1,), (2,)]

pwm_loopback_pins = [(4, 5)]

encoder_loopback_id = 0
encoder_loopback_out_pins = (4, 12)
encoder_loopback_in_pins = (5, 13)

import sys
if "C6" in sys.implementation._machine:
    # 01Space C6 board
    # also ESP32-C6-DevKitC-1 doesn't seem to like 12/13 connected together
    encoder_loopback_out_pins = (4, 6)
    encoder_loopback_in_pins = (5, 7)

if "P4" in sys.implementation._machine:
    # FireBeetle 2 board
    encoder_loopback_out_pins = (4, 7)
    encoder_loopback_in_pins = (5, 8)

if "S2" in sys.implementation._machine:
    # FeatherS2 board
    uart_loopback_kwargs = {"tx": 5, "rx": 6}
    encoder_loopback_out_pins = (5, 10)
    encoder_loopback_in_pins = (6, 11)
