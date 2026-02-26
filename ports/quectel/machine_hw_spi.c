/*
 * Copyright (c) Quectel Wireless Solution, Co., Ltd.All Rights Reserved.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  
 *     http://www.apache.org/licenses/LICENSE-2.0
 *  
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "mphalport.h"

#if MICROPY_QPY_MACHINE_SPI

#include "helios_spi.h"
#include "helios_debug.h"


#define HELIOS_SPI_LOG(msg, ...)      custom_log("machine spi", msg, ##__VA_ARGS__)


const mp_obj_type_t machine_hard_spi_type;

typedef struct _machine_hard_spi_obj_t {
    mp_obj_base_t base;
	uint32_t port;
	uint32_t mode;
	uint32_t clk;
} machine_hard_spi_obj_t;

static void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = self_in;
    mp_printf(print, "spi%d", self->port);
}

mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_port, ARG_mode, ARG_clk };
    static const mp_arg_t allowed_args[] = {
		{ MP_QSTR_port, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
		{ MP_QSTR_clk, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	uint32_t port = args[ARG_port].u_int;
	uint32_t mode = args[ARG_mode].u_int;
	uint32_t clk = args[ARG_clk].u_int;
    int ret=0;

    if (port > HELIOS_SPI3) {
       mp_raise_ValueError(MP_ERROR_TEXT("port must be (0~3)"));
    }
	
	if ( mode > 3) {
       mp_raise_ValueError(MP_ERROR_TEXT("mode must be (0~3)"));
    }

#if defined(PLAT_Unisoc)	
	if (clk > 9) {
       mp_raise_ValueError(MP_ERROR_TEXT("clk must be (0~9)"));
    }
#elif defined(PLAT_RDA)
	if (clk > 39) {
       mp_raise_ValueError(MP_ERROR_TEXT("clk must be (0~39)"));
    }
#elif defined(PLAT_Unisoc_8850)
if (clk > 14) 
{
   mp_raise_ValueError(MP_ERROR_TEXT("clk must be (0~14)"));
}
#elif defined(PLAT_SONY_ALT1350)
if (clk > 30) 
{
   mp_raise_ValueError(MP_ERROR_TEXT("clk must be (0~30)"));
}
#else
	if (clk > 6) {
	   mp_raise_ValueError(MP_ERROR_TEXT("clk must be (0~6)"));
	}
#endif


    machine_hard_spi_obj_t *self = m_new_obj(machine_hard_spi_obj_t);

    self->base.type = &machine_hard_spi_type;
	self->port = port;
	self->mode = mode;
	self->clk = clk;
	
	ret = Helios_SPI_Init((Helios_SPINum) self->port, (Helios_SPIMode) self->mode, (uint32_t) self->clk);
	if(ret != 0) {
		mp_raise_ValueError(MP_ERROR_TEXT("spi init fail"));
	}
	
    HELIOS_SPI_LOG("Helios_SPI_Init %d\r\n",ret);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_arg_t machine_spi_mem_allowed_args[] = {
    { MP_QSTR_databuf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_datasize,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
};
static mp_obj_t machine_spi_write_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_databuf, ARG_datasize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_spi_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_spi_mem_allowed_args), machine_spi_mem_allowed_args, args);
		
	machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_databuf].u_obj, &bufinfo, MP_BUFFER_READ);
	
	int length = (size_t)args[ARG_datasize].u_int > bufinfo.len ? bufinfo.len : (size_t)args[ARG_datasize].u_int;
    // do the transfer
	
	int ret = Helios_SPI_Write((Helios_SPINum) self->port, (void*) bufinfo.buf, (size_t) length);
    if (ret < 0) {
        HELIOS_SPI_LOG("Helios_SPI_Write ret=%d\r\n",ret);
        mp_raise_OSError(-ret);
    }
    
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_write_obj, 1, machine_spi_write_mem);

static mp_obj_t machine_spi_read_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_databuf, ARG_datasize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_spi_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_spi_mem_allowed_args), machine_spi_mem_allowed_args, args);
		
	machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    // get the buffer to read the data into
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_databuf].u_obj, &bufinfo, MP_BUFFER_WRITE);
	
	int length = (size_t)args[ARG_datasize].u_int > bufinfo.len ? bufinfo.len : (size_t)args[ARG_datasize].u_int;
	
    // do the transfer
	int ret = Helios_SPI_Read((Helios_SPINum) self->port, (void*) bufinfo.buf, (size_t) length);
    if (ret < 0) {
        HELIOS_SPI_LOG("spi read ret=%d\r\n",ret);
        mp_raise_OSError(-ret);
    }

    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_read_obj, 1, machine_spi_read_mem);
#ifndef PLAT_SONY_ALT1350
static const mp_arg_t machine_spi_write_read_mem_allowed_args[] = {
	{ MP_QSTR_readbuf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_writebuf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_datasize,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
};
static mp_obj_t machine_spi_write_read_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_readbuf, ARG_writebuf, ARG_datasize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_spi_write_read_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_spi_write_read_mem_allowed_args), machine_spi_write_read_mem_allowed_args, args);
		
	machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    // get the buffer to read the data into
    mp_buffer_info_t readbuf;
    mp_get_buffer_raise(args[ARG_readbuf].u_obj, &readbuf, MP_BUFFER_WRITE);
	mp_buffer_info_t writebuf;
    mp_get_buffer_raise(args[ARG_writebuf].u_obj, &writebuf, MP_BUFFER_READ);
	
	int length = ((size_t)args[ARG_datasize].u_int > readbuf.len ? readbuf.len : (size_t)args[ARG_datasize].u_int);
	
	
    // do the transfer
	int ret = Helios_SPI_WriteRead((Helios_SPINum) self->port,(void*) readbuf.buf, (size_t) length,(void*) writebuf.buf,(size_t)args[ARG_datasize].u_int);
    
    if (ret < 0) {
        HELIOS_SPI_LOG("Helios_SPI_WriteRead ret=%d\r\n",ret);
        mp_raise_OSError(-ret);
    }
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_write_read_obj, 1, machine_spi_write_read_mem);

#else
static const mp_arg_t machine_spi_half_duplex_write_read_mem_allowed_args[] = {
	{ MP_QSTR_readbuf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_writebuf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_write_datasize,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_read_datasize,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
};
static mp_obj_t machine_spi_half_duplex_write_read_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_readbuf, ARG_writebuf, ARG_write_datasize, ARG_read_datasize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_spi_half_duplex_write_read_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_spi_half_duplex_write_read_mem_allowed_args), machine_spi_half_duplex_write_read_mem_allowed_args, args);
		
	machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    // get the buffer to read the data into
    mp_buffer_info_t readbuf;
    mp_get_buffer_raise(args[ARG_readbuf].u_obj, &readbuf, MP_BUFFER_WRITE);
	mp_buffer_info_t writebuf;
    mp_get_buffer_raise(args[ARG_writebuf].u_obj, &writebuf, MP_BUFFER_READ);
	
	int write_length = ((size_t)args[ARG_write_datasize].u_int > writebuf.len ? writebuf.len : (size_t)args[ARG_write_datasize].u_int);
    int read_length = ((size_t)args[ARG_read_datasize].u_int > readbuf.len ? readbuf.len : (size_t)args[ARG_read_datasize].u_int);
	
	
    // do the transfer
	int ret = Helios_SPI_WriteRead((Helios_SPINum) self->port,(void*) readbuf.buf, (size_t) read_length,(void*) writebuf.buf,(size_t)write_length);
    
    if (ret < 0) {
        HELIOS_SPI_LOG("Helios_SPI_WriteRead ret=%d\r\n",ret);
        mp_raise_OSError(-ret);
    }
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_half_duplex_write_read_obj, 1, machine_spi_half_duplex_write_read_mem);
#endif
static const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_spi_write_obj) },
#ifndef PLAT_SONY_ALT1350
	{ MP_ROM_QSTR(MP_QSTR_write_read), MP_ROM_PTR(&machine_spi_write_read_obj) },
#else
    { MP_ROM_QSTR(MP_QSTR_half_duplex_write_read), MP_ROM_PTR(&machine_spi_half_duplex_write_read_obj) },
#endif
#if !defined(BOARD_EC800GCN_GA)      
    { MP_ROM_QSTR(MP_QSTR_SPI0), MP_ROM_INT(HELIOS_SPI0) },
#endif         
#if !defined(PLAT_RDA) && !defined(BOARD_EG915UEU_AB)
    { MP_ROM_QSTR(MP_QSTR_SPI1), MP_ROM_INT(HELIOS_SPI1) },
#endif
};

MP_DEFINE_CONST_DICT(mp_machine_hard_spi_locals_dict, machine_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_hard_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_hard_spi_make_new,
    print, machine_hard_spi_print,
    locals_dict, &mp_machine_hard_spi_locals_dict
    );

#endif
