#define MICROPY_HW_BOARD_NAME       "STM32H7B3I-DK"
#define MICROPY_HW_MCU_NAME         "STM32H7B3LIH6Q"

#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (0)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

// The board has a 24MHz HSE, the following gives 280MHz CPU speed
#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (280)
#define MICROPY_HW_CLK_PLLP (2)
#define MICROPY_HW_CLK_PLLQ (2)
#define MICROPY_HW_CLK_PLLR (2)

// The USB clock is set using PLL3 (48Mhz usb clock)
#define MICROPY_HW_CLK_PLL3M (12)
#define MICROPY_HW_CLK_PLL3N (192)
#define MICROPY_HW_CLK_PLL3P (17)
#define MICROPY_HW_CLK_PLL3Q (8)
#define MICROPY_HW_CLK_PLL3R (2)

// 6 wait states when running at 280MHz (VOS0 range)
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_6

#if 0
// 512MBit external OSPI flash, used for either the filesystem or XIP memory mapped
#define MICROPY_HW_OSPIFLASH_SIZE_BITS_LOG2 (29)
#define MICROPY_HW_OSPIFLASH_CS     (pin_G6)
#define MICROPY_HW_OSPIFLASH_CLK    (pin_B2)
#define MICROPY_HW_OSPIFLASH_DQS    (pin_C5)
#define MICROPY_HW_OSPIFLASH_IO0    (pin_P8)
#define MICROPY_HW_OSPIFLASH_IO1    (pin_F9)
#define MICROPY_HW_OSPIFLASH_IO2    (pin_F7)
#define MICROPY_HW_OSPIFLASH_IO3    (pin_F6)
#define MICROPY_HW_OSPIFLASH_IO4    (pin_C1)
#define MICROPY_HW_OSPIFLASH_IO5    (pin_H3)
#define MICROPY_HW_OSPIFLASH_IO6    (pin_D6)
#define MICROPY_HW_OSPIFLASH_IO7    (pin_G14)
#endif

// UART buses
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
#define MICROPY_HW_UART4_TX         (pin_H13) // Arduino Connector CN11-Pin1
#define MICROPY_HW_UART4_RX         (pin_H14) // Arduino Connector CN11-Pin2
#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_G11)   // red
#define MICROPY_HW_LED2             (pin_G2)    // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

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
