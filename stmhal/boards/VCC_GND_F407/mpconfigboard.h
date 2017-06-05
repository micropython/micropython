#define VCC_GND_F407

#define MICROPY_HW_BOARD_NAME       "VCC-GND STM32F407"
#define MICROPY_HW_MCU_NAME         "STM32F407"

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_CAN       (1)

// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// UART config
#if 0
// A9 is used for USB VBUS detect, and A10 is used for USB_FS_ID.
// UART1 is also on PB6/7 but PB6 is tied to the Audio SCL line.
// Without board modifications, this makes UART1 unusable on this board.
#define MICROPY_HW_UART1_TX     (pin_A9)   // PA9,PB6
#define MICROPY_HW_UART1_RX     (pin_A10)  // PA10,PB7
#endif

#define MICROPY_HW_UART2_TX     (pin_A2)  // PA2,PD5
#define MICROPY_HW_UART2_RX     (pin_A3)  // PA3,PD6
#define MICROPY_HW_UART2_RTS    (pin_A1)  // PA1,PD4
#define MICROPY_HW_UART2_CTS    (pin_A0)  // PA0,PD3

#define MICROPY_HW_UART3_TX     (pin_D8)  // PB10,PC10,PD8
#define MICROPY_HW_UART3_RX     (pin_D9)  // PB11,PC11,PD9
#define MICROPY_HW_UART3_RTS    (pin_D12) // PB14,PD12
#define MICROPY_HW_UART3_CTS    (pin_D11) // PB13,PD11

#define MICROPY_HW_UART4_TX     (pin_A0)  // PA0,PC10
#define MICROPY_HW_UART4_RX     (pin_A1)  // PA1,PC11

//#define MICROPY_HW_UART5_TX     (pin_C12) // PC12
//#define MICROPY_HW_UART5_TX     (pin_D2)  // PD2

// NOTE: PC7 is connected to MCLK on the Audio chip. This is an input signal
//       so I think as long as you're not using the audio chip then it should
//       be fine to use as a UART pin.
#define MICROPY_HW_UART6_TX     (pin_C6) // PC6,PG14
#define MICROPY_HW_UART6_RX     (pin_C7) // PC7,PG9

//#define MICROPY_HW_UART7_TX     (pin_E8) // PE8,PF7
//#define MICROPY_HW_UART7_RX     (pin_E7) // PE7,PF6

//#define MICROPY_HW_UART8_TX     (pin_E1) // PE1
//#define MICROPY_HW_UART8_RX     (pin_E0) // PE0

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B6)  // PB8,PB6
#define MICROPY_HW_I2C1_SDA (pin_B7)  // PB9,PB7
#define MICROPY_HW_I2C2_SCL (pin_B10) // PB10
#define MICROPY_HW_I2C2_SDA (pin_B11) // PB11
//#define MICROPY_HW_I2C3_SCL (pin_A8) // not sure?
//#define MICROPY_HW_I2C3_SDA (pin_C9) // not sure?
// EEPROM on I2C1 PB6/7

// I2S busses
// I2S2_CK  PB13
// I2S2_MCK PC6
// I2S2_SD  PB15
// I2S2_WS  PB12
// I2S3_CK  PB3
// I2S3_MCK PC7
// I2S3_SD  PB5
// I2S3_WS  PA15

// SPI busses
#define MICROPY_HW_SPI1_NSS  (pin_A4)  // PA4
#define MICROPY_HW_SPI1_SCK  (pin_A5)  // PA5,PB3
#define MICROPY_HW_SPI1_MISO (pin_A6)  // PA6,PB4
#define MICROPY_HW_SPI1_MOSI (pin_A7)  // PA7,PB5

#define MICROPY_HW_SPI2_NSS  (pin_B12) // PB12
#define MICROPY_HW_SPI2_SCK  (pin_B13) // PB13
#define MICROPY_HW_SPI2_MISO (pin_B14) // PB14
#define MICROPY_HW_SPI2_MOSI (pin_B15) // PB15

#define MICROPY_HW_SPI3_NSS  (pin_A15) // PA15
#define MICROPY_HW_SPI3_SCK  (pin_B3)  // PB3
#define MICROPY_HW_SPI3_MISO (pin_B4)  // PB4
#define MICROPY_HW_SPI3_MOSI (pin_B5)  // PB5

// CAN busses
// CAN_TX PB9,PD1,PA12
// CAN_RX PB8,PD0,PA11

// DAC
// DAC_OUT1 PA4
// DAC_OUT2 PA5

// LEDs
#define MICROPY_HW_LED1             (pin_B9) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
// 1      - PC10 - DAT2/RES
// 2      - PC11 - CD/DAT3/CS
// 3      - PD2  - CMD/DI
// 4      - VCC  - VDD
// 5      - PC12 - CLK/SCLK
// 6      - GND  - VSS
// 7      - PC8  - DAT0/D0
// 8      - PC9  - DAT1/RES
// 9  SW2 - GND
// 10 SW1 - PA8

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
