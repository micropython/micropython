#define MICROPY_HW_BOARD_NAME       "NetduinoPlus2"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"

#define MICROPY_HW_HAS_SWITCH       (1)

#define MICROPY_HW_HAS_FLASH        (1)
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
#define MICROPY_HW_ENABLE_CAN       (0)

void NETDUINO_PLUS_2_board_early_init(void);
#define MICROPY_BOARD_EARLY_INIT    NETDUINO_PLUS_2_board_early_init

// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// UART config
#define MICROPY_HW_UART1_PORT (GPIOA)
#define MICROPY_HW_UART1_PINS (GPIO_PIN_9 | GPIO_PIN_10)
#define MICROPY_HW_UART2_PORT (GPIOA)
#define MICROPY_HW_UART2_PINS (GPIO_PIN_2 | GPIO_PIN_3)
#define MICROPY_HW_UART2_RTS  (GPIO_PIN_1)
#define MICROPY_HW_UART2_CTS  (GPIO_PIN_0)
#define MICROPY_HW_UART3_PORT (GPIOD)
#define MICROPY_HW_UART3_PINS (GPIO_PIN_8 | GPIO_PIN_9)
#define MICROPY_HW_UART3_RTS  (GPIO_PIN_12)
#define MICROPY_HW_UART3_CTS  (GPIO_PIN_11)
#define MICROPY_HW_UART4_PORT (GPIOA)
#define MICROPY_HW_UART4_PINS (GPIO_PIN_0 | GPIO_PIN_1)
#define MICROPY_HW_UART6_PORT (GPIOC)
#define MICROPY_HW_UART6_PINS (GPIO_PIN_6 | GPIO_PIN_7)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)

// SPI busses
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_B11)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_A10)   // Blue LED
#define MICROPY_HW_LED2             (pin_C13)   // White LED (aka Power)
#define MICROPY_HW_LED3             (pin_A10)   // Same as Led(1)
#define MICROPY_HW_LED4             (pin_C13)   // Same as Led(2)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB VBUS detect pin
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
