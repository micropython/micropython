/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Vekatech Ltd.
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

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "hal_data.h"

void next_img();
void SnapShot();
extern volatile uint8_t CAM_rdy;

#if MODULE_LCD_ENABLED

#define FT5X06_DOWN          0
#define FT5X06_UP            1
#define FT5X06_CONTACT       2
#define FT5X06_NUM_POINTS    5
#define FT5X06_REG_TD_STATUS 0x02

#define extract_e(t) ((uint8_t)((t).event))
#define extract_x(t) ((int16_t)(((t).x_msb << 8) | ((t).x_lsb)))
#define extract_y(t) ((int16_t)(((t).y_msb << 8) | ((t).y_lsb)))

typedef enum
{
    TOUCH_EVENT_NONE,
    TOUCH_EVENT_DOWN,
    TOUCH_EVENT_HOLD,
    TOUCH_EVENT_MOVE,
    TOUCH_EVENT_UP
} touch_event_t;

typedef struct st_touch_coord
{
    uint16_t x;
    uint16_t y;
    touch_event_t event;
} touch_coord_t;

typedef struct st_ft5x06_touch
{
    uint8_t x_msb : 4;
    uint8_t       : 2;
    uint8_t event : 2;
    uint8_t x_lsb;

    uint8_t y_msb : 4;
    uint8_t id    : 4;
    uint8_t y_lsb : 8;

    uint8_t res1;
    uint8_t res2;
} ft5x06_touch_t;

/* Complete FT5X06 data payload (number of active points + all five touch points) */

typedef struct st_touch_data
{
    uint8_t num_stamp_points;
    touch_coord_t stamp_point[FT5X06_NUM_POINTS];
} touch_data_t;

typedef struct st_ft5x06_payload
{
    uint8_t num_points_active;
    ft5x06_touch_t data_raw[FT5X06_NUM_POINTS];
} ft5x06_payload_t;

typedef struct _machine_lcd_obj_t {
    mp_obj_base_t base;
    uint8_t idx;                // currently used buffer;
    uint8_t isinited;
    uint8_t isstarted;
    uint8_t ischanged;
    mp_hal_pin_obj_t bl;  // backlight ctr (ON OFF)
    mp_hal_pin_obj_t en;  // enable ctr (ON OFF)
    mp_hal_pin_obj_t tch; // touch

    uint8_t num_points;
    touch_coord_t point[FT5X06_NUM_POINTS];
} machine_lcd_obj_t;

static volatile uint8_t I2C_complete = 0;

touch_data_t locked;

STATIC machine_lcd_obj_t machine_lcd_obj = {{&machine_lcd_type}, 0, 0, 0, 0, pin_P801, pin_P806, pin_P800, 0};

void touch_i2c_callback(i2c_master_callback_args_t *p_args) {
    I2C_complete = 1;
}

void ft5x06_payload_get() {
    touch_coord_t new_touch;
    ft5x06_payload_t touch_payload;

    /* Clear payload struct */
    memset(&touch_payload, 0, sizeof(ft5x06_payload_t));

    /* Read the data about the touch point(s) */
    uint8_t reg = FT5X06_REG_TD_STATUS;

    /* Write TD_STATUS address */
    I2C_complete = 0;
    R_SCI_I2C_Write(&g_i2c_touch_ctrl, &reg, 1, true);
    while (!I2C_complete) {
        ;                 // i2c_wait();

    }
    /* Read TD_STATUS through all five TOUCHn_** register sets */
    I2C_complete = 0;
    R_SCI_I2C_Read(&g_i2c_touch_ctrl, (uint8_t *)&touch_payload, sizeof(ft5x06_payload_t), false);
    while (!I2C_complete) {
        ;                 // i2c_wait();

    }
    if (touch_payload.num_points_active) {
        /* Process the raw data for the touch point(s) into useful data */
        for (uint8_t i = 0; i < FT5X06_NUM_POINTS; i++)
        {
            new_touch.x = (uint16_t)extract_x(touch_payload.data_raw[i]);
            new_touch.y = (uint16_t)extract_y(touch_payload.data_raw[i]);
            new_touch.event = extract_e(touch_payload.data_raw[i]);

            /* Set event type based on received data */
            switch (new_touch.event)
            {
                case FT5X06_DOWN:
                    machine_lcd_obj.point[i].event = TOUCH_EVENT_DOWN;
                    break;
                case FT5X06_UP:
                    machine_lcd_obj.point[i].event = TOUCH_EVENT_UP;
                    break;
                case FT5X06_CONTACT:
                    /* Check if the point is moving or not */
                    if ((machine_lcd_obj.point[i].x != new_touch.x) || (machine_lcd_obj.point[i].y != new_touch.y)) {
                        machine_lcd_obj.point[i].event = TOUCH_EVENT_MOVE;
                    } else {
                        machine_lcd_obj.point[i].event = TOUCH_EVENT_HOLD;
                    }
                    break;
                default:
                    machine_lcd_obj.point[i].event = TOUCH_EVENT_NONE;
                    break;
            }

            /* Set new coordinates */
            machine_lcd_obj.point[i].x = new_touch.x;
            machine_lcd_obj.point[i].y = new_touch.y;
        }

        /* Pass the number of active touch points through */
        machine_lcd_obj.num_points = touch_payload.num_points_active;
    }
}

void callback_icu(external_irq_callback_args_t *p_args) { // callback_icu
    if (g_external_irq11_cfg.channel == p_args->channel) {
        ft5x06_payload_get();
    }
}

void lcd_Vsync_ISR(display_callback_args_t *p_args) {
    if (p_args->event == DISPLAY_EVENT_LINE_DETECTION) {
        if (CAM_rdy) {
            SnapShot();
            next_img();
        }
    }
}

STATIC mp_int_t machine_lcd_set_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    // mp_obj_framebuf_t *self = MP_OBJ_TO_PTR(self_in);
    bufinfo->buf = g_display0_cfg.input[0].p_base;
    bufinfo->len = g_display0_cfg.input[0].hstride * g_display0_cfg.input[0].vsize * ((g_display0_cfg.input[0].format < DISPLAY_IN_FORMAT_16BITS_RGB565)? 4 : 2);
    bufinfo->typecode = 'B'; // view framebuf as bytes
    return 0;
}

STATIC mp_obj_t machine_lcd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return self object
    return MP_OBJ_FROM_PTR(&machine_lcd_obj);
}

STATIC mp_obj_t lcd_init(mp_obj_t self_in) {
    machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->isinited) {
        if (FSP_SUCCESS == R_GLCDC_Open(&g_display0_ctrl, &g_display0_cfg)) {
            self->isinited = 1;
            ra_gpio_config(self->bl->pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_LOW_POWER, 0);
            ra_gpio_config(self->en->pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_LOW_POWER, 0);
            ra_gpio_write(self->bl->pin, 1);
            ra_gpio_write(self->en->pin, 1);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't init R_GLCDC"));
        }

        ra_gpio_config(self->tch->pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_LOW_POWER, 0);
        ra_gpio_write(self->tch->pin, 1);

        if (FSP_SUCCESS == R_SCI_I2C_Open(&g_i2c_touch_ctrl, &g_i2c_touch_cfg)) {
            /* Enable touch IRQ */
            R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
            R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't init R_SCI"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Olready inited!"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_lcd_init_obj, lcd_init);

STATIC mp_obj_t lcd_touched(mp_obj_t self_in) {
    machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    if (!locked.num_stamp_points) {
        if (self->num_points) {
            locked.num_stamp_points = (self->num_points > FT5X06_NUM_POINTS)? FT5X06_NUM_POINTS : self->num_points;

            for (uint8_t i = 0; i < locked.num_stamp_points; i++) {
                locked.stamp_point[i] = self->point[i];
            }

            self->num_points = 0;
        } else {
            locked.num_stamp_points = 0;
        }
    }

    FSP_CRITICAL_SECTION_EXIT;

    return MP_OBJ_NEW_SMALL_INT(locked.num_stamp_points);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_lcd_touched_obj, lcd_touched);

STATIC mp_obj_t lcd_touches(mp_obj_t self_in) {
    // machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    const char *ev[] = {"NONE", "DOWN", "HOLD", "MOVE", "UP"};

    if (!locked.num_stamp_points) {
        lcd_touched(self_in);
    }

    mp_obj_list_t *list = mp_obj_new_list(locked.num_stamp_points, NULL);

    if (locked.num_stamp_points) {
        // Add elements to the list
        for (uint8_t i = 0; i < locked.num_stamp_points; i++) {
            list->items[i] = mp_obj_new_tuple(3, (mp_obj_t[]) {mp_obj_new_int(locked.stamp_point[i].x), mp_obj_new_int(locked.stamp_point[i].y), mp_obj_new_str(ev[locked.stamp_point[i].event], strlen(ev[locked.stamp_point[i].event]))});
        }

        locked.num_stamp_points = 0;
    }
    return MP_OBJ_FROM_PTR(list);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_lcd_touches_obj, lcd_touches);

STATIC mp_obj_t lcd_deinit(mp_obj_t self_in) {
    machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->isinited) {
        if (FSP_SUCCESS == R_GLCDC_Close(&g_display0_ctrl)) {
            self->isinited = 0;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't close R_GLCDC"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not inited! call LCD.init() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_lcd_deinit_obj, lcd_deinit);

STATIC mp_obj_t lcd_start(mp_obj_t self_in) {
    machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->isstarted) {
        if (FSP_SUCCESS == R_GLCDC_Start(&g_display0_ctrl)) {
            self->isstarted = 1;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't start R_GLCDC"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Olready started!"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_lcd_start_obj, lcd_start);

STATIC mp_obj_t lcd_stop(mp_obj_t self_in) {
    machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->isstarted) {
        if (FSP_SUCCESS == R_GLCDC_Stop(&g_display0_ctrl)) {
            self->isstarted = 0;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Can't stop R_GLCDC"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Not started! call LCD.start() first"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_lcd_stop_obj, lcd_stop);

/*
STATIC mp_obj_t lcd_changebuf(mp_obj_t self_in, mp_obj_t idx){
	machine_lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
	mp_int_t i = mp_obj_get_int(idx);

	self->ischanged = 0;
	if(self->isinited && self->isstarted)
		if(FSP_SUCCESS == R_GLCDC_BufferChange(&g_display0_ctrl, fb_background[i? 1 : 0], DISPLAY_FRAME_LAYER_1))
		{
			self->ischanged = 1;
			self->idx = i;
		}
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_lcd_changebuf_obj, lcd_changebuf);
*/
STATIC const mp_rom_map_elem_t lcd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&machine_lcd_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&machine_lcd_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_start),               MP_ROM_PTR(&machine_lcd_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),                MP_ROM_PTR(&machine_lcd_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_touched),             MP_ROM_PTR(&machine_lcd_touched_obj) },
    { MP_ROM_QSTR(MP_QSTR_touches),             MP_ROM_PTR(&machine_lcd_touches_obj) },
    // control
    // { MP_ROM_QSTR(MP_QSTR_changebuf),           MP_ROM_PTR(&machine_lcd_changebuf_obj) },
};
STATIC MP_DEFINE_CONST_DICT(lcd_locals_dict, lcd_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_lcd_type,
    MP_QSTR_LCD,
    MP_TYPE_FLAG_NONE,
    make_new, machine_lcd_make_new,
    buffer, machine_lcd_set_buffer,
    locals_dict, &lcd_locals_dict
    );

#endif // MODULE_LCD_ENABLED
