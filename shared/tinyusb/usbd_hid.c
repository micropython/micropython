
#include "py/runtime.h"
#include "usbd.h"

// tusb.h is not available when running the string preprocessor
#ifndef NO_QSTR
#include "tusb.h"
#endif

#if MICROPY_HW_USB_HID

typedef struct {
    uint8_t descriptors[MICROPY_HW_USB_MAX_HID][255];
    int len;
} usbd_desc_hid_t;
usbd_desc_hid_t usbd_desc_hid;

void usbd_reset_hid(void) {
    usbd_desc_hid.len = 0;
}

static int usbd_desc_hid_add(usbd_desc_hid_t *hid, char *name, uint8_t protocol, uint8_t ep_bufsize, uint8_t interval, uint8_t *desc, uint8_t desc_len) {
    uint8_t *spot;

    // trim the endpoint buffer size if too big
    if (ep_bufsize > CFG_TUD_HID_EP_BUFSIZE) {
        ep_bufsize = CFG_TUD_HID_EP_BUFSIZE;
    }

    uint8_t descriptor[] = {
        TUD_HID_DESCRIPTOR(0, 0, protocol, desc_len, 0x80, ep_bufsize, interval),
    };
    // check for hid slots
    if (hid->len > MICROPY_HW_USB_MAX_HID) {
        return -1;
    }
    // check for description too big
    if (desc_len > 255) {
        return -1;
    }

    // try to save the usb descriptor
    if (usbd_desc_add_descriptor(descriptor, sizeof(descriptor), name)) {
        return -1;
    }

    // save the hid descriptor
    spot = hid->descriptors[hid->len];
    memcpy(spot, desc, desc_len);
    hid->len++;
    return 0;
}

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    if (instance >= usbd_desc_hid.len) {
        return NULL;
    }
    return usbd_desc_hid.descriptors[instance];
}

void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
    (void) instance;
    (void) protocol;
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
    (void) instance;
    (void) report;
    (void) len;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}

typedef struct _usb_device_hid_obj_t {
    mp_obj_base_t base;
} usb_device_hid_obj_t;

// singleton
STATIC const usb_device_hid_obj_t usb_device_hid_obj = {{&usbd_hid_type}};

STATIC mp_obj_t usb_device_hid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    return (mp_obj_t)&usb_device_hid_obj;
}

// HID.add_descriptor(name, protocol, ep_bufsize, interval, descriptor)
STATIC mp_obj_t usbd_hid_add_descriptor(size_t n_args, const mp_obj_t *args) {
    (void) n_args;
    mp_buffer_info_t bufinfo = {0};
    char *name;
    uint8_t protocol;
    uint8_t ep_bufsize;
    uint8_t interval;
    int err;

    mp_check_self(mp_obj_is_str_or_bytes(args[1]));
    name = (char*) mp_obj_str_get_str(args[1]);
    protocol = mp_obj_get_int(args[2]);
    ep_bufsize = mp_obj_get_int(args[3]);
    interval = mp_obj_get_int(args[4]);
    mp_get_buffer_raise(args[5], &bufinfo, MP_BUFFER_READ);
    err = usbd_desc_hid_add(&usbd_desc_hid, name, protocol, ep_bufsize, interval, bufinfo.buf, bufinfo.len);
    if (err) {
        mp_raise_ValueError(MP_ERROR_TEXT("could not add descriptor"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usbd_hid_add_descriptor_obj, 6, 6, usbd_hid_add_descriptor);

// HID.report(instance, report_id, buffer)
STATIC mp_obj_t usbd_hid_report(size_t n_args, const mp_obj_t *args) {
    (void) n_args;
    mp_buffer_info_t bufinfo = {0};
    uint8_t report_id;
    uint8_t instance;

    instance = mp_obj_get_int(args[1]);
    report_id = mp_obj_get_int(args[2]);
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);

    // run the task once if not ready, then just exit
    if (!tud_hid_n_ready(instance)) {
        tud_task();
        if (!tud_hid_n_ready(instance)) {
            return mp_obj_new_bool(false);
        }
    }

    return mp_obj_new_bool(tud_hid_n_report(instance, report_id, bufinfo.buf, bufinfo.len));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usbd_hid_report_obj, 4, 4, usbd_hid_report);

// HID.ready(instance)
STATIC mp_obj_t usbd_hid_ready(mp_obj_t obj0, mp_obj_t obj1) {
    (void) obj0;
    int instance;

    instance = mp_obj_get_int(obj1);
    return mp_obj_new_bool(tud_hid_n_ready(instance));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(usbd_hid_ready_obj, usbd_hid_ready);

STATIC const mp_rom_map_elem_t usb_device_hid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_add_descriptor), (mp_obj_t)&usbd_hid_add_descriptor_obj},
    { MP_ROM_QSTR(MP_QSTR_report), (mp_obj_t)&usbd_hid_report_obj},
    { MP_ROM_QSTR(MP_QSTR_ready), (mp_obj_t)&usbd_hid_ready_obj},
};
STATIC MP_DEFINE_CONST_DICT(usb_device_hid_locals_dict, usb_device_hid_locals_dict_table);

const mp_obj_type_t usbd_hid_type = {
    { &mp_type_type },
    .name = MP_QSTR_HID,
    .make_new = usb_device_hid_make_new,
    .locals_dict = (mp_obj_t)&usb_device_hid_locals_dict,
};

#else // #if MICROPY_HW_USB_HID

void usbd_reset_hid(void) {

}

#endif