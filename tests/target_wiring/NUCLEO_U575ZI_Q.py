# Target wiring for NUCLEO_U575ZI_Q.
#
# Connect:
# - D12 to D14 (Arduino labels)
# - D13 to D15 (Arduino labels)
# - D52 to D53 (Arduino labels)

pin_loopback_pins = [("D12", "D14"), ("D13", "D15"), ("D52", "D53")]
pwm_loopback_pins = [("D12", "D14"), ("D13", "D15")]

# UART(2) is on PD5/PD6 = Arduino D53/D52.
uart_loopback_args = (2,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,), (3,)]

# CAN args assume no connection for single device tests.
can_args = (1,)
can_kwargs = {}

i2c_args_controller = {"scl": "D13", "sda": "D12"}  # Arduino D13/D12
i2c_args_target = (1,)  # PB8/PB9 = Arduino D15/D14
