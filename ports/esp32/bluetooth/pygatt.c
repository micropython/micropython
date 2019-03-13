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

// this is the actual C-structure for our new object
typedef struct _pygatt_GATTToolBackend_obj_t {
    // base represents some basic information, like type
    mp_obj_base_t base;
    // a member created by us
    uint8_t hello_number;
} pygatt_GATTToolBackend_obj_t;

STATIC const mp_map_elem_t pygatt_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pygatt) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GATTToolBackend), (mp_obj_t)&pygatt_GATTToolBackend_obj },
};
STATIC MP_DEFINE_CONST_DICT(pygatt_globals, pygatt_globals_table);

#endif // MICROPY_PY_BLUETOOTH
