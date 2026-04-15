#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_err.h>
#include <esp_log.h>

#include "include/driver_badge_bsp.h"

#define TAG "badge_bsp"

static size_t                       h_res;
static size_t                       v_res;
static lcd_color_rgb_pixel_format_t color_fmt;
static lcd_rgb_data_endian_t        data_endian;

esp_err_t driver_badge_bsp_init(void)
{
	esp_err_t err = bsp_display_get_parameters(&h_res, &v_res, &color_fmt, &data_endian);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "h_res: %d, v_res: %d, color_fmt: %d, data_endian: %d", h_res, v_res, color_fmt, data_endian);
	} else {
		ESP_LOGI(TAG, "error: %d", err);
	}
	return err;
}

size_t driver_badge_bsp_get_width(void)
{
	return h_res;
}

size_t driver_badge_bsp_get_height(void)
{
	return v_res;
}

size_t driver_badge_bsp_get_size(void)
{
	size_t size = h_res * v_res * 2;
	return size;
}

esp_err_t driver_badge_bsp_set_backlight_brightness(uint8_t percentage)
{
	return bsp_display_set_backlight_brightness(percentage);
}

esp_err_t driver_badge_bsp_flush(const uint8_t *buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	return bsp_display_blit(x0, y0, x1 - x0 + 1, y1 - y0 + 1, buffer);
}
