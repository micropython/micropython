# Target wiring for esp32 boards.
#
# Connect:
# - pin_1a to pin_1b (uart, pwm, encoder, i2c_scl)
# - pin_2a to pin_2b (encoder, i2c_sda)

import sys

if ("ESP32-C" in sys.implementation._machine) or ("ESP32-H2" in sys.implementation._machine):
    spi_standalone_args_list = [(1,)]
else:
    spi_standalone_args_list = [(1,), (2,)]


# For example: build="ESP32_GENERIC" for "ESP32_GENERIC-SPIRAM"
_build, _, _variant = sys.implementation._build.partition("-")

pin_2a, pin_2b = {
    # ESP32-DevKitC V4, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/_images/esp32_devkitC_v4_pinlayout.png
    "ESP32_GENERIC": (12, 13),
    # ESP8684-DevKitC-02, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c2/_images/esp8684-devkitc-02-pinout_v1.0.png
    "ESP32_GENERIC_C2": (6, 7),
    # ESP32-C3-DevKitC-02, https://docs.espressif.com/projects/esp-idf/en/v5.2/esp32c3/_images/esp32-c3-devkitc-02-v1-pinout.png
    "ESP32_GENERIC_C3": (6, 7),
    # ESP32-C5-DevKitC-1-N8R8, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c5/_images/esp32-c5-devkitc-1-pin-layout_v1.2.png
    "ESP32_GENERIC_C5": (6, 7),
    # ESP32-C6-DevKitC-1-N8, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/_images/esp32-c6-devkitc-1-pin-layout.png
    "ESP32_GENERIC_C6": (6, 7),
    # ESP32-H2-DevKitM-1-N4S, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/_images/esp32-h2-devkitm-1-v1.2_pinlayout.png
    "ESP32_GENERIC_H2": (10, 11),
    # ESP32-S2-DevKitC-1-N8R2, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s2/_images/esp32-s2-devkitc-1-v1-pinout.png
    "ESP32_GENERIC_S2": (10, 11),
    # ESP32-S3-DevKitC-1-N8R8, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/_images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg
    "ESP32_GENERIC_S3": (10, 11),
    # https://www.wemos.cc/en/latest/c3/c3_mini.html, https://www.wemos.cc/en/latest/_images/c3_mini_v2.1.0_4_16x9.png
    "LOLIN_C3_MINI": (0, 1),
}.get(_build, (6, 7))

pin_1a = 4
pin_1b = 5

uart_loopback_args = (1,)
uart_loopback_kwargs = {"tx": pin_1a, "rx": pin_1b}

pwm_loopback_pins = [(pin_1a, pin_1b)]

encoder_loopback_id = 0
encoder_loopback_out_pins = (pin_1a, pin_2a)
encoder_loopback_in_pins = (pin_1b, pin_2b)

i2c_loopback_target_args = (0,)
i2c_loopback_controller_kwargs = {"scl": pin_1a, "sda": pin_2a}
i2c_loopback_target_kwargs = {"scl": pin_1b, "sda": pin_2b}
