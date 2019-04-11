#pragma once

#include "esp_gap_ble_api.h"

#define MP_BT_MAX_ATTR_SIZE (20)

typedef esp_bt_uuid_t mp_bt_uuid_t;
typedef uint16_t mp_bt_service_handle_t;
typedef uint16_t mp_bt_characteristic_handle_t;
typedef esp_ble_adv_type_t mp_bt_adv_type_t;

void mp_bt_init(void);
