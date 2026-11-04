# Target wiring for NUCLEO_G0B1RE.
#
# Connect:
# - PC4 to PC5 (D0 to D1 on Arduino header)

pin_loopback_pins = [("D0", "D1")]

pwm_loopback_pins = [("D0", "D1")]

# UART(3) is on PC4/PC5.
uart_loopback_args = (3,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,), (2,)]
