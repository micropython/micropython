// Board definition for the Seeed Studio XIAO RP2040 Plus
// https://www.seeedstudio.com/XIAO-RP2040-Plus-p-6258.html

#ifndef _BOARDS_SEEED_XIAO_RP2040_PLUS_H
#define _BOARDS_SEEED_XIAO_RP2040_PLUS_H

// For board detection
#define SEEED_XIAO_RP2040_PLUS

// --- LED ---
// User LED (single color, active low)
#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25
#endif

#ifndef PICO_DEFAULT_LED_PIN_INVERTED
#define PICO_DEFAULT_LED_PIN_INVERTED 1
#endif

// --- WS2812 RGB LED ---
#ifndef PICO_DEFAULT_WS2812_PIN
#define PICO_DEFAULT_WS2812_PIN 12
#endif

// WS2812 power enable (active high)
#define SEEED_XIAO_RP2040_PLUS_NEOPIXEL_POWER_PIN 24

// ------------- UART -------------//
#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif

#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 0
#endif

#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 1
#endif

// ------------- I2C0 -------------//
// I2C0 uses i2c1 hardware instance: SDA=GPIO6, SCL=GPIO7
#ifndef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C 1
#endif

#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 6
#endif

#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 7
#endif

// ------------- I2C1 (new on Plus) -------------//
// I2C1 uses i2c0 hardware instance: SDA=GPIO20, SCL=GPIO21
#define PICO_DEFAULT_I2C1 0
#define PICO_DEFAULT_I2C1_SDA_PIN 20
#define PICO_DEFAULT_I2C1_SCL_PIN 21

// ------------- SPI -------------//
#ifndef PICO_DEFAULT_SPI
#define PICO_DEFAULT_SPI 0
#endif

#ifndef PICO_DEFAULT_SPI_TX_PIN
#define PICO_DEFAULT_SPI_TX_PIN 3
#endif

#ifndef PICO_DEFAULT_SPI_RX_PIN
#define PICO_DEFAULT_SPI_RX_PIN 4
#endif

#ifndef PICO_DEFAULT_SPI_SCK_PIN
#define PICO_DEFAULT_SPI_SCK_PIN 2
#endif

// ------------- FLASH -------------//

// Best performance/compatibility with selected flash
#define PICO_BOOT_STAGE2_CHOOSE_W25X10CL 1

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

// XIAO RP2040 Plus has 2MB SPI flash
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
#endif

// All boards have B1+ RP2040
#ifndef PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED 0
#endif

#endif
