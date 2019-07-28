#define YeHuo_F103VE

#define MICROPY_HW_BOARD_NAME       "YeHuo STM32F103VE"
#define MICROPY_HW_MCU_NAME         "STM32F103VE"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_VFS_FAT             (0)
#define MICROPY_HW_ENABLE_STORAGE   (0)
#define MICROPY_PY_PYB_LEGACY       (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_HW_HAS_FLASH        (0)
#define MICROPY_HW_ENABLE_USB       (0)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_USB_FS           (0)

// HSE is 8MHz
#define MICROPY_HW_CLK_USE_HSE (1)
#define MICROPY_HW_SYSCLK_SRC  (RCC_SYSCLKSOURCE_PLLCLK)
#define MICROPY_HW_HSE_PREDIV  (RCC_HSE_PREDIV_DIV1)
#define MICROPY_HW_CLK_PLLMUL  (RCC_PLL_MUL9)

// The board has a 32kHz crystal for the RTC
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
// #define MICROPY_HW_RTC_USE_CALOUT   (1)  // turn on/off PC13 512Hz output

// --------------------------------------------------------------
// USART1 use to auto download with hardware
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)

// USART2
// #define MICROPY_HW_UART2_TX     (pin_A2)  // PA2
// #define MICROPY_HW_UART2_RX     (pin_A3)  // PA3

// USART3 for esp8266
#define MICROPY_HW_UART3_TX     (pin_B10)  // PB10,PC10
#define MICROPY_HW_UART3_RX     (pin_B11)  // PB11,PC11

// UART4
// #define MICROPY_HW_UART4_TX     (pin_C10)  // PC10,PA0
// #define MICROPY_HW_UART4_RX     (pin_C11)  // PC11,PA1

// UART5
// #define MICROPY_HW_UART5_TX     (pin_C12) // PC12
// #define MICROPY_HW_UART5_RX     (pin_D2)  // PD2

#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
// #define MICROPY_HW_I2C1_SCL (pin_B6)  // PB8,PB6
// #define MICROPY_HW_I2C1_SDA (pin_B7)  // PB9,PB7
// #define MICROPY_HW_I2C2_SCL (pin_B10) // PB10
// #define MICROPY_HW_I2C2_SDA (pin_B11) // PB11


// 3 x SPI, SPI1 for W25Q16
// #define MICROPY_HW_SPI2_NSS  (pin_B12) // PB12,PB9
// #define MICROPY_HW_SPI2_SCK  (pin_B13) // PB13,PB10
// #define MICROPY_HW_SPI2_MISO (pin_B14) // PB14,PC2
// #define MICROPY_HW_SPI2_MOSI (pin_B15) // PB15,PC3

// #define MICROPY_HW_SPI3_NSS  (pin_A15) // PA15
// #define MICROPY_HW_SPI3_SCK  (pin_B3)  // PB3,PC10
// #define MICROPY_HW_SPI3_MISO (pin_B4)  // PB4,PC11
// #define MICROPY_HW_SPI3_MOSI (pin_B5)  // PB5,PC12

// 1 x CAN busses
// #define MICROPY_HW_CAN1_TX (pin_B9) // PB9,PD1,PA12
// #define MICROPY_HW_CAN1_RX (pin_B8) // PB8,PD0,PA11

// DAC
// DAC_OUT1 PA4
// DAC_OUT2 PA5

// KEY0 has pullup, and pressing the switch makes the input go low
// #define MICROPY_HW_USRSW_PIN        (pin_C13)
// #define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
// #define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
// #define MICROPY_HW_USRSW_PRESSED    (0)

// KEY1 - no support for multiple user buttons, yet
// pin_A0

// LEDs
#define MICROPY_HW_LED1             (pin_B0)  // Green LED D1
#define MICROPY_HW_LED2             (pin_B1)  // Blue  LED D2
#define MICROPY_HW_LED3             (pin_B5)  // Red   LED D3

#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))
