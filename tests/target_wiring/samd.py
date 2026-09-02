# Target wiring for general samd board.
#
# Connect:
# - D0 to D1

import sys
if "Xiao" in sys.implementation._machine:
    # TODO: xiao needs proper pin naming
    pin_loopback_pins = [("A0_D0", "A1_D1")]
    pwm_loopback_pins = [("A0_D0", "A1_D1")]
    adc_loopback_pins = [("A0_D0", "A1_D1")]

    uart_loopback_args = ()
    uart_loopback_kwargs = {"tx": "A0_D1", "rx": "A1_D0"}
else:
    pin_loopback_pins = [("D0", "D1")]
    pwm_loopback_pins = [("D0", "D1")]
    adc_loopback_pins = [("D0", "D1")]

    uart_loopback_args = ()
    uart_loopback_kwargs = {"tx": "D1", "rx": "D0"}

spi_standalone_args_list = [()]
