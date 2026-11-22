# Target wiring for NUCLEO_L152RE.
#
# Connect:
# - PA9 to PA10 (D8 to D2 on Arduino header)
# - PB3 to PB5 (D3 to D4 on Arduino header)

pin_loopback_pins = [("D2", "D8"), ("D3", "D4")]

# D3=PB3 has TIM2_CH2, D4=PB5 has TIM3_CH2.
pwm_loopback_pins = [("D3", "D4"), ("D4", "D3")]

# UART(1) is on PA9/PA10.
uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,), (2,), (3,)]
