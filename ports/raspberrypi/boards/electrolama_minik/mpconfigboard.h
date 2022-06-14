#define MICROPY_HW_BOARD_NAME  "Electrolama minik"
#define MICROPY_HW_MCU_NAME    "rp2040"

// GPIO25 is the WS2812 LED on the module
#define MICROPY_HW_NEOPIXEL    (&pin_GPIO25)

// GPIO15 is not routed out
#define IGNORE_PIN_GPIO15      1
