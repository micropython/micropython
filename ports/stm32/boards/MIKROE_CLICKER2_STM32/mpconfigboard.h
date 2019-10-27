#define MICROPY_HW_BOARD_NAME       "MIKROE_CLICKER2_STM32"
#define MICROPY_HW_MCU_NAME         "STM32F407"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)

// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM         (25)
#define MICROPY_HW_CLK_PLLN         (336)
#define MICROPY_HW_CLK_PLLP         (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ         (7)
#define MICROPY_HW_CLK_LAST_FREQ    (1)

// The board has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (0)  // turn on/off PC13 512Hz output

// UART config
// mikroBUS slot 1
#define MICROPY_HW_UART2_NAME       "SLOT1"
#define MICROPY_HW_UART2_TX         (pin_D5)
#define MICROPY_HW_UART2_RX         (pin_D6)
// mikroBUS slot 2
#define MICROPY_HW_UART3_NAME       "SLOT2"
#define MICROPY_HW_UART3_TX         (pin_D8)
#define MICROPY_HW_UART3_RX         (pin_D9)
// HDR2
#define MICROPY_HW_UART4_NAME       "HDR2"
#define MICROPY_HW_UART4_TX         (pin_A0)
#define MICROPY_HW_UART4_RX         (pin_A1)

// I2C buses
// mikroBUS slot 2 / HDR2
#define MICROPY_HW_I2C2_NAME        "SLOT2"
#define MICROPY_HW_I2C2_SCL         (pin_B10)
#define MICROPY_HW_I2C2_SDA         (pin_B11)
// mikroBUS slot 1
#define MICROPY_HW_I2C3_NAME        "SLOT1"
#define MICROPY_HW_I2C3_SCL         (pin_A8)
#define MICROPY_HW_I2C3_SDA         (pin_C9)

// SPI buses
// mikroBUS slot 2 / HDR1
#define MICROPY_HW_SPI2_NAME        "SLOT2"
#define MICROPY_HW_SPI2_NSS         (pin_E11)
#define MICROPY_HW_SPI2_SCK         (pin_B13)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)
// mikroBUS slot 1
#define MICROPY_HW_SPI3_NAME        "SLOT1"
#define MICROPY_HW_SPI3_NSS         (pin_E8)
#define MICROPY_HW_SPI3_SCK         (pin_C10)
#define MICROPY_HW_SPI3_MISO        (pin_C11)
#define MICROPY_HW_SPI3_MOSI        (pin_C12)

// USRSW is pulled high; pressing the button makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_E0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_E12) // red
#define MICROPY_HW_LED2             (pin_E15) // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)

// Bootloader configuration (only needed if Mboot is used)
#define MBOOT_I2C_PERIPH_ID         2
#define MBOOT_I2C_SCL               (pin_B10)
#define MBOOT_I2C_SDA               (pin_B11)
#define MBOOT_I2C_ALTFUNC           (4)
#define MBOOT_BOOTPIN_PIN           (pin_A10)
#define MBOOT_BOOTPIN_PULL          (MP_HAL_PIN_PULL_NONE)
#define MBOOT_BOOTPIN_ACTIVE        (0)
#define MBOOT_FSLOAD                (1)
