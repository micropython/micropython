#include "bsp/device.h"

esp_err_t driver_badge_bsp_init(void);
esp_err_t driver_framebuffer_init();

void tanmatsu_board_startup(void) {
	// Initialize the Board Support Package
    const bsp_configuration_t bsp_configuration = {
        .display =
            {
                .requested_color_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
                .num_fbs                = 1,
            },
    };
    if (bsp_device_initialize(&bsp_configuration) == ESP_OK) {
		if (driver_badge_bsp_init() == ESP_OK) {
			driver_framebuffer_init();
		}
	}
}
