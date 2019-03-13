#if MICROPY_PY_BLUETOOTH

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatts_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "extmod/modbluetooth.h"
