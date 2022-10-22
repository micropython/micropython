// MCU config
#define MICROPY_HW_BOARD_NAME       "RA6M1_EK"
#define MICROPY_HW_MCU_NAME         "RA6M1"
#define MICROPY_HW_MCU_SYSCLK       120000000
#define MICROPY_HW_MCU_PCLK         60000000

// module config
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_PY_BUILTINS_COMPLEX (1)
#define MICROPY_PY_GENERATOR_PEND_THROW (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_UHEAPQ           (1)
#define MICROPY_PY_UTIMEQ           (1)
#define MICROPY_PY_THREAD           (0) // disable ARM_THUMB_FP using vldr due to RA has single float only

// peripheral config
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_RTC_SOURCE       (0)     // subclock
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// board config

// UART
#define MICROPY_HW_UART0_TX         (pin_P411) // REPL
#define MICROPY_HW_UART0_RX         (pin_P410) // REPL
// #define MICROPY_HW_UART1_TX       (pin_P213) // Disable (Conflict with XTAL)
// #define MICROPY_HW_UART1_RX       (pin_P212) // Disable (Conflict with XTAL)
#define MICROPY_HW_UART2_TX         (pin_P302)
#define MICROPY_HW_UART2_RX         (pin_P301)
// #define MICROPY_HW_UART3_TX       (pin_P409) // Disable
// #define MICROPY_HW_UART3_RX       (pin_P408) // Disable
// #define MICROPY_HW_UART3_CTS      (pin_P411) // Disable (Conflict with UART0_TX)
// #define MICROPY_HW_UART4_TX       (pin_P205) // Disable (Conflict with TSCAP-A)
// #define MICROPY_HW_UART4_RX       (pin_P206) // Disable
// #define MICROPY_HW_UART4_CTS      (pin_P401) // Disable
#define MICROPY_HW_UART8_TX         (pin_P105) // PMOD B
#define MICROPY_HW_UART8_RX         (pin_P104) // PMOD B
#define MICROPY_HW_UART8_CTS        (pin_P107) // PMOD B
// #define MICROPY_HW_UART9_TX       (pin_P602) // Disable (does not work properly)
// #define MICROPY_HW_UART9_RX       (pin_P601) // Disable (does not work properly)
// #define MICROPY_HW_UART9_CTS      (pin_P301) // Disable (does not work properly)
#define MICROPY_HW_UART_REPL        HW_UART_0
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
#define MICROPY_HW_I2C0_SCL         (pin_P400)
#define MICROPY_HW_I2C0_SDA         (pin_P401)
// #define MICROPY_HW_I2C1_SCL       (pin_P100) // Disable (Conflict with SPI0)
// #define MICROPY_HW_I2C1_SDA       (pin_P101) // Disable (Conflict with SPI0)

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P103) // PMOD A
#define MICROPY_HW_SPI0_RSPCK       (pin_P102) // PMOD A
#define MICROPY_HW_SPI0_MISO        (pin_P100) // PMOD A
#define MICROPY_HW_SPI0_MOSI        (pin_P101) // PMOD A
// #define MICROPY_HW_SPI1_SSL       (pin_P108) // Disable (Used for J-Link)
// #define MICROPY_HW_SPI1_RSPCK     (pin_P111)
// #define MICROPY_HW_SPI1_MISO      (pin_P110)
// #define MICROPY_HW_SPI1_MOSI      (pin_P109)

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P415)
#define MICROPY_HW_USRSW_PULL       (MP_HAL_PIN_PULL_NONE)
#define MICROPY_HW_USRSW_EXTI_MODE  (MP_HAL_PIN_TRIGGER_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P112)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
