#define MICROPY_HW_BOARD_NAME       "NUCLEO-F303K8"
#define MICROPY_HW_MCU_NAME         "STM32F303K8TX"

#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_PY_USOCKET          (0)
#define MICROPY_PY_NETWORK          (0)
#define MICROPY_PY_STM              (0)
#define MICROPY_PY_PYB_LEGACY       (0)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_HAS_SWITCH       (1)

// For system clock, enable one source:
//#define MICROPY_HW_CLK_USE_HSI      (1) // internal 8MHz -> PLL = 72MHz.
#define MICROPY_HW_CLK_USE_HSI72    (1) // internal 72MHz.
//#define MICROPY_HW_CLK_USE_HSE      (1) // external crystal -> PLL = 72MHz.
// For HSE set the crystal / clock input frequency HSE_VALUE in stm32f3xx_hal_conf.h
#if MICROPY_HW_CLK_USE_HSE
#define MICROPY_HW_CLK_USE_BYPASS   (1) // HSE comes from STLINK 8MHz, not crystal.
#endif

// The board has an external 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)

// USART2 is connected to the embeded ST-LINK USB VCP
#define MICROPY_HW_UART_REPL PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD 115200

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_A15)
#define MICROPY_HW_I2C1_SDA (pin_A14)

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A15)   //              pin 17 on CN7
#define MICROPY_HW_SPI1_SCK     (pin_A5)    // Arduino D13, pin 11 on CN10
#define MICROPY_HW_SPI1_MISO    (pin_A6)    // Arduino D12, pin 13 on CN10
#define MICROPY_HW_SPI1_MOSI    (pin_A7)    // Arduino D11, pin 15 on CN10

// USER B1 has a pull-up and is active low
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (0)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// NUCLEO-64 has one user LED
#define MICROPY_HW_LED1             (pin_B3) // green
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (MICROPY_HW_ENABLE_USB)
#define MICROPY_HW_USB_MSC          (0)
#define MICROPY_HW_USB_HID          (0)
#define USBD_CDC_RX_DATA_SIZE       (256)
#define USBD_CDC_TX_DATA_SIZE       (256)

