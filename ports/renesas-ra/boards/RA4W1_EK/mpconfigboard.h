// MCU config
#define MICROPY_HW_BOARD_NAME       "RA4W1_EK"
#define MICROPY_HW_MCU_NAME         "RA4W1"
#define MICROPY_HW_MCU_SYSCLK       48000000
#define MICROPY_HW_MCU_PCLK         48000000

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
#define MICROPY_HW_RTC_SOURCE       (1)     // 0: subclock, 1: LOCO (32.768khz)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// board config

// UART
// #define MICROPY_HW_UART0_TX       (pin_P101) // Disable (Conflict with PMOD)
// #define MICROPY_HW_UART0_RX       (pin_P100) // Disable (Conflict with PMOD)
// #define MICROPY_HW_UART0_CTS      (pin_P103) // Disable (Conflict with PMOD)
#define MICROPY_HW_UART1_TX         (pin_P213)
#define MICROPY_HW_UART1_RX         (pin_P212)
// #define MICROPY_HW_UART1_CTS      (pin_P101) // Disable (Conflict with PMOD)
#define MICROPY_HW_UART4_TX         (pin_P205) // REPL
#define MICROPY_HW_UART4_RX         (pin_P206) // REPL
// #define MICROPY_HW_UART4_CTS      (pin_P407) // Conflict with PMOD1-IO1
#define MICROPY_HW_UART9_TX         (pin_P109)
#define MICROPY_HW_UART9_RX         (pin_P110)
// #define MICROPY_HW_UART9_CTS      (pin_P108) // NC
#define MICROPY_HW_UART_REPL        HW_UART_4
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
#define MICROPY_HW_I2C0_SCL         (pin_P204) // Note that conflict with PMOD IO0
#define MICROPY_HW_I2C0_SDA         (pin_P407) // Note that conflict with PMOD IO1
// #define MICROPY_HW_I2C1_SCL       (pin_P100) // Disable (Conflict with SPI0)
// #define MICROPY_HW_I2C1_SDA       (pin_P101) // Disable (Conflict with SPI0)

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P103) // PMOD
#define MICROPY_HW_SPI0_RSPCK       (pin_P102) // PMOD
#define MICROPY_HW_SPI0_MISO        (pin_P100) // PMOD
#define MICROPY_HW_SPI0_MOSI        (pin_P101) // PMOD
// #define MICROPY_HW_SPI1_SSL       (pin_P108) // Disable (vector not registered)
// #define MICROPY_HW_SPI1_RSPCK     (pin_P111) // Disable (vector not registered)
// #define MICROPY_HW_SPI1_MISO      (pin_P110) // Disable (vector not registered)
// #define MICROPY_HW_SPI1_MOSI      (pin_P109) // Disable (vector not registered)

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P402)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P106)
#define MICROPY_HW_LED2             (pin_P404)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_low(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_high(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
