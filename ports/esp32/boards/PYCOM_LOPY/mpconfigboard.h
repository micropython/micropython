#define MICROPY_HW_BOARD_NAME               "Pycom LoPy"
#define MICROPY_HW_MCU_NAME                 "ESP32"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-lopy"

// LoRa radio (SX1272) pin configuration
#define MICROPY_HW_LORA_MOSI                (27)
#define MICROPY_HW_LORA_MISO                (19)
#define MICROPY_HW_LORA_SCLK                (5)
#define MICROPY_HW_LORA_CS                  (17)
#define MICROPY_HW_LORA_DIO0                (23)
#define MICROPY_HW_LORA_RESET               (18)
#define MICROPY_HW_LORA_CHIP                (1272)

// RGB LED (WS2812 NeoPixel)
#define MICROPY_HW_NEOPIXEL_PIN             (0)
