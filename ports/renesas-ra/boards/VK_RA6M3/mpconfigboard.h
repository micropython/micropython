// MCU config
#define MICROPY_HW_BOARD_NAME       "VK-RA6M3"
#define MICROPY_HW_MCU_NAME         "RA6M3"
#define MICROPY_HW_MCU_SYSCLK       120000000
#define MICROPY_HW_MCU_PCLK         120000000

// module config
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_PY_BUILTINS_COMPLEX (1)
#define MICROPY_PY_GENERATOR_PEND_THROW (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_HEAPQ            (1)
#define MICROPY_PY_THREAD           (0) // disable ARM_THUMB_FP using vldr due to RA has single float only
#define MICROPY_HW_ETH_MDC          (1)
#define MICROPY_PY_NETWORK          (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT MICROPY_HW_BOARD_NAME

#define MODULE_LCD_ENABLED          (1)
#define MODULE_CAM_ENABLED          (1)

// peripheral config
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_RTC_SOURCE       (1)     // 0: subclock
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_USBDEV    (1)
#define MICROPY_HW_ENABLE_UART_REPL (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_HAS_QSPI_FLASH   (1)
#define MICROPY_HW_HAS_SDHI_CARD    (1)

// board config

// UART
#define MICROPY_HW_UART7_TX         (pin_P613) // D1
#define MICROPY_HW_UART7_RX         (pin_P614) // D0
#define MICROPY_HW_UART8_TX         (pin_P105) // Modbus (RS485)
#define MICROPY_HW_UART8_RX         (pin_P104) // Modbus (RS485)
#define MICROPY_HW_UART9_TX         (pin_P109) // REPL
#define MICROPY_HW_UART9_RX         (pin_P110) // REPL
#define MICROPY_HW_UART_REPL        HW_UART_9
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C
#define MICROPY_HW_I2C1_SCL         (pin_P205)
#define MICROPY_HW_I2C1_SDA         (pin_P206)

// SPI
#define MICROPY_HW_SPI1_SSL         (pin_P301) // D10
#define MICROPY_HW_SPI1_RSPCK       (pin_P204) // D13
#define MICROPY_HW_SPI1_MISO        (pin_P202) // D12
#define MICROPY_HW_SPI1_MOSI        (pin_P203) // D11

// PWM
// #define MICROPY_HW_PWM_8A           (pin_P107) // D7
#define MICROPY_HW_PWM_8A           (pin_P605) // LED_B
#define MICROPY_HW_PWM_2A           (pin_P113) // D4
#define MICROPY_HW_PWM_2B           (pin_P114) // D5
#define MICROPY_HW_PWM_4A           (pin_P115) // D6
// #define MICROPY_HW_PWM_4A           (pin_P302) // D9
#define MICROPY_HW_PWM_5B           (pin_P202) // D12
#define MICROPY_HW_PWM_5A           (pin_P203) // D11
// #define MICROPY_HW_PWM_4B           (pin_P204) // D13
// #define MICROPY_HW_PWM_4B           (pin_P301) // D10
#define MICROPY_HW_PWM_4B           (pin_P608) // LED_Y
#define MICROPY_HW_PWM_7B           (pin_P303) // D8
// #define MICROPY_HW_PWM_7B           (pin_P602) // PN4_3
// #define MICROPY_HW_PWM_7A           (pin_P304) // PN4_8
#define MICROPY_HW_PWM_7A           (pin_P603) // LED_R
#define MICROPY_HW_PWM_6B           (pin_P600) // PN4_4
#define MICROPY_HW_PWM_6A           (pin_P601) // LED_G
#define MICROPY_HW_PWM_8B           (pin_P604) // PN4_2

// DAC
#define MICROPY_HW_DAC0             (pin_P014) // A4
#define MICROPY_HW_DAC1             (pin_P015) // A5

// Switch
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_P008)
// #define MICROPY_HW_USRSW_PIN        (pin_P009)
#define MICROPY_HW_USRSW_PULL       (MP_HAL_PIN_PULL_NONE)
#define MICROPY_HW_USRSW_EXTI_MODE  (MP_HAL_PIN_TRIGGER_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_P603)
#define MICROPY_HW_LED2             (pin_P601)
#define MICROPY_HW_LED3             (pin_P605)
#define MICROPY_HW_LED4             (pin_P608)
#define MICROPY_HW_LED_ON(pin)      mp_hal_pin_high(pin)
#define MICROPY_HW_LED_OFF(pin)     mp_hal_pin_low(pin)
#define MICROPY_HW_LED_TOGGLE(pin)  mp_hal_pin_toggle(pin)
