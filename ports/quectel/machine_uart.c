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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "runtime.h"
#include "stream.h"
#include "mperrno.h"
#include "mphalport.h"

#if MICROPY_QPY_MACHINE_UART

#include "helios_uart.h"
#include "helios_pin.h"
#include "helios_debug.h"
#include "callbackdeal.h"

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    unsigned int uart_num;
    Helios_UARTConfig config;
} machine_uart_obj_t;

static c_callback_t *callback_cur[HELIOS_UARTMAX] = {0};

static machine_uart_obj_t *uart_self_obj[HELIOS_UARTMAX] = {0};


const mp_obj_type_t machine_uart_type;
static const char *_parity_name[] = {"None", "1", "0"};

/******************************************************************************/
// MicroPython bindings for UART

static void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, flow=%u",
        self->uart_num, self->config.baudrate, self->config.data_bit, _parity_name[self->config.parity_bit],
        self->config.stop_bit, self->config.flow_ctrl);
    mp_printf(print, ")");
}

static void machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop,  ARG_flow};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_parity, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
		{ MP_QSTR_flow, MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	// get baudrate bits
    switch (args[ARG_baudrate].u_int) {
        case HELIOS_UART_BAUD_300:
		case HELIOS_UART_BAUD_600:
		case HELIOS_UART_BAUD_1200:
		case HELIOS_UART_BAUD_2400:
		case HELIOS_UART_BAUD_3600:
		case HELIOS_UART_BAUD_4800:
		case HELIOS_UART_BAUD_7200:
		case HELIOS_UART_BAUD_9600:
		case HELIOS_UART_BAUD_14400:
		case HELIOS_UART_BAUD_19200:
		case HELIOS_UART_BAUD_28800:
		case HELIOS_UART_BAUD_33600:
		case HELIOS_UART_BAUD_38400:
		case HELIOS_UART_BAUD_57600:
		case HELIOS_UART_BAUD_115200:
		case HELIOS_UART_BAUD_230400:
		case HELIOS_UART_BAUD_460800:
		case HELIOS_UART_BAUD_921600:
		case HELIOS_UART_BAUD_1000000:
		case HELIOS_UART_BAUD_1842000:
		case HELIOS_UART_BAUD_3686400:
		case HELIOS_UART_BAUD_4468750:
            self->config.baudrate = args[ARG_baudrate].u_int;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid baudrate"));
            break;
    }

    // get data bits
#if defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
	switch (args[ARG_bits].u_int) {
        case 8:
            self->config.data_bit = HELIOS_UART_DATABIT_8;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid data bits, Unisoc platform only support 8 data bit."));
            break;
    }
#else
    switch (args[ARG_bits].u_int) {
        case 5:
            self->config.data_bit = HELIOS_UART_DATABIT_5;
            break;
        case 6:
            self->config.data_bit = HELIOS_UART_DATABIT_6;
            break;
        case 7:
            self->config.data_bit = HELIOS_UART_DATABIT_7;
            break;
        case 8:
            self->config.data_bit = HELIOS_UART_DATABIT_8;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid data bits"));
            break;
    }
#endif
	// get parity bits
    switch (args[ARG_parity].u_int) {
        case 0:
			self->config.parity_bit = HELIOS_UART_PARITY_NONE;
            break;
        case 1:
            self->config.parity_bit = HELIOS_UART_PARITY_EVEN;
            break;
        case 2:
            self->config.parity_bit = HELIOS_UART_PARITY_ODD;
            break;
		#if defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1602)
		case 3:
            self->config.parity_bit = HELIOS_UART_PARITY_MARK;
            break;	
		case 4:
            self->config.parity_bit = HELIOS_UART_PARITY_SPACE;
            break;
		#endif
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid parity bits"));
            break;
    }

    // get stop bits
    switch (args[ARG_stop].u_int) {
        case 1:
            self->config.stop_bit = HELIOS_UART_STOP_1;
            break;
        case 2:
            self->config.stop_bit = HELIOS_UART_STOP_2;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
            break;
    }

	// get flow bits
    switch (args[ARG_flow].u_int) {
        case 0:
			self->config.flow_ctrl = HELIOS_UART_FC_NONE;
            break;
        case 1:
            self->config.flow_ctrl = HELIOS_UART_FC_HW;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid flow bits"));
            break;
    }
	Helios_UARTInitStruct uart_para = {0};
	Helios_UARTConfig uart_config = {0};
	uart_para.uart_config = &uart_config;
	
	memcpy((void*)&uart_config,(void*)&self->config, sizeof(Helios_UARTConfig));
	if(self->uart_num == QPY_REPL_UART) {
		mp_hal_port_open(0);
	}
//Helios_UART_Init has created a timer in kernal and Helios_UART_Deinit will delete this timer of Sony.
//So if we deint uart first, moudle will dump.
#ifndef PLAT_SONY_ALT1350
		Helios_UART_Deinit((Helios_UARTNum) self->uart_num);
#endif

	if(Helios_UART_Init((Helios_UARTNum) self->uart_num, &uart_para) != 0) {
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) init fail"), self->uart_num);
	}
}

static mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get uart id
    mp_int_t uart_num = mp_obj_get_int(args[0]);
    if (uart_num < HELIOS_UART0 || uart_num > HELIOS_UARTMAX) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) does not exist"), uart_num);
    }

    // create instance
    if (uart_self_obj[uart_num] == NULL)
    {
    	uart_self_obj[uart_num] = mp_obj_malloc_with_finaliser(machine_uart_obj_t, &machine_uart_type);
    }
	machine_uart_obj_t *self = uart_self_obj[uart_num];
	
    self->base.type = &machine_uart_type;
    self->uart_num = uart_num;
	self->config.baudrate = HELIOS_UART_BAUD_115200;
	self->config.data_bit = HELIOS_UART_DATABIT_8;
	self->config.stop_bit = HELIOS_UART_STOP_1;
	self->config.parity_bit = HELIOS_UART_PARITY_NONE;
	self->config.flow_ctrl = HELIOS_UART_FC_NONE;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

static mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
	
	int ret = Helios_UART_Deinit((Helios_UARTNum) self->uart_num);
	if(self->uart_num == QPY_REPL_UART) {
		mp_hal_port_open(1);
	}
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);


static mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t rxbufsize;
    rxbufsize = Helios_UART_Any((Helios_UARTNum) self->uart_num);
    return MP_OBJ_NEW_SMALL_INT(rxbufsize);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

#if defined (PLAT_ASR) || defined (PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8850_R02) \
	|| defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_EIGEN) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_ASR_1602) || defined(PLAT_aic8800m40) || defined(PLAT_Unisoc_8910_R06)
enum{ARG_self, ARG_gipo_n, ARG_direc, ARG_block};
static mp_obj_t machine_uart_485_control(mp_uint_t n_args, const mp_obj_t *args){
	int ret = -1;
	if(n_args < 3)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid parameter"));
	}
	machine_uart_obj_t *self = MP_OBJ_TO_PTR(args[ARG_self]);
	size_t gpio = mp_obj_get_int(args[ARG_gipo_n]);
	Helios_UART_Direc dire = (Helios_UART_Direc)mp_obj_get_int(args[ARG_direc]);
	ret = Helios_UART_SetCtl_485(self->uart_num,gpio,dire);
#if defined(PLAT_Unisoc_8850)
	int set = 0;
	if(n_args >= 4)
	{
		set = mp_obj_get_int(args[ARG_block]);
	}
	Helios_UART_SetBlockMode_485((char)set);
#endif
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_uart_485_control_obj, 3, 4, (mp_obj_t)machine_uart_485_control);

#endif

void helios_uart_callback_to_python(uint64_t ind_type, Helios_UARTNum port, uint64_t size)
{  	
	if(callback_cur[port] == NULL) {
		return;
	}
    st_CallBack_Uart *Uart_t = malloc(sizeof(st_CallBack_Uart));
    if(NULL != Uart_t) 
    {                                                                
	    Uart_t->ind_type = ind_type;                                                             
	    Uart_t->port = port;                                                            
	    Uart_t->size = size;  
        Uart_t->callback = *callback_cur[port];
	    qpy_send_msg_to_callback_deal_thread(CALLBACK_TYPE_ID_UART, Uart_t); 
	}
}



static mp_obj_t helios_uart_set_callback(mp_obj_t self_in, mp_obj_t callback)
{
	machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
	static c_callback_t cb[sizeof(callback_cur) / sizeof(callback_cur[0])] = {0};
    memset(&cb[self->uart_num], 0, sizeof(c_callback_t));
    cb[self->uart_num].arg = mp_obj_new_tuple(3, NULL);
	callback_cur[self->uart_num] = &cb[self->uart_num];
	mp_sched_schedule_callback_register(callback_cur[self->uart_num], callback);
	
	int ret = Helios_UART_SetCallback((Helios_UARTNum) self->uart_num, (Helios_UARTCallback) helios_uart_callback_to_python);
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_set_callback_obj, helios_uart_set_callback);

static mp_obj_t helios_uart_deinit(mp_obj_t self_in)
{
    int ret = 0;
	
	machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

	uart_self_obj[self->uart_num] = NULL;
	callback_cur[self->uart_num] = NULL;
	ret = Helios_UART_Deinit((Helios_UARTNum) self->uart_num);

	if(self->uart_num == QPY_REPL_UART) {
		mp_hal_port_open(1);
	}

	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(helios_uart_deinit_obj, helios_uart_deinit);

#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
static mp_obj_t machine_uart_rx_disable(mp_obj_t self_in,mp_obj_t set) 
{
    int ret = -1;
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t set_value = mp_obj_get_int(set);
    if(set_value > 1)
    {
        mp_raise_ValueError(MP_ERROR_TEXT("setting_value must be 0 or 1"));
    }
    ret = Helios_Uart_Disable_Rx(self->uart_num,set_value);
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_rx_disable_obj, machine_uart_rx_disable);
#endif

static const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___del__), 	MP_ROM_PTR(&helios_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_callback), MP_ROM_PTR(&machine_uart_set_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeOnce), MP_ROM_PTR(&mp_stream_write1_obj) },
    { MP_ROM_QSTR(MP_QSTR_readOnce), MP_ROM_PTR(&mp_stream_read1_obj) },
	
#if defined(PLAT_RDA)
	{ MP_ROM_QSTR(MP_QSTR_UART1), MP_ROM_INT(HELIOS_UART1) },
#endif
#if defined (PLAT_ASR) || defined (PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8850_R02) \
    || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_EIGEN) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_ASR_1602) || defined(PLAT_aic8800m40) || defined(PLAT_Unisoc_8910_R06)
	{ MP_ROM_QSTR(MP_QSTR_control_485), MP_ROM_PTR(&machine_uart_485_control_obj) },
	PLAT_GPIO_DEF(PLAT_GPIO_NUM),
#endif
#if defined (PLAT_ASR) || defined(PLAT_Qualcomm) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_EIGEN) \
    || defined(PLAT_ASR_1602) || defined(PLAT_ECR6600) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(BOARD_BC32RA) || defined(BOARD_BC92RB) \
    || defined(PLAT_EIGEN_718) || defined(PLAT_aic8800m40)
    { MP_ROM_QSTR(MP_QSTR_UART0), MP_ROM_INT(HELIOS_UART0) },
#endif

#if !defined(PLAT_RDA)
#if !defined(PLAT_ECR6600)
	{ MP_ROM_QSTR(MP_QSTR_UART1), MP_ROM_INT(HELIOS_UART1) },
#endif
    { MP_ROM_QSTR(MP_QSTR_UART2), MP_ROM_INT(HELIOS_UART2) },
#if !defined(PLAT_ECR6600)
    { MP_ROM_QSTR(MP_QSTR_UART3), MP_ROM_INT(HELIOS_UART3) },
#endif
#endif

#if defined(PLAT_Qualcomm) \
	|| defined (PLAT_Unisoc) \
	|| defined(PLAT_Unisoc_8910_R05) \
	|| defined(PLAT_Unisoc_8910_R06) \
	|| defined(BOARD_EC600GCN_LD) \
    || defined(BOARD_EC600GCN_LA) \
    || defined(BOARD_EC600GCN_LA_CDD) \
	|| defined(BOARD_EC600GCN_LD_YM) \
	|| defined(BOARD_EG700GCN_LC) \
	|| defined (PLAT_ASR) \
    || MICROPY_QPY_UART4
	{ MP_ROM_QSTR(MP_QSTR_UART4), MP_ROM_INT(HELIOS_UART4) },
#endif

#if defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8850_R02) || MICROPY_QPY_USB_DOUBLE_CDC
    { MP_ROM_QSTR(MP_QSTR_UART5), MP_ROM_INT(HELIOS_UART5) },
#endif

#if defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8850_R02)
    { MP_ROM_QSTR(MP_QSTR_UART6), MP_ROM_INT(HELIOS_UART6) },
#endif

#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
    { MP_ROM_QSTR(MP_QSTR_rx_auto_disable), MP_ROM_PTR(&machine_uart_rx_disable_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_REPL_UART), MP_ROM_INT(QPY_REPL_UART) },
};

static MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

static mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }
    
	int bytes_read = Helios_UART_Read((Helios_UARTNum) self->uart_num, buf_in, size);
    if (bytes_read < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_read;
}

static mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int bytes_written = Helios_UART_Write((Helios_UARTNum) self->uart_num, (void*)buf_in, size);
    if (bytes_written < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // return number of bytes written
    return bytes_written;
}

static mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    //machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = (mp_uint_t)arg;
        ret = 0;
        size_t rxbufsize = 1;
        if ((flags & MP_STREAM_POLL_RD) && rxbufsize > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && 1) { // FIXME: uart_tx_any_room(self->uart_num)
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = machine_uart_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_NONE,
    make_new, machine_uart_make_new,
    print, machine_uart_print,
    protocol, &uart_stream_p,
    locals_dict, &machine_uart_locals_dict
    );

#endif
