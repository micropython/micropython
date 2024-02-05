// Board definition for the nullbits Bit-C PRO

#ifndef _BOARDS_NULLBITS_BIT_C_PRO_H
#define _BOARDS_NULLBITS_BIT_C_PRO_H

// For board detection
#define NULLBITS_BIT_C_PRO

// On some samples, the xosc can take longer to stabilize than is usual
#ifndef PICO_XOSC_STARTUP_DELAY_MULTIPLIER
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64
#endif

// --- BOARD SPECIFIC ---
#define BIT_C_PRO_LED_R_PIN 16
#define BIT_C_PRO_LED_G_PIN 17
#define BIT_C_PRO_LED_B_PIN 18

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

// --- LED ---
// Set the default LED to the Bit-C PRO's B led
#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN BIT_C_PRO_LED_B_PIN
#endif

#ifndef PICO_DEFAULT_LED_PIN_INVERTED
#define PICO_DEFAULT_LED_PIN_INVERTED 1
#endif
// no PICO_DEFAULT_WS2812_PIN

// ------------- I2C -------------//
#ifndef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C 0
#endif

#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 2
#endif

#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 3
#endif

// ------------- SPI -------------//
#ifndef PICO_DEFAULT_SPI
#define PICO_DEFAULT_SPI 0
#endif

#ifndef PICO_DEFAULT_SPI_TX_PIN
#define PICO_DEFAULT_SPI_TX_PIN 23
#endif

#ifndef PICO_DEFAULT_SPI_RX_PIN
#define PICO_DEFAULT_SPI_RX_PIN 20
#endif

#ifndef PICO_DEFAULT_SPI_SCK_PIN
#define PICO_DEFAULT_SPI_SCK_PIN 22
#endif

#ifndef PICO_DEFAULT_SPI_CSN_PIN
#define PICO_DEFAULT_SPI_CSN_PIN 21
#endif

// ------------- FLASH -------------//

// Best performance/compatibility with selected flash
#define PICO_BOOT_STAGE2_CHOOSE_W25X10CL 1

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

// Bit-C PRO has 4MB SPI flash
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (4 * 1024 * 1024)
#endif

// All boards have B1+ RP2040
#ifndef PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED 0
#endif

#endif
