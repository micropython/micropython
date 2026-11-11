# Target wiring ESP32-C5-DevKitC-1-N8R8
#
# https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c5/esp32-c5-devkitc-1/index.html
# https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c5/_images/esp32-c5-devkitc-1-pin-layout_v1.2.png
#
# Connect:
# - pin_1a to pin_1b (uart, pwm, encoder, i2c_scl)
# - pin_2a to pin_2b (encoder, i2c_sda)

spi_standalone_args_list = [(1,)]

pin_1a = 4
pin_1b = 5
pin_2a = 6
pin_2b = 7

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": pin_1a, "rx": pin_1b}

pwm_loopback_pins = [(pin_1a, pin_1b)]

encoder_loopback_id = 0
encoder_loopback_out_pins = (pin_1a, pin_2a)
encoder_loopback_in_pins = (pin_1b, pin_2b)

i2c_loopback_target_args = (0,)
i2c_loopback_controller_kwargs = {"scl": pin_1a, "sda": pin_2a}
i2c_loopback_target_kwargs = {"scl": pin_1b, "sda": pin_2b}
