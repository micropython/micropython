#define PYBV10

#define MICROPY_HW_BOARD_NAME       "PYBv1.0"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (1)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_I2C1      (1)
#define MICROPY_HW_ENABLE_SPI1      (1)
#define MICROPY_HW_ENABLE_SPI3      (0)
#define MICROPY_HW_ENABLE_CC3K      (0)

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_B3)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// The pyboard has 4 LEDs
// These four defines are mandatory
#define PYB_LED_STORAGE1			1
#define PYB_LED_STORAGE2			2
#define PYB_LED_ERROR1				1
#define PYB_LED_ERROR2				2
// LEDs' pin mapping on the board
// Usage : {{&pyb_led_type, nb_led, pin mapping}
#define MICROPY_HW_LED_MAPPING \
	{{&pyb_led_type}, 1, &pin_A13}, /* red */ \
	{{&pyb_led_type}, 2, &pin_A14}, /* green */ \
	{{&pyb_led_type}, 3, &pin_A15}, /* yellow */ \
	{{&pyb_led_type}, 4, &pin_B4}, /* blue */

#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRRL = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRRH = pin->pin_mask)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
