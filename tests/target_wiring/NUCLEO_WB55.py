# Target wiring for NUCLEO_WB55.
#
# Connect:
# - PA2 to PA3 (D0 to D1 on Arduino header)
# - PA5 to PB8=I2C1_SCL (D13 to D15 on Arduino header)
# - PA6 to PB9=I2C1_SDA (D12 to D14 on Arduino header)

# LPUART(1) is on PA2/PA3.
uart_loopback_args = ("LP1",)
uart_loopback_kwargs = {}

spi_standalone_args_list = [(1,), (2,)]

pwm_loopback_pins = [("D1", "D0"), ("D12", "D14"), ("D13", "D15")]

i2c_args_controller = {"scl": "D13", "sda": "D12"}  # Arduino header D13/D12
i2c_args_target = (1,)  # PB8/PB9, Arduino header D15/D14
