
#ifndef MICROPY_INCLUDED_USBD_SHARED_H_
#define MICROPY_INCLUDED_USBD_SHARED_H_

#include "py/obj.h"

void usbd_descriptor_reset(void);

extern const mp_obj_type_t usbd_descriptor_type;

#endif
