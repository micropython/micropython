# Target wiring ESP32-S3-DevKitC-1-N8R8
# https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html
# https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/_images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg
#
# Connect:
# - pin_1a to pin_1b (uart, pwm, encoder, i2c_scl)
# - pin_2a to pin_2b (encoder, i2c_sda)

spi_standalone_args_list = [(1,), (2,)]

pin_1a = 4
pin_1b = 5
pin_2a = 10
pin_2b = 11

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": pin_1a, "rx": pin_1b}

pwm_loopback_pins = [(pin_1a, pin_1b)]

encoder_loopback_id = 0
encoder_loopback_out_pins = (pin_1a, pin_2a)
encoder_loopback_in_pins = (pin_1b, pin_2b)

i2c_loopback_target_args = (0,)
i2c_loopback_controller_kwargs = {"scl": pin_1a, "sda": pin_2a}
i2c_loopback_target_kwargs = {"scl": pin_1b, "sda": pin_2b}
