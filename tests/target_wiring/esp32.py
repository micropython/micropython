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
    # ESP32-C6-DevKitC-1 (doesn't seem to like 12/13 connected together)
    # connect 4 to 5 and 6 to 7
    pin_loopback_pins = [(4, 5), (6, 7)]
    encoder_loopback_out_pins = (4, 6)
    encoder_loopback_in_pins = (5, 7)
    i2c_args_controller = {"scl": 4, "sda": 6}
    i2c_args_target = (0,)
    i2c_kwargs_target = {"scl": 5, "sda": 7}

if "P4" in sys.implementation._machine:
    # FireBeetle 2 board; connect 4/5 and 7/8
    pwm_loopback_pins += [(7, 8)]
    encoder_loopback_out_pins = (4, 7)
    encoder_loopback_in_pins = (5, 8)
    i2c_args_controller = {"scl": 4, "sda": 7}
    i2c_args_target = (0,)
    i2c_kwargs_target = {"scl": 5, "sda": 8}

if "S2" in sys.implementation._machine:
    # FeatherS2 board, connect 5/6 and 10/11
    pin_loopback_pins = [(5, 6), (10, 11)]
    pwm_loopback_pins = [(5, 6)]
    uart_loopback_kwargs = {"tx": 5, "rx": 6}
    encoder_loopback_out_pins = (5, 10)
    encoder_loopback_in_pins = (6, 11)
    i2c_args_controller = {"scl": 5, "sda": 10}
    i2c_args_target = (0,)
    i2c_kwargs_target = {"scl": 6, "sda": 11}
