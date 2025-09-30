#define MICROPY_HW_BOARD_NAME       "NUCLEO-H7A3ZI-Q"
#define MICROPY_HW_MCU_NAME         "STM32H7A3ZI"

#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// The board has a 8MHz HSE, the following gives 280MHz CPU speed
#define MICROPY_HW_CLK_PLLM         (1)
#define MICROPY_HW_CLK_PLLN         (70)
#define MICROPY_HW_CLK_PLLP         (2)
#define MICROPY_HW_CLK_PLLQ         (14)
#define MICROPY_HW_CLK_PLLR         (14)
#define MICROPY_HW_CLK_PLLVCI       (RCC_PLL1VCIRANGE_1)
#define MICROPY_HW_CLK_PLLVCO       (RCC_PLL1VCOWIDE)
#define MICROPY_HW_CLK_PLLFRAC      (0)

// The USB clock is set using PLL3 (48Mhz usb clock)
#define MICROPY_HW_CLK_PLL3M        (1)
#define MICROPY_HW_CLK_PLL3N        (30)
#define MICROPY_HW_CLK_PLL3P        (2)
#define MICROPY_HW_CLK_PLL3Q        (5)
#define MICROPY_HW_CLK_PLL3R        (2)
#define MICROPY_HW_CLK_PLL3VCI      (RCC_PLL3VCIRANGE_1)
#define MICROPY_HW_CLK_PLL3VCO      (RCC_PLL3VCOWIDE)
#define MICROPY_HW_CLK_PLL3FRAC     (0)

// 6 wait states when running at 280MHz (VOS0 range)
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_6

// SMPS configuration
#define MICROPY_HW_PWR_SMPS_CONFIG  (PWR_DIRECT_SMPS_SUPPLY)

// UART config
#define MICROPY_HW_UART2_TX                 (pin_D5)
#define MICROPY_HW_UART2_RX                 (pin_D6)
#define MICROPY_HW_UART2_RTS                (pin_D4)
#define MICROPY_HW_UART2_CTS                (pin_D3)
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

#define MICROPY_HW_UART_REPL                PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD           115200


// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B0)   // green 
#define MICROPY_HW_LED2             (pin_E1)    // orange
#define MICROPY_HW_LED3             (pin_B14)    // red - not working?
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (0)
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_IN_FS     (1)
#define MICROPY_HW_USB_MAIN_DEV     (USB_PHY_HS_ID)
#define MICROPY_HW_USB_CDC_NUM      (2)
#define MICROPY_HW_USB_MSC          (0)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_I8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

#define MICROPY_HW_SDMMC_D0                 (pin_C8)
#define MICROPY_HW_SDMMC_D1                 (pin_C9)
#define MICROPY_HW_SDMMC_D2                 (pin_C10)
#define MICROPY_HW_SDMMC_D3                 (pin_C11)
#define MICROPY_HW_SDMMC_CK                 (pin_C12)
#define MICROPY_HW_SDMMC_CMD                (pin_D2)
