#pragma once

#include "esp_gap_ble_api.h"

#define MP_BT_MAX_ATTR_SIZE (20)

typedef esp_ble_adv_type_t mp_bt_adv_type_t;

void mp_bt_init(void);
