
#ifndef MICROPY_INCLUDED_USBD_SHARED_H_
#define MICROPY_INCLUDED_USBD_SHARED_H_

#include "py/obj.h"

// defined externally (needed per port)

int usbd_serialnumber(uint8_t *buf);

// external use

void usbd_reset_all(void);
void usbd_reset_descriptor(void);
void usbd_reset_hid(void);

// internal

extern const mp_obj_type_t usbd_descriptor_type;
extern const mp_obj_type_t usbd_hid_type;

int usbd_desc_add_descriptor(uint8_t *desc, int len, char *name);

#endif
