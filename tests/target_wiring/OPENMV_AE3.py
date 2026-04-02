# Target wiring for OPENMV_AE3 board.
#
# Break out board V4 left pins mapping (it's wired incorrectly):
# - p3 = p5_2
# - p2 = p5_3
# - p1 = p5_0
# - p0 = p5_1
# - p4 = p0_5
# - p5 = p0_4
#
# Connect:
# - P4 and P5 (P0_5/P0_4, for UART1 TX and RX)
# - P0 and P2
# - P1 and P3

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(0,)]

pwm_loopback_pins = [("P4", "P5")]
pwm_loopback_pins += [("P0", "P2"), ("P1", "P3")]

i2c_args_controller = {"scl": "P2", "sda": "P3"}
i2c_args_target = (2,)  # on pins P0/P1
