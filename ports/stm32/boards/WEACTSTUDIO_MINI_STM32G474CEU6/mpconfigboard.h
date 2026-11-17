#define MICROPY_HW_BOARD_NAME       "WEACTSTUDIO_MINI_STM32G474CEU6"
#define MICROPY_HW_MCU_NAME         "STM32G474"


#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1) // A4, A5, A6
#define MICROPY_HW_ENABLE_USB       (1) // USB-C DP->PA12 DN->PA11
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_HAS_SWITCH       (1) // C13 user key
#define MICROPY_HW_HAS_FLASH        (0) // QSPI extflash not mounted

// https://www.st.com/resource/en/datasheet/stm32g474cb.pdf 5.3.9 PLL characteristics
// https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf 7.4.4 PLL configuration register
//
// • f(VCO clock) = f(PLL clock input) × (PLLN / PLLM) (96MHz ~ 344MHz) = 8 * 85 / 2 = 340MHz
// • f(PLL_P) = f(VCO clock) / PLLP  (2.0645 MHz ~  170 MHz)            = 340 / 2 = 170MHz
// • f(PLL_Q) = f(VCO clock) / PLLQ  (8 MHZ ~ 170 MHz)                  = 340 / 2 = 170MHz
// • f(PLL_R) = f(VCO clock) / PLLR  (8 MHz ~ 170 MHz)                  = 340 / 2 = 170MHz
// The board has an HSE XTAL 3225 8MHz connected at OSC_IN/OSC_OUT, the following gives 170MHz CPU speed
#define MICROPY_HW_CLK_PLLM         (2)
#define MICROPY_HW_CLK_PLLN         (85)
#define MICROPY_HW_CLK_PLLP         (2)
#define MICROPY_HW_CLK_PLLQ         (2)
#define MICROPY_HW_CLK_PLLR         (2)

// The board has a 32.768 kHz LSE NX3215SA connected at OSC32_IN/OSC32_OUT
#define MICROPY_HW_RTC_USE_LSE      (1)

#define MICROPY_HW_CLK_USE_HSI48    (1) // for RNG

// 4 wait states, see 3.3.3 Read access latency table
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

#define MICROPY_HW_USB_CDC          (1)

// UART config , same as NUCLEO_G474RE, if flash on U3 is used, then B10/B11 will be used by QSPI1 and UART3 is not usable
#define MICROPY_HW_LPUART1_TX       (pin_A2)  // A2, B11, C1
#define MICROPY_HW_LPUART1_RX       (pin_A3)  // A3, B10, C0
#define MICROPY_HW_UART3_TX         (pin_B10) // B9, B10, C10, D8
#define MICROPY_HW_UART3_RX         (pin_B11) // B8 = boot0, B11, C11, D9, E15

// I2C buses, same as NUCLEO G474RE
#define MICROPY_HW_I2C1_SCL         (pin_B8) // A13, A15, B8 (=boot0)
#define MICROPY_HW_I2C1_SDA         (pin_B9) // A14, B7, B9

// SPI, same as NUCLEO G474RE
// if flash on U3 is used, then B0/B1/B10/B11/A6/A7 must be used for QSPI1
#define MICROPY_HW_SPI1_NSS         (pin_A4) // A4, A15
#define MICROPY_HW_SPI1_SCK         (pin_A5) // A5, B3
#define MICROPY_HW_SPI1_MISO        (pin_A6) // A6, B4
#define MICROPY_HW_SPI1_MOSI        (pin_A7) // A7, B5

// user key config
#define MICROPY_HW_USRSW_PIN       (pin_C13)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_C6)    // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))
