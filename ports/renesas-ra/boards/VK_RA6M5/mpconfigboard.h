// MCU config
#define MICROPY_HW_BOARD_NAME       "VK-RA6M5"
#define MICROPY_HW_MCU_NAME         "RA6M5"
#define MICROPY_HW_MCU_SYSCLK       200000000
#define MICROPY_HW_MCU_PCLK         100000000

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
#define MICROPY_HW_RTC_SOURCE       (1)     // 0: subclock
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_HAS_QSPI_FLASH   (1)
#define MICROPY_HW_HAS_SDHI_CARD    (1)

// board config

// UART
#define MICROPY_HW_UART7_TX         (pin_P613) // D1
#define MICROPY_HW_UART7_RX         (pin_P614) // D0
#define MICROPY_HW_UART6_TX         (pin_P506) // Modbus (RS485)
#define MICROPY_HW_UART6_RX         (pin_P505) // Modbus (RS485)
#define MICROPY_HW_UART9_TX         (pin_P109) // REPL
#define MICROPY_HW_UART9_RX         (pin_P110) // REPL
#define MICROPY_HW_UART_REPL        HW_UART_9
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
#define MICROPY_HW_I2C2_SCL         (pin_P415)
#define MICROPY_HW_I2C2_SDA         (pin_P414)

// SPI
#define MICROPY_HW_SPI0_SSL         (pin_P301) // D10
#define MICROPY_HW_SPI0_RSPCK       (pin_P204) // D13
#define MICROPY_HW_SPI0_MISO        (pin_P202) // D12
#define MICROPY_HW_SPI0_MOSI        (pin_P203) // D11

// PWM
#define MICROPY_HW_PWM_2A           (pin_P113) // D5
#define MICROPY_HW_PWM_2B           (pin_P114) // D6
#define MICROPY_HW_PWM_3A           (pin_P111) // D3
// #define MICROPY_HW_PWM_3A           (pin_P403) // PN3_6
#define MICROPY_HW_PWM_3B           (pin_P112) // D4
// #define MICROPY_HW_PWM_3B           (pin_P404) // PN3_7
#define MICROPY_HW_PWM_4A           (pin_P115) // D9
// #define MICROPY_HW_PWM_4A           (pin_P302) // H6_4
#define MICROPY_HW_PWM_4B           (pin_P608) // D7
// #define MICROPY_HW_PWM_4B           (pin_P301) // D10
// #define MICROPY_HW_PWM_4B           (pin_P204) // D13
// #define MICROPY_HW_PWM_5A           (pin_P203) // D11
// #define MICROPY_HW_PWM_5B           (pin_P202) // D12
#define MICROPY_HW_PWM_6B           (pin_P408) // PN3_8
#define MICROPY_HW_PWM_7A           (pin_P304) // H6_5
#define MICROPY_HW_PWM_7B           (pin_P303) // H6_3
#define MICROPY_HW_PWM_8A           (pin_P605) // PN4_4
#define MICROPY_HW_PWM_8B           (pin_P604) // PN4_3

// DAC
#define MICROPY_HW_DAC0             (pin_P014) // A4
#define MICROPY_HW_DAC1             (pin_P015) // A5

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P010)
// #define MICROPY_HW_USRSW_PIN        (pin_P009)
#define MICROPY_HW_USRSW_PULL       (MP_HAL_PIN_PULL_NONE)
#define MICROPY_HW_USRSW_EXTI_MODE  (MP_HAL_PIN_TRIGGER_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P006)
#define MICROPY_HW_LED2             (pin_P007)
#define MICROPY_HW_LED3             (pin_P008)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
