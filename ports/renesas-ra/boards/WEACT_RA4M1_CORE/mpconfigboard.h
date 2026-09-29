// MCU config
#define MICROPY_HW_BOARD_NAME       "WEACT_RA4M1_CORE"
#define MICROPY_HW_MCU_NAME         "RA4M1"
#define MICROPY_HW_MCU_SYSCLK       48000000
#define MICROPY_HW_MCU_PCLK         48000000

// module config - features disabled to conserve flash space
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_GENERATOR_PEND_THROW (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_HEAPQ            (0)
#define MICROPY_PY_THREAD           (0) // disable ARM_THUMB_FP using vldr due to RA has single float only

// peripheral config
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_RTC_SOURCE       (1)    // 0: subclock, 1:LOCO (32.768khz)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_USBDEV    (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

// board config
#define MICROPY_HW_UART9_TX         (pyb_pin_MBTX9) // REPL
#define MICROPY_HW_UART9_RX         (pyb_pin_MBRX9) // REPL
#define MICROPY_HW_UART_REPL        HW_UART_9
#define MICROPY_HW_UART_REPL_BAUD   115200

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pyb_pin_USERSW)
#define MICROPY_HW_USRSW_PULL       (MP_HAL_PIN_PULL_UP)
#define MICROPY_HW_USRSW_EXTI_MODE  (MP_HAL_PIN_TRIGGER_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED1)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
