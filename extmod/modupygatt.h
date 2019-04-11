#pragma once

#include <stdbool.h>
#include "bluetooth/bluetooth.h"
#include "py/obj.h"
#include "esp_bt_defs.h"

typedef struct {
    mp_obj_base_t          base;
    mp_bt_uuid_t           uuid;
    mp_bt_service_handle_t handle;
    #if ESP_PLATFORM
    uint8_t                esp_gatts_if; /* Only for ESP-IDF */
    #endif
} mp_bt_service_t;

// A characteristic.
// Object fits in 4 words (1 GC object), with 1 byte unused at the end.
typedef struct {
    mp_obj_base_t                 base;
    mp_bt_uuid_t                  uuid;
    mp_bt_service_t               *service;
    mp_bt_characteristic_handle_t value_handle;
    uint8_t                       flags;
} mp_bt_characteristic_t;

// Parse an UUID object from the caller and stores the result in the uuid
// parameter. Must accept both strings and integers for 128-bit and 16-bit
// UUIDs.
void mp_bt_parse_uuid(mp_obj_t obj, mp_bt_uuid_t *uuid);

// Parse a string UUID object into the 16-byte buffer. The string must be
// the correct size, otherwise this function will throw an error.
void mp_bt_parse_uuid_str(mp_obj_t obj, uint8_t *uuid);

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

// Try to disconnect from remote device. Returns errno on failure.
int mp_bt_disconnect(esp_bd_addr_t device);

// Discover BT characteristics and display them. Returns errno on failure.
int mp_bt_discover_characteristics(void);

// Write to characteristic. Returns errno on failure.
int mp_bt_char_write_handle(uint16_t handle, uint8_t* value, uint8_t length, bool wait_for_response);

// Read char from characteristic and return bytearray. Returns errno on failure.
int mp_bt_char_read(mp_bt_characteristic_t *characteristic, void *value, size_t *value_len);

// Data types of advertisement packet.
#define MP_BLE_GAP_AD_TYPE_FLAG                  (0x01)
#define MP_BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME   (0x09)

// Flags element of advertisement packet.
#define MP_BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE         (0x02)  // discoverable for everyone
#define MP_BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED         (0x04)  // BLE only - no classic BT supported
#define MP_BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE   (MP_BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE | MP_BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED)

#define MP_BLE_FLAG_READ     (1 << 1)
#define MP_BLE_FLAG_WRITE    (1 << 3)
#define MP_BLE_FLAG_NOTIFY   (1 << 4)
