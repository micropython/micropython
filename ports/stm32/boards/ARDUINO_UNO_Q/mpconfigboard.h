#define MICROPY_HW_BOARD_NAME               "ARDUINO_UNO_Q"
#define MICROPY_HW_MCU_NAME                 "STM32U585"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (0)   // USB-C is owned by the application processor
#define MICROPY_HW_ENABLE_RNG               (0)
#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_HAS_FLASH                (1)
#define MICROPY_HW_HAS_SWITCH               (0)

// 16MHz HSE (HSE_VALUE in stm32u5xx_hal_conf.h), PLL to 160MHz.
#define MICROPY_HW_CLK_PLLM                 (1)
#define MICROPY_HW_CLK_PLLN                 (10)
#define MICROPY_HW_CLK_PLLP                 (10)
#define MICROPY_HW_CLK_PLLQ                 (2)
#define MICROPY_HW_CLK_PLLR                 (1)
#define MICROPY_HW_CLK_PLLVCI               (RCC_PLLVCIRANGE_1)
#define MICROPY_HW_CLK_PLLFRAC              (0)

// 4 wait states for 160MHz.
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_4

#define MICROPY_HW_RTC_USE_LSE              (1)
#define MICROPY_HW_RCC_RTC_CLKSOURCE        (RCC_RTCCLKSOURCE_LSE)
#define MICROPY_HW_RCC_HSI48_STATE          (RCC_HSI48_ON)

// USART1 on the Arduino header (D1/D0).
#define MICROPY_HW_UART1_TX                 (pin_B6)
#define MICROPY_HW_UART1_RX                 (pin_B7)

#define MICROPY_HW_I2C2_SCL                 (pin_B10)
#define MICROPY_HW_I2C2_SDA                 (pin_B11)

// SPI2 on the Arduino header.
#define MICROPY_HW_SPI2_NSS                 (pin_B9)
#define MICROPY_HW_SPI2_SCK                 (pin_B13)
#define MICROPY_HW_SPI2_MISO               (pin_B14)
#define MICROPY_HW_SPI2_MOSI               (pin_B15)

// SPI3 connects to the application processor.
#define MICROPY_HW_SPI3_NSS                 (pin_G12)
#define MICROPY_HW_SPI3_SCK                 (pin_G9)
#define MICROPY_HW_SPI3_MISO               (pin_G10)
#define MICROPY_HW_SPI3_MOSI               (pin_B5)

// LPUART1 connects to the application processor (/dev/ttyHS1) and carries the REPL.
#define MICROPY_HW_LPUART1_TX               (pin_G7)
#define MICROPY_HW_LPUART1_RX               (pin_G8)
#define MICROPY_HW_UART_REPL                PYB_LPUART_1
#define MICROPY_HW_UART_REPL_BAUD           115200
