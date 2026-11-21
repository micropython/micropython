# Target wiring for STM32N6570_DK.
#
# Connect:
# - D0 to D1 on the Arduino header (PF6 to PD5)

# UART(2) is on D0/D1.
uart_loopback_args = (2,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(5,)]

pwm_loopback_pins = [("D0", "D1")]
