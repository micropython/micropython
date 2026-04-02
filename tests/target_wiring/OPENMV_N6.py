# Target wiring for OPENMV_N6.
#
# Connect:
# - P4 to P5 (PB10 to PB11)
# - P7 to P8 (PG13 to PD13)

# UART(3) is on P4/P5.
uart_loopback_args = (3,)
uart_loopback_kwargs = {}

# Test SPI(2) and SPI(4).
spi_standalone_args_list = [(2,), (4,)]

# Test TIM2 and TIM4.
pwm_loopback_pins = [("P4", "P5"), ("P7", "P8")]
