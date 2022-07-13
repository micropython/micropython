// MCU config
#define MICROPY_HW_BOARD_NAME       "RA6M2_EK"
#define MICROPY_HW_MCU_NAME         "RA6M2"
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
#define MICROPY_HW_RTC_SOURCE       (0)     // 0: subclock
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// board config

// UART
#define MICROPY_HW_UART0_TX         (pin_P411) // REPL
#define MICROPY_HW_UART0_RX         (pin_P410) // REPL
// #define MICROPY_HW_UART0_CTS      (pin_P413) // NC
// #define MICROPY_HW_UART1_TX       (pin_P709) // Disable (read not work properly)
// #define MICROPY_HW_UART1_RX       (pin_P708) // Disable (read not work properly)
// #define MICROPY_HW_UART1_CTS      (pin_P711) // Disable (read not work properly)
// #define MICROPY_HW_UART2_TX       (pin_P302) // Disable
// #define MICROPY_HW_UART2_RX       (pin_P301) // Disable
// #define MICROPY_HW_UART2_CTS      (pin_P203) // NC
// #define MICROPY_HW_UART3_TX       (pin_P310) // Disable
// #define MICROPY_HW_UART3_RX       (pin_P309) // Disable
// #define MICROPY_HW_UART3_CTS      (pin_P312) // Disable
// #define MICROPY_HW_UART4_TX       (pin_P512) // Disable (Conflict with I2C2)
// #define MICROPY_HW_UART4_RX       (pin_P511) // Disable (Conflict with I2C2)
// #define MICROPY_HW_UART4_CTS      (pin_P401) // Disable (Conflict with PMOD B)
// #define MICROPY_HW_UART5_TX       (pin_P501) // Disable
// #define MICROPY_HW_UART5_RX       (pin_P502) // Disable
// #define MICROPY_HW_UART5_CTS      (pin_P504) // Disable
// #define MICROPY_HW_UART6_TX       (pin_P506) // Disable (read not work properly)
// #define MICROPY_HW_UART6_RX       (pin_P505) // Disable (read not work properly)
// #define MICROPY_HW_UART6_CTS      (pin_P503) // Disable (read not work properly)
#define MICROPY_HW_UART7_TX         (pin_P401) // PMOD B
#define MICROPY_HW_UART7_RX         (pin_P402) // PMOD B
#define MICROPY_HW_UART7_CTS        (pin_P403) // PMOD B
// #define MICROPY_HW_UART8_TX       (pin_P105) // Disable (conflict with USER SW1)
// #define MICROPY_HW_UART8_RX       (pin_P104)
#define MICROPY_HW_UART9_TX         (pin_P602)
#define MICROPY_HW_UART9_RX         (pin_P601)
#define MICROPY_HW_UART9_CTS        (pin_P603)
#define MICROPY_HW_UART_REPL        HW_UART_0
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
// #define MICROPY_HW_I2C0_SCL       (pin_P400) // Disable (Conflict with PMOD B)
// #define MICROPY_HW_I2C0_SDA       (pin_P401) // Disable (Conflict with PMOD B)
// #define MICROPY_HW_I2C1_SCL       (pin_P100) // Disable (Conflict with SPI & SDcard)
// #define MICROPY_HW_I2C1_SDA       (pin_P101) // Disable (Conflict with SPI & SDcard)
#define MICROPY_HW_I2C2_SCL         (pin_P512)
#define MICROPY_HW_I2C2_SDA         (pin_P511)

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P103) // PMOD A
#define MICROPY_HW_SPI0_RSPCK       (pin_P102)
#define MICROPY_HW_SPI0_MISO        (pin_P100)
#define MICROPY_HW_SPI0_MOSI        (pin_P101)
#define MICROPY_HW_SPI1_SSL         (pin_P703) // Pin Header
#define MICROPY_HW_SPI1_RSPCK       (pin_P702)
#define MICROPY_HW_SPI1_MISO        (pin_P700)
#define MICROPY_HW_SPI1_MOSI        (pin_P701)

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P105)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P106)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
