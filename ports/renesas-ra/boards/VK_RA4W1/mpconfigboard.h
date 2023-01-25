// MCU config
#define MICROPY_HW_BOARD_NAME       "VK-RA4W1"
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
#define MICROPY_HW_UART1_TX         (pin_P213)
#define MICROPY_HW_UART1_RX         (pin_P212)
// #define MICROPY_HW_UART1_CTS      (pin_P101) // Disable (Conflict with PMOD)
#define MICROPY_HW_UART9_TX         (pin_P109)
#define MICROPY_HW_UART9_RX         (pin_P110)
// #define MICROPY_HW_UART9_CTS      (pin_P108) // NC
#define MICROPY_HW_UART_REPL        HW_UART_9
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
#define MICROPY_HW_I2C1_SCL         (pin_P205) // Note that conflict with PMOD IO0
#define MICROPY_HW_I2C1_SDA         (pin_P206) // Note that conflict with PMOD IO1

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P104) // D10
#define MICROPY_HW_SPI0_RSPCK       (pin_P102) // D13
#define MICROPY_HW_SPI0_MISO        (pin_P100) // D12
#define MICROPY_HW_SPI0_MOSI        (pin_P101) // D11

// PWM
#define MICROPY_HW_PWM_1A           (pin_P105) // D3
#define MICROPY_HW_PWM_8B           (pin_P106) // D8
#define MICROPY_HW_PWM_8A           (pin_P107) // D9
#define MICROPY_HW_PWM_3A           (pin_P111) // D2
#define MICROPY_HW_PWM_4B           (pin_P204) // LED2
#define MICROPY_HW_PWM_3B           (pin_P404) // LED1
#define MICROPY_HW_PWM_5A           (pin_P409) // D4

// DAC
#define MICROPY_HW_DAC0             (pin_P014) // A3

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P414)
#define MICROPY_HW_USRSW_PULL       (MP_HAL_PIN_PULL_NONE)
#define MICROPY_HW_USRSW_EXTI_MODE  (MP_HAL_PIN_TRIGGER_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P404)
#define MICROPY_HW_LED2             (pin_P204)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_low(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_high(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
