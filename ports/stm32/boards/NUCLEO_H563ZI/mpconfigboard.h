#define MICROPY_HW_BOARD_NAME               "NUCLEO_H563ZI"
#define MICROPY_HW_MCU_NAME                 "STM32H563ZI"

#define MICROPY_PY_PYB_LEGACY               (0)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_ENABLE_RNG               (1)
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_HAS_SWITCH               (1)
#define MICROPY_HW_HAS_FLASH                (1)

// The board has a 8MHz oscillator, the following gives 250MHz CPU speed
#define MICROPY_HW_CLK_USE_BYPASS           (1)
#define MICROPY_HW_CLK_PLLM                 (2)
#define MICROPY_HW_CLK_PLLN                 (125)
#define MICROPY_HW_CLK_PLLP                 (2)
#define MICROPY_HW_CLK_PLLQ                 (2)
#define MICROPY_HW_CLK_PLLR                 (2)
#define MICROPY_HW_CLK_PLLVCI_LL            (LL_RCC_PLLINPUTRANGE_4_8)
#define MICROPY_HW_CLK_PLLVCO_LL            (LL_RCC_PLLVCORANGE_WIDE)
#define MICROPY_HW_CLK_PLLFRAC              (0)

// PLL3 with Q output at 48MHz for USB
// #define MICROPY_HW_CLK_USE_PLL3_FOR_USB
#define MICROPY_HW_CLK_PLL3M                (8)
#define MICROPY_HW_CLK_PLL3N                (192)
#define MICROPY_HW_CLK_PLL3P                (2)
#define MICROPY_HW_CLK_PLL3Q                (4)
#define MICROPY_HW_CLK_PLL3R                (2)
#define MICROPY_HW_CLK_PLL3FRAC             (0)
#define MICROPY_HW_CLK_PLL3VCI_LL           (LL_RCC_PLLINPUTRANGE_1_2)
#define MICROPY_HW_CLK_PLL3VCO_LL           (LL_RCC_PLLVCORANGE_MEDIUM)

// 5 wait states, according to Table 37, Reference Manual (RM0481 Rev 1)
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_5

// There is an external 32kHz oscillator
#define MICROPY_HW_RTC_USE_LSE              (1)

// UART config
#define MICROPY_HW_UART1_TX                 (pin_B6) // SB14: Arduino Connector CN10-Pin14 (D1)
#define MICROPY_HW_UART1_RX                 (pin_B7) // SB63: Arduino Connector CN10-Pin16 (D0)
#define MICROPY_HW_UART3_TX                 (pin_D8) // SB23: ST-Link
#define MICROPY_HW_UART3_RX                 (pin_D9) // SB18: ST-Link

// Connect REPL to UART3 which is provided on ST-Link USB interface
#define MICROPY_HW_UART_REPL                PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD           115200

// I2C buses
#define MICROPY_HW_I2C1_SCL                 (pin_B8) // Arduino Connector CN7-Pin2 (D15)
#define MICROPY_HW_I2C1_SDA                 (pin_B9) // Arduino Connector CN7-Pin4 (D14)
#define MICROPY_HW_I2C2_SCL                 (pin_F1) // Connector CN9-Pin19
#define MICROPY_HW_I2C2_SDA                 (pin_F0) // Connector CN9-Pin21

// SPI buses
// PD14 according to datasheet not working as SPI1_NSS, have to use as GPIO, not as AF
#define MICROPY_HW_SPI1_NSS                 (pin_D14) // Arduino Connector CN7-Pin16 (D10)
#define MICROPY_HW_SPI1_SCK                 (pin_A5) // Arduino Connector CN7-Pin10 (D13)
#define MICROPY_HW_SPI1_MISO                (pin_G9) // Arduino Connector CN7-Pin12 (D12)
#define MICROPY_HW_SPI1_MOSI                (pin_B5) // Arduino Connector CN7-Pin14 (D11)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN                (pin_C13)
#define MICROPY_HW_USRSW_PULL               (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE          (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED            (1)

// LEDs
#define MICROPY_HW_LED1                     (pin_B0) // Green
#define MICROPY_HW_LED2                     (pin_F4) // Orange
#define MICROPY_HW_LED3                     (pin_G4) // Red
#define MICROPY_HW_LED_ON(pin)              (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)             (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS                   (1)
#define MICROPY_HW_USB_MAIN_DEV             (USB_PHY_FS_ID)

// Ethernet via RMII
#define MICROPY_HW_ETH_MDC          (pin_C1)
#define MICROPY_HW_ETH_MDIO         (pin_A2)
#define MICROPY_HW_ETH_RMII_REF_CLK (pin_A1)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pin_A7)
#define MICROPY_HW_ETH_RMII_RXD0    (pin_C4)
#define MICROPY_HW_ETH_RMII_RXD1    (pin_C5)
#define MICROPY_HW_ETH_RMII_TX_EN   (pin_G11)
#define MICROPY_HW_ETH_RMII_TXD0    (pin_G13)
#define MICROPY_HW_ETH_RMII_TXD1    (pin_B15)
