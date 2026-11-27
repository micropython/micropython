# Target wiring for general esp8266 board.
#
# Connect:
# - GPIO4 to GPIO5

pin_loopback_pins = [(4, 5)]
pwm_loopback_pins = [(4, 5)]
adc_loopback_pins = []

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,)]
