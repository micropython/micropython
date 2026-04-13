#ifndef NO_QSTR
#include <sdkconfig.h>
#include "rp2040.h"
#include "ice40.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include <driver/uart.h>

#ifdef CONFIG_DRIVER_ILI9341_ENABLE
#include "driver_ili9341.h"
#endif

#include <esp_system.h>
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#endif /* NO_QSTR */

#define TAG "RP2040_UPY"

#define RP2040_ADDR      (0x17)
#define GPIO_INT_RP2040  (34)

#define SPI_BUS               (VSPI_HOST)
#define SPI_MAX_TRANSFER_SIZE (4094)
#define GPIO_SPI_CS_FPGA (27)
#define GPIO_INT_FPGA    (39)  // Active low

#define MAXIMUM_CALLBACK_RETRY (10)

#define STATIC static

static ICE40 ice40;
static RP2040 rp2040;
static mp_obj_t touch_callback = mp_const_none;

static void button_handler(void *parameter);
//static void webusb_handler(void *parameter);

static esp_err_t ice40_get_done_wrapper(bool* done) {
    uint16_t  buttons;
    esp_err_t res = rp2040_read_buttons(&rp2040, &buttons);
    if (res != ESP_OK) return res;
    *done = !((buttons >> 5) & 0x01);
    return ESP_OK;
}

static esp_err_t ice40_set_reset_wrapper(bool reset) {
    esp_err_t res = rp2040_set_fpga(&rp2040, reset);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    return res;
}

void driver_mch22_init() {
	ESP_LOGI(TAG, "driver_mch22_init()");

    rp2040.i2c_bus = 1;
    rp2040.i2c_address = RP2040_ADDR;
    rp2040.pin_interrupt = GPIO_INT_RP2040;
    rp2040.queue = xQueueCreate(15, sizeof(rp2040_input_message_t));
    rp2040_init(&rp2040);

    ice40.spi_bus               = SPI_BUS;
    ice40.pin_cs                = GPIO_SPI_CS_FPGA;
    ice40.pin_done              = -1;
    ice40.pin_reset             = -1;
    ice40.pin_int               = GPIO_INT_FPGA;
    ice40.spi_speed_full_duplex = 26700000;
    ice40.spi_speed_half_duplex = 40000000;
    ice40.spi_speed_turbo       = 80000000;
    ice40.spi_input_delay_ns    = 10;
    ice40.spi_max_transfer_size = SPI_MAX_TRANSFER_SIZE;
    ice40.get_done              = ice40_get_done_wrapper;
    ice40.set_reset             = ice40_set_reset_wrapper;

    esp_err_t res = ice40_init(&ice40);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Initializing ICE40 failed");
    } else {
        bool done;
        res = ice40_get_done(&ice40, &done);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read ICE40 done state");
        } else if (done) {
            ESP_LOGE(TAG, "ICE40 indicates done in disabled state");
        }
    }

    xTaskCreatePinnedToCore(button_handler, "button_handler_task", 2048, NULL, 5,  NULL, MP_TASK_COREID);
    //xTaskCreatePinnedToCore(webusb_handler, "webusb_handler_task", 2048, NULL, 100,  NULL, MP_TASK_COREID);
}

static mp_obj_t buttons() {
    uint16_t value;
    rp2040_read_buttons(&rp2040, &value);
    return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(buttons_obj, buttons);

static mp_obj_t get_gpio_dir(mp_obj_t gpio) {
    uint8_t value_gpio = mp_obj_get_int(gpio);
    bool value_direction;
    rp2040_get_gpio_dir(&rp2040, value_gpio, &value_direction);
    return mp_obj_new_int(value_direction);
}
static MP_DEFINE_CONST_FUN_OBJ_1(get_gpio_dir_obj, get_gpio_dir);

static mp_obj_t set_gpio_dir(mp_obj_t gpio, mp_obj_t value) {
    uint8_t value_gpio = mp_obj_get_int(gpio);
    bool value_value = (bool) mp_obj_get_int(value);
    rp2040_set_gpio_dir(&rp2040, value_gpio, value_value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(set_gpio_dir_obj, set_gpio_dir);

static mp_obj_t get_gpio_value(mp_obj_t gpio) {
    uint8_t value_gpio = mp_obj_get_int(gpio);
    bool value_value;
    rp2040_get_gpio_value(&rp2040, value_gpio, &value_value);
    return mp_obj_new_int(value_value);
}
static MP_DEFINE_CONST_FUN_OBJ_1(get_gpio_value_obj, get_gpio_value);

static mp_obj_t set_gpio_value(mp_obj_t gpio, mp_obj_t value) {
    uint8_t value_gpio = mp_obj_get_int(gpio);
    bool value_value = (bool) mp_obj_get_int(value);
    rp2040_set_gpio_value(&rp2040, value_gpio, value_value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(set_gpio_value_obj, set_gpio_value);

static mp_obj_t set_handler(mp_obj_t handler) {
    touch_callback = handler;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_handler_obj, set_handler);

static mp_obj_t get_brightness() {
    uint8_t value;
    rp2040_get_lcd_backlight(&rp2040, &value);
    return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_brightness_obj, get_brightness);

static mp_obj_t set_brightness(mp_obj_t brightness) {
    uint8_t value = mp_obj_get_int(brightness);
    rp2040_set_lcd_backlight(&rp2040, value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_brightness_obj, set_brightness);

/*static mp_obj_t enable_webusb() {
    uart_set_pin(UART_NUM_0, -1, -1, -1, -1);
    uart_set_pin(CONFIG_DRIVER_FSOVERBUS_UART_NUM, 1, 3, -1, -1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(enable_webusb_obj, enable_webusb);

static mp_obj_t disable_webusb() {
    uart_set_pin(CONFIG_DRIVER_FSOVERBUS_UART_NUM, -1, -1, -1, -1);
    uart_set_pin(UART_NUM_0, 1, 3, -1, -1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(disable_webusb_obj, enable_webusb);*/

static mp_obj_t driver_ice40_load_bitstream(mp_obj_t bitstream) {
    if (!MP_OBJ_IS_TYPE(bitstream, &mp_type_bytes)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Expected a bytestring like object"));
        return mp_const_none;
    }
    mp_uint_t length;
    uint8_t* data = (uint8_t*) mp_obj_str_get_data(bitstream, &length);
    esp_err_t res = ice40_load_bitstream(&ice40, data, length);
    if (res != ESP_OK) mp_raise_ValueError(MP_ERROR_TEXT("Failed to load bitstream"));
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice40_load_bitstream_obj, driver_ice40_load_bitstream);

static mp_obj_t driver_ice40_disable() {
    ice40_disable(&ice40);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0 (ice40_disable_obj, driver_ice40_disable);

static uint8_t* get_buffer_data(mp_obj_t transmit_data, mp_uint_t* length)
{
    uint8_t* data_out = NULL;
    if (MP_OBJ_IS_TYPE(transmit_data, &mp_type_bytes)) {
        data_out = (uint8_t*) mp_obj_str_get_data(transmit_data, length);
    } else if (MP_OBJ_IS_TYPE(transmit_data, &mp_type_bytearray)) {
        mp_buffer_info_t bufinfo;

        if (mp_get_buffer(transmit_data, &bufinfo, MP_BUFFER_READ)) {
            data_out = (uint8_t*) bufinfo.buf;
            *length = (mp_uint_t) bufinfo.len;
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Expected a bytestring like object"));
    }
    return data_out;
}

static mp_obj_t driver_ice40_transaction(mp_obj_t transmit_data) {
    mp_uint_t length;
    uint8_t* data_out = get_buffer_data(transmit_data, &length);

    if (data_out != NULL) { 
        uint8_t* data_in = heap_caps_malloc(length + 4, MALLOC_CAP_DMA);
        if (data_in == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("Out of memory"));
            return mp_const_none;
        }

        esp_err_t res = ice40_transaction(&ice40, data_out, length, data_in, length);

        if (res != ESP_OK) {
            heap_caps_free(data_in);
            mp_raise_ValueError(MP_ERROR_TEXT("Failed to execute transaction"));
            return mp_const_none;
        }

        mp_obj_t data_in_obj = mp_obj_new_bytes(data_in, length);
        heap_caps_free(data_in);
        return data_in_obj;
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice40_transaction_obj, driver_ice40_transaction);

static mp_obj_t driver_ice40_receive(mp_obj_t length_obj) {
    size_t length = mp_obj_get_int(length_obj);
    uint8_t* data_in = heap_caps_malloc(length, MALLOC_CAP_DMA);
    if (data_in == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Out of memory"));
        return mp_const_none;
    }

    esp_err_t res = ice40_receive(&ice40,  data_in, length);

    if (res != ESP_OK) {
        heap_caps_free(data_in);
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to execute transaction"));
        return mp_const_none;
    }

    mp_obj_t data_in_obj = mp_obj_new_bytes(data_in, length);
    heap_caps_free(data_in);
    return data_in_obj;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice40_receive_obj, driver_ice40_receive);

static mp_obj_t driver_ice40_send(mp_obj_t transmit_data) {
    mp_uint_t length;
    uint8_t* data_out = get_buffer_data(transmit_data, &length);

    if (data_out != NULL) {
        esp_err_t res = ice40_send(&ice40, data_out, length);

        if (res != ESP_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Failed to execute transaction"));
        }
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice40_send_obj, driver_ice40_send);

static mp_obj_t driver_ice40_send_turbo(mp_obj_t transmit_data) {
    mp_uint_t length;
    uint8_t* data_out = get_buffer_data(transmit_data, &length);

    if (data_out != NULL) {
        esp_err_t res = ice40_send_turbo(&ice40, data_out, length);

        if (res != ESP_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Failed to execute transaction"));
        }
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(ice40_send_turbo_obj, driver_ice40_send_turbo);

static mp_obj_t driver_mch22_set_lcd_mode(mp_obj_t mode) {
#ifdef CONFIG_DRIVER_ILI9341_ENABLE
    driver_ili9341_set_mode(mp_obj_get_int(mode));
#endif
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(mch22_set_lcd_mode_obj, driver_mch22_set_lcd_mode);

static mp_obj_t driver_mch22_return_to_launcher() {
    REG_WRITE(RTC_CNTL_STORE0_REG, 0);
    esp_restart();
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(mch22_return_to_launcher_obj, driver_mch22_return_to_launcher);

static mp_obj_t read_vbat() {
    float vbat = 0;
    if (rp2040_read_vbat(&rp2040, &vbat) != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to get battery voltage"));
        return mp_const_none;
    }
    return mp_obj_new_float(vbat);
}
static MP_DEFINE_CONST_FUN_OBJ_0(read_vbat_obj, read_vbat);

static mp_obj_t read_vusb() {
    float vusb = 0;
    if (rp2040_read_vusb(&rp2040, &vusb) != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to get USB voltage"));
        return mp_const_none;
    }
    return mp_obj_new_float(vusb);
}
static MP_DEFINE_CONST_FUN_OBJ_0(read_vusb_obj, read_vusb);

typedef enum {SAO_IO0_PIN, SAO_IO1_PIN, PROTO_0_PIN, PROTO_1_PIN} gpio_name_t;

STATIC const mp_rom_map_elem_t mch22_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_buttons), MP_ROM_PTR(&buttons_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_gpio_dir), MP_ROM_PTR(&get_gpio_dir_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_gpio_dir), MP_ROM_PTR(&set_gpio_dir_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_gpio_value), MP_ROM_PTR(&get_gpio_value_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_gpio_value), MP_ROM_PTR(&set_gpio_value_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_brightness), MP_ROM_PTR(&get_brightness_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&set_brightness_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_handler), MP_ROM_PTR(&set_handler_obj)},
    //{MP_ROM_QSTR(MP_QSTR_enable_webusb), MP_ROM_PTR(&enable_webusb_obj)},
    //{MP_ROM_QSTR(MP_QSTR_disable_webusb), MP_ROM_PTR(&disable_webusb_obj)},
    {MP_ROM_QSTR(MP_QSTR_fpga_load), MP_ROM_PTR(&ice40_load_bitstream_obj)},
    {MP_ROM_QSTR(MP_QSTR_fpga_disable), MP_ROM_PTR(&ice40_disable_obj)},
    {MP_ROM_QSTR(MP_QSTR_fpga_transaction), MP_ROM_PTR(&ice40_transaction_obj)},
    {MP_ROM_QSTR(MP_QSTR_fpga_receive), MP_ROM_PTR(&ice40_receive_obj)},
    {MP_ROM_QSTR(MP_QSTR_fpga_send), MP_ROM_PTR(&ice40_send_obj)},
    {MP_ROM_QSTR(MP_QSTR_fpga_send_turbo), MP_ROM_PTR(&ice40_send_turbo_obj)},
    {MP_ROM_QSTR(MP_QSTR_lcd_mode), MP_ROM_PTR(&mch22_set_lcd_mode_obj)},
    {MP_ROM_QSTR(MP_QSTR_exit_python), MP_ROM_PTR(&mch22_return_to_launcher_obj)},
    {MP_ROM_QSTR(MP_QSTR_read_vbat), MP_ROM_PTR(&read_vbat_obj)},
    {MP_ROM_QSTR(MP_QSTR_read_vusb), MP_ROM_PTR(&read_vusb_obj)},

    { MP_ROM_QSTR(MP_QSTR_SAO_IO0_PIN), MP_ROM_INT(SAO_IO0_PIN) },
    { MP_ROM_QSTR(MP_QSTR_SAO_IO1_PIN), MP_ROM_INT(SAO_IO1_PIN) },
    { MP_ROM_QSTR(MP_QSTR_PROTO_0_PIN), MP_ROM_INT(PROTO_0_PIN) },
    { MP_ROM_QSTR(MP_QSTR_PROTO_1_PIN), MP_ROM_INT(PROTO_1_PIN) },
};

STATIC MP_DEFINE_CONST_DICT(mch22_module_globals, mch22_module_globals_table);

//===================================
const mp_obj_module_t mp_module_mch22 = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mch22_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_mch22, mp_module_mch22);


static void button_handler(void *parameter) {
    rp2040_input_message_t message;
    while(1) {
        xQueueReceive(rp2040.queue, &message, portMAX_DELAY);
        // uPy scheduler tends to get full for unknown reasons, so to not lose any interrupts,
        // we try until the schedule succeeds.
        if (touch_callback != mp_const_none) {
            mp_obj_t res = mp_obj_new_int(message.input << 1 | message.state);
            bool succeeded = mp_sched_schedule(touch_callback, res);
            int retry = 0;
            while (!succeeded && retry < MAXIMUM_CALLBACK_RETRY) {
                ESP_LOGW(TAG, "Failed to call touch callback, retrying");
                vTaskDelay(pdMS_TO_TICKS(50));
                succeeded = mp_sched_schedule(touch_callback, res);
                retry++;
            }
        }
    }
}

/*static void webusb_handler(void *parameter) {
    uint8_t modeCurrent = 0;
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(100));
        uint8_t mode;
        rp2040_get_webusb_mode(&rp2040, &mode);
        if (mode != modeCurrent) {
            if (mode) {
                enable_webusb();
            } else {
                disable_webusb();
            }
            modeCurrent = mode;
        }
    }
}*/
