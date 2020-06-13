#define MICROPY_HW_BOARD_NAME       "STM32F407IG_ICORE3"
#define MICROPY_HW_MCU_NAME         "STM32F407IG"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_USB_MSC          (1)
#define MICROPY_PY_NETWORK          (1)
#define MICROPY_PY_THREAD           (1)

// HSE is 8MHz
#define MICROPY_HW_CLK_PLLM (24)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

// UART config
#define MICROPY_HW_UART1_TX     (pin_A9)
#define MICROPY_HW_UART1_RX     (pin_A10)
#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)
#define MICROPY_HW_UART3_RTS    (pin_D12)
#define MICROPY_HW_UART3_CTS    (pin_D11)
#define MICROPY_HW_UART4_TX     (pin_A0)
#define MICROPY_HW_UART4_RX     (pin_A1)
#define MICROPY_HW_UART6_TX     (pin_C6)
#define MICROPY_HW_UART6_RX     (pin_C7)

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)
#define MICROPY_HW_I2C2_SCL (pin_F1)
#define MICROPY_HW_I2C2_SDA (pin_F0)
#define MICROPY_HW_I2C3_SCL (pin_H7)
#define MICROPY_HW_I2C3_SDA (pin_H8)

// SPI busses
#define MICROPY_HW_SPI2_NSS  (pin_I0)
#define MICROPY_HW_SPI2_SCK  (pin_I1)
#define MICROPY_HW_SPI2_MISO (pin_I2)
#define MICROPY_HW_SPI2_MOSI (pin_I3)

// CAN busses
#define MICROPY_HW_CAN1_TX (pin_A12)
#define MICROPY_HW_CAN1_RX (pin_A11)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A2)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_I5) // red
#define MICROPY_HW_LED2             (pin_I6) // green
#define MICROPY_HW_LED3             (pin_I7) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// USB config
#define MICROPY_HW_USB_HS              (1)
#define MICROPY_HW_USB_HS_ULPI_NXT     (pin_H4)
#define MICROPY_HW_USB_HS_ULPI_DIR     (pin_I11)
#define MICROPY_HW_USB_HS_ULPI_STP     (pin_C0)
#define MICROPY_HW_USB_HS_ULPI_CLK     (pin_A5)
#define MICROPY_HW_USB_OTG_HS_ULPI_D0  (pin_A3)
#define MICROPY_HW_USB_OTG_HS_ULPI_D1  (pin_B0)
#define MICROPY_HW_USB_OTG_HS_ULPI_D2  (pin_B1)
#define MICROPY_HW_USB_OTG_HS_ULPI_D3  (pin_B10)
#define MICROPY_HW_USB_OTG_HS_ULPI_D4  (pin_B11)
#define MICROPY_HW_USB_OTG_HS_ULPI_D5  (pin_B12)
#define MICROPY_HW_USB_OTG_HS_ULPI_D6  (pin_B13)
#define MICROPY_HW_USB_OTG_HS_ULPI_D7  (pin_B5)

