#define MICROPY_HW_BOARD_NAME       "OLIMEX STM32-E407"
#define MICROPY_HW_MCU_NAME         "STM32F407"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)

// HSE is 12MHz
#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// UART config
#define MICROPY_HW_UART1_TX     (pin_B6)
#define MICROPY_HW_UART1_RX     (pin_B7)
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)
#define MICROPY_HW_UART2_RTS    (pin_A1)
#define MICROPY_HW_UART2_CTS    (pin_A0)
#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)
#define MICROPY_HW_UART3_RTS    (pin_D12)
#define MICROPY_HW_UART3_CTS    (pin_D11)
#if MICROPY_HW_HAS_SWITCH == 0
// NOTE: A0 also connects to the user switch. To use UART4 you should
//       set MICROPY_HW_HAS_SWITCH to 0, and also remove SB20 (on the back
//       of the board near the USER switch).
#define MICROPY_HW_UART4_TX     (pin_A0)
#define MICROPY_HW_UART4_RX     (pin_A1)
#endif
#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

// I2C buses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

// SPI buses
#define MICROPY_HW_SPI1_NSS  (pin_A4)
#define MICROPY_HW_SPI1_SCK  (pin_A5)
#define MICROPY_HW_SPI1_MISO (pin_A6)
#define MICROPY_HW_SPI1_MOSI (pin_A7)
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)

// CAN buses
#define MICROPY_HW_CAN1_TX (pin_B9)
#define MICROPY_HW_CAN1_RX (pin_B8)
#define MICROPY_HW_CAN2_TX (pin_B13)
#define MICROPY_HW_CAN2_RX (pin_B12)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_C13)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD card detect switch (Disabled, as some cards are not correctly detected, with it enabled)
// #define MICROPY_HW_SDCARD_DETECT_PIN        (pin_C11)
// #define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
// #define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

// Ethernet via RMII
#define MICROPY_HW_ETH_MDC          (pin_C1)
#define MICROPY_HW_ETH_MDIO         (pin_A2)
#define MICROPY_HW_ETH_RMII_REF_CLK (pin_A1)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pin_A7)
#define MICROPY_HW_ETH_RMII_RXD0    (pin_C4)
#define MICROPY_HW_ETH_RMII_RXD1    (pin_C5)
#define MICROPY_HW_ETH_RMII_TX_EN   (pin_G11)
#define MICROPY_HW_ETH_RMII_TXD0    (pin_G13)
#define MICROPY_HW_ETH_RMII_TXD1    (pin_G14)
