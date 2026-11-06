# Target wiring for zephyr nucleo_wb55rg.
#
# Connect:
# - TX=PC0 to RX=PC1

from machine import Pin

uart_loopback_args = ("lpuart1",)
uart_loopback_kwargs = {}

# PB8=I2C1_SCL, PB9=I2C1_SDA (on Arduino header D15/D14)
# PC0=I2C3_SCL, PC1=I2C3_SDA (on Arduino header A0/A1)
i2c_loopback_controller_kwargs = {"scl": Pin(("gpiob", 8)), "sda": Pin(("gpiob", 9))}
i2c_loopback_target_args = ("i2c3",)
i2c_loopback_target_kwargs = {}
