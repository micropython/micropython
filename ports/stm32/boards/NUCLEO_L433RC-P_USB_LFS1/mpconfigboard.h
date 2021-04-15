#define MICROPY_HW_BOARD_NAME       "NUCLEO-L433RC-P_USB_LFS1"
#define MICROPY_HW_MCU_NAME         "STM32L433RC-P"

// Disable features on devices with 256KB flash.
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_GENERATOR_PEND_THROW (0)
#define MICROPY_PY_FRAMEBUF         (0) // 4KB.
#define MICROPY_PY_USOCKET          (0)
#define MICROPY_PY_NETWORK          (0)
#define MICROPY_PY_ONEWIRE          (0)
#define MICROPY_PY_STM              (0)
#define MICROPY_PY_PYB_LEGACY       (0)
#define MICROPY_PY_UHEAPQ           (0)
#define MICROPY_PY_UTIMEQ           (0)

#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (0) // SERVO requires TIM5 (not on L433).
#define MICROPY_HW_ENABLE_USB       (1) // requires a custom USB cable on PA11 & PA12. Approx 10KB.
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_CAN       (0) // Approx 6KB.

// MSI is used and is 4MHz
#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (40)
#define MICROPY_HW_CLK_PLLR (RCC_PLLR_DIV2) // SYSCLK = 4*1*40/2 = 80MHz.
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV7)
#define MICROPY_HW_CLK_PLLQ (RCC_PLLQ_DIV2)

// The board has an external 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)
#define MICROPY_HW_UART3_TX     (pin_C10)
#define MICROPY_HW_UART3_RX     (pin_C11)
#define MICROPY_HW_LPUART1_TX   (pin_C1)
#define MICROPY_HW_LPUART1_RX   (pin_C0)

// I2C busses
#define MICROPY_HW_I2C1_SCL     (pin_B8)    // Arduino D15, pin 3 on CN6
#define MICROPY_HW_I2C1_SDA     (pin_B7)    // Arduino D14, pin 5 on CN6
#define MICROPY_HW_I2C2_SCL     (pin_B10)   // Arduino D6,  pin 25 on CN6
#define MICROPY_HW_I2C2_SDA     (pin_B11)   //              pin 16 on CN6
#define MICROPY_HW_I2C3_SCL     (pin_A7)    //              pin 24 on CN6
#define MICROPY_HW_I2C3_SDA     (pin_B4)    //              pin 35 on CN5
#define MICROPY_HW_I2C4_SCL     (pin_C0)    //              pin 38 on CN5
#define MICROPY_HW_I2C4_SDA     (pin_C1)    //              pin 36 on CN5

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_A15)   //              pin 27 on CN6
#define MICROPY_HW_SPI1_SCK     (pin_A5)    //              pin 28 on CN6
#define MICROPY_HW_SPI1_MISO    (pin_A6)    //              pin 26 on CN6
#define MICROPY_HW_SPI1_MOSI    (pin_A7)    //              pin 24 on CN6
#define MICROPY_HW_SPI2_NSS     (pin_B12)   //              pin 13 on CN5
#define MICROPY_HW_SPI2_SCK     (pin_B13)   // Arduino D13, pin 11 on CN6
#define MICROPY_HW_SPI2_MISO    (pin_B14)   // Arduino D12, pin 13 on CN6
#define MICROPY_HW_SPI2_MOSI    (pin_B15)   // Arduino D11, pin 15 on CN6
#define MICROPY_HW_SPI3_NSS     (pin_A4)    //              pin 30 on CN6
#define MICROPY_HW_SPI3_SCK     (pin_C10)   //              pin 1 on CN5
#define MICROPY_HW_SPI3_MISO    (pin_C11)   //              pin 2 on CN5
#define MICROPY_HW_SPI3_MOSI    (pin_C12)   //              pin 3 on CN5

// CAN busses
// #define MICROPY_HW_CAN1_TX      (pin_B9)    //              pin 37 on CN5
// #define MICROPY_HW_CAN1_RX      (pin_B8)    //              pin 3 on CN6

// USER B1 has a pull-up and is active low
#define MICROPY_HW_USRSW_PIN        (pin_C13)
#define MICROPY_HW_USRSW_PULL       (0)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// NUCLEO-64 has one user LED
#define MICROPY_HW_LED1             (pin_B15) // green, pin 15 on CN6
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_USB_MSC          (0)
#define MICROPY_HW_USB_HID          (0)
#define USBD_CDC_RX_DATA_SIZE       (256)
#define USBD_CDC_TX_DATA_SIZE       (256)
