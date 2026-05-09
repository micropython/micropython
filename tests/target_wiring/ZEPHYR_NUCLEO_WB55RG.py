# Target wiring for zephyr nucleo_wb55rg.
#
# Connect:
# - PC0=TX to PC1=RX (A0 to A1 on Arduino header)
# - PA5 to PB8=I2C1_SCL (D13 to D15 on Arduino header)
# - PA6 to PB9=I2C1_SDA (D12 to D14 on Arduino header)

from machine import Pin

uart_loopback_args = ("lpuart1",)
uart_loopback_kwargs = {}

spi_standalone_args_list = [("spi1",)]

# for machine.Pin
pwm_loopback_pins = [(("gpioc", 0), ("gpioc", 1))]

i2c_args_controller = {"scl": Pin(("gpioa", 5)), "sda": Pin(("gpioa", 6))}
i2c_args_target = ("i2c1",)
