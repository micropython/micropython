#pragma once

#include <stdbool.h>
#include "bluetooth/bluetooth.h"
#include "py/obj.h"
#include "esp_bt_defs.h"

// Enables the Bluetooth stack. Returns errno on failure.
int mp_bt_enable(void);

// Disables the Bluetooth stack. Is a no-op when not enabled.
int mp_bt_disable(void);

// Returns true when the Bluetooth stack is enabled.
bool mp_bt_is_enabled(void);

// Enables the Bluetooth scan. Returns errno on failure.
int mp_bt_scan(void);

// Try to connect to address. Returns errno on failure.
int mp_bt_connect(esp_bd_addr_t device);

// Returns true when the Bluetooth is connected.
bool mp_bt_is_connected(void);

// Try to disconnect from remote device. Returns errno on failure.
int mp_bt_disconnect(esp_bd_addr_t device);

// Discover BT characteristics and display them. Returns errno on failure.
int mp_bt_discover_characteristics(void);

// Write to characteristic. Returns errno on failure.
int mp_bt_char_write_handle(uint16_t handle, uint8_t* value, uint8_t length, bool wait_for_response);

// Read char from characteristic and return bytearray. Returns errno on failure.
int mp_bt_char_read(uint16_t value_handle, void *value, size_t *value_len);

// Read char from characteristic by handle and return bytearray. Returns errno on failure.
int mp_bt_char_read_handle(uint16_t value_handle, void *value, size_t *value_len);

#define GATTC_TAG "APP"
