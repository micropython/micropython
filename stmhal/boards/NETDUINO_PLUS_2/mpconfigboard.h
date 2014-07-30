#define NETDUINO_PLUS_2

#define MICROPY_HW_BOARD_NAME       "NetduinoPlus2"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"

#define MICROPY_HW_HAS_SWITCH       (1)

// On the netuino, the sdcard appears to be wired up as a 1-bit
// SPI, so the driver needs to be converted to support that before
// we can turn this on.
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_I2C1      (0)
#define MICROPY_HW_ENABLE_SPI1      (0)
#define MICROPY_HW_ENABLE_SPI3      (0)
#define MICROPY_HW_ENABLE_CC3K      (0)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_B11)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
// These four defines are mandatory
#define PYB_LED_STORAGE1			1
#define PYB_LED_STORAGE2			2
#define PYB_LED_ERROR1				1
#define PYB_LED_ERROR1				2
// LED's pin mapping on the board
// Usage : {{&pyb_led_type, nb_led, pin mapping}
#define MICROPY_HW_LED_MAPPING \
	{{&pyb_led_type}, 1, &pin_A10}, /* blue */ \
	{{&pyb_led_type}, 2, &pin_C13}, /* white */

#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRRL = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRRH = pin->pin_mask)
