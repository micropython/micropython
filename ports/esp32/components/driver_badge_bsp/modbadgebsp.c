#ifndef NO_QSTR
#include <sdkconfig.h>
#include <esp_system.h>
#include <esp_err.h>
#include <esp_log.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "include/driver_badge_bsp.h"

#endif /* NO_QSTR */

#define TAG "BADGEBSP"

#define MAXIMUM_CALLBACK_RETRY (10)

#define STATIC static

static mp_obj_t touch_callback = mp_const_none;

static void button_handler(void *parameter);
static QueueHandle_t input_queue;

void driver_badgebsp_init() {
	ESP_LOGI(TAG, "driver_badgebsp_init()");

	bsp_input_get_queue(&input_queue);

    xTaskCreatePinnedToCore(button_handler, "button_handler_task", 2048, NULL, 5,  NULL, MP_TASK_COREID);
}

static mp_obj_t buttons() {
    uint16_t value = 0;
    //rp2040_read_buttons(&rp2040, &value);
    return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(buttons_obj, buttons);

static mp_obj_t set_handler(mp_obj_t handler) {
    touch_callback = handler;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_handler_obj, set_handler);

static mp_obj_t get_brightness() {
    uint8_t value = 0;
    return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_brightness_obj, get_brightness);

static mp_obj_t set_brightness(mp_obj_t brightness) {
    uint8_t value = mp_obj_get_int(brightness);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_brightness_obj, set_brightness);

static mp_obj_t driver_badgebsp_return_to_launcher() {
    //REG_WRITE(RTC_CNTL_STORE0_REG, 0);
    //esp_restart();
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(badgebsp_return_to_launcher_obj, driver_badgebsp_return_to_launcher);

typedef enum {SAO_IO0_PIN, SAO_IO1_PIN, PROTO_0_PIN, PROTO_1_PIN} gpio_name_t;

STATIC const mp_rom_map_elem_t badgebsp_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_buttons), MP_ROM_PTR(&buttons_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_brightness), MP_ROM_PTR(&get_brightness_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&set_brightness_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_handler), MP_ROM_PTR(&set_handler_obj)},
    {MP_ROM_QSTR(MP_QSTR_exit_python), MP_ROM_PTR(&badgebsp_return_to_launcher_obj)},
};

STATIC MP_DEFINE_CONST_DICT(badgebsp_module_globals, badgebsp_module_globals_table);

//===================================
const mp_obj_module_t mp_module_badgebsp = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&badgebsp_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_badgebsp, mp_module_badgebsp);


static void button_handler(void *parameter) {
    bsp_input_event_t message;
    while(1) {
        xQueueReceive(input_queue, &message, portMAX_DELAY);
		switch (message.type) {
			case INPUT_EVENT_TYPE_NAVIGATION:
				//ESP_LOGI(TAG, "INPUT_EVENT_TYPE_NAVIGATION, key: %02x, state: %d", message.args_navigation.key, message.args_navigation.state);
				break;
			case INPUT_EVENT_TYPE_KEYBOARD:
				//ESP_LOGI(TAG, "INPUT_EVENT_TYPE_KEYBOARD, utf8: %s, ascii: %c", message.args_keyboard.utf8, message.args_keyboard.ascii);
				break;
			case INPUT_EVENT_TYPE_ACTION:
				//ESP_LOGI(TAG, "INPUT_EVENT_TYPE_KEYBOARD, type: %d, state: %d", message.args_action.type, message.args_action.state);
				break;
			case INPUT_EVENT_TYPE_SCANCODE:
				//ESP_LOGI(TAG, "INPUT_EVENT_TYPE_SCANCODE, scancode: %x", message.args_scancode.scancode);
				break;
			default:
				break;
		}

        // uPy scheduler tends to get full for unknown reasons, so to not lose any interrupts,
        // we try until the schedule succeeds.
        if (touch_callback != mp_const_none && message.type == INPUT_EVENT_TYPE_SCANCODE) {
            mp_obj_t res = mp_obj_new_int(message.args_scancode.scancode);
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
