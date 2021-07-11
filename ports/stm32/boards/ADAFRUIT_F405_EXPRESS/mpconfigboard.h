#define MICROPY_HW_BOARD_NAME       "Adafruit Feather STM32F405"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)

// HSE is 12MHz
#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)
#define MICROPY_HW_CLK_LAST_FREQ (1)

// The Feather has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// UART config
#define MICROPY_HW_UART3_NAME   "UART3"    // on RX / TX
#define MICROPY_HW_UART3_TX     (pin_B10)  // TX
#define MICROPY_HW_UART3_RX     (pin_B11)  // RX
#define MICROPY_HW_UART3_RTS    (pin_B14)  // MISO
#define MICROPY_HW_UART3_CTS    (pin_B13)  // SCK

#define MICROPY_HW_UART2_NAME   "UART2"   // on SDA/SCL
#define MICROPY_HW_UART2_TX     (pin_B6)  // SCL
#define MICROPY_HW_UART2_RX     (pin_B7)  // SDA

#define MICROPY_HW_UART6_NAME   "UART6"   // on D5/D6
#define MICROPY_HW_UART6_TX     (pin_C6)  // D6
#define MICROPY_HW_UART6_RX     (pin_C7)  // D5

// I2C buses
#define MICROPY_HW_I2C1_NAME "I2C1"
#define MICROPY_HW_I2C1_SCL (pin_B6)  // SCL
#define MICROPY_HW_I2C1_SDA (pin_B7)  // SDA
#define MICROPY_HW_I2C2_NAME "I2C2"
#define MICROPY_HW_I2C2_SCL (pin_B10)  // TX
#define MICROPY_HW_I2C2_SDA (pin_B11)  // RX

// SPI buses
#define MICROPY_HW_SPI1_NAME "SPIFLASH"
#define MICROPY_HW_SPI1_NSS  (pin_A15) // FLASH CS
#define MICROPY_HW_SPI1_SCK  (pin_B3) // FLASH CLK
#define MICROPY_HW_SPI1_MISO (pin_B4) // FLASH MISO
#define MICROPY_HW_SPI1_MOSI (pin_B5) // FLASH MOSI
#define MICROPY_HW_SPI2_NAME "SPI1"
#define MICROPY_HW_SPI2_NSS  (pin_B12) // SD DETECT
#define MICROPY_HW_SPI2_SCK  (pin_B13) // SCK
#define MICROPY_HW_SPI2_MISO (pin_B14) // MISO
#define MICROPY_HW_SPI2_MOSI (pin_B15) // MOSI

// CAN buses
#define MICROPY_HW_CAN1_NAME "CAN1"
#define MICROPY_HW_CAN1_TX   (pin_B9) // D10
#define MICROPY_HW_CAN1_RX   (pin_B8) // D9

// The Feather has 1 LED
#define MICROPY_HW_LED1             (pin_C1) // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_B12)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

// Bootloader configuration (only needed if Mboot is used)
#define MBOOT_I2C_PERIPH_ID 1
#define MBOOT_I2C_SCL (pin_B8)
#define MBOOT_I2C_SDA (pin_B9)
#define MBOOT_I2C_ALTFUNC (4)
