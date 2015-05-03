#define PYBV3

#define MICROPY_HW_BOARD_NAME       "PYBv3"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (1)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_SPI1      (1)
#define MICROPY_HW_ENABLE_SPI2      (1)
#define MICROPY_HW_ENABLE_SPI3      (0)
#define MICROPY_HW_ENABLE_CAN       (1)

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// The pyboard has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)

// X-skin: X9=PB6=SCL, X10=PB7=SDA
#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)

// Y-skin: Y9=PB10=SCL, Y10=PB11=SDA
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_A13)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_A8)  // R1 - red
#define MICROPY_HW_LED2             (pin_A10) // R2 - red
#define MICROPY_HW_LED3             (pin_C4)  // G1 - green
#define MICROPY_HW_LED4             (pin_C5)  // G2 - green
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRRH = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRRL = pin->pin_mask)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_C13)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_SET)

// USB VBUS detect pin
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)

// UARTs config
#define PYB_USART_1_PORT    GPIOA
#define PYB_USART_1_PINS    (GPIO_PIN_9 | GPIO_PIN_10)
#define PYB_USART_2_PORT    GPIOA
#define PYB_USART_2_PINS    (GPIO_PIN_2 | GPIO_PIN_3)
#define PYB_USART_2_RTS     GPIO_PIN_1
#define PYB_USART_2_CTS     GPIO_PIN_0
#define PYB_USART_3_PORT    GPIOB
#define PYB_USART_3_PINS    (GPIO_PIN_10 | GPIO_PIN_11)
#define PYB_USART_3_RTS     GPIO_PIN_14
#define PYB_USART_3_CTS     GPIO_PIN_13
#define PYB_USART_4_PORT    GPIOA
#define PYB_USART_4_PINS    (GPIO_PIN_0 | GPIO_PIN_1)
// There is no 5
#define PYB_USART_6_PORT    GPIOC
#define PYB_USART_6_PINS    (GPIO_PIN_6 | GPIO_PIN_7)
