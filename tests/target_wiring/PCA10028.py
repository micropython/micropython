# Target wiring for PCA10028.
#
# This MCU only has one UART and it's used for the REPL.
#
# Connect:
# - P0.01 and P0.02

pin_loopback_pins = [("P1", "P2")]
pwm_loopback_pins = pin_loopback_pins

#uart_loopback_args = (0,)
#uart_loopback_kwargs = {}

spi_standalone_args_list = [(0,), (1,)]
