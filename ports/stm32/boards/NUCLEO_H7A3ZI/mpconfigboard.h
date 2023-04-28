#define MICROPY_HW_BOARD_NAME       "NUCLEO_H7A3ZI"
#define MICROPY_HW_MCU_NAME         "STM32H7A3"


#define MICROPY_PY_PYB_LEGACY       (0)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_MMCARD    (1)

#define MICROPY_BOARD_EARLY_INIT    NUCLEO_H7A3_board_early_init
void NUCLEO_H7A3_board_early_init(void);

// Settings for 280 MHz cpu clock.
// Values from stm32cubemx clock configuration.
// HSE external clock is 8 MHz.

#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (70)
#define MICROPY_HW_CLK_PLLP (2)
#define MICROPY_HW_CLK_PLLQ (14)
#define MICROPY_HW_CLK_PLLR (14)
#define MICROPY_HW_CLK_PLLVCI       (RCC_PLL1VCIRANGE_1)
#define MICROPY_HW_CLK_PLLVCO       (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC      (0)


// Settings for USB clock.
// Values from stm32cubemx clock configuration.
// HSE external clock is 8 MHz.
#define MICROPY_HW_CLK_PLL3M (1)
#define MICROPY_HW_CLK_PLL3N (30)
#define MICROPY_HW_CLK_PLL3P (2)
#define MICROPY_HW_CLK_PLL3Q (5)
#define MICROPY_HW_CLK_PLL3R (2)
#define MICROPY_HW_CLK_PLL3VCI      (RCC_PLL3VCIRANGE_1)
#define MICROPY_HW_CLK_PLL3VCO      (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC     (0)


// AXI bus at 280 MHz, 8 wait states.
// From cubemx -> Pinout & Configuration -> System Core -> RCC -> Parameter Settings -> System Parameters -> Flash Latency
// See RM0433 Reference Manual, Table 17 "FLASH recommended number of wait states and programming delay."
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_7

// RTC
// LSE external crystal is 32768 Hz.
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)


// UART config
#define MICROPY_HW_UART2_TX         (pin_D5)
#define MICROPY_HW_UART2_RX         (pin_D6)
#define MICROPY_HW_UART2_RTS        (pin_D4)
#define MICROPY_HW_UART2_CTS        (pin_D3)
#define MICROPY_HW_UART3_TX         (pin_D8)
#define MICROPY_HW_UART3_RX         (pin_D9)
#define MICROPY_HW_UART5_TX         (pin_B6)
#define MICROPY_HW_UART5_RX         (pin_B12)
#define MICROPY_HW_UART6_TX         (pin_C6)
#define MICROPY_HW_UART6_RX         (pin_C7)
#define MICROPY_HW_UART7_TX         (pin_F7)
#define MICROPY_HW_UART7_RX         (pin_F6)
#define MICROPY_HW_UART8_TX         (pin_E1)
#define MICROPY_HW_UART8_RX         (pin_E0)

#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C buses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)
#define MICROPY_HW_I2C2_SCL         (pin_F1)
#define MICROPY_HW_I2C2_SDA         (pin_F0)
#define MICROPY_HW_I2C4_SCL         (pin_F14)
#define MICROPY_HW_I2C4_SDA         (pin_F15)

// SPI buses
#define MICROPY_HW_SPI3_NSS         (pin_A4)
#define MICROPY_HW_SPI3_SCK         (pin_B3)
#define MICROPY_HW_SPI3_MISO        (pin_B4)
#define MICROPY_HW_SPI3_MOSI        (pin_B5)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B0)    // green
#define MICROPY_HW_LED2             (pin_E1)    // yellow
#define MICROPY_HW_LED3             (pin_B14)   // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
// TODO: work out how to configure PA11/12 for USB_OTG_HS
//#define MICROPY_HW_USB_HS               (1)
//#define MICROPY_HW_USB_HS_IN_FS         (1)
//#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
//#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

// FDCAN bus
#define MICROPY_HW_CAN1_NAME  "FDCAN1"
#define MICROPY_HW_CAN1_TX    (pin_D1)
#define MICROPY_HW_CAN1_RX    (pin_D0)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_G2)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_FS           (0)
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_IN_FS     (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)
#define MICROPY_HW_USB_CDC_NUM      (2)
#define MICROPY_HW_USB_MSC          (0)

/*
// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B0)    // green
#define MICROPY_HW_LED2             (pin_B7)    // blue
#define MICROPY_HW_LED3             (pin_B14)   // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// not truncated
*/

