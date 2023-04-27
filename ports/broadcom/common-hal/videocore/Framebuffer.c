
#include "bindings/videocore/Framebuffer.h"

#include "py/gc.h"
#include "py/runtime.h"

#include "peripherals/broadcom/caches.h"
#include "peripherals/broadcom/vcmailbox.h"

void common_hal_videocore_framebuffer_construct(videocore_framebuffer_obj_t *self,
    mp_uint_t width, mp_uint_t height) {
    // These will be modified on success to indicate actual values.
    uint32_t virtual_width = width;
    uint32_t virtual_height = height;
    uint32_t physical_width = width;
    // TODO: Make the FB twice as tall if double buffering.
    uint32_t physical_height = height;
    uint32_t pitch = 0;
    uint32_t bits_per_pixel = 0;
    self->framebuffer = vcmailbox_get_framebuffer(&virtual_width,
        &virtual_height,
        &physical_width,
        &physical_height,
        &pitch,
        &bits_per_pixel);
    if (self->framebuffer == NULL) {
        if (gc_alloc_possible()) {
            mp_raise_ValueError(translate("no fb"));
        } else {
            mp_printf(&mp_plat_print, "no fb\n");
        }
        self->width = 0;
        self->height = 0;
        return;
    }
    self->width = virtual_width;
    self->height = virtual_height;
    self->pitch = pitch;
}

void common_hal_videocore_framebuffer_deinit(videocore_framebuffer_obj_t *self) {
    if (vcmailbox_release_framebuffer()) {
        self->framebuffer = NULL;
    }
    self->base.type = &mp_type_NoneType;
}

bool common_hal_videocore_framebuffer_deinited(videocore_framebuffer_obj_t *self) {
    return self->framebuffer == NULL;
}

void common_hal_videocore_framebuffer_refresh(videocore_framebuffer_obj_t *self) {
    data_clean(self->framebuffer, sizeof(uint32_t) * self->width * self->height);
}

int common_hal_videocore_framebuffer_get_width(videocore_framebuffer_obj_t *self) {
    return self->width;
}

int common_hal_videocore_framebuffer_get_height(videocore_framebuffer_obj_t *self) {
    return self->height;
}

mp_int_t common_hal_videocore_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    videocore_framebuffer_obj_t *self = (videocore_framebuffer_obj_t *)self_in;
    bufinfo->buf = self->framebuffer;
    bufinfo->typecode = 'L';
    bufinfo->len = self->pitch * self->height * sizeof(uint32_t);
    return 0;
}

int common_hal_videocore_framebuffer_get_row_stride(videocore_framebuffer_obj_t *self) {
    return self->pitch;
}
