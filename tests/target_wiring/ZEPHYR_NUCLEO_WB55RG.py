# Target wiring for zephyr nucleo_wb55rg.
#
# Connect:
# - PC0=TX to PC1=RX (A0 to A1 on Arduino header)
# - PA5 to PB8=I2C1_SCL (D13 to D15 on Arduino header)
# - PA6 to PB9=I2C1_SDA (D12 to D14 on Arduino header)

from machine import Pin

from machine import Pin
pin_loopback_pins = [(("gpioc", 0), ("gpioc", 1)), (("gpioa", 5), ("gpiob", 8)), (("gpioa", 6), ("gpiob", 9))]
adc_loopback_pins = []  # TODO, currently only configured on PA0 and PC2

uart_loopback_args = ("lpuart1",)
uart_loopback_kwargs = {}

# PB8=I2C1_SCL, PB9=I2C1_SDA (on Arduino header D15/D14)
# PC0=I2C3_SCL, PC1=I2C3_SDA (on Arduino header A0/A1)
i2c_loopback_controller_kwargs = {"scl": Pin(("gpiob", 8)), "sda": Pin(("gpiob", 9))}
i2c_loopback_target_args = ("i2c3",)
i2c_loopback_target_kwargs = {}

####

spi_standalone_args_list = [("spi1",)]

i2c_args_controller = {"scl": Pin(("gpioa", 5)), "sda": Pin(("gpioa", 6))}
i2c_args_target = ("i2c1",)
