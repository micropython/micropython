# Target wiring for OPENMV_AE3.
#
# Connect:
# - P4 to P5 on the pin header (P0_5 and P0_4, UART1 TX and RX).
#
# The I2C and display entries use devices on the board and carrier board.

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(0,)]

pwm_loopback_pins = [("P0_4", "P0_5")]

adc_loopback_pins = [("P0_5", "P0_4")]

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

# VL53L8CX time-of-flight sensor on I2C3, powered by P5_6.
i2c_scan_devices = [(3, 0x29, "P5_6")]

# SSD1351 OLED on the carrier board.
ssd1351_display = {"spi": 0, "cs": "P5_2", "dc": "P1_2", "rst": "P7_3"}
