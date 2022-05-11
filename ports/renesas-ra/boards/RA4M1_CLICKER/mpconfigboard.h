// MCU config
#define MICROPY_HW_BOARD_NAME       "RA4M1_CLICKER"
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
#define MICROPY_HW_RTC_SOURCE       (0)         // 0: subclock, 1:LOCO
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// board config

// UART
#define MICROPY_HW_UART0_TX         (pin_P411) // MBTX0
#define MICROPY_HW_UART0_RX         (pin_P410) // MBRX0
// #define MICROPY_HW_UART0_CTS     (pin_P103) // Disable (Conflict with SSLA0)
#define MICROPY_HW_UART1_TX         (pin_P401) // REPL
#define MICROPY_HW_UART1_RX         (pin_P402) // REPL
#define MICROPY_HW_UART_REPL        HW_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
// #define MICROPY_HW_I2C0_SCL       (pin_P400) // Disable (Conflict with REPL)
// #define MICROPY_HW_I2C0_SDA       (pin_P401) // Disable (Conflict with REPL)
#define MICROPY_HW_I2C1_SCL         (pin_P205)
#define MICROPY_HW_I2C1_SDA         (pin_P206)

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P103)
#define MICROPY_HW_SPI0_RSPCK       (pin_P102)
#define MICROPY_HW_SPI0_MISO        (pin_P100)
#define MICROPY_HW_SPI0_MOSI        (pin_P101)

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P304)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P409)
#define MICROPY_HW_LED2             (pin_P408)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
