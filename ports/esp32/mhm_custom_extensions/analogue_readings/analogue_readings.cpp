#include <iostream>
#include <assert.h>

extern "C"
{

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_partition.h"
#include "esp_timer.h"

#include "py/objlist.h"
#include "py/runtime.h"

#include "analouge_readings.h"

    mp_obj_t trigger_readings(mp_obj_t read_for_time_ms);
    mp_obj_t read_batch(mp_obj_t batch_size, mp_obj_t offset);
}

#define CONSTANT_TYPE constexpr const

using read_adc_fn_ptr = int (*)(adc_channel_t);
using reading_size = uint16_t;

CONSTANT_TYPE int PARTITION_SIZE_BYTES = 240000;
CONSTANT_TYPE int PARTITION_NUM_READINGS = PARTITION_SIZE_BYTES / sizeof(reading_size);
CONSTANT_TYPE esp_partition_type_t PARTITION_TYPE = ESP_PARTITION_TYPE_DATA;
CONSTANT_TYPE esp_partition_subtype_t PARTITION_SUBTYPE = ESP_PARTITION_SUBTYPE_DATA_SPIFFS;
CONSTANT_TYPE char PARTITION_NAME[] = "storage";

CONSTANT_TYPE int MAX_STATIC_BUFFER_SIZE = 2048; // not bytes, number of elements

CONSTANT_TYPE int DEFAULT_VREF = 1100;   // use adc2_vref_to_gpio() to obtain a better estimate
CONSTANT_TYPE int NO_OF_SAMPLES = 40000; // multisampling

//GPIO34 if ADC1, GPIO14 if ADC2
CONSTANT_TYPE adc1_channel_t channel = ADC1_CHANNEL_7;
CONSTANT_TYPE adc_bits_width_t width = ADC_WIDTH_BIT_12;

CONSTANT_TYPE adc_atten_t atten = ADC_ATTEN_DB_11;
CONSTANT_TYPE adc_unit_t unit = ADC_UNIT_1;

CONSTANT_TYPE gpio_num_t load_switch_pin = GPIO_NUM_27;
CONSTANT_TYPE gpio_mode_t load_switch_mode = GPIO_MODE_OUTPUT;

static bool has_been_initialised = false;

const esp_partition_t* get_partition() {
    const esp_partition_t* partition = esp_partition_find_first(PARTITION_TYPE, PARTITION_SUBTYPE, PARTITION_NAME);
    assert(partition != NULL);
    return partition;
}

void analogue_init() {
    if (has_been_initialised) return;

    gpio_set_direction(load_switch_pin, load_switch_mode);

    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    }
    else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }
}

template<typename ReadFn>
void read_into_buffer(reading_size buffer[], int size, ReadFn read_fn) {
    for (int i = 0; i < size; i++) {
        buffer[i] = read_fn();
    }
}

struct readings_info {
    int64_t time_read_for_ms;
    int64_t num_readings;
};

readings_info read_analogue(int read_for_time_ms) {
    analogue_init();

    static reading_size buffer[MAX_STATIC_BUFFER_SIZE];
    auto partition = get_partition();

    auto read_adc = []() {
        return adc1_get_raw(channel);
        };

    auto curr_time_ms = []() {
        return esp_timer_get_time() / 1000;
        };

    int max_batch_size = PARTITION_NUM_READINGS;
    int curr_offset_bytes = 0;
    auto initial_time_ms = curr_time_ms(); // time in milliseconds since boot
    int readings_made = 0;

    while (max_batch_size > 0 && curr_time_ms() - initial_time_ms < read_for_time_ms) {
        int curr_read_size = std::min(max_batch_size, MAX_STATIC_BUFFER_SIZE); // don't buffer overflow
        int curr_read_size_bytes = curr_read_size * sizeof(reading_size);

        read_into_buffer(buffer, MAX_STATIC_BUFFER_SIZE, read_adc);

        // erase the range, then write to it
        esp_partition_erase_range(partition, curr_offset_bytes, curr_read_size_bytes);
        esp_partition_write(partition, curr_offset_bytes, buffer, curr_read_size_bytes);

        curr_offset_bytes += curr_read_size_bytes;
        readings_made += curr_read_size;
        max_batch_size -= curr_read_size;
    }

    auto diff_time = curr_time_ms() - initial_time_ms;
    auto num_readings = readings_made;

    return { .time_read_for_ms = diff_time, .num_readings = num_readings };
}

void add_buff_items_to_list(reading_size buffer[], int size, mp_obj_t list) {
    mp_check_self(mp_obj_is_type(self_in, &mp_type_list));

    for (int i = 0; i < size; i++) {
        mp_obj_list_append(list, mp_obj_new_int(buffer[i]));
    }
}

void read_flash_to_buff(const esp_partition_t* partition, mp_obj_t list, int offset, int batch_size)
{
    mp_check_self(mp_obj_is_type(self_in, &mp_type_list));

    static reading_size buffer[MAX_STATIC_BUFFER_SIZE];
    int max_batch_size = batch_size;
    int curr_offset = offset;

    // reads in chunks of MAX_STATIC_BUFFER_SIZE
    while (max_batch_size > 0)
    {
        int curr_read_size = std::min(max_batch_size, MAX_STATIC_BUFFER_SIZE); // don't buffer overflow

        esp_err_t ret = esp_partition_read(partition, curr_offset, buffer, MAX_STATIC_BUFFER_SIZE);
        assert(ret == ESP_OK);

        add_buff_items_to_list(buffer, curr_read_size, list);

        curr_offset += MAX_STATIC_BUFFER_SIZE;
        max_batch_size -= MAX_STATIC_BUFFER_SIZE;
    }
}

mp_obj_t trigger_readings(mp_obj_t read_for_time_ms) {
    int read_for_time_ms_num = mp_obj_get_int(read_for_time_ms);
    readings_info read_data = read_analogue(read_for_time_ms_num);

    mp_obj_t readings_tuple[2] = {
        mp_obj_new_int(read_data.num_readings),
        mp_obj_new_int(read_data.time_read_for_ms)
    };

    return mp_obj_new_tuple(2, readings_tuple);
}

mp_obj_t read_batch(mp_obj_t batch_size, mp_obj_t offset)
{
    int batch_size_num = mp_obj_get_int(batch_size);
    int byte_offset = mp_obj_get_int(offset);
    mp_obj_t list = mp_obj_new_list(0, NULL);

    auto partition = get_partition();
    read_flash_to_buff(partition, list, byte_offset, batch_size_num);

    return list;
}
