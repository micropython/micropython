# Target wiring for general nrf board.
#
# Connect:
# - UART0 TX and RX

pin_loopback_pins = [("TX", "RX")]
pwm_loopback_pins = [("TX", "RX")]
adc_loopback_pins = [("TX", "RX")]

uart_loopback_args = (0,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,)]
