#pragma once

#include <stdbool.h>
//#include "bluetooth/bluetooth.h"
#include "py/obj.h"

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
