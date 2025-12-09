#define MICROPY_HW_BOARD_NAME               "NUCLEO_H7A3ZI_Q"
#define MICROPY_HW_MCU_NAME                 "STM32H7A3ZIT6Q"

#define MICROPY_HW_ENABLE_RTC               (1)
#define MICROPY_HW_ENABLE_SERVO             (1)
#define MICROPY_HW_ENABLE_RNG               (0) // RNG needs proper configuration
#define MICROPY_HW_ENABLE_ADC               (1)
#define MICROPY_HW_ENABLE_DAC               (1)
#define MICROPY_HW_ENABLE_USB               (1)
#define MICROPY_HW_ENABLE_SDCARD            (0)
#define MICROPY_HW_HAS_SWITCH               (1)
#define MICROPY_HW_HAS_FLASH                (1)

#define MICROPY_BOARD_EARLY_INIT            NUCLEO_H7A3ZIQ_board_early_init

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// There is no external HS crystal, instead it comes from ST-LINK MCO output which is 8MHz.
// The following gives a 280Hz CPU speed.
#define MICROPY_HW_CLK_USE_BYPASS           (1)
#define MICROPY_HW_CLK_PLLM                 (2)
#define MICROPY_HW_CLK_PLLN                 (140)
#define MICROPY_HW_CLK_PLLP                 (2)
#define MICROPY_HW_CLK_PLLQ                 (4)
#define MICROPY_HW_CLK_PLLR                 (2)
#define MICROPY_HW_CLK_PLLVCI               (RCC_PLL1VCIRANGE_2)
#define MICROPY_HW_CLK_PLLVCO               (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC              (0)

// The USB clock is set using PLL3
#define MICROPY_HW_CLK_PLL3M                (1)
#define MICROPY_HW_CLK_PLL3N                (24)
#define MICROPY_HW_CLK_PLL3P                (2)
#define MICROPY_HW_CLK_PLL3Q                (4)
#define MICROPY_HW_CLK_PLL3R                (2)
#define MICROPY_HW_CLK_PLL3VCI              (RCC_PLL3VCIRANGE_3)
#define MICROPY_HW_CLK_PLL3VCO              (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC             (0)

// 6 wait states when running at 280MHz (VOS0 range)
#define MICROPY_HW_FLASH_LATENCY            FLASH_LATENCY_6

// The board has an external 32kHz crystal attached
#define MICROPY_HW_RTC_USE_LSE              (1)

// SMPS configuration
#define MICROPY_HW_PWR_SMPS_CONFIG          (PWR_DIRECT_SMPS_SUPPLY)

// UART config
#define MICROPY_HW_UART1_TX                 (pin_B14)   // Arduino D1,  pin14 on CN10
#define MICROPY_HW_UART1_RX                 (pin_B15)   // Arduino D0,  pin16 on CN10
#define MICROPY_HW_UART2_TX                 (pin_D5)    //              pin 6 on CN9
#define MICROPY_HW_UART2_RX                 (pin_D6)    //              pin 4 on CN9
#define MICROPY_HW_UART2_RTS                (pin_D4)    //              pin 8 on CN9
#define MICROPY_HW_UART2_CTS                (pin_D3)    //              pin10 on CN9
#define MICROPY_HW_UART3_TX                 (pin_D8)
#define MICROPY_HW_UART3_RX                 (pin_D9)
#define MICROPY_HW_UART5_TX                 (pin_B6)
#define MICROPY_HW_UART5_RX                 (pin_B12)
#define MICROPY_HW_UART6_TX                 (pin_C6)
#define MICROPY_HW_UART6_RX                 (pin_C7)
#define MICROPY_HW_UART7_TX                 (pin_F7)
#define MICROPY_HW_UART7_RX                 (pin_F6)
#define MICROPY_HW_UART8_TX                 (pin_E1)
#define MICROPY_HW_UART8_RX                 (pin_E0)

// UART3 connects to the STLINK-V3 on the Nucleo board
// and this is exposed as a USB Serial port.
#define MICROPY_HW_UART_REPL                PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD           115200

// I2C buses
#define MICROPY_HW_I2C1_SCL                 (pin_B8)    // Arduino D15, pin 2 on CN7
#define MICROPY_HW_I2C1_SDA                 (pin_B9)    // Arduino D14, pin 4 on CN7
#define MICROPY_HW_I2C4_SCL                 (pin_F14)   //              pin19 on CN9
#define MICROPY_HW_I2C4_SDA                 (pin_F15)   //              pin12 on CN9

// SPI buses
#define MICROPY_HW_SPI1_NSS                 (pin_A4)    //              pin17 on CN7
#define MICROPY_HW_SPI1_SCK                 (pin_A5)    // Arduino D13, pin10 on CN7
#define MICROPY_HW_SPI1_MISO                (pin_A6)    // Arduino D12, pin12 on CN7
#define MICROPY_HW_SPI1_MOSI                (pin_A7)    // Arduino D11, pin14 on CN7
#define MICROPY_HW_SPI2_NSS                 (pin_B12)
#define MICROPY_HW_SPI2_SCK                 (pin_B10)
#define MICROPY_HW_SPI2_MISO                (pin_C2)
#define MICROPY_HW_SPI2_MOSI                (pin_C1)
#define MICROPY_HW_SPI3_NSS                 (pin_A15)
#define MICROPY_HW_SPI3_SCK                 (pin_B2)
#define MICROPY_HW_SPI3_MISO                (pin_C10)
#define MICROPY_HW_SPI3_MOSI                (pin_C11)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN                (pin_C13)
#define MICROPY_HW_USRSW_PULL               (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE          (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED            (1)

// LEDs
#define MICROPY_HW_LED1                     (pin_B0)    // green
#define MICROPY_HW_LED2                     (pin_E1)    // yellow
#define MICROPY_HW_LED3                     (pin_B14)   // red
#define MICROPY_HW_LED_ON(pin)              (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)             (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_HS                   (1)
#define MICROPY_HW_USB_HS_IN_FS             (1)
#define MICROPY_HW_USB_VBUS_DETECT_PIN      (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN           (pin_A10)

// FDCAN bus
#define MICROPY_HW_CAN1_NAME                "FDCAN1"
#define MICROPY_HW_CAN1_TX                  (pin_D1)
#define MICROPY_HW_CAN1_RX                  (pin_D0)

void NUCLEO_H7A3ZIQ_board_early_init(void);
