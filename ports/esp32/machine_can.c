/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include <mpconfigport.h>

//Headers of ESP-IDF library
#include "soc/dport_reg.h" //FIXME: check if path is found. full path is ESPIDF/components/soc/esp32/include/soc/dport_reg.h
#include "driver/can.h"
#include "esp_err.h"
#include "esp_log.h"

#include "machine_can.h"

//#if MICROPY_HW_ENABLE_CAN

const machine_hw_can_obj_t can_default = {.tx=6, .rx=4};

STATIC const mp_obj_type_t machine_hw_can_type; //FIXME: da popolare

STATIC void machine_hw_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "CAN(tx=%u, rx=%u)", self->tx, self->rx);
}

STATIC mp_obj_t machine_hw_can_init(mp_obj_t self_in) {
    /*static const can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
    static const can_timing_config_t t_config = CAN_TIMING_CONFIG_25KBITS();
    //Set TX queue length to 0 due to listen only mode
    can_general_config_t g_config = {.mode = mode,
                                    .tx_io = tx, .rx_io = rx,
                                    .clkout_io = CAN_IO_UNUSED, .bus_off_io = CAN_IO_UNUSED,
                                    .tx_queue_len = 0, .rx_queue_len = 5,
                                    .alerts_enabled = CAN_ALERT_NONE,
                                    .clkout_divider = 0};
    ESP_ERROR_CHECK(can_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI("CAN", "Driver installed");
    ESP_ERROR_CHECK(can_start());
    ESP_LOGI("CAN", "Driver started");*/
    return mp_const_none;
}


/*void machine_can_trasmit(machine_hw_can_obj_t *self, uint32_t address, size_t length, mp_machine_can_buf_t *data)
{
    can_message_t tx_msg = {.data_length_code = length, .identifier = address, .flags = CAN_MSG_FLAG_SELF};
    for (uint8_t i=0; i<length; i++){ tx_msg.data[i] = data[i]; }
    ESP_ERROR_CHECK(can_transmit(&tx_msg, portMAX_DELAY));
}*/

/*void machine_can_deinit(machine_can_obj_t *self){
	ESP_ERROR_CHECK(can_stop());
    ESP_LOGI("CAN", "Driver stopped");
    ESP_ERROR_CHECK(can_driver_uninstall());
    ESP_LOGI("CAN", "Driver uninstalled");
}*/

//#endif // MICROPY_HW_ENABLE_CAN