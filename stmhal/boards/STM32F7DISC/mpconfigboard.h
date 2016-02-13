#define STM32F7DISC

#define MICROPY_HW_BOARD_NAME       "F7DISC"
#define MICROPY_HW_MCU_NAME         "STM32F746"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_CAN       (1)

#define MICROPY_BOARD_EARLY_INIT    STM32F7DISC_board_early_init
void STM32F7DISC_board_early_init(void);

// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (7)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_6

// UART config
#define MICROPY_HW_UART1_TX_PORT    (GPIOA)
#define MICROPY_HW_UART1_TX_PIN     (GPIO_PIN_9)
#define MICROPY_HW_UART1_RX_PORT    (GPIOB)
#define MICROPY_HW_UART1_RX_PIN     (GPIO_PIN_7)

#define MICROPY_HW_UART6_PORT       (GPIOC)
#define MICROPY_HW_UART6_PINS       (GPIO_PIN_6 | GPIO_PIN_7)
#define MICROPY_HW_UART7_PORT       (GPIOF)
#define MICROPY_HW_UART7_PINS       (GPIO_PIN_6 | GPIO_PIN_7)

#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)

#define MICROPY_HW_I2C3_SCL         (pin_H7)
#define MICROPY_HW_I2C3_SDA         (pin_H8)

// The STM32F7 uses a TIMINGR register which is configured using an Excel
// Spreadsheet from AN4235: http://www.st.com/web/en/catalog/tools/PF258335
// We use an array of baudrates and corresponding TIMINGR values.
//
// The value 0x40912732 was obtained from the DISCOVERY_I2Cx_TIMING constant
// defined in the STM32F7Cube file Drivers/BSP/STM32F746G-Discovery/stm32f7456g_discovery.h
#define MICROPY_HW_I2C_BAUDRATE_TIMING  {{100000, 0x40912732}}
#define MICROPY_HW_I2C_BAUDRATE_DEFAULT 100000
#define MICROPY_HW_I2C_BAUDRATE_MAX     100000

// SPI
#define MICROPY_HW_SPI2_NSS         (pin_I0)
#define MICROPY_HW_SPI2_SCK         (pin_I1)
#define MICROPY_HW_SPI2_MISO        (pin_B14)
#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_I11)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_I1) // green
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRR = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRR = (pin->pin_mask << 16))

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_C13)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config (CN13 - USB OTG FS)
// The Hardware VBUS detect only works on pin PA9. The STM32F7 Discovery uses
// PA9 for VCP_TX functionality and connects the VBUS to pin J12 (so software
// only detect). So we don't define the VBUS detect pin since that requires PA9.

/*#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_J12)*/
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
