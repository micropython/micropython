
#include "py/runtime.h"
#include "usbd.h"
#include "tusb_config.h"

// tusb.h is not available when running the string preprocessor
#ifndef NO_QSTR
#include "tusb.h"
#endif

void usbd_reset_all(void) {
    usbd_reset_descriptor();
    usbd_reset_hid();
}

STATIC mp_obj_t usbd_ready(void) {
    return mp_obj_new_bool(tud_ready());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(usbd_ready_obj, usbd_ready);

STATIC mp_obj_t usbd_connected(void) {
    return mp_obj_new_bool(tud_connected());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(usbd_connected_obj, usbd_connected);

STATIC mp_obj_t usbd_remote_wakeup(void) {
    return mp_obj_new_bool(tud_remote_wakeup());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(usbd_remote_wakeup_obj, usbd_remote_wakeup);

STATIC const mp_rom_map_elem_t usb_device_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_device) },

    { MP_ROM_QSTR(MP_QSTR_ready), (mp_obj_t)&usbd_ready_obj},
    { MP_ROM_QSTR(MP_QSTR_connected), (mp_obj_t)&usbd_connected_obj},
    { MP_ROM_QSTR(MP_QSTR_remote_wakeup), (mp_obj_t)&usbd_remote_wakeup_obj},

    { MP_ROM_QSTR(MP_QSTR_Descriptor), MP_ROM_PTR(&usbd_descriptor_type) },
    #if MICROPY_HW_USB_HID
    { MP_ROM_QSTR(MP_QSTR_HID), MP_ROM_PTR(&usbd_hid_type) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(usb_device_module_globals, usb_device_module_globals_table);

const mp_obj_module_t mp_module_usb_device = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_device_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_device, mp_module_usb_device);
