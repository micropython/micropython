#pragma once

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>

void rp2040_bl_install_uart();
void rp2040_bl_uninstall_uart();

bool rp2040_bl_sync();
bool rp2040_bl_get_info(uint32_t* flash_start, uint32_t* flash_size, uint32_t* erase_size, uint32_t* write_size, uint32_t* max_data_len);
bool rp2040_bl_erase(uint32_t address, uint32_t length);
bool rp2040_bl_crc(uint32_t address, uint32_t length, uint32_t* crc);
bool rp2040_bl_read(uint32_t address, uint32_t length, uint8_t* data);
bool rp2040_bl_write(uint32_t address, uint32_t length, uint8_t* data, uint32_t* crc);
bool rp2040_bl_seal(uint32_t vtor, uint32_t length, uint32_t crc);
bool rp2040_bl_go(uint32_t vtor);
