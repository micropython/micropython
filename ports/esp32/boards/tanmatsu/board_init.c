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
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sd_pwr_ctrl.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "sdmmc_cmd.h"
#include "bsp/device.h"
#include "bsp/power.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/mphal.h"

#include "usb/usb_host.h"
#include "usb/msc_host.h"

esp_err_t driver_badge_bsp_init(void);
esp_err_t driver_framebuffer_init();

static char const           TAG[] = "board_init";

static sd_pwr_ctrl_handle_t sd_pwr_handle = NULL;

sdmmc_card_t card;


static esp_err_t reset_sd_card(void) {
    if (sd_pwr_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
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

static sd_pwr_ctrl_handle_t initialize_sd_ldo(void) {
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;
    esp_err_t            res             = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
        return NULL;
    }
    // Don't set voltage here - let SDMMC driver set it via host.io_voltage (3.3V default)
    return pwr_ctrl_handle;
}

esp_err_t sd_init(void) {
    esp_err_t res;

    if (sd_pwr_handle == NULL) {
        ESP_LOGI(TAG, "Acquiring SD LDO power control handle");
        sd_pwr_handle = initialize_sd_ldo();
    }

    ESP_LOGI(TAG, "Initializing SD card (no filesystem mount)");

    // Power cycle the SD card to ensure it's in a known state
    reset_sd_card();
    sdmmc_host_t host    = SDMMC_HOST_DEFAULT();
    host.slot            = SDMMC_HOST_SLOT_0;     // Use SLOT0 for native IOMUX pins
    host.max_freq_khz    = SDMMC_FREQ_HIGHSPEED;  // 40MHz
    host.pwr_ctrl_handle = sd_pwr_handle;

    // Allocate DMA buffer in internal RAM
    static DRAM_DMA_ALIGNED_ATTR uint8_t dma_buf[512 * 4];
    host.dma_aligned_buffer = dma_buf;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk   = GPIO_NUM_43;
    slot_config.cmd   = GPIO_NUM_44;
    slot_config.d0    = GPIO_NUM_39;
    slot_config.d1    = GPIO_NUM_40;
    slot_config.d2    = GPIO_NUM_41;
    slot_config.d3    = GPIO_NUM_42;
    slot_config.width = 4;

    ESP_LOGI(TAG, "Initializing SDMMC host and card");

    // Initialize host
    res = sdmmc_host_init();
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_host_init failed: %s", esp_err_to_name(res));
        return res;
    }

    // Initialize slot
    res = sdmmc_host_init_slot(host.slot, &slot_config);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_host_init_slot failed: %s", esp_err_to_name(res));
        return res;
    }
	
    // Initialize card (but do NOT mount filesystem)
    res = sdmmc_card_init(&host, &card);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_card_init failed: %s", esp_err_to_name(res));
        return res;
    }

    ESP_LOGI(TAG, "SD card initialized (no filesystem mounted)");
    sdmmc_card_print_info(stdout, &card);
    return ESP_OK;
}

void driver_badgebsp_init();

void tanmatsu_board_startup(void) {
	// Initialize the Board Support Package
    const bsp_configuration_t bsp_configuration = {
        .display =
            {
                .requested_color_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
                .num_fbs                = 1,
            },
    };
	sd_init();
	nvs_flash_init();
    if (bsp_device_initialize(&bsp_configuration) == ESP_OK) {
		if (driver_badge_bsp_init() == ESP_OK) {
			bsp_power_set_radio_state(BSP_POWER_RADIO_STATE_APPLICATION);
			driver_badgebsp_init();
			driver_framebuffer_init();
		}
	}
}

#ifdef XX
typedef struct _usbmsc_obj_t {
    mp_obj_base_t base;
    msc_host_device_handle_t dev;
    uint32_t block_size;
    uint32_t block_count;
    bool ready;
} usbmsc_obj_t;

usbmsc_obj_t usbmsc_singleton;
mp_obj_t usbmsc_py_callback = mp_const_none;
//extern mp_obj_t usbmsc_py_callback;

static void msc_event_cb(const msc_host_event_t *event, void *arg) {
    switch (event->event) {
        case MSC_DEVICE_CONNECTED: {
			ESP_LOGI(TAG, "USB connected");
			usbmsc_singleton.dev = event->device.address;
            msc_host_get_sector_size(dev, &usbmsc_singleton.block_size);
            msc_host_get_sector_count(dev, &usbmsc_singleton.block_count);
			usbmsc_singleton.ready = true;
            if (usbmsc_py_callback != mp_const_none) {
                mp_sched_schedule(usbmsc_py_callback, MP_OBJ_NEW_QSTR(MP_QSTR_connected));
            }
            break;
        }

        case MSC_DEVICE_DISCONNECTED: {
			ESP_LOGI(TAG, "USB disconnected");
			usbmsc_singleton.dev = NULL;
			usbmsc_singleton.block_size = 0;
			usbmsc_singleton.block_count = 0;
			usbmsc_singleton.ready = false;

            if (usbmsc_py_callback != mp_const_none) {
                mp_sched_schedule(usbmsc_py_callback, MP_OBJ_NEW_QSTR(MP_QSTR_disconnected));
            }
            break;
        }
    }
}

void usb_start(void)
{
    // 1. Install USB host
    usb_host_config_t host_cfg = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LOWMED,
    };
    usb_host_install(&host_cfg);

    // 2. Install MSC host with your event handler
    msc_host_driver_config_t msc_cfg = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .callback = my_msc_event_handler,
        .callback_arg = NULL,
    };
    msc_host_install(&msc_cfg);
}
#endif