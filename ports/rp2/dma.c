/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2023 Mark Burton
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

#include "string.h"

#include "py/gc.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "shared/runtime/mpirq.h"

#include "hardware/dma.h"

#define DBG_PRINTF(...)  mp_printf(&mp_plat_print, "rp2.dma: " __VA_ARGS__)

extern const mp_obj_type_t dma_DMA_type;
extern const mp_obj_type_t dma_Timer_type;

/*****************************************************************************
 * Data types
*/
typedef struct _dma_DMA_obj_t {
    mp_obj_base_t base;
    uint8_t dma_channel;
    uint32_t requests;
    uint32_t transfers;
    mp_obj_t handler;
} dma_DMA_obj_t;

typedef struct _dma_Timer_obj_t {
    mp_obj_base_t base;
    uint8_t timer;
} dma_Timer_obj_t;

/******************************************************************************
 * Private methods
*/
void dma_irq_handler()
{
    uint32_t insts0 = dma_hw->ints0;
    // clear the dma irq
    dma_hw->ints0 = dma_hw->ints0;

    dma_DMA_obj_t *dma_callback = NULL;
    mp_obj_t callback = mp_const_none;
    
    for(uint8_t i=0;i<NUM_DMA_CHANNELS;i++)
    {
        if(insts0 & (1 << i))
        {
            dma_callback = MP_STATE_PORT(dma_DMA_obj_all)[i];
            if(dma_callback)
            {
                callback = dma_callback->handler;
                if(callback != mp_const_none)
                {
                    mp_sched_lock();
                    // When executing code within a handler we must lock the GC to prevent
                    // any memory allocations.  We must also catch any exceptions.
                    gc_lock();
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) 
                    {
                        uint32_t status =  dma_channel_hw_addr(i)->ctrl_trig;
                        mp_call_function_1(callback, mp_obj_new_int_from_uint((status >> 29) & 3));
                        dma_callback->handler = mp_const_none;
                        nlr_pop();
                    } 
                    else 
                    {}

                    gc_unlock();
                    mp_sched_unlock();
                }
            }
        }
    }
}

void dma_irq_init(dma_DMA_obj_t *self, mp_obj_t handler)
{
    self->handler = handler;
    MP_STATE_PORT(dma_DMA_obj_all)[self->dma_channel] = self;
    dma_channel_set_irq0_enabled(self->dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

 void check_init(dma_DMA_obj_t *self)
 {
    if(self->dma_channel == 0xff)
    {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("DMA channel not claimed")); 
    }
 }


uint32_t dma_get_error_status(uint channel)
{
    dma_channel_hw_t* dma = dma_channel_hw_addr(channel);
    return (dma->ctrl_trig>>29) & 7;
}

void dma_check_and_raise_status(dma_DMA_obj_t *self,dma_channel_config* config)
{
    uint32_t status =  dma_get_error_status(self->dma_channel);
    if(status & 0x04) 
    {
        dma_channel_hw_t* dma = dma_channel_hw_addr(self->dma_channel);
        channel_config_set_enable(config, false);
        // disable the channel on IRQ0
        dma_channel_set_irq0_enabled(self->dma_channel, false);
        // abort the channel
        dma_channel_abort(self->dma_channel);
        // clear the spurious IRQ (if there was one)
        dma_channel_acknowledge_irq0(self->dma_channel);
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("DMA transfer error %d read addr: 0x%08x write addr: 0x%08x"), status & 3,dma->read_addr, dma->write_addr); 
    }
}

void dma_start_request(dma_DMA_obj_t *self, 
                       dma_channel_config* config, 
                       void * to_address_ptr,
                       void *from_address_ptr,
                       uint32_t transfer_count,
                       uint32_t dreq,
                       uint32_t data_size,
                       mp_obj_t handler)
{
    if(dreq != 0)
        channel_config_set_dreq(config, dreq);
    
    channel_config_set_transfer_data_size(config, data_size);

    // IRQ stuff
    if(mp_obj_is_callable(handler))
    {
        dma_irq_init(self, handler);
    }
    else
    {
        dma_channel_set_irq0_enabled(self->dma_channel, false);
    }
    
    // configure a one shot DMA request.
    dma_channel_configure(self->dma_channel, config,
        to_address_ptr,   // Destinatinon pointer
        from_address_ptr, // Source pointer
        transfer_count,   // Number of transfers
        true              // Start immediately
    );

    dma_check_and_raise_status(self, config);
}


MP_REGISTER_ROOT_POINTER(struct _dma_DMA_obj_t *dma_DMA_obj_all[NUM_DMA_CHANNELS]);

void dma_init(void)
{
    memset(MP_STATE_PORT(dma_DMA_obj_all), 0, sizeof(MP_STATE_PORT(dma_DMA_obj_all)));
}

STATIC mp_obj_t dma_DMA_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    dma_DMA_obj_t *self = mp_obj_malloc(dma_DMA_obj_t, &dma_DMA_type);
    self->dma_channel = 0xff;
    self->handler = mp_const_none;
    self->requests = 0;
    self->transfers = 0;
    return MP_OBJ_FROM_PTR(self);
}
STATIC void dma_DMA_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<DMA channel=%d request count=%d transfers=%d>", self->dma_channel, self->requests, self->transfers);
}

STATIC mp_obj_t dma_DMA_claim(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->dma_channel = dma_claim_unused_channel(false);
    if(self->dma_channel == -1)
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("DMA No channels available")); 

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_claim_obj, dma_DMA_claim);

STATIC mp_obj_t dma_DMA_unclaim(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(self->dma_channel != 0xff)
    {
        dma_irqn_set_channel_enabled(DMA_IRQ_0, self->dma_channel, false);
        dma_channel_abort(self->dma_channel);  
        dma_channel_unclaim(self->dma_channel);
        self->dma_channel = 0xff;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_unclaim_obj, dma_DMA_unclaim);
 
STATIC mp_obj_t dma_DMA_isclaimed(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool claimed = dma_channel_is_claimed(self->dma_channel);
    if(claimed)
        return mp_const_true;
    else
        return mp_const_false;
    
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_isclaimed_obj, dma_DMA_isclaimed);
 

/*****************************************************************************
 * write_from
 * 
 * Arguments
 * from_buffer - bytearray 
 * data_size - 1,2,4 bytes (use constant)
 * transfer_count - 
 * to_address - address to send data
 * dreq - optional throttle transfer. Default is to go as fast as possible
 * handler - optional IRQ handler
 * 
*/
STATIC mp_obj_t dma_DMA_write_from(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_from_buffer, ARG_to_address, ARG_dreq, ARG_transfer_count, ARG_data_size, ARG_handler};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_from_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_to_address, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_dreq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_transfer_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_data_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DMA_SIZE_8} },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    
    check_init(self);

    void * to_address_ptr = (void*)args[ARG_to_address].u_int;
    uint32_t dreq = args[ARG_dreq].u_int;
    uint32_t transfer_count = args[ARG_transfer_count].u_int;
    uint32_t data_size = args[ARG_data_size].u_int;
    mp_obj_t handler = args[ARG_handler].u_obj;
    
    mp_buffer_info_t bufinfo;
    bufinfo.buf = NULL;
    mp_get_buffer_raise(args[ARG_from_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    if(transfer_count == 0)
        transfer_count = bufinfo.len >> data_size;

    self->requests++;
    self->transfers+=transfer_count;
    
    dma_channel_config config = dma_channel_get_default_config(self->dma_channel);
    
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);

    dma_start_request(self, &config, to_address_ptr, bufinfo.buf, transfer_count, dreq, data_size, handler);

    return mp_obj_new_int(bufinfo.len);
    
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(dma_DMA_write_from_obj, 1, dma_DMA_write_from);


/*****************************************************************************
 * read_into
 * 
 * Arguments
 * from_address - address to send data
 * data_size - 1,2,4 bytes (use constant)
 * transfer_count - 
 * to_buffer - bytearray 
 * dreq - optional throttle transfer. Default is to go as fast as possible
 * handler - optional IRQ handler
 * 
*/
STATIC mp_obj_t dma_DMA_read_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_from_address, ARG_to_buffer, ARG_transfer_count, ARG_data_size, ARG_dreq,  ARG_handler};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_from_address, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_to_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_transfer_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_data_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DMA_SIZE_8} },
        { MP_QSTR_dreq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_init(self);
    
    void * from_address_ptr = (void*)(args[ARG_from_address].u_int);

    uint32_t dreq = args[ARG_dreq].u_int;
    uint32_t transfer_count = args[ARG_transfer_count].u_int;
    uint32_t data_size = args[ARG_data_size].u_int;
    mp_obj_t handler = args[ARG_handler].u_obj;

    mp_buffer_info_t bufinfo;
    bufinfo.buf = NULL;
    mp_get_buffer_raise(args[ARG_to_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    if(transfer_count == 0)
        transfer_count = bufinfo.len >> data_size;

    self->requests++;
    self->transfers+=transfer_count;

    dma_channel_config config = dma_channel_get_default_config(self->dma_channel);
    channel_config_set_read_increment(&config, false);
    channel_config_set_write_increment(&config, true);

    dma_start_request(self, &config, bufinfo.buf, from_address_ptr, transfer_count, dreq, data_size, handler);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(dma_DMA_read_into_obj, 1, dma_DMA_read_into);

/*****************************************************************************
 * copy
 * 
 * Arguments
 * from_buffer - source bytearray
 * data_size - 1,2,4 bytes (use constant)
 * to_buffer - bytearray 
 * handler - optional IRQ handler
 * 
*/
STATIC mp_obj_t dma_DMA_copy(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_from_buffer, ARG_to_buffer, ARG_handler};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_from_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_to_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_handler, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_buffer_info_t from_buffer;
    from_buffer.buf = NULL;
    mp_get_buffer_raise(args[ARG_from_buffer].u_obj, &from_buffer, MP_BUFFER_READ);

    mp_buffer_info_t to_buffer;
    to_buffer.buf = NULL;
    mp_get_buffer_raise(args[ARG_to_buffer].u_obj, &to_buffer, MP_BUFFER_WRITE);
    mp_obj_t handler = args[ARG_handler].u_obj;

    if (from_buffer.len > to_buffer.len)
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("DMA buffer overrun %d > %d "), from_buffer.len,  to_buffer.len); 

    self->requests++;
    self->transfers+=from_buffer.len;

    dma_channel_config config = dma_channel_get_default_config(self->dma_channel);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, true);

    dma_start_request(self, &config, to_buffer.buf, from_buffer.buf, from_buffer.len, 0x3f, DMA_SIZE_8, handler);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(dma_DMA_copy_obj, 1, dma_DMA_copy);

STATIC mp_obj_t dma_DMA_isbusy(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(dma_channel_is_busy(self->dma_channel));
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_isbusy_obj, dma_DMA_isbusy);

STATIC mp_obj_t dma_DMA_error_status(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t status =  dma_channel_hw_addr(self->dma_channel)->ctrl_trig;
    return mp_obj_new_int_from_uint((status >> 29) & 3);
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_error_status_obj, dma_DMA_error_status);

STATIC mp_obj_t dma_DMA_clear_error(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    hw_set_bits(&dma_hw->ch[self->dma_channel].al1_ctrl, 3u << 29);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_clear_error_obj, dma_DMA_clear_error);

STATIC mp_obj_t dma_DMA_request_count(mp_obj_t self_in) {
    dma_DMA_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->requests);

}
MP_DEFINE_CONST_FUN_OBJ_1(dma_DMA_request_count_obj, dma_DMA_request_count);

STATIC const mp_rom_map_elem_t dma_DMA_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_claim),          MP_ROM_PTR(&dma_DMA_claim_obj) },
    { MP_ROM_QSTR(MP_QSTR_unclaim),        MP_ROM_PTR(&dma_DMA_unclaim_obj) },
    { MP_ROM_QSTR(MP_QSTR_isclaimed),      MP_ROM_PTR(&dma_DMA_isclaimed_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_from),     (mp_obj_t)&dma_DMA_write_from_obj },
    { MP_ROM_QSTR(MP_QSTR_read_into),      MP_ROM_PTR(&dma_DMA_read_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy),           MP_ROM_PTR(&dma_DMA_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_isbusy),         MP_ROM_PTR(&dma_DMA_isbusy_obj) },
    { MP_ROM_QSTR(MP_QSTR_error_status),   (mp_obj_t)&dma_DMA_error_status_obj },
    { MP_ROM_QSTR(MP_QSTR_clear_error),    MP_ROM_PTR(&dma_DMA_clear_error_obj) },
    { MP_ROM_QSTR(MP_QSTR_request_count),  MP_ROM_PTR(&dma_DMA_request_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_DMA_SIZE_8),     MP_ROM_INT(DMA_SIZE_8) },
    { MP_ROM_QSTR(MP_QSTR_DMA_SIZE_16),    MP_ROM_INT(DMA_SIZE_16) },
    { MP_ROM_QSTR(MP_QSTR_DMA_SIZE_32),    MP_ROM_INT(DMA_SIZE_32) },
};

STATIC MP_DEFINE_CONST_DICT(dma_DMA_locals_dict, dma_DMA_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    dma_DMA_type,
    MP_QSTR_DMA,
    MP_TYPE_FLAG_NONE,
    make_new, dma_DMA_make_new,
    print, dma_DMA_print,
    locals_dict, &dma_DMA_locals_dict
    );


///////////////////////////////////////////////////////////////////////////////////
/*
 * Timer
 * Arguments
 * timer_id 
*/
STATIC mp_obj_t dma_Timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    dma_Timer_obj_t *self = mp_obj_malloc(dma_Timer_obj_t, &dma_Timer_type);
    self->timer = 0xff;
    return MP_OBJ_FROM_PTR(self);
}

STATIC void dma_Timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    dma_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<Timer timer=%d>", self->timer);
}


STATIC mp_obj_t dma_Timer_claim(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timer_id, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xff} },
    };
    
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    dma_Timer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    uint32_t timer_id = args[0].u_int;
    if (self->timer != 0xff)
    {
        mp_raise_ValueError("Timer already claimed");
    }
    else if (timer_id == 0xff)
    {
        int claimed_timer_id = dma_claim_unused_timer(false);
        if (claimed_timer_id == -1)
        {
            mp_raise_ValueError("No timers available");
        }
        else
        {
            self->timer = claimed_timer_id;
        }
    }
    else
    {
        self->timer = timer_id;
        if(dma_timer_is_claimed(self->timer))
        {
            mp_raise_ValueError("Timer already claimed");
        }
        else
        {
            dma_timer_claim(self->timer);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(dma_Timer_claim_obj, 1, dma_Timer_claim);

STATIC mp_obj_t dma_Timer_unclaim(mp_obj_t self_in) {
    dma_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if(dma_timer_is_claimed(self->timer))
    {
        dma_timer_unclaim(self->timer);
        self->timer = 0xff;
    }
    else
    {
        mp_raise_ValueError("Timer already unclaimed");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_Timer_unclaim_obj, dma_Timer_unclaim);

STATIC mp_obj_t dma_Timer_isclaimed(mp_obj_t self_in) {
    dma_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(dma_timer_is_claimed(self->timer));
    
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_Timer_isclaimed_obj, dma_Timer_isclaimed);


STATIC mp_obj_t dma_Timer_set(mp_obj_t self_in, mp_obj_t numerator_obj, mp_obj_t denominator_obj) {
    dma_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint16_t numerator = mp_obj_get_int(numerator_obj);
    uint16_t denominator = mp_obj_get_int(denominator_obj);
    dma_timer_set_fraction(self->timer, numerator, denominator);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(dma_Timer_set_obj, dma_Timer_set);

STATIC mp_obj_t dma_Timer_dreq(mp_obj_t self_in) {
    dma_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(dma_get_timer_dreq(self->timer));
}
MP_DEFINE_CONST_FUN_OBJ_1(dma_Timer_dreq_obj, dma_Timer_dreq);


STATIC const mp_rom_map_elem_t dma_Timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_claim),     MP_ROM_PTR(&dma_Timer_claim_obj) },
    { MP_ROM_QSTR(MP_QSTR_unclaim),   MP_ROM_PTR(&dma_Timer_unclaim_obj) },
    { MP_ROM_QSTR(MP_QSTR_isclaimed), MP_ROM_PTR(&dma_Timer_isclaimed_obj) },
    { MP_ROM_QSTR(MP_QSTR_set),       MP_ROM_PTR(&dma_Timer_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_dreq),      MP_ROM_PTR(&dma_Timer_dreq_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dma_Timer_locals_dict, dma_Timer_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    dma_Timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, dma_Timer_make_new,
    print, dma_Timer_print,
    locals_dict, &dma_Timer_locals_dict
    );
