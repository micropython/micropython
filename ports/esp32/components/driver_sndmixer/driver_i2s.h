#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
//#include "driver/i2s.h"
#include "driver/i2s_common.h"
#include "driver/i2s_etm.h"
#include "driver/i2s_pdm.h"
#include "driver/i2s_std.h"
#include "driver/i2s_tdm.h"
#include "driver/i2s_types.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
//#include "driver/adc.h"
//#include "driver/dac.h"
#include "soc/rtc_cntl_reg.h"

// Start audio output driver
void driver_i2s_sound_start();

// Stop audio output driver
void driver_i2s_sound_stop();

// Push audio to the driver
void driver_i2s_sound_push(int16_t *buf, int len, int stereo);

// Set the volume (0-255)
void driver_i2s_set_volume(uint8_t new_volume);

// Get the volume
uint8_t driver_i2s_get_volume();

// Mute audio output
void driver_i2s_sound_mute(int doMute);
