#define MICROPY_HW_BOARD_NAME       "STM32F412RG" //my custom PCB
#define MICROPY_HW_MCU_NAME         "STM32F412RG"
#define MICROPY_HW_FLASH_FS_LABEL   "VCCGNDF407VE"

// 1 = use internal flash (1024 KByte)
// 0 = use external SPI flash
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SERVO     (1)

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (8) // 1M HSE
#define MICROPY_HW_CLK_PLLN (192) // this number is the PLL clock in MHz
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)// divide PLL clock by this to get core clock
#define MICROPY_HW_CLK_PLLQ (4)// divide core clock by this to get

// does not have a 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (0)

// UART config
#define MICROPY_HW_UART1_TX     (pin_B6)
#define MICROPY_HW_UART1_RX     (pin_B7)
#define MICROPY_HW_UART2_TX     (pin_A2)
#define MICROPY_HW_UART2_RX     (pin_A3)
#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

// I2C buses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)


// SPI buses
#define MICROPY_HW_SPI1_NSS  (pin_A4)
#define MICROPY_HW_SPI1_SCK  (pin_A5)
#define MICROPY_HW_SPI1_MISO (pin_A6)
#define MICROPY_HW_SPI1_MOSI (pin_A7)


// 用户开关
#define MICROPY_HW_USRSW_PIN        (pin_C6)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// LEDs
#define MICROPY_HW_LED1             (pin_A8) // red
#define MICROPY_HW_LED2             (pin_A9) // green
#define MICROPY_HW_LED3             (pin_A10) // orange
//#define MICROPY_HW_LED4             (pin_D15) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS              (1)
//#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
//#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
