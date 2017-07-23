#define MICROPY_HW_BOARD_NAME       "LIMIFROG"
#define MICROPY_HW_MCU_NAME         "STM32L476"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_CAN       (0)

#define MICROPY_BOARD_EARLY_INIT    LIMIFROG_board_early_init
void LIMIFROG_board_early_init(void);

// MSI is used and is 4MHz
#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (40)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV7)
#define MICROPY_HW_CLK_PLLR (RCC_PLLR_DIV2)
#define MICROPY_HW_CLK_PLLQ (RCC_PLLQ_DIV2)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// USART config
#define MICROPY_HW_UART3_TX (pin_C10)
#define MICROPY_HW_UART3_RX (pin_C11)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A4)
#define MICROPY_HW_SPI1_SCK     (pin_A5)
#define MICROPY_HW_SPI1_MISO    (pin_A6)
#define MICROPY_HW_SPI1_MOSI    (pin_A7)

#define MICROPY_HW_SPI3_NSS     (pin_A15)
#define MICROPY_HW_SPI3_SCK     (pin_B3)
#define MICROPY_HW_SPI3_MISO    (pin_B4)
#define MICROPY_HW_SPI3_MOSI    (pin_B5)

#define MICROPY_HW_USRSW_PIN        (pin_A15)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_C3) // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
// #define MICROPY_HW_USB_OTG_ID_PIN      (pin_C12) // This is not the official ID Pin which should be PA10
