#include <sdkconfig.h>
#include <esp_log.h>
#include <driver/i2s_std.h>
#include "driver_i2s.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#ifdef CONFIG_DRIVER_SNDMIXER_DEBUG
int min_val = 0, max_val = 0;
#endif

struct Config {
    uint8_t volume;
} config;

static int soundRunning = 0;

#ifdef CONFIG_DRIVER_SNDMIXER_I2S_PORT1
static const i2s_port_t g_i2s_port = I2S_NUM_1;
#else
static const i2s_port_t g_i2s_port = I2S_NUM_0;
#endif

static i2s_chan_handle_t tx_chan = NULL;

void driver_i2s_sound_start(void) {
	if (soundRunning) return;
	
    config.volume = 255;

    int rate     = CONFIG_DRIVER_SNDMIXER_SAMPLE_RATE;

    // 1) Create TX channel
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(g_i2s_port, I2S_ROLE_MASTER);

    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_chan, NULL));

    // 2) Configure standard I2S mode (Philips)
    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(rate),
        .slot_cfg = {
            .data_bit_width  = CONFIG_DRIVER_SNDMIXER_BITS_PER_SAMPLE,
            .slot_bit_width  = CONFIG_DRIVER_SNDMIXER_BITS_PER_SAMPLE,
#if defined(CONFIG_DRIVER_SNDMIXER_I2S_CHANNEL_FORMAT_OR)
            .slot_mode       = I2S_SLOT_MODE_MONO,
#elif defined(CONFIG_DRIVER_SNDMIXER_I2S_CHANNEL_FORMAT_OL)
            .slot_mode       = I2S_SLOT_MODE_MONO,
#elif defined(CONFIG_DRIVER_SNDMIXER_I2S_CHANNEL_FORMAT_AL)
            .slot_mode       = I2S_SLOT_MODE_STEREO,
#elif defined(CONFIG_DRIVER_SNDMIXER_I2S_CHANNEL_FORMAT_AR)
            .slot_mode       = I2S_SLOT_MODE_STEREO,
#else
            .slot_mode       = I2S_SLOT_MODE_STEREO,
#endif
            .slot_mask       = I2S_STD_SLOT_BOTH,
            .ws_width        = CONFIG_DRIVER_SNDMIXER_BITS_PER_SAMPLE,
            .ws_pol          = false,
            .bit_shift       = true,
            .msb_right       = true,
        },
        .gpio_cfg = {
            .mclk = CONFIG_DRIVER_SNDMIXER_PIN_MCK,
            .bclk = CONFIG_DRIVER_SNDMIXER_PIN_BCK,
            .ws   = CONFIG_DRIVER_SNDMIXER_PIN_WS,
            .dout = CONFIG_DRIVER_SNDMIXER_PIN_DATA_OUT,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));

    soundRunning = 1;
}

void driver_i2s_sound_stop(void) {
	if (!soundRunning) return;
    if (tx_chan) {
        i2s_channel_disable(tx_chan);
        i2s_del_channel(tx_chan);
        tx_chan = NULL;
    }
    soundRunning = 0;
}

#define SND_CHUNKSZ 32
void driver_i2s_sound_push(int16_t *buf, int len, int stereo_input) {
    if (!tx_chan || !soundRunning) {
        return;
    }

    int16_t tmpb[SND_CHUNKSZ * 2];
    int i = 0;
    while (i < len) {
        int plen = len - i;
        if (plen > SND_CHUNKSZ) {
            plen = SND_CHUNKSZ;
        }
        for (int sample = 0; sample < plen; sample++) {
            int32_t s[2] = {0, 0};
            if (stereo_input) {
                s[0] = buf[(i + sample) * 2 + 0];
                s[1] = buf[(i + sample) * 2 + 1];
            } else {
                s[0] = s[1] = buf[i + sample];
            }

            s[0] = (s[0] * config.volume) / 255;
            s[1] = (s[1] * config.volume) / 255;

#ifdef CONFIG_DRIVER_SNDMIXER_I2S_DATA_FORMAT_UNSIGNED
            s[0] -= INT16_MIN;
            s[1] -= INT16_MIN;
#endif
            tmpb[(sample * 2) + 0] = (int16_t)s[0];
            tmpb[(sample * 2) + 1] = (int16_t)s[1];

#ifdef CONFIG_DRIVER_SNDMIXER_DEBUG
            min_val = s[0] < min_val ? s[0] : min_val;
            max_val = s[0] > max_val ? s[0] : max_val;
            if (i == 0 && sample == 0 && rand() < (RAND_MAX / 100)) {
                ESP_LOGW("Sndmixer", "[global vol %d]: min %d max %d", config.volume, min_val, max_val);
            }
#endif
        }

        size_t bytes_written = 0;
        ESP_ERROR_CHECK(i2s_channel_write(tx_chan,
                                          tmpb,
                                          plen * 2 * sizeof(tmpb[0]),
                                          &bytes_written,
                                          portMAX_DELAY));
        i += plen;
    }
}

void driver_i2s_set_volume(uint8_t new_volume) {
    config.volume = new_volume;
}

uint8_t driver_i2s_get_volume(void) {
    return config.volume;
}

void driver_i2s_sound_mute(int doMute) {
    if (!tx_chan) {
        return;
    }
    if (doMute) {
        i2s_channel_disable(tx_chan);
    } else {
        i2s_channel_enable(tx_chan);
    }
}

#endif
