// machine_cont_adc.c : ESP32-C3 continuous ADC -> UDP for MicroPython v1.28 / ESP-IDF v5.5.1
//
// Packet format, little-endian, mono only, 8-byte header:
//   uint16 seq             = packet sequence number, wraps at 65535
//   uint8  codec           = 0: PCM16, 1: IMA-style ADPCM4
//   uint8  step_index      = initial ADPCM step index; 0 for PCM16
//   uint16 sample_rate_hz  = sender sample rate, max 65535 Hz
//   int16  predictor       = first PCM sample for ADPCM; reserved for PCM16
//   uint8 payload[]
//
// Decoded sample count is derived from UDP datagram length:
//   PCM16:  payload_bytes / 2
//   ADPCM4: 1 + payload_bytes * 2
//
// To make ADPCM4 self-delimiting without a sample_count field, every ADPCM
// packet carries an odd number of decoded samples. If a partial ADPCM packet
// would otherwise be even length, the last sample is duplicated once.
//
// MicroPython API as a standalone module:
//   import cont_adc
//   cont_adc.config_adc_udp_stream(channel[, atten[, iir_coeff[, unit]]])
//     iir_coeff: 0/None/omitted = disabled; otherwise one of 2, 4, 8, 16, 32, 64.
//     unit: omitted/None = ADC_UNIT_1; ADC_UNIT_2 is allowed if valid for the selected channel/target.
//   cont_adc.start_adc_udp_stream(sample_rate_hz, dest_ip, dest_port[, codec])
//   cont_adc.stop_adc_udp_stream()
//   cont_adc.stats_adc_udp_stream()
//
// Short aliases are also provided: config(), start(), stop(), stats().
// If you prefer exposing the functions under machine.*, add the *_obj symbols
// to ports/esp32/modmachine.c and disable/ignore MP_REGISTER_MODULE below.

#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>

#include "py/runtime.h"
#include "py/obj.h"
#include "py/objmodule.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_filter.h"
#include "soc/adc_channel.h"
#include "soc/soc_caps.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"

#define TAG "ADC_UDP_STREAM"

#ifndef SOC_ADC_DIGI_RESULT_BYTES
#define SOC_ADC_DIGI_RESULT_BYTES SOC_ADC_DIGI_DATA_BYTES_PER_CONV
#endif

/* ESP-IDF v5.5.x does not provide adc_continuous_parse_data()/adc_continuous_data_t
 * on ESP32-C3. adc_continuous_read() returns raw conversion results; the v5.5.1
 * official continuous_read example casts each SOC_ADC_DIGI_RESULT_BYTES entry to
 * adc_digi_output_data_t and uses type2 on targets other than ESP32/ESP32-S2.
 */
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define CONT_ADC_OUTPUT_TYPE         ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define CONT_ADC_GET_CHANNEL(p_data) ((p_data)->type1.channel)
#define CONT_ADC_GET_DATA(p_data)    ((p_data)->type1.data)
#else
#define CONT_ADC_OUTPUT_TYPE         ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define CONT_ADC_GET_CHANNEL(p_data) ((p_data)->type2.channel)
#define CONT_ADC_GET_DATA(p_data)    ((p_data)->type2.data)
#endif

/* ---------------- User-tunable compile-time parameters ---------------- */

#ifndef CONT_ADC_SAMPLES_PER_PACKET
#define CONT_ADC_SAMPLES_PER_PACKET 320u
#endif

#ifndef CONT_ADC_CONV_FRAME_SIZE
#define CONT_ADC_CONV_FRAME_SIZE 512u
#endif

#ifndef CONT_ADC_STORE_FRAMES
#define CONT_ADC_STORE_FRAMES 8u
#endif

#ifndef CONT_ADC_SEND_TIMEOUT_MS
#define CONT_ADC_SEND_TIMEOUT_MS 20u
#endif

#ifndef CONT_ADC_TASK_STACK_BYTES
#define CONT_ADC_TASK_STACK_BYTES 4096u
#endif

#if (CONT_ADC_CONV_FRAME_SIZE % SOC_ADC_DIGI_RESULT_BYTES) != 0
#error "CONT_ADC_CONV_FRAME_SIZE must be a multiple of SOC_ADC_DIGI_RESULT_BYTES"
#endif

#if CONT_ADC_SAMPLES_PER_PACKET < 2
#error "CONT_ADC_SAMPLES_PER_PACKET must be at least 2"
#endif

/* ---------------- Packet constants ---------------- */

#define AUDIO_CODEC_PCM16           0u
#define AUDIO_CODEC_ADPCM4          1u
#define AUDIO_PACKET_HEADER_BYTES   8u
#define AUDIO_SAMPLE_RATE_MAX       65535u
#define AUDIO_PACKET_MAX_BYTES      (AUDIO_PACKET_HEADER_BYTES + CONT_ADC_SAMPLES_PER_PACKET * 2u)

/* Return codes from start/stop, keeping the original integer style. */
#define CADC_RET_OK                 0
#define CADC_RET_ALREADY_RUNNING    1
#define CADC_RET_NOT_RUNNING        1
#define CADC_RET_BAD_ARG           -2
#define CADC_RET_UDP_FAIL          -3
#define CADC_RET_ADC_FAIL          -4
#define CADC_RET_TASK_FAIL         -5
#define CADC_RET_BAD_CODEC         -6
#define CADC_RET_STOP_TIMEOUT      -7

/* ---------------- Runtime state ---------------- */

static adc_channel_t s_adc_channel = ADC_CHANNEL_0;
static adc_atten_t s_adc_atten = ADC_ATTEN_DB_0;
static adc_unit_t s_adc_unit = ADC_UNIT_1;

static adc_continuous_handle_t s_adc_handle = NULL;
static adc_iir_filter_handle_t s_iir_filter = NULL;
static volatile bool s_adc_started = false;
static int s_udp_sock = -1;
static struct sockaddr_in s_dest_addr;

static TaskHandle_t s_stream_task_handle = NULL;
static SemaphoreHandle_t s_task_started_sem = NULL;
static SemaphoreHandle_t s_task_done_sem = NULL;
static volatile bool s_running = false;

static uint8_t *s_adc_buf = NULL;
static uint8_t *s_packet_buf = NULL;

static uint8_t s_codec_mode = AUDIO_CODEC_PCM16;
static uint16_t s_sample_rate_hz = 0;
static uint16_t s_packet_seq = 0;
static uint8_t s_adpcm_step_index = 20;
static esp_err_t s_task_start_error = ESP_OK;
static bool s_iir_filter_enabled = false;
static adc_digi_iir_filter_coeff_t s_iir_filter_coeff = ADC_DIGI_IIR_FILTER_COEFF_64;

static uint32_t s_packets_sent = 0;
static uint32_t s_send_errors = 0;
static uint32_t s_adc_overflows = 0;
static uint32_t s_adc_read_errors = 0;
static uint32_t s_bad_adc_samples = 0;
static esp_err_t s_last_error = ESP_OK;

/* ---------------- Little-endian helpers ---------------- */

static inline void put_u16le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xffu);
    p[1] = (uint8_t)((v >> 8) & 0xffu);
}

static inline void put_s16le(uint8_t *p, int16_t v) {
    put_u16le(p, (uint16_t)v);
}

static inline void write_audio_header(uint8_t *p,
                                      uint16_t seq,
                                      uint8_t codec,
                                      uint8_t step_index,
                                      uint16_t sample_rate_hz,
                                      int16_t predictor) {
    put_u16le(p + 0, seq);
    p[2] = codec;
    p[3] = step_index;
    put_u16le(p + 4, sample_rate_hz);
    put_s16le(p + 6, predictor);
}

static inline int16_t clip_i16(int32_t x) {
    if (x < -32768) {
        return -32768;
    }
    if (x > 32767) {
        return 32767;
    }
    return (int16_t)x;
}

/* Convert unsigned 12-bit ADC value to signed 16-bit PCM.
 * Assumes the analog front-end is biased near ADC midscale.
 */
static inline int16_t adc12_to_pcm16(uint32_t raw) {
    return clip_i16((((int32_t)(raw & 0x0fffu)) - 2048) << 4);
}

/* ---------------- IMA-style ADPCM encoder ---------------- */

static const int16_t ima_step_table[89] = {
      7,     8,     9,    10,    11,    12,    13,    14,
     16,    17,    19,    21,    23,    25,    28,    31,
     34,    37,    41,    45,    50,    55,    60,    66,
     73,    80,    88,    97,   107,   118,   130,   143,
    157,   173,   190,   209,   230,   253,   279,   307,
    337,   371,   408,   449,   494,   544,   598,   658,
    724,   796,   876,   963,  1060,  1166,  1282,  1411,
   1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
   3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
   7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
  15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
  32767
};

static const int8_t ima_index_table[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

static inline uint8_t ima_adpcm_encode_nibble(int16_t sample,
                                              int16_t *predictor,
                                              uint8_t *step_index) {
    int step = ima_step_table[*step_index];
    int diff = (int)sample - (int)(*predictor);
    uint8_t code = 0;

    if (diff < 0) {
        code = 8;
        diff = -diff;
    }

    int delta = step >> 3;
    if (diff >= step) {
        code |= 4;
        diff -= step;
        delta += step;
    }
    if (diff >= (step >> 1)) {
        code |= 2;
        diff -= (step >> 1);
        delta += (step >> 1);
    }
    if (diff >= (step >> 2)) {
        code |= 1;
        delta += (step >> 2);
    }

    if (code & 8) {
        *predictor = clip_i16((int32_t)(*predictor) - delta);
    } else {
        *predictor = clip_i16((int32_t)(*predictor) + delta);
    }

    int new_index = (int)(*step_index) + ima_index_table[code & 0x0f];
    if (new_index < 0) {
        new_index = 0;
    } else if (new_index > 88) {
        new_index = 88;
    }
    *step_index = (uint8_t)new_index;

    return code & 0x0f;
}

/* ---------------- Packet builder ---------------- */

typedef struct {
    uint8_t *buf;
    uint8_t codec;
    uint16_t sample_count;
    int16_t header_predictor;
    int16_t adpcm_predictor;
    int16_t last_pcm;
    uint8_t header_step_index;
} packet_builder_t;

static inline void packet_begin(packet_builder_t *pb, uint8_t *buf, uint8_t codec) {
    pb->buf = buf;
    pb->codec = codec;
    pb->sample_count = 0;
    pb->header_predictor = 0;
    pb->adpcm_predictor = 0;
    pb->last_pcm = 0;
    pb->header_step_index = s_adpcm_step_index;
}

static inline void packet_add_sample(packet_builder_t *pb, int16_t pcm) {
    uint16_t n = pb->sample_count;

    if (pb->codec == AUDIO_CODEC_ADPCM4) {
        if (n == 0) {
            pb->header_predictor = pcm;
            pb->adpcm_predictor = pcm;
            pb->last_pcm = pcm;
            pb->header_step_index = s_adpcm_step_index;
            pb->sample_count = 1;
            return;
        }

        uint8_t code = ima_adpcm_encode_nibble(pcm, &pb->adpcm_predictor, &s_adpcm_step_index);
        uint16_t code_index = (uint16_t)(n - 1u);
        uint8_t *dst = pb->buf + AUDIO_PACKET_HEADER_BYTES + (code_index >> 1);
        if ((code_index & 1u) == 0) {
            *dst = code;
        } else {
            *dst |= (uint8_t)(code << 4);
        }
    } else {
        if (n == 0) {
            pb->header_predictor = pcm;
        }
        put_s16le(pb->buf + AUDIO_PACKET_HEADER_BYTES + ((size_t)n * 2u), pcm);
    }

    pb->last_pcm = pcm;
    pb->sample_count = (uint16_t)(n + 1u);
}

static inline size_t packet_payload_len(const packet_builder_t *pb) {
    if (pb->codec == AUDIO_CODEC_ADPCM4) {
        // ADPCM packets are forced to odd decoded sample counts, so payload_bytes = (samples - 1) / 2.
        return (size_t)(pb->sample_count / 2u);
    }
    return (size_t)pb->sample_count * 2u;
}

static inline uint16_t packet_target_samples(uint8_t codec) {
    if (codec == AUDIO_CODEC_ADPCM4 && (CONT_ADC_SAMPLES_PER_PACKET & 1u) == 0) {
        // One extra ADPCM sample still uses much less space than a PCM packet and keeps length self-delimiting.
        return (uint16_t)(CONT_ADC_SAMPLES_PER_PACKET + 1u);
    }
    return (uint16_t)CONT_ADC_SAMPLES_PER_PACKET;
}

static inline void packet_make_length_decodable(packet_builder_t *pb) {
    if (pb->codec == AUDIO_CODEC_ADPCM4 && pb->sample_count != 0 && (pb->sample_count & 1u) == 0) {
        // Without sample_count in the header, ADPCM nibble length is ambiguous for even sample counts.
        // Duplicate the final sample once so receiver can derive samples = 1 + payload_bytes * 2.
        packet_add_sample(pb, pb->last_pcm);
    }
}

static void udp_send_buf(const uint8_t *buf, size_t len) {
    if (s_udp_sock < 0) {
        s_send_errors++;
        return;
    }

    ssize_t sent = sendto(s_udp_sock,
                          buf,
                          len,
                          0,
                          (const struct sockaddr *)&s_dest_addr,
                          sizeof(s_dest_addr));
    if (sent == (ssize_t)len) {
        s_packets_sent++;
    } else {
        s_send_errors++;
        if (sent < 0) {
            ESP_LOGW(TAG, "sendto failed: errno %d", errno);
        } else {
            ESP_LOGW(TAG, "sendto short write: %d/%u", (int)sent, (unsigned)len);
        }
    }
}

static inline void packet_finish_and_send(packet_builder_t *pb) {
    if (pb->sample_count == 0) {
        return;
    }

    packet_make_length_decodable(pb);
    size_t payload_len = packet_payload_len(pb);
    write_audio_header(pb->buf,
                       s_packet_seq,
                       pb->codec,
                       (pb->codec == AUDIO_CODEC_ADPCM4) ? pb->header_step_index : 0,
                       s_sample_rate_hz,
                       pb->header_predictor);
    udp_send_buf(pb->buf, AUDIO_PACKET_HEADER_BYTES + payload_len);
    s_packet_seq++;

    packet_begin(pb, pb->buf, pb->codec);
}

/* ---------------- ADC and UDP setup ---------------- */

static bool is_valid_atten(int atten) {
    return atten == ADC_ATTEN_DB_0
        || atten == ADC_ATTEN_DB_2_5
        || atten == ADC_ATTEN_DB_6
        || atten == ADC_ATTEN_DB_12;
}

static bool is_valid_adc_unit_int(int unit) {
    return unit == ADC_UNIT_1 || unit == ADC_UNIT_2;
}

static bool is_valid_adc_channel_for_unit(adc_unit_t unit, int channel) {
    if (channel < 0) {
        return false;
    }
    int io_num = -1;
    return adc_continuous_channel_to_io(unit, (adc_channel_t)channel, &io_num) == ESP_OK;
}


static bool iir_coeff_from_int(int coeff, adc_digi_iir_filter_coeff_t *out_coeff) {
    switch (coeff) {
        case 2:
            *out_coeff = ADC_DIGI_IIR_FILTER_COEFF_2;
            return true;
        case 4:
            *out_coeff = ADC_DIGI_IIR_FILTER_COEFF_4;
            return true;
        case 8:
            *out_coeff = ADC_DIGI_IIR_FILTER_COEFF_8;
            return true;
        case 16:
            *out_coeff = ADC_DIGI_IIR_FILTER_COEFF_16;
            return true;
        case 32:
            *out_coeff = ADC_DIGI_IIR_FILTER_COEFF_32;
            return true;
        case 64:
            *out_coeff = ADC_DIGI_IIR_FILTER_COEFF_64;
            return true;
        default:
            return false;
    }
}

static int iir_coeff_to_int(adc_digi_iir_filter_coeff_t coeff) {
    switch (coeff) {
        case ADC_DIGI_IIR_FILTER_COEFF_2:
            return 2;
        case ADC_DIGI_IIR_FILTER_COEFF_4:
            return 4;
        case ADC_DIGI_IIR_FILTER_COEFF_8:
            return 8;
        case ADC_DIGI_IIR_FILTER_COEFF_16:
            return 16;
        case ADC_DIGI_IIR_FILTER_COEFF_32:
            return 32;
        case ADC_DIGI_IIR_FILTER_COEFF_64:
            return 64;
        default:
            return 0;
    }
}

static void cleanup_iir_filter(void) {
    if (s_iir_filter != NULL) {
        (void)adc_continuous_iir_filter_disable(s_iir_filter);
        (void)adc_del_continuous_iir_filter(s_iir_filter);
        s_iir_filter = NULL;
    }
}

static esp_err_t init_udp_socket(const char *ip, uint16_t port) {
    s_udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (s_udp_sock < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket: errno %d", errno);
        return ESP_FAIL;
    }

    struct timeval tv = {
        .tv_sec = (time_t)(CONT_ADC_SEND_TIMEOUT_MS / 1000u),
        .tv_usec = (suseconds_t)((CONT_ADC_SEND_TIMEOUT_MS % 1000u) * 1000u),
    };
    (void)setsockopt(s_udp_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    memset(&s_dest_addr, 0, sizeof(s_dest_addr));
    s_dest_addr.sin_family = AF_INET;
    s_dest_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &s_dest_addr.sin_addr) != 1) {
        ESP_LOGE(TAG, "Invalid IPv4 address: %s", ip ? ip : "<null>");
        close(s_udp_sock);
        s_udp_sock = -1;
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "UDP socket ready, dest=%s:%u", ip, (unsigned)port);
    return ESP_OK;
}

static esp_err_t init_adc_config(uint32_t sample_rate_hz) {
#if defined(SOC_ADC_SAMPLE_FREQ_THRES_LOW) && defined(SOC_ADC_SAMPLE_FREQ_THRES_HIGH)
    if (sample_rate_hz < SOC_ADC_SAMPLE_FREQ_THRES_LOW || sample_rate_hz > SOC_ADC_SAMPLE_FREQ_THRES_HIGH) {
        ESP_LOGE(TAG, "sample_rate_hz out of range: %u", (unsigned)sample_rate_hz);
        return ESP_ERR_INVALID_ARG;
    }
#endif

    adc_continuous_handle_cfg_t handle_cfg = {
        .max_store_buf_size = CONT_ADC_CONV_FRAME_SIZE * CONT_ADC_STORE_FRAMES,
        .conv_frame_size = CONT_ADC_CONV_FRAME_SIZE,
        .flags.flush_pool = 1,
    };

    esp_err_t ret = adc_continuous_new_handle(&handle_cfg, &s_adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "adc_continuous_new_handle failed: %s", esp_err_to_name(ret));
        return ret;
    }

    adc_digi_pattern_config_t pattern = {
        .atten = s_adc_atten,
        .channel = s_adc_channel,
        .unit = s_adc_unit,
        .bit_width = ADC_BITWIDTH_12,
    };

    adc_continuous_config_t dig_cfg = {
        .pattern_num = 1,
        .adc_pattern = &pattern,
        .sample_freq_hz = sample_rate_hz,
        .conv_mode = (s_adc_unit == ADC_UNIT_2) ? ADC_CONV_SINGLE_UNIT_2 : ADC_CONV_SINGLE_UNIT_1,
        .format = CONT_ADC_OUTPUT_TYPE,
    };

    ret = adc_continuous_config(s_adc_handle, &dig_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "adc_continuous_config failed: %s", esp_err_to_name(ret));
        adc_continuous_deinit(s_adc_handle);
        s_adc_handle = NULL;
        return ret;
    }

    if (s_iir_filter_enabled) {
        adc_continuous_iir_filter_config_t filter_cfg = {
            .unit = s_adc_unit,
            .channel = s_adc_channel,
            .coeff = s_iir_filter_coeff,
        };

        ret = adc_new_continuous_iir_filter(s_adc_handle, &filter_cfg, &s_iir_filter);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "adc_new_continuous_iir_filter failed: %s", esp_err_to_name(ret));
            adc_continuous_deinit(s_adc_handle);
            s_adc_handle = NULL;
            return ret;
        }

        ret = adc_continuous_iir_filter_enable(s_iir_filter);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "adc_continuous_iir_filter_enable failed: %s", esp_err_to_name(ret));
            cleanup_iir_filter();
            adc_continuous_deinit(s_adc_handle);
            s_adc_handle = NULL;
            return ret;
        }
    }

    return ESP_OK;
}

static void send_stop_hint(void) {
    if (s_udp_sock >= 0) {
        (void)sendto(s_udp_sock,
                     "",
                     0,
                     0,
                     (const struct sockaddr *)&s_dest_addr,
                     sizeof(s_dest_addr));
    }
}

static void free_stream_buffers(void) {
    if (s_adc_buf != NULL) {
        heap_caps_free(s_adc_buf);
        s_adc_buf = NULL;
    }
    if (s_packet_buf != NULL) {
        heap_caps_free(s_packet_buf);
        s_packet_buf = NULL;
    }
}

static esp_err_t alloc_stream_buffers(void) {
    free_stream_buffers();

    // Keep sizeable buffers off the FreeRTOS task stack. heap_caps_malloc() returns memory
    // suitably aligned for ordinary objects, including the 4-byte ADC result entries.
    s_adc_buf = (uint8_t *)heap_caps_malloc(CONT_ADC_CONV_FRAME_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    s_packet_buf = (uint8_t *)heap_caps_malloc(AUDIO_PACKET_MAX_BYTES, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (s_adc_buf == NULL || s_packet_buf == NULL) {
        ESP_LOGE(TAG, "failed to allocate stream buffers: adc=%u packet=%u bytes",
                 (unsigned)CONT_ADC_CONV_FRAME_SIZE,
                 (unsigned)AUDIO_PACKET_MAX_BYTES);
        free_stream_buffers();
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static void cleanup_stream_resources(void) {
    if (s_adc_handle != NULL) {
        if (s_adc_started) {
            (void)adc_continuous_stop(s_adc_handle);
            s_adc_started = false;
        }
        cleanup_iir_filter();
        (void)adc_continuous_deinit(s_adc_handle);
        s_adc_handle = NULL;
    }

    if (s_udp_sock >= 0) {
        close(s_udp_sock);
        s_udp_sock = -1;
    }

    free_stream_buffers();

    if (s_task_started_sem != NULL) {
        vSemaphoreDelete(s_task_started_sem);
        s_task_started_sem = NULL;
    }

    if (s_task_done_sem != NULL) {
        vSemaphoreDelete(s_task_done_sem);
        s_task_done_sem = NULL;
    }
}

/* ---------------- Streaming task ---------------- */

static inline bool adc_output_to_pcm(const adc_digi_output_data_t *d, int16_t *pcm) {
    /* ESP-IDF v5.5.1 raw conversion result parsing.
     *
     * Do not use adc_continuous_parse_data() here: that helper exists in newer
     * ESP-IDF docs/releases, but not in the v5.5.1 headers used by MicroPython
     * v1.28.0 at the time of this module.
     *
     * Also do not require d->type2.unit to match s_adc_unit. Espressif's v5.5.1
     * continuous_read example only validates the channel for the configured
     * single-unit conversion mode. Some targets/IDF combinations encode the
     * unit field differently, while channel+single-unit mode is sufficient here.
     */
    const uint32_t chan_num = CONT_ADC_GET_CHANNEL(d);
    uint32_t max_channels = SOC_ADC_CHANNEL_NUM(0);
    if (s_adc_unit == ADC_UNIT_2) {
        max_channels = SOC_ADC_CHANNEL_NUM(1);
    }
    if (chan_num >= max_channels || (adc_channel_t)chan_num != s_adc_channel) {
        return false;
    }

    *pcm = adc12_to_pcm16(CONT_ADC_GET_DATA(d));
    return true;
}

static void adc_udp_stream_task(void *arg) {
    (void)arg;

    packet_builder_t pb;
    uint32_t adc_bytes = 0;
    const uint8_t codec = s_codec_mode;
    const uint16_t target_samples = packet_target_samples(codec);
    uint8_t *adc_buf = s_adc_buf;
    uint8_t *packet_buf = s_packet_buf;

    if (adc_buf == NULL || packet_buf == NULL) {
        s_task_start_error = ESP_ERR_NO_MEM;
        s_last_error = ESP_ERR_NO_MEM;
        s_running = false;
        if (s_task_started_sem != NULL) {
            xSemaphoreGive(s_task_started_sem);
        }
        goto task_exit;
    }

    esp_err_t ret = adc_continuous_start(s_adc_handle);
    s_task_start_error = ret;
    if (ret != ESP_OK) {
        s_last_error = ret;
        ESP_LOGE(TAG, "adc_continuous_start failed: %s", esp_err_to_name(ret));
        s_running = false;
        if (s_task_started_sem != NULL) {
            xSemaphoreGive(s_task_started_sem);
        }
        goto task_exit;
    }
    s_adc_started = true;
    if (s_task_started_sem != NULL) {
        xSemaphoreGive(s_task_started_sem);
    }

    packet_begin(&pb, packet_buf, codec);

    while (s_running) {
        ret = adc_continuous_read(s_adc_handle,
                                  adc_buf,
                                  CONT_ADC_CONV_FRAME_SIZE,
                                  &adc_bytes,
                                  50);
        if (ret == ESP_ERR_TIMEOUT) {
            continue;
        }
        if (ret == ESP_ERR_INVALID_STATE) {
            // Usually means producer outran consumer. With flush_pool enabled, continue with the newest data.
            s_adc_overflows++;
            continue;
        }
        if (ret != ESP_OK) {
            s_adc_read_errors++;
            s_last_error = ret;
            ESP_LOGW(TAG, "adc_continuous_read failed: %s", esp_err_to_name(ret));
            s_running = false;
            break;
        }

        for (uint32_t pos = 0;
             pos + SOC_ADC_DIGI_RESULT_BYTES <= adc_bytes && s_running;
             pos += SOC_ADC_DIGI_RESULT_BYTES) {

            const adc_digi_output_data_t *d = (const adc_digi_output_data_t *)(const void *)&adc_buf[pos];
            int16_t pcm;
            if (!adc_output_to_pcm(d, &pcm)) {
                s_bad_adc_samples++;
                continue;
            }

            packet_add_sample(&pb, pcm);
            if (pb.sample_count >= target_samples) {
                packet_finish_and_send(&pb);
            }
        }
    }

    // Flush a partial packet before notifying the desktop receiver to stop.
    packet_finish_and_send(&pb);
    send_stop_hint();

    if (s_adc_started) {
        (void)adc_continuous_stop(s_adc_handle);
        s_adc_started = false;
    }

task_exit:
    ESP_LOGI(TAG,
             "stream task exit: sent=%u send_errors=%u overflows=%u read_errors=%u bad_samples=%u",
             (unsigned)s_packets_sent,
             (unsigned)s_send_errors,
             (unsigned)s_adc_overflows,
             (unsigned)s_adc_read_errors,
             (unsigned)s_bad_adc_samples);

    s_stream_task_handle = NULL;
    if (s_task_done_sem != NULL) {
        xSemaphoreGive(s_task_done_sem);
    }
    vTaskDelete(NULL);
}

/* ---------------- Public MicroPython API ---------------- */

static mp_obj_t config_adc_udp_stream(size_t n_args, const mp_obj_t *args) {
    // args: channel[, atten[, iir_coeff[, unit]]]
    // iir_coeff: 0/None/omitted disables IIR; 2, 4, 8, 16, 32, or 64 enables it.
    // unit: None/omitted selects ADC_UNIT_1; ADC_UNIT_2 is permitted when the
    // selected target/channel supports it.
    if (s_running || s_stream_task_handle != NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("ADC UDP stream is running"));
    }

    int atten = (int)s_adc_atten;
    if (n_args >= 2 && args[1] != mp_const_none) {
        atten = mp_obj_get_int(args[1]);
        if (!is_valid_atten(atten)) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid ADC attenuation"));
        }
    }

    bool iir_enabled = false;
    adc_digi_iir_filter_coeff_t iir_coeff = ADC_DIGI_IIR_FILTER_COEFF_64;
    if (n_args >= 3 && args[2] != mp_const_none) {
        int coeff_arg = mp_obj_get_int(args[2]);
        if (coeff_arg != 0) {
            if (!iir_coeff_from_int(coeff_arg, &iir_coeff)) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid IIR coefficient; use 0, 2, 4, 8, 16, 32, or 64"));
            }
            iir_enabled = true;
        }
    }

    int unit_arg = ADC_UNIT_1;
    if (n_args >= 4 && args[3] != mp_const_none) {
        unit_arg = mp_obj_get_int(args[3]);
        if (!is_valid_adc_unit_int(unit_arg)) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid ADC unit"));
        }
    }

    int channel = mp_obj_get_int(args[0]);
    adc_unit_t unit = (adc_unit_t)unit_arg;
    if (!is_valid_adc_channel_for_unit(unit, channel)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ADC channel for selected unit"));
    }

    s_adc_channel = (adc_channel_t)channel;
    s_adc_atten = (adc_atten_t)atten;
    s_adc_unit = unit;
    s_iir_filter_enabled = iir_enabled;
    s_iir_filter_coeff = iir_coeff;
    return mp_const_none;
}

static mp_obj_t start_adc_udp_stream(size_t n_args, const mp_obj_t *args) {
    // args: sample_rate_hz, dest_ip, dest_port[, codec]
    if (s_running || s_stream_task_handle != NULL) {
        ESP_LOGW(TAG, "stream already running");
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_ALREADY_RUNNING);
    }

    mp_int_t sample_rate_arg = mp_obj_get_int(args[0]);
    const char *dest_ip = mp_obj_str_get_str(args[1]);
    int dest_port = mp_obj_get_int(args[2]);
    int codec_arg = (n_args >= 4) ? mp_obj_get_int(args[3]) : AUDIO_CODEC_PCM16;

    if (sample_rate_arg <= 0 || sample_rate_arg > AUDIO_SAMPLE_RATE_MAX || dest_ip == NULL || dest_port <= 0 || dest_port > 65535) {
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_BAD_ARG);
    }
    if (codec_arg != AUDIO_CODEC_PCM16 && codec_arg != AUDIO_CODEC_ADPCM4) {
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_BAD_CODEC);
    }

    s_last_error = ESP_OK;
    s_packet_seq = 0;
    s_sample_rate_hz = (uint16_t)sample_rate_arg;
    s_adpcm_step_index = 20;
    s_packets_sent = 0;
    s_send_errors = 0;
    s_adc_overflows = 0;
    s_adc_read_errors = 0;
    s_bad_adc_samples = 0;
    s_codec_mode = (uint8_t)codec_arg;

    s_task_started_sem = xSemaphoreCreateBinary();
    s_task_done_sem = xSemaphoreCreateBinary();
    if (s_task_started_sem == NULL || s_task_done_sem == NULL) {
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_TASK_FAIL);
    }

    esp_err_t ret = alloc_stream_buffers();
    if (ret != ESP_OK) {
        s_last_error = ret;
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_TASK_FAIL);
    }

    ret = init_udp_socket(dest_ip, (uint16_t)dest_port);
    if (ret != ESP_OK) {
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_UDP_FAIL);
    }

    ret = init_adc_config((uint32_t)sample_rate_arg);
    if (ret != ESP_OK) {
        s_last_error = ret;
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_ADC_FAIL);
    }

    s_task_start_error = ESP_OK;
    s_running = true;

    BaseType_t task_ok = xTaskCreate(adc_udp_stream_task,
                                     "adc_udp_stream",
                                     CONT_ADC_TASK_STACK_BYTES,
                                     NULL,
                                     uxTaskPriorityGet(NULL),
                                     &s_stream_task_handle);
    if (task_ok != pdPASS) {
        ESP_LOGE(TAG, "failed to create stream task");
        s_running = false;
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_TASK_FAIL);
    }

    if (xSemaphoreTake(s_task_started_sem, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "stream task did not report ADC start in time");
        s_running = false;
        if (s_task_done_sem != NULL) {
            (void)xSemaphoreTake(s_task_done_sem, pdMS_TO_TICKS(1000));
        }
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_TASK_FAIL);
    }

    if (s_task_start_error != ESP_OK) {
        cleanup_stream_resources();
        return MP_OBJ_NEW_SMALL_INT(s_task_start_error == ESP_ERR_NO_MEM ? CADC_RET_TASK_FAIL : CADC_RET_ADC_FAIL);
    }

    ESP_LOGI(TAG,
             "ADC->UDP started: rate=%u codec=%s unit=%d channel=%u atten=%d iir=%s coeff=%d samples/packet=%u",
             (unsigned)sample_rate_arg,
             codec_arg == AUDIO_CODEC_ADPCM4 ? "ADPCM4" : "PCM16",
             (int)s_adc_unit,
             (unsigned)s_adc_channel,
             (int)s_adc_atten,
             s_iir_filter_enabled ? "on" : "off",
             s_iir_filter_enabled ? iir_coeff_to_int(s_iir_filter_coeff) : 0,
             (unsigned)packet_target_samples((uint8_t)codec_arg));

    return MP_OBJ_NEW_SMALL_INT(CADC_RET_OK);
}

static mp_obj_t stop_adc_udp_stream(void) {
    bool had_resources = s_running || s_stream_task_handle != NULL || s_adc_handle != NULL || s_udp_sock >= 0;
    if (!had_resources) {
        return MP_OBJ_NEW_SMALL_INT(CADC_RET_NOT_RUNNING);
    }

    ESP_LOGI(TAG, "stopping ADC->UDP stream");
    s_running = false;

    // Unblock adc_continuous_read() promptly. The stream task also stops ADC on self-exit/fatal-error paths.
    if (s_adc_handle != NULL && s_adc_started) {
        (void)adc_continuous_stop(s_adc_handle);
        s_adc_started = false;
    }

    if (s_stream_task_handle != NULL && s_task_done_sem != NULL) {
        if (xSemaphoreTake(s_task_done_sem, pdMS_TO_TICKS(1000)) != pdTRUE) {
            ESP_LOGE(TAG, "stream task did not exit in time; resources left allocated");
            return MP_OBJ_NEW_SMALL_INT(CADC_RET_STOP_TIMEOUT);
        }
    }

    cleanup_stream_resources();
    ESP_LOGI(TAG, "ADC->UDP stopped");
    return MP_OBJ_NEW_SMALL_INT(CADC_RET_OK);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(config_adc_udp_stream_obj, 1, 4, config_adc_udp_stream);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(start_adc_udp_stream_obj, 3, 4, start_adc_udp_stream);
MP_DEFINE_CONST_FUN_OBJ_0(stop_adc_udp_stream_obj, stop_adc_udp_stream);
