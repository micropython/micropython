#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sd_pwr_ctrl.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "sdmmc_cmd.h"
#include "bsp/device.h"

esp_err_t driver_badge_bsp_init(void);
esp_err_t driver_framebuffer_init();

static char const           TAG[] = "sdcard";

static esp_err_t reset_sd_card(void) {
	ESP_LOGI(TAG, "Acquiring SD LDO power control handle");
    sd_pwr_ctrl_handle_t sd_pwr_handle = NULL;

    sd_pwr_ctrl_ldo_config_t cfg = {
        .ldo_chan_id = 4,   // LDO_VO4 powers SDMMC IO
    };

    // 1. Create the power-control driver
    ESP_ERROR_CHECK(sd_pwr_ctrl_new_on_chip_ldo(&cfg, &sd_pwr_handle));
	
	ESP_LOGI(TAG, "Power cycling SD card...");

    // Pull all SDIO bus lines low
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(GPIO_NUM_39) | BIT64(GPIO_NUM_40) | BIT64(GPIO_NUM_41) | BIT64(GPIO_NUM_42) |
                        BIT64(GPIO_NUM_43) | BIT64(GPIO_NUM_44),
        .mode         = GPIO_MODE_OUTPUT_OD,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_cfg);
    gpio_set_level(GPIO_NUM_39, 0);
    gpio_set_level(GPIO_NUM_40, 0);
    gpio_set_level(GPIO_NUM_41, 0);
    gpio_set_level(GPIO_NUM_42, 0);
    gpio_set_level(GPIO_NUM_43, 0);
    gpio_set_level(GPIO_NUM_44, 0);

    // Decrease LDO output voltage to minimum
    sd_pwr_ctrl_set_io_voltage(sd_pwr_handle, 0);
    vTaskDelay(pdMS_TO_TICKS(150));  // Wait 150ms for card to power down

    // Power on the SD card at 3.3V & release GPIOs
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_config(&gpio_cfg);
    sd_pwr_ctrl_set_io_voltage(sd_pwr_handle, 3300);
    vTaskDelay(pdMS_TO_TICKS(150));  // Wait 150ms for card to stabilize
    return ESP_OK;
}

void tanmatsu_board_startup(void) {
	// Initialize the Board Support Package
    const bsp_configuration_t bsp_configuration = {
        .display =
            {
                .requested_color_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
                .num_fbs                = 1,
            },
    };
	reset_sd_card();
    if (bsp_device_initialize(&bsp_configuration) == ESP_OK) {
		if (driver_badge_bsp_init() == ESP_OK) {
			driver_framebuffer_init();
		}
	}
}
