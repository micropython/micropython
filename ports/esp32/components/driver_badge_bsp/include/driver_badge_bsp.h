#ifndef DRIVER_BADGE_BSP_H
#define DRIVER_BADGE_BSP_H

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>
#include "bsp/device.h"
#include "bsp/display.h"
#include "bsp/input.h"

__BEGIN_DECLS

extern esp_err_t driver_badge_bsp_init(void);
extern size_t driver_badge_bsp_get_width(void);
extern size_t driver_badge_bsp_get_height(void);
extern size_t driver_badge_bsp_get_size(void);
extern esp_err_t driver_badge_bsp_set_backlight_brightness(uint8_t percentage);

extern esp_err_t driver_badge_bsp_flush(const uint8_t *buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

__END_DECLS

#endif // DRIVER_BADGE_BSP_H
