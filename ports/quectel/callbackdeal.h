/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

// empty file
#ifndef __CALLBACKDEAL_H__
#define __CALLBACKDEAL_H__

#include "py/runtime.h"

#if MICROPY_QPY_MODULE_BT || MICROPY_QPY_MODULE_BLE
#include "helios_btble.h"
#endif

#if MICROPY_QPY_MODULE_ALIPAY
#include "helios_aliiot.h"
#endif

#if MICROPY_QPY_MACHINE_UART
#include "helios_uart.h"
#endif

#if MICROPY_ENABLE_CALLBACK_DEAL

typedef struct _c_callback_t {
    bool is_method;
    mp_obj_t method_self;
    qstr method_name;
    mp_obj_t cb;
    mp_obj_t arg;
}c_callback_t;

typedef struct{
    uint8_t callback_type_id;
    void *msg;
}st_CallBack_Deal;

enum {
    CALLBACK_TYPE_ID_NONE,
    CALLBACK_TYPE_ID_LOAD_BOUNDMETH,
    CALLBACK_TYPE_ID_EXTINT,
    CALLBACK_TYPE_ID_OSTIMER,
    CALLBACK_TYPE_ID_HWTIMER,
    CALLBACK_TYPE_ID_AUDIO_AUDIO,
    CALLBACK_TYPE_ID_AUDIO_RECORD,
    CALLBACK_TYPE_ID_AUDIO_TTS,
    CALLBACK_TYPE_ID_CAM_CAP,
    CALLBACK_TYPE_ID_VOICECALL_RECORD,
#if MICROPY_QPY_MODULE_QUECIOT
    CALLBACK_TYPE_ID_QUETCH_CALLCB,
#endif
#if MICROPY_QPY_MODULE_POC
    CALLBACK_TYPE_ID_POC,
#endif
#if MICROPY_QPY_MACHINE_KEYPAD
    CALLBACK_TYPE_ID_KEYPAD,
#endif
#if MICROPY_QPY_MACHINE_KEY
	CALLBACK_TYPE_ID_KEY,
#endif
#if MICROPY_QPY_SENSOR_ICC_KEY
	CALLBACK_TYPE_ID_ICCKEY,
#endif
#if MICROPY_QPY_SENSOR_HANDMIC_KEY
	CALLBACK_TYPE_ID_HANDMIC_KEY,
#endif
#if MICROPY_QPY_MODULE_EXTUART_WK2114
	CALLBACK_TYPE_ID_WK2114,
#endif
#if MICROPY_QPY_MODULE_GPIO_ENCODER
	CALLBACK_TYPE_ID_ENCODER,
#endif
#if MICROPY_QPY_MODULE_EXTGPIO_AW9523
	CALLBACK_TYPE_ID_AW9523,
#endif
    CALLBACK_TYPE_ID_TP,
#if MICROPY_QPY_MODULE_BT
	CALLBACK_TYPE_ID_BT,
#endif
#if MICROPY_QPY_MODULE_BLE
	CALLBACK_TYPE_ID_BLE,
#endif
#if MICROPY_QPY_MODULE_ALIPAY
    CALLBACK_TYPE_ID_ALIPAY,
#endif
#if MICROPY_QPY_LPM_WAKEUP
    CALLBACK_TYPE_ID_WAKEUP,
#endif
#if MICROPY_QPY_MACHINE_UART
    CALLBACK_TYPE_ID_UART,
#endif
#if MICROPY_QPY_MODULE_NET
    CALLBACK_TYPE_ID_NET,
#endif
#if MICROPY_QPY_MODULE_DATACALL
    CALLBACK_TYPE_ID_DATACALL,
#endif
#if MICROPY_QPY_MODULE_ALIPAY_COLLECTION
    CALLBACK_TYPE_ID_ALIPAY_COLLECTION,
#endif
#if MICROPY_QPY_MODULE_SMS
    CALLBACK_TYPE_ID_SMS,
#endif
#if MICROPY_QPY_MODULE_JRTC
	CALLBACK_TYPE_ID_JRTC,
#endif
#if MICROPY_QPY_MODULE_SIF
    CALLBACK_TYPE_ID_SIF,
#endif
#if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)
#if MICROPY_QPY_MACHINE_TIMER
    CALLBACK_TYPE_ID_TIMER,
#endif
#endif
#if defined(BOARD_EC600MCN_LA_POC_XBND) || defined(BOARD_EC600MCN_LE_POC_XBND) || defined(BOARD_EC600MEU_LA_POC_XBND)\
 || defined(BOARD_EC600MLA_LA_POC_XBND)
	CALLBACK_TYPE_ID_POCSPEAK,
#endif
#if MICROPY_QPY_MODULE_NFC
    CALLBACK_TYPE_ID_NFC,
#endif
#if MICROPY_QPY_SENSOR_VC9202
	CALLBACK_TYPE_ID_VC9202,
#endif
#if MICROPY_QPY_MODULE_LWM2M
    CALLBACK_TYPE_ID_LWM2M,
#endif
#if MICROPY_QPY_MODULE_ESIM && MICROPY_QPY_MODULE_ESIM_IPA
    CALLBACK_TYPE_ID_ESIM_IPA,
#endif
};

typedef struct CallBack_LoadBoundMeth{
    c_callback_t *callback;
    void *arg;
    struct CallBack_LoadBoundMeth *next;
}st_CallBack_LoadBoundMeth;

typedef struct{
    uint8_t key_event;
    c_callback_t callback;
}st_CallBack_IccKey;

typedef struct{
	uint8_t gpio_number;
    uint8_t key_event;
    c_callback_t callback;
}st_CallBack_Key;

typedef struct{
    uint8_t state;
    c_callback_t callback;
}st_CallBack_TP;

typedef struct{
    uint8_t pin_no;
    uint8_t edge;
    c_callback_t callback;
}st_CallBack_Extint;

typedef struct{
    uint8_t key_value;
    uint8_t key_state;
    c_callback_t callback;
}st_CallBack_Handmic;

typedef struct{
    uint32_t int_type;
    uint32_t port;
    uint32_t read_lenth;
    c_callback_t callback;
}st_CallBack_Wk2114;

typedef struct{
    uint8_t pin_no;
    uint8_t pin_level;
    c_callback_t callback;
}st_CallBack_AW9523;

typedef struct{
    uint32_t spin_direct;
    c_callback_t callback;
}st_CallBack_Encoder;

enum {
    POC_CAL_ELEM_INT,
    POC_CAL_ELEM_STR,
    POC_CAL_ELEM_UINT64,
    POC_CAL_ELEM_ARRAY_INT,
    POC_CAL_ELEM_FLOAT,
};

typedef struct 
{
    int type;
    void* data;
}st_CallBack_POC_ELEM;

typedef struct 
{
    st_CallBack_POC_ELEM *para;
    int len; 
    c_callback_t callback; 
}st_CallBack_POC;


enum {RECORE_TYPE_FILE,RECORE_TYPE_STREAM};
typedef struct{
    int record_type;
    char * p_data;
    int len;
    int res;
    c_callback_t callback;
}st_CallBack_AudioRecord;
typedef struct{
    uint32_t        event;
    uint32_t        errcode;
    void      		*valueT;
    uint32_t        valLen;
    c_callback_t    call_cb;
}st_CallBack_Queth;

typedef struct{
    mp_obj_t fun_in;
	size_t n_args;
	size_t n_kw;
	mp_obj_t *args;
}st_CallBack_lvgl;

typedef struct{
    uint32_t result;
    c_callback_t callback;
}st_CallBack_jrtc;

typedef struct{
    uint8_t event_id;
    uint8_t row;
    uint8_t col;
    c_callback_t callback;
}st_CallBack_Keypad;

#if MICROPY_QPY_SENSOR_VC9202
typedef struct{
    uint32_t work_mode;
    uint16_t value;
    uint32_t measure_mode;
    c_callback_t callback;
}st_CallBack_Sensor_VC9202;
#endif
#if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)
#if MICROPY_QPY_MACHINE_TIMER
typedef struct{
    c_callback_t callback;
}st_CallBack_Timer;
#endif
#endif

#if defined(BOARD_EC600MCN_LA_POC_XBND) || defined(BOARD_EC600MCN_LE_POC_XBND) || defined(BOARD_EC600MEU_LA_POC_XBND)\
 || defined(BOARD_EC600MLA_LA_POC_XBND)
typedef struct{
    uint8_t speak_op;
    int speak_result;
    c_callback_t callback;
}st_CallBack_PocSpeak;
#endif
#if MICROPY_QPY_MACHINE_UART
typedef struct{
    uint64_t ind_type;
    Helios_UARTNum port;
    uint64_t size;
    c_callback_t callback;
}st_CallBack_Uart;
#endif

#if MICROPY_QPY_MODULE_NFC
typedef struct{
    uint16_t oid;
	void* data;
	uint8_t data_len;
    c_callback_t callback;
}st_CallBack_nfc;
#endif

#if MICROPY_QPY_MODULE_NET
typedef struct{
    int32_t event_id;
	int32_t status;
	int32_t act;
	int32_t lac;
	int32_t cid;
    c_callback_t callback;
}st_CallBack_Net;
#endif

#if MICROPY_QPY_MODULE_DATACALL
typedef struct{
    int32_t profile_idx;
	uint8_t sim_id;
	int32_t nw_status;
    c_callback_t callback;
}st_CallBack_Datacall;
#endif

#if MICROPY_QPY_MODULE_SMS
typedef struct{
	uint8_t sim_id;
	uint8_t index;
	char storage[5];
    c_callback_t callback;
}st_CallBack_SMS;
#endif

#if MICROPY_QPY_MODULE_BT || MICROPY_QPY_MODULE_BLE
typedef struct{
    //void *msg;
	Helios_Event event;
    c_callback_t callback;
}st_CallBack_BT;
#endif

#if MICROPY_QPY_MODULE_ALIPAY
typedef struct{
    helios_pay_msg_cmd_t cmd;
    helios_pay_rsp_code_t rsp_code;
    char *result;
    int result_len;
    void *userdata; 
    c_callback_t callback;
}st_CallBack_ALIPAY;
#endif

#if MICROPY_QPY_MODULE_ALIPAY_COLLECTION
typedef struct{
    int msgID;
    int data;
    c_callback_t callback;
}st_CallBack_ALIPAY_COLLECTION;
#endif

#if MICROPY_QPY_LPM_WAKEUP
typedef struct{
    uint8_t level;
    c_callback_t callback;
} st_CallBack_Wakeup;
#endif

#if MICROPY_QPY_MODULE_SIF
typedef struct{
    int data_len;
    uint8_t *data;
    c_callback_t callback;
} st_CallBack_SIF;
#endif

bool mp_sched_schedule_callback_register(c_callback_t *callback, mp_obj_t func_obj);
bool mp_sched_schedule_ex(c_callback_t *callback, mp_obj_t arg);

void qpy_callback_deal_init(void);
void qpy_callback_deal_deinit(void);
int qpy_send_msg_to_callback_deal_thread(uint8_t id, void *msg);
bool qpy_is_callback_deal_thread(void);
st_CallBack_LoadBoundMeth * qpy_callback_para_node_add(c_callback_t *callback, mp_obj_t arg);
void qpy_callback_para_node_delete(st_CallBack_LoadBoundMeth *cb_msg);
void qpy_callback_para_link_free_all(void);
void qpy_callback_para_link_mark_in_used(void);
#endif
#endif //__CALLBACKDEAL_H__