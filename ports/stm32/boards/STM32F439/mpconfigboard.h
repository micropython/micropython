#define MICROPY_HW_BOARD_NAME       "CustomPCB"
#define MICROPY_HW_MCU_NAME         "STM32F439"

#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (1) //works with no SD card too
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_CAN       (1)

// SD card detect switch
#if MICROPY_HW_HAS_SDCARD
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (1)
#endif

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8) //divide external clock by this to get 1MHz
#define MICROPY_HW_CLK_PLLN (384) //this number is the PLL clock in MHz
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2) //divide PLL clock by this to get core clock
#define MICROPY_HW_CLK_PLLQ (8) //divide core clock by this to get 48MHz

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)
#define MICROPY_HW_UART2_RTS    (pin_D1)
#define MICROPY_HW_UART2_CTS    (pin_D0)
#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)
#define MICROPY_HW_UART3_RTS    (pin_D12)
#define MICROPY_HW_UART3_CTS    (pin_D11)
#define MICROPY_HW_UART4_TX     (pin_A0)
#define MICROPY_HW_UART4_RX     (pin_A1)
#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_A8)
#define MICROPY_HW_I2C1_SDA (pin_C9)

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A4)
#define MICROPY_HW_SPI1_SCK     (pin_A5)
#define MICROPY_HW_SPI1_MISO    (pin_A6)
#define MICROPY_HW_SPI1_MOSI    (pin_A7)
#if defined(USE_USB_HS_IN_FS)
// The HS USB uses B14 & B15 for D- and D+
#else
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)
#endif
#define MICROPY_HW_SPI3_NSS     (pin_E11)
#define MICROPY_HW_SPI3_SCK     (pin_E12)
#define MICROPY_HW_SPI3_MISO    (pin_E13)
#define MICROPY_HW_SPI3_MOSI    (pin_E14)
//#define MICROPY_HW_SPI4_NSS     (pin_E11)
//#define MICROPY_HW_SPI4_SCK     (pin_E12)
//#define MICROPY_HW_SPI4_MISO    (pin_E13)
//#define MICROPY_HW_SPI4_MOSI    (pin_E14)
//#define MICROPY_HW_SPI5_NSS     (pin_F6)
//#define MICROPY_HW_SPI5_SCK     (pin_F7)
//#define MICROPY_HW_SPI5_MISO    (pin_F8)
//#define MICROPY_HW_SPI5_MOSI    (pin_F9)
//#define MICROPY_HW_SPI6_NSS     (pin_G8)
//#define MICROPY_HW_SPI6_SCK     (pin_G13)
//#define MICROPY_HW_SPI6_MISO    (pin_G12)
//#define MICROPY_HW_SPI6_MOSI    (pin_G14)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

