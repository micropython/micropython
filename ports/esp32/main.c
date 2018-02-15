#include "esp32task.h"

void app_main(void) {
	nvs_flash_init();
	micropython_task(0);
}

