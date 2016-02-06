#include STM32_HAL_H

#define MICROPY_HW_BOARD_NAME       "F429I-DISCO"
#define MICROPY_HW_MCU_NAME         "STM32F429"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_CAN       (1)

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// UART config
#define MICROPY_HW_UART1_PORT (GPIOA)
#define MICROPY_HW_UART1_PINS (GPIO_PIN_9 | GPIO_PIN_10)
#define MICROPY_HW_UART2_PORT (GPIOD)
#define MICROPY_HW_UART2_PINS (GPIO_PIN_8 | GPIO_PIN_9)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_A8)
#define MICROPY_HW_I2C1_SDA (pin_C9)

// SPI busses
//#define MICROPY_HW_SPI1_NSS     (pin_A4)
//#define MICROPY_HW_SPI1_SCK     (pin_A5)
//#define MICROPY_HW_SPI1_MISO    (pin_A6)
//#define MICROPY_HW_SPI1_MOSI    (pin_A7)
#if defined(USE_USB_HS_IN_FS)
// The HS USB uses B14 & B15 for D- and D+
#else
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)
#endif
//#define MICROPY_HW_SPI4_NSS     (pin_E11)
//#define MICROPY_HW_SPI4_SCK     (pin_E12)
//#define MICROPY_HW_SPI4_MISO    (pin_E13)
//#define MICROPY_HW_SPI4_MOSI    (pin_E14)
#define MICROPY_HW_SPI5_NSS     (pin_F6)
#define MICROPY_HW_SPI5_SCK     (pin_F7)
#define MICROPY_HW_SPI5_MISO    (pin_F8)
#define MICROPY_HW_SPI5_MOSI    (pin_F9)
//#define MICROPY_HW_SPI6_NSS     (pin_G8)
//#define MICROPY_HW_SPI6_SCK     (pin_G13)
//#define MICROPY_HW_SPI6_MISO    (pin_G12)
//#define MICROPY_HW_SPI6_MOSI    (pin_G14)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_G14) // red
#define MICROPY_HW_LED2             (pin_G13) // green
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRRL = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRRH = pin->pin_mask)

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_B13)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_B12)
