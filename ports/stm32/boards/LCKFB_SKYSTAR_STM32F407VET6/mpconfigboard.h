/*
 * MicroPython board configuration for LCKFB-SkyStar-STM32F407VET6
 * Author: LCKFB-YZH
 *
 * Hardware: STM32F407VET6, 8MHz HSE, USB-FS on PA11/PA12,
 *           TF card on SDIO (PC8-PC12, PD2), TF detect on PD3
 *           https://oshwhub.com/li-chuang-kai-fa-ban/li-chuang-liang-shan-pai-tian-kong-xing-kai-fa-ban
 */

/* ======================== Board information ======================== */
#define MICROPY_HW_BOARD_NAME       "LCKFB-SkyStar-STM32F407VET6"
#define MICROPY_HW_MCU_NAME         "STM32F407VET6"

/* ======================== Feature enables ======================== */
#define MICROPY_HW_HAS_SWITCH       (1)     // Defined later in this file
#define MICROPY_HW_HAS_FLASH        (1)     // Use internal Flash as filesystem storage
#define MICROPY_HW_ENABLE_RNG       (1)     // Hardware random number generator
#define MICROPY_HW_ENABLE_RTC       (1)     // Real-time clock
#define MICROPY_HW_ENABLE_ADC       (1)     // Analog-to-digital converter
#define MICROPY_HW_ENABLE_DAC       (1)     // DAC (PA4/PA5)
#define MICROPY_HW_ENABLE_USB       (1)     // USB support
#define MICROPY_HW_ENABLE_SDCARD    (1)     // TF/SD card support
#define MICROPY_HW_ENABLE_SERVO     (1)     // Servo/PWM output

/* ======================== Clock configuration ======================== */
/*
 * Clock tree calculation (STM32F407, target SYSCLK = 168MHz):
 *
 *   HSE = 8MHz
 *   PLL_INPUT  = HSE / PLLM = 8MHz / 8 = 1MHz
 *   VCO_OUTPUT = PLL_INPUT × PLLN = 1MHz × 336 = 336MHz
 *   SYSCLK     = VCO_OUTPUT / PLLP = 336MHz / 2 = 168MHz  ✓
 *   USB_CLK    = VCO_OUTPUT / PLLQ = 336MHz / 7 = 48MHz   ✓ (USB requires 48MHz)
 *
 * This is the classic 8MHz → 168MHz setup, identical to STM32F4DISC.
 */
#define MICROPY_HW_CLK_PLLM (8)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

/* Uncomment if a 32.768kHz LSE crystal (RTC) is populated; SkyStar Lite omits it so leave disabled for compatibility. */
// #define MICROPY_HW_RTC_USE_LSE      (1)

/* ======================== UART configuration ======================== */
/* STM32F407VET6 supports UART1~UART6; enable as wired. */
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)
#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)

/* ======================== I2C configuration ======================== */
#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)

/* ======================== SPI configuration ======================== */
#define MICROPY_HW_SPI2_SCK  (pin_B10)
#define MICROPY_HW_SPI2_MISO (pin_C2)
#define MICROPY_HW_SPI2_MOSI (pin_C3)

/* ======================== SD Card (TF) SDIO configuration ======================== */
/*
 * TF card pinout:
 *   PC12 → SDIO_CK   (clock)
 *   PD2  → SDIO_CMD  (command)
 *   PC8  → SDIO_D0   (data0)
 *   PC9  → SDIO_D1   (data1)
 *   PC10 → SDIO_D2   (data2)
 *   PC11 → SDIO_D3   (data3)
 *   PD3  → TF_DETECT (card detect, active low = card inserted)
 */
#define MICROPY_HW_SDCARD_CK               (pin_C12)
#define MICROPY_HW_SDCARD_CMD              (pin_D2)
#define MICROPY_HW_SDCARD_D0               (pin_C8)
#define MICROPY_HW_SDCARD_D1               (pin_C9)
#define MICROPY_HW_SDCARD_D2               (pin_C10)
#define MICROPY_HW_SDCARD_D3               (pin_C11)

/*
 * TF card detect configuration:
 * DETECT_PIN: PD3
 * DETECT_PULL: pull-up (high when no card)
 * DETECT_PRESENT: GPIO_PIN_RESET means low level = card present
 */
#define MICROPY_HW_SDCARD_DETECT_PIN       (pin_D3)
#define MICROPY_HW_SDCARD_DETECT_PULL      (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT   (GPIO_PIN_RESET)

/* Auto-mount SD card to /sd at boot */
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT    (1)

/* ======================== USB configuration ======================== */
/*
 * PA11 → USB_DM (D-)
 * PA12 → USB_DP (D+)
 *
 * USB Full Speed (FS), used for CDC (REPL console) and MSC mass storage.
 */
#define MICROPY_HW_USB_FS              (1)

/* ======================== LED configuration ======================== */
#define MICROPY_HW_LED1             (pin_B2)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

/* ======================== User button configuration ======================== */
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLDOWN)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)
