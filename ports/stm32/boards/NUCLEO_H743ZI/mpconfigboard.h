// the board definition file has been maximized to use every
// possible peripheral with the board. The pin allocation has selected
// to provide "maximum" compatibility with the Arduino connector on the board

#define MICROPY_HW_BOARD_NAME       "NUCLEO_H743ZI"
#define MICROPY_HW_MCU_NAME         "STM32H743"

#define MICROPY_HW_FLASH_FS_LABEL   "nucleoh7"

#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_ADC       (0)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_ENABLE_CAN       (0)

#define MICROPY_BOARD_EARLY_INIT    NUCLEO_H743ZI_board_early_init
void NUCLEO_H743ZI_board_early_init(void);

// The board has an 8MHz HSE, the following gives 400MHz CPU speed
#define MICROPY_HW_CLK_PLLM (4)
#define MICROPY_HW_CLK_PLLN (400)
#define MICROPY_HW_CLK_PLLP (2)
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_CLK_PLLR (2)

// with the 8MHz HSE, settings of the PLL for USB
#define MICROPY_HW_PLL3_PLL3M (4)
#define MICROPY_HW_PLL3_PLL3N (120)
#define MICROPY_HW_PLL3_PLL3P (2)
#define MICROPY_HW_PLL3_PLL3Q (5)
#define MICROPY_HW_PLL3_PLL3R (2)

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// alternative REPL port
#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

// UART1 config
#define MICROPY_HW_UART1_TX   (pin_B6)
#define MICROPY_HW_UART1_RX   (pin_B15)

// UART2 config
#define MICROPY_HW_UART2_TX   (pin_D5)
#define MICROPY_HW_UART2_RX   (pin_D6)
#define MICROPY_HW_UART2_CTS  (pin_A0) // optional use
#define MICROPY_HW_UART2_RTS  (pin_D4) // optional use

// UART3 config
#define MICROPY_HW_UART3_TX    (pin_D8)  // STLINK VCOM
#define MICROPY_HW_UART3_RX    (pin_D9)  // STLINK VCOM
//#define MICROPY_HW_UART3_CTS    (pin_D11) // optional use
//#define MICROPY_HW_UART3_RTS    (pin_D12) // optional use

// UART6 Available serial interface on Arduino connector
#define MICROPY_HW_UART6_TX   (pin_G14) // "D1"
#define MICROPY_HW_UART6_RX   (pin_G9)  // "D0"
#define MICROPY_HW_UART6_CTS  (pin_G15) // optional use
#define MICROPY_HW_UART6_RTS  (pin_G8)  // optional use

// UART7 config
#define MICROPY_HW_UART7_TX    (pin_E8)
#define MICROPY_HW_UART7_RX    (pin_E7)
#define MICROPY_HW_UART7_CTS  (pin_E10) // optional use
#define MICROPY_HW_UART7_RTS  (pin_E9)  // optional use

// UART8 config
#define MICROPY_HW_UART8_TX    (pin_E1)
#define MICROPY_HW_UART8_RX    (pin_E0)
#define MICROPY_HW_UART8_CTS  (pin_D14) // optional use
#define MICROPY_HW_UART8_RTS  (pin_D15) // optional use


// Available I2C interface on Arduino connector
#define MICROPY_HW_I2C1_SCL    (pin_B8)  // "D15"
#define MICROPY_HW_I2C1_SDA    (pin_B9)  // "D14"

#define MICROPY_HW_I2C2_SCL    (pin_F1)
#define MICROPY_HW_I2C2_SDA    (pin_F0)
// optional when using SMBUS instead of I2C
#define MICROPY_HW_I2C2_SMB    (pin_F2)  // not implemented

// Available I2C interface on Arduino connector
#define MICROPY_HW_I2C4_SCL    (pin_F14) // "D4"
#define MICROPY_HW_I2C4_SDA    (pin_F15) // "D2"


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

// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

#define MICROPY_HW_USB_SOF_TP1         (pin_A8) // assigned on this board
#define MICROPY_HW_USB_FS_POWER        (pin_G6) // if no OTG > LOW!
#define MICROPY_HW_USB_OVER_CURRENT    (pin_G7) // fault signal


// SD card detect switch (SDMMC1)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_G2)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)


// The ARDUINO SPI1 can only used when DAC1 (PA5) is not in use and PA7 is "freed"
// with JP6 as this signal is shared with the RMII INTERFACE for Ethernet (RMII_CRS_DV)
#define MICROPY_HW_SPI1_SCK    (pin_A5)
#define MICROPY_HW_SPI1_MISO  (pin_A6)
#define MICROPY_HW_SPI1_MOSI  (pin_A7)
// not on Arduino connector, so use a different GPIO and configure no hardware support for NSS
#define MICROPY_HW_SPI1_NSS    (pin_A15)

// SPI2 with NSS select support
#define MICROPY_HW_SPI2_SCK    (pin_B10)
#define MICROPY_HW_SPI2_MISO  (pin_C2)
#define MICROPY_HW_SPI2_MOSI  (pin_C3)
// or configure no hardware support for NSS and use a different GPIO
#define MICROPY_HW_SPI2_NSS    (pin_B12)  // optionally

// SPI4 with NSS select support
#define MICROPY_HW_SPI4_SCK    (pin_E2)
#define MICROPY_HW_SPI4_MISO  (pin_E5)
#define MICROPY_HW_SPI4_MOSI  (pin_E6)
// or configure no hardware support for NSS and use a different GPIO
#define MICROPY_HW_SPI4_NSS    (pin_E4) // optionally

// SPI5 with NSS select support
#define MICROPY_HW_SPI5_SCK    (pin_F7)
#define MICROPY_HW_SPI5_MISO  (pin_F8)
#define MICROPY_HW_SPI5_MOSI  (pin_F11)
// or configure no hardware support for NSS and use a different GPIO
#define MICROPY_HW_SPI5_NSS    (pin_F6)  // optionally


// CAN1 bus active on other pins PD0,PD1 and set in can.c
#define MICROPY_HW_CAN1_RX  (pin_D0)  // still hard coded in can.c
#define MICROPY_HW_CAN1_TX  (pin_D1)
