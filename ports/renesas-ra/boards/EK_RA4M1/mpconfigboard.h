// MCU config
#define MICROPY_HW_BOARD_NAME       "EK-RA4M1"
#define MICROPY_HW_MCU_NAME         "RA4M1"
#define MICROPY_HW_MCU_SYSCLK       48000000
#define MICROPY_HW_MCU_PCLK         48000000

// module config
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_GENERATOR_PEND_THROW (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_UHEAPQ           (0)
#define MICROPY_PY_UTIMEQ           (0)
#define MICROPY_PY_THREAD           (0)

// peripheral config
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_RTC_SOURCE       (0)    // 0: subclock, 1:LOCO
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// board config

// UART
#define MICROPY_HW_UART0_TX         (pin_P411) // REPL
#define MICROPY_HW_UART0_RX         (pin_P410) // REPL
#define MICROPY_HW_UART1_TX         (pin_P401) // PMOD B
#define MICROPY_HW_UART1_RX         (pin_P402) // PMOD B
#define MICROPY_HW_UART1_CTS        (pin_P403) // PMOD B
#define MICROPY_HW_UART2_TX         (pin_P302)
#define MICROPY_HW_UART2_RX         (pin_P301)
#define MICROPY_HW_UART2_CTS        (pin_P203) // (Conflict with SPI1)
// #define MICROPY_HW_UART9_TX       (pin_P602) // Disable (vector not registered)
// #define MICROPY_HW_UART9_RX       (pin_P601) // Disable (vector not registered)
// #define MICROPY_HW_UART9_CTS      (pin_P603) // Disable (vector not registered)
#define MICROPY_HW_UART_REPL        HW_UART_0
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C (not supported)
// #define MICROPY_HW_I2C0_SCL      (pin_P204) // Conflict with SPI1
// #define MICROPY_HW_I2C0_SDA      (pin_P407) // Conflict with USB_VBUS
// #define MICROPY_HW_I2C0_SCL      (pin_P400) // Conflict wiht PMOD B (GPIO)
// #define MICROPY_HW_I2C0_SDA      (pin_P401) // Conflict with PMOD B (TXD1)
// #define MICROPY_HW_I2C1_SCL      (pin_P100) // Conflict with PMOD A (MISOA_A)
// #define MICROPY_HW_I2C1_SDA      (pin_P101) // Conflict with PMOD A (MOSIA_A)

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P103) // PMOD A
#define MICROPY_HW_SPI0_RSPCK       (pin_P102) // PMOD A
#define MICROPY_HW_SPI0_MISO        (pin_P100) // PMOD A
#define MICROPY_HW_SPI0_MOSI        (pin_P101) // PMOD A
#define MICROPY_HW_SPI1_SSL         (pin_P206) // Use SSLB1 due to SSLB0 is used for TSCAP
// #define MICROPY_HW_SPI1_RSPCK     (pin_P204) // Disable (vector not registered)
// #define MICROPY_HW_SPI1_MISO      (pin_P202) // Disable (vector not registered)
// #define MICROPY_HW_SPI1_MOSI      (pin_P203) // Disable (vector not registered)

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P105)
#define MICROPY_HW_USRSW_PULL       (MP_HAL_PIN_PULL_NONE)
#define MICROPY_HW_USRSW_EXTI_MODE  (MP_HAL_PIN_TRIGGER_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P106)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
