
#include "py/runtime.h"
#include "usbd.h"

STATIC const mp_rom_map_elem_t usb_device_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_device) },

    { MP_ROM_QSTR(MP_QSTR_Descriptor), MP_ROM_PTR(&usbd_descriptor_type) },
};
STATIC MP_DEFINE_CONST_DICT(usb_device_module_globals, usb_device_module_globals_table);

const mp_obj_module_t mp_module_usb_device = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_device_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_device, mp_module_usb_device);
