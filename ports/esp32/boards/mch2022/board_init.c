#include "esp_log.h"
#include "nvs_flash.h"

esp_err_t start_buses();
void driver_mch22_init();
int sndmixer_init(int p_no_channels, int stereo);
esp_err_t driver_ili9341_init(void);
esp_err_t driver_framebuffer_init();

void mch2022_board_startup(void) {
	nvs_flash_init();
	start_buses();
	driver_mch22_init();
	driver_ili9341_init();
	driver_framebuffer_init();
	sndmixer_init(2, 1);
}
