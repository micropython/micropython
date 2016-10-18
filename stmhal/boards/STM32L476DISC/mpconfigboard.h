#include STM32_HAL_H

#define MICROPY_HW_BOARD_NAME       "L476-DISCO"
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

// MSI is used and is 4MHz
#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (40)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV7)
#define MICROPY_HW_CLK_PLLR (RCC_PLLR_DIV2)
#define MICROPY_HW_CLK_PLLQ (RCC_PLLQ_DIV2)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_4

// USART config
#define MICROPY_HW_UART2_PORT (GPIOD)
#define MICROPY_HW_UART2_PINS (GPIO_PIN_5 | GPIO_PIN_6)

// USART 2 is connected to the virtual com port on the ST-LINK
#define MICROPY_HW_UART_REPL        PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B6)
#define MICROPY_HW_I2C1_SDA (pin_B7)
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)
// We use an array of baudrates and corresponding TIMINGR values.
//
// The value 0x90112626 was obtained from the DISCOVERY_I2C1_TIMING constant
// defined in the STM32L4Cube file Drivers/BSP/STM32L476G-Discovery/stm32l476g_discovery.h
#define MICROPY_HW_I2C_BAUDRATE_TIMING  {{100000, 0x90112626}}
#define MICROPY_HW_I2C_BAUDRATE_DEFAULT 100000
#define MICROPY_HW_I2C_BAUDRATE_MAX     100000

// SPI busses
#define MICROPY_HW_SPI2_NSS     (pin_D0)
#define MICROPY_HW_SPI2_SCK     (pin_D1)
#define MICROPY_HW_SPI2_MISO    (pin_D3)
#define MICROPY_HW_SPI2_MOSI    (pin_D4)

// Joystick is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_B2) // red
#define MICROPY_HW_LED2             (pin_E8) // green
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
// #define MICROPY_HW_USB_OTG_ID_PIN      (pin_C12) // This is not the official ID Pin which should be PA10
