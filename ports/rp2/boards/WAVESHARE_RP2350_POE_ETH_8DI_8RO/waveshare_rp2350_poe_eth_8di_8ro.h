#ifndef _BOARDS_WAVESHARE_RP2350_POE_ETH_8DI_8RO_H
#define _BOARDS_WAVESHARE_RP2350_POE_ETH_8DI_8RO_H

// UART0 is exposed on the pin header.
#define PICO_DEFAULT_UART 0
#define PICO_DEFAULT_UART_TX_PIN 0
#define PICO_DEFAULT_UART_RX_PIN 1

// The default I2C bus connects to the RTC and the pin header.
#define PICO_DEFAULT_I2C 1
#define PICO_DEFAULT_I2C_SDA_PIN 6
#define PICO_DEFAULT_I2C_SCL_PIN 7

// The default SPI bus connects to the SD card slot. SPI0 is used by W5500.
#define PICO_DEFAULT_SPI 1
#define PICO_DEFAULT_SPI_SCK_PIN 26
#define PICO_DEFAULT_SPI_TX_PIN 27
#define PICO_DEFAULT_SPI_RX_PIN 28
#define PICO_DEFAULT_SPI_CSN_PIN 31

// Flash configuration.
#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1
#define PICO_FLASH_SIZE_BYTES (16 * 1024 * 1024)
#define PICO_FLASH_SPI_CLKDIV 2

#endif
