#ifndef DRIVER_ILI9341_H
#define DRIVER_ILI9341_H

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240

#ifdef CONFIG_DRIVER_ILI9341_8C
	#define ILI9341_BUFFER_SIZE ILI9341_WIDTH * ILI9341_HEIGHT
#else
	#define ILI9341_BUFFER_SIZE ILI9341_WIDTH * ILI9341_HEIGHT * 2
#endif

__BEGIN_DECLS

extern esp_err_t driver_ili9341_init(void);
extern esp_err_t driver_ili9341_set_backlight(bool state);
extern esp_err_t driver_ili9341_write_partial(const uint8_t *buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern esp_err_t driver_ili9341_set_mode(bool mode);

__END_DECLS

#endif // DRIVER_ILI9341_H
