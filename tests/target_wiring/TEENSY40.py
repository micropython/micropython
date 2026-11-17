# Target wiring for TEENSY40.
# https://www.pjrc.com/store/teensy40.html
# https://github.com/KurtE/TeensyDocuments/blob/master/Teensy4%20Pins.pdf
#
# Connect:
# - D0 to D1 (uart1, pwd, encoder)
# - D2 to D3 (pwm, encoder)
# - D11 to D12 (pwm)
# - A3 to A4 (i2c sda)
# - A5 to A6 (i2c scl)

uart_loopback_args = (1,)
uart_loopback_kwargs = {}

spi_standalone_args_list = [()]

pwm_loopback_pins = [
    ("D0", "D1"),  # FLEXPWM X and UART 1
    ("D2", "D3"),  # FLEXPWM A/B
    ("D11", "D12"),  # QTMR and MOSI/MISO of SPI 0
]

encoder_loopback_id = 0
encoder_loopback_out_pins = ("D0", "D2")
encoder_loopback_in_pins = ("D1", "D3")

# CAN args assume no connection for single device tests
can_args = (1,)
can_kwargs = {}

i2c_loopback_controller_kwargs = {"scl": "A6", "sda": "A3"}  # D20/D17
i2c_loopback_target_args = (0,)  # pins 19(A5,scl)/18(A4,sda) On Teensy 4.x
i2c_loopback_target_kwargs = {}
