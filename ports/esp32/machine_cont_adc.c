#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "esp_adc/adc_continuous.h"
#include "soc/adc_channel.h"  // for ADC_CHANNEL_x on ESP32-C3 :contentReference[oaicite:2]{index=2}

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define TAG "ADC_UDP_STREAM"

/* ---------------- ADC configuration ----------------
* ESP32-C3:
*  - ADC1 channels map to GPIO0-4 (ADC_CHANNEL_0..4). :contentReference[oaicite:3]{index=3}
*  - We’ll use ADC1, single channel (ADC_CHANNEL_0 on GPIO0) as the example.
* Change ADC_CHANNEL_0 and ADC_ATTEN_DB_11 as needed.
*/

uint8_t CADC_CHANNEL_USED = ADC_CHANNEL_0;
uint8_t CADC_ATTEN_USED = ADC_ATTEN_DB_0;
uint8_t CADC_UNIT_USED = ADC_UNIT_1;

/* ---------------- Streaming buffers ----------------
* We implement a simple ping-pong double buffer:
*  - ADC task fills buffer[write_idx]
*  - UDP task sends buffer[read_idx]
*  - Indices advance modulo 2
*/

#define NUM_STREAM_BUFFERS     2
#define STREAM_BUFFER_BYTES    1024    // bytes per UDP packet (tune as needed)

/* ADC driver internal pool / frame (separate from our ping-pong buffers) :contentReference[oaicite:4]{index=4} */
#define ADC_CONV_FRAME_SIZE    256     // bytes per ADC frame (must be multiple of SOC_ADC_DIGI_RESULT_BYTES)
#define ADC_MAX_STORE_BUF_SIZE (ADC_CONV_FRAME_SIZE * 8)

static adc_continuous_handle_t s_adc_handle = NULL;
static int s_udp_sock = -1;
static struct sockaddr_in s_dest_addr;

static TaskHandle_t s_adc_task_handle = NULL;
static TaskHandle_t s_udp_task_handle = NULL;

static uint8_t s_stream_buffers[NUM_STREAM_BUFFERS][STREAM_BUFFER_BYTES];
static volatile size_t s_stream_lengths[NUM_STREAM_BUFFERS];
static volatile bool   s_stream_ready[NUM_STREAM_BUFFERS];
static volatile int    s_write_index = 0;
static volatile int    s_read_index  = 0;

static volatile bool s_running = false;

/* ---------------- Internal helpers ---------------- */

static esp_err_t init_udp_socket(const char *ip, uint16_t port)
{
	s_udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (s_udp_sock < 0) {
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		return ESP_FAIL;
	}

	// Optional: set small send timeout so stop() is responsive
	struct timeval tv = {
		.tv_sec = 0,
		.tv_usec = 100 * 1000, // 100 ms
	};
	setsockopt(s_udp_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

	memset(&s_dest_addr, 0, sizeof(s_dest_addr));
	s_dest_addr.sin_family = AF_INET;
	s_dest_addr.sin_port   = htons(port);
	s_dest_addr.sin_addr.s_addr = inet_addr(ip); // assumes IPv4 dotted-quad

	ESP_LOGI(TAG, "UDP socket ready, dest=%s:%u", ip, (unsigned)port);
	return ESP_OK;
}

static esp_err_t init_adc(uint32_t sample_rate_hz){
	adc_continuous_handle_cfg_t handle_cfg = {
		.max_store_buf_size = ADC_MAX_STORE_BUF_SIZE,
		.conv_frame_size    = ADC_CONV_FRAME_SIZE,
	};

	esp_err_t ret = adc_continuous_new_handle(&handle_cfg, &s_adc_handle);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "adc_continuous_new_handle failed: %s", esp_err_to_name(ret));
		return ret;
	}

	// Configure one channel, single-unit, TYPE2 format :contentReference[oaicite:5]{index=5}
	adc_digi_pattern_config_t pattern = {
		.atten     = CADC_ATTEN_USED,
		.channel   = CADC_CHANNEL_USED,
		.unit      = CADC_UNIT_USED,
		.bit_width = ADC_BITWIDTH_12,
	};

	adc_continuous_config_t dig_cfg = {
		.pattern_num    = 1,
		.adc_pattern    = &pattern,
		.sample_freq_hz = sample_rate_hz,
		.conv_mode      = ADC_CONV_SINGLE_UNIT_1,       // ADC1 only on ESP32-C3
		.format         = ADC_DIGI_OUTPUT_FORMAT_TYPE2, // gives adc_digi_output_data_t
	};

	ret = adc_continuous_config(s_adc_handle, &dig_cfg);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "adc_continuous_config failed: %s", esp_err_to_name(ret));
		adc_continuous_deinit(s_adc_handle);
		s_adc_handle = NULL;
		return ret;
	}

	ret = adc_continuous_start(s_adc_handle);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "adc_continuous_start failed: %s", esp_err_to_name(ret));
		adc_continuous_deinit(s_adc_handle);
		s_adc_handle = NULL;
		return ret;
	}

	ESP_LOGI(TAG, "ADC continuous started at %u Hz", (unsigned)sample_rate_hz);
	return ESP_OK;
}

/* ---------------- Tasks ---------------- */

/* ADC acquisition task
* - Reads from adc_continuous_read()
* - Packs consecutive frames into the current ping-pong buffer
* - Marks buffer as ready when full
*/
static void adc_acquisition_task(void *arg){
	esp_err_t ret;
	uint8_t src_buf[ADC_CONV_FRAME_SIZE] __attribute__((aligned(4)));
	uint16_t *dst_ptr = NULL;
	uint32_t *src_ptr = NULL;
	int32_t src_bytes = 0, dst_bytes = 0;

	while (s_running) {
		int wi = s_write_index;

		// Wait if the next buffer is still not consumed (ping-pong)
		while (s_running && s_stream_ready[wi]) {
			vTaskDelay(pdMS_TO_TICKS(1));
		}

		// Copy surplus data if there exists
		dst_ptr = (uint16_t*)s_stream_buffers[wi];
		for(dst_bytes=STREAM_BUFFER_BYTES; src_bytes>0; src_bytes-=4, dst_bytes-=2)
			*(dst_ptr++) = *(src_ptr++);

		while (dst_bytes > 0 && s_running) {
			if (!s_running)
				break;

			ret = adc_continuous_read(s_adc_handle,
									src_buf,
									ADC_CONV_FRAME_SIZE,
									(uint32_t*)&src_bytes,
									50 /* timeout ms */);

			if (ret == ESP_OK && src_bytes > 0) {
				src_ptr = (uint32_t*)src_buf;
				while(dst_bytes > 0 && src_bytes > 0){
					*(dst_ptr++) = (uint16_t)*(src_ptr++);
					dst_bytes -= 2;
					src_bytes -= 4;
				}
			} else if (ret == ESP_ERR_TIMEOUT) {
				// No data yet, just loop again
				continue;
			} else {
				ESP_LOGW(TAG, "adc_continuous_read error: %s", esp_err_to_name(ret));
				// For INVALID_STATE or others, break and let stop() clean up
				break;
			}
		}

		s_stream_lengths[wi] = STREAM_BUFFER_BYTES - dst_bytes;
		s_stream_ready[wi] = true;

		if (!s_running) {
			break;
		}

		// Move to next buffer
		s_write_index = (wi + 1) % NUM_STREAM_BUFFERS;
	}

	ESP_LOGI(TAG, "ADC acquisition task exiting");
	s_adc_task_handle = NULL;
	vTaskDelete(NULL);
}

/* UDP sender task
* - Waits for a buffer to become ready
* - Sends it as a single UDP datagram
*/
static void udp_sender_task(void *arg){
	while (s_running) {
		int ri = s_read_index;

		if (!s_stream_ready[ri]) {
			vTaskDelay(pdMS_TO_TICKS(1));
			continue;
		}

		size_t len = s_stream_lengths[ri];
		if (len > 0 && s_udp_sock >= 0) {
			int sent = sendto(s_udp_sock,
							s_stream_buffers[ri],
							len,
							0,
							(struct sockaddr *)&s_dest_addr,
							sizeof(s_dest_addr));
			if (sent < 0) {
				ESP_LOGW(TAG, "sendto failed: errno %d", errno);
			}
		}

		// Mark buffer as free and advance
		s_stream_ready[ri] = false;
		s_read_index = (ri + 1) % NUM_STREAM_BUFFERS;
	}

	ESP_LOGI(TAG, "UDP sender task exiting");
	s_udp_task_handle = NULL;
	vTaskDelete(NULL);
}

/* ---------------- Public API ---------------- */
static mp_obj_t config_adc_udp_stream(size_t n_args, const mp_obj_t *args){
	/* args: channel, attn, unit */
	CADC_CHANNEL_USED = mp_obj_get_int(args[0]);
	if(n_args>=2) CADC_ATTEN_USED = mp_obj_get_int(args[1]);
	if(n_args>=3) CADC_UNIT_USED = mp_obj_get_int(args[2]);
	return mp_const_none;
}

static mp_obj_t start_adc_udp_stream(mp_obj_t _channel_sample_rate_hz,
							mp_obj_t _dest_ip,
							mp_obj_t _dest_port){
	/* _channel_sample_rate_hz:
		highest-order 1 byte is the ADC channel number
		lowest-order 3 bytes is the sampling rate in Hz */
	int channel_sample_rate_hz = mp_obj_get_int(_channel_sample_rate_hz);
	const char *dest_ip = mp_obj_str_get_str(_dest_ip);
	int dest_port = mp_obj_get_int(_dest_port);

	if (s_running) {
		ESP_LOGW(TAG, "Stream already running");
		return MP_OBJ_NEW_SMALL_INT(1);
	}

	if (!dest_ip) {
		return MP_OBJ_NEW_SMALL_INT(-2);
	}

	// Reset state
	for (int i = 0; i < NUM_STREAM_BUFFERS; ++i) {
		s_stream_ready[i] = false;
		s_stream_lengths[i] = 0;
	}
	s_write_index = 0;
	s_read_index  = 0;

	esp_err_t ret = init_udp_socket(dest_ip, dest_port);
	if (ret != ESP_OK) {
		return MP_OBJ_NEW_SMALL_INT(-3);
	}

	ret = init_adc(channel_sample_rate_hz);
	if (ret != ESP_OK) {
		close(s_udp_sock);
		s_udp_sock = -1;
		return MP_OBJ_NEW_SMALL_INT(-4);
	}

	s_running = true;
	int mp_prio = uxTaskPriorityGet(NULL);

	BaseType_t r1 = xTaskCreate(adc_acquisition_task,
								"adc_acquisition_task",
								4096,
								NULL,
								mp_prio,
								&s_adc_task_handle);

	BaseType_t r2 = xTaskCreate(udp_sender_task,
								"udp_sender_task",
								4096,
								NULL,
								mp_prio,
								&s_udp_task_handle);

	if (r1 != pdPASS || r2 != pdPASS) {
		ESP_LOGE(TAG, "Failed to create tasks");
		s_running = false;

		if (s_adc_handle) {
			adc_continuous_stop(s_adc_handle);
			adc_continuous_deinit(s_adc_handle);
			s_adc_handle = NULL;
		}
		if (s_udp_sock >= 0) {
			close(s_udp_sock);
			s_udp_sock = -1;
		}
		return MP_OBJ_NEW_SMALL_INT(-5);
	}

	ESP_LOGI(TAG, "ADC->UDP streaming started");
	return MP_OBJ_NEW_SMALL_INT(0);
}

static mp_obj_t stop_adc_udp_stream(void){
	if (!s_running) {
		return MP_OBJ_NEW_SMALL_INT(1);
	}

	ESP_LOGI(TAG, "Stopping ADC->UDP streaming");
	s_running = false;

	// Give tasks a moment to cleanly exit
	vTaskDelay(pdMS_TO_TICKS(100));

	// Stop ADC; adc_continuous_read will start returning errors
	if (s_adc_handle) {
		adc_continuous_stop(s_adc_handle);
		adc_continuous_deinit(s_adc_handle);
		s_adc_handle = NULL;
	}

	// UDP: send the remaining data (if there exist), followed by a 0-byte packet to hint closure, and close the socket
	if (s_udp_sock >= 0) {
		for(int i=0; i<NUM_STREAM_BUFFERS; i++, s_read_index=(s_read_index+1)%NUM_STREAM_BUFFERS){
			if (s_stream_ready[s_read_index] && s_stream_lengths[s_read_index])
				sendto(s_udp_sock, s_stream_buffers[s_read_index], s_stream_lengths[s_read_index], 0, (struct sockaddr *)&s_dest_addr, sizeof(s_dest_addr));
		}
		sendto(s_udp_sock, s_stream_buffers[s_read_index], 0, 0, (struct sockaddr *)&s_dest_addr, sizeof(s_dest_addr));
		shutdown(s_udp_sock, 0);
		close(s_udp_sock);
		s_udp_sock = -1;
	}

	ESP_LOGI(TAG, "ADC->UDP streaming stopped");
	return MP_OBJ_NEW_SMALL_INT(0);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(config_adc_udp_stream_obj, 1, 3, config_adc_udp_stream);
static MP_DEFINE_CONST_FUN_OBJ_3(start_adc_udp_stream_obj, start_adc_udp_stream);
static MP_DEFINE_CONST_FUN_OBJ_0(stop_adc_udp_stream_obj, stop_adc_udp_stream);