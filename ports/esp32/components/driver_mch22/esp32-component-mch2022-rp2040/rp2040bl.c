#include "rp2040bl.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <sdkconfig.h>

#include "driver/uart.h"
#include "rp2040.h"
#include "string.h"

#define RP2040_BL_UART 0

void rp2040_bl_install_uart() {
    if (uart_is_driver_installed(RP2040_BL_UART)) return;
    fflush(stdout);
    ESP_ERROR_CHECK(uart_driver_install(RP2040_BL_UART, 2048, 0, 0, NULL, 0));
    uart_config_t uart_config = {
        .baud_rate  = 921600,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(RP2040_BL_UART, &uart_config));
}

void rp2040_bl_uninstall_uart() {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return;
    uart_driver_delete(RP2040_BL_UART);
}

void flush_stdin() {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return;
    uint8_t data[256];
    uart_read_bytes(RP2040_BL_UART, data, sizeof(data), 10 / portTICK_PERIOD_MS);
}

bool read_stdin(uint8_t* buffer, uint32_t len, uint32_t timeout) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    int read = uart_read_bytes(RP2040_BL_UART, buffer, len, timeout / portTICK_PERIOD_MS);
    return (read == len);
}

bool rp2040_bl_sync() {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[] = "SYNC";
    uart_write_bytes(RP2040_BL_UART, command, 4);
    uint8_t rx_buffer[4 * 6];
    read_stdin(rx_buffer, sizeof(rx_buffer), 1000);
    if (memcmp(rx_buffer, "PICO", 4) != 0) return false;
    return true;
}

bool rp2040_bl_get_info(uint32_t* flash_start, uint32_t* flash_size, uint32_t* erase_size, uint32_t* write_size, uint32_t* max_data_len) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[] = "INFO";
    uart_write_bytes(RP2040_BL_UART, command, 4);
    uint8_t rx_buffer[4 * 6];
    read_stdin(rx_buffer, sizeof(rx_buffer), 1000);
    if (memcmp(rx_buffer, "OKOK", 4) != 0) return false;
    memcpy((uint8_t*) flash_start, &rx_buffer[4 * 1], 4);
    memcpy((uint8_t*) flash_size, &rx_buffer[4 * 2], 4);
    memcpy((uint8_t*) erase_size, &rx_buffer[4 * 3], 4);
    memcpy((uint8_t*) write_size, &rx_buffer[4 * 4], 4);
    memcpy((uint8_t*) max_data_len, &rx_buffer[4 * 5], 4);
    return true;
}

bool rp2040_bl_erase(uint32_t address, uint32_t length) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[12];
    snprintf(command, 5, "ERAS");
    memcpy(command + 4, (char*) &address, 4);
    memcpy(command + 8, (char*) &length, 4);
    uart_write_bytes(RP2040_BL_UART, command, sizeof(command));
    uint8_t rx_buffer[4];
    read_stdin(rx_buffer, sizeof(rx_buffer), 10000);
    if (memcmp(rx_buffer, "OKOK", 4) != 0) return false;
    return true;
}

bool rp2040_bl_crc(uint32_t address, uint32_t length, uint32_t* crc) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[12];
    snprintf(command, 5, "CRCC");
    memcpy(command + 4, (char*) &address, 4);
    memcpy(command + 8, (char*) &length, 4);
    uart_write_bytes(RP2040_BL_UART, command, sizeof(command));
    uint8_t rx_buffer[8];
    read_stdin(rx_buffer, sizeof(rx_buffer), 10000);
    if (memcmp(rx_buffer, "OKOK", 4) != 0) return false;
    memcpy((uint8_t*) crc, &rx_buffer[4 * 1], 4);
    return true;
}

bool rp2040_bl_read(uint32_t address, uint32_t length, uint8_t* data) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[12];
    snprintf(command, 5, "READ");
    memcpy(command + 4, (char*) &address, 4);
    memcpy(command + 8, (char*) &length, 4);
    uart_write_bytes(RP2040_BL_UART, command, sizeof (command));
    uint8_t rx_buffer[4];
    read_stdin(rx_buffer, sizeof(rx_buffer), 10000);
    read_stdin(data, length, 10000);
    if (memcmp(rx_buffer, "OKOK", 4) != 0) return false;
    return true;
}

bool rp2040_bl_write(uint32_t address, uint32_t length, uint8_t* data, uint32_t* crc) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[12];
    snprintf(command, 5, "WRIT");
    memcpy(command + 4, (char*) &address, 4);
    memcpy(command + 8, (char*) &length, 4);
    uart_write_bytes(RP2040_BL_UART, command, sizeof(command));
    uart_write_bytes(RP2040_BL_UART, data, length);
    uint8_t rx_buffer[8];
    read_stdin(rx_buffer, sizeof(rx_buffer), 10000);
    if (memcmp(rx_buffer, "OKOK", 4) != 0) return false;
    memcpy((uint8_t*) crc, &rx_buffer[4 * 1], 4);
    return true;
}

bool rp2040_bl_seal(uint32_t vtor, uint32_t length, uint32_t crc) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[16];
    snprintf(command, 5, "SEAL");
    memcpy(command + 4, (char*) &vtor, 4);
    memcpy(command + 8, (char*) &length, 4);
    memcpy(command + 12, (char*) &crc, 4);
    uart_write_bytes(RP2040_BL_UART, command, sizeof(command));
    uint8_t rx_buffer[4];
    read_stdin(rx_buffer, sizeof(rx_buffer), 10000);
    if (memcmp(rx_buffer, "OKOK", 4) != 0) return false;
    return true;
}

bool rp2040_bl_go(uint32_t vtor) {
    if (!uart_is_driver_installed(RP2040_BL_UART)) return false;
    flush_stdin();
    char command[8];
    snprintf(command, 5, "GOGO");
    memcpy(command + 4, (char*) &vtor, 4);
    uart_write_bytes(RP2040_BL_UART, command, sizeof(command));
    return true;
}
