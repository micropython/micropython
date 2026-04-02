# Target wiring for NUCLEO_H723ZG.
#
# TODO: need to work out why D12/13/14/15 don't work properly,
# because it would be good to use those for I2C tests.
#
# Connect:
# - D0 to D1 (Arduino labels)
# - D52 to D53 (Arduino labels)

# UART(2) is on PD5/PD6 = D53/D52.
uart_loopback_args = (2,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(3,)]

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

pwm_loopback_pins = [("D0", "D1")]
pin_loopback_pins = [("D0", "D1")]
#pwm_loopback_pins = [("D14", "D12"), ("D15", "D13")]
#pwm_loopback_pins = [("D12", "D14"), ("D13", "D15")]

#i2c_args_controller = {"scl": "D13", "sda": "D12"}  # Arduino header D13/D12
#i2c_args_target = (1,)  # PB8/PB9, Arduino header D15/D14
i2c_args_controller = {"scl": "fail", "sda": "fail"}
i2c_args_target = ("fail",)
