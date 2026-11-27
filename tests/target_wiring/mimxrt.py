# Target wiring for general mimxrt board.
#
# Connect:
# - D0 to D1 (uart1,pwm,encoder)
# - D2 to D3 (encoder)
# - D0 to A5 (i2c scl)
# - D1 to A4 (i2c sda)

import sys

if "Teensy" in sys.implementation._machine:
    # Teensy 4.x
    pin_loopback_pins = [
        ("D0", "D1"),  # FLEXPWM X and UART 1
        ("D2", "D3"),  # FLEXPWM A/B
        ("D11", "D12"),  # QTMR and MOSI/MISO of SPI 0
    ]
    pwm_loopback_pins = pin_loopback_pins
    adc_loopback_pins = []  # TODO find pins with ADC
else:
    pin_loopback_pins = [("D0", "D1")]
    pwm_loopback_pins = pin_loopback_pins
    adc_loopback_pins = []  # TODO find pins with ADC

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [()]

pwm_loopback_pins = [("D0", "D1")]

encoder_loopback_id = 0
encoder_loopback_out_pins = ("D0", "D2")
encoder_loopback_in_pins = ("D1", "D3")

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

i2c_loopback_controller_kwargs = {"scl": "D0", "sda": "D1"}
i2c_loopback_target_args = (0,)  # pins A5(scl)/A4(sda)
i2c_loopback_target_kwargs = {}
