# Target wiring for general mimxrt board.
#
# Connect:
# - UART1 TX and RX, usually D0 and D1

import sys

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [()]

if "Teensy" in sys.implementation._machine:
    # Teensy 4.x
    pwm_loopback_pins = [
        ("D0", "D1"),  # FLEXPWM X and UART 1
        ("D2", "D3"),  # FLEXPWM A/B
        ("D11", "D12"),  # QTMR and MOSI/MISO of SPI 0
    ]
else:
    pwm_loopback_pins = [("D0", "D1")]

encoder_loopback_id = 0
encoder_loopback_out_pins = ("D0", "D2")
encoder_loopback_in_pins = ("D1", "D3")
