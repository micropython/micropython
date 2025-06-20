/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "adc.h"
#include "pin.h"
#include "timer.h"

#if MICROPY_HW_ENABLE_ADC

/// \moduleref pyb
/// \class ADC - analog to digital conversion: read analog values on a pin
///
/// Usage:
///
///     adc = pyb.ADC(pin)              # create an analog object from a pin
///     val = adc.read()                # read an analog value
///
///     adc = pyb.ADCAll(resolution)    # creale an ADCAll object
///     val = adc.read_channel(channel) # read the given channel
///     val = adc.read_core_temp()      # read MCU temperature
///     val = adc.read_core_vbat()      # read MCU VBAT
///     val = adc.read_core_vref()      # read MCU VREF



#define ADCx_CLK_ENABLE         __HAL_RCC_ADC2_CLK_ENABLE



typedef struct _tiger_obj_adc_t 
{
    mp_obj_base_t base;
    bool enable;
	volatile uint32_t n_seq_adc;
    volatile uint32_t n_seq_adc_temp;
    volatile uint32_t avg_data;
	volatile uint32_t ch_cycles[18];
	ADC_HandleTypeDef *handle;
} tiger_obj_adc_t;

static ADC_HandleTypeDef adc_handle;
static tiger_obj_adc_t adc;



void ADC_IRQHandler( void )
{
	IRQ_ENTER(INT_ADC);
	
	
	if(__HAL_ADC_GET_FLAG((adc.handle),ADC_FLAG_EOC))
	{
		adc.n_seq_adc_temp--;
		adc.avg_data += HAL_ADC_GetValue((adc.handle));
		

		if (adc.n_seq_adc_temp)
		{    
			adc.handle->Instance->CR2 |= (ADC_CR2_EOCS);
		}
		else
		{    
			adc.avg_data /= adc.n_seq_adc;
			adc.handle->Instance->CR1 &= ~ADC_CR1_EOCIE;
		}
		
	}    

	if(__HAL_ADC_GET_FLAG((adc.handle), ADC_FLAG_OVR))
	{
		__HAL_ADC_CLEAR_FLAG((adc.handle), ADC_FLAG_OVR);
		adc.handle->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
	}
    IRQ_EXIT(INT_ADC);
}

static void Adc_Connect(void)
{
	ADCx_CLK_ENABLE();
}

static void Adc_Reset(tiger_obj_adc_t *obj)
{
	
    obj->handle->Instance->CR1 = 0x00000000L;    // device disabled, all interrupts disabled
    obj->handle->Instance->CR2 = 0x00000000L;    // device disabled, all interrupts disabled

    obj->enable = true;
}

static void Adc_Init(void)
{
	NVIC_SetPriority(ADC_IRQn, IRQ_PRI_ADC);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
}

void Adc_Prepare( tiger_obj_adc_t *obj, const machine_pin_obj_t * pin, uint8_t channel )
{
	Adc_Connect();
    Adc_Reset(obj);

    obj->ch_cycles[channel]  = ADC_SAMPLETIME_480CYCLES;
	ADC_TypeDef *instance = obj->handle->Instance;        

    if (channel < 10) //Check here
    {
		instance->SMPR2 &= ~ADC_SMPR2(7, channel);
		instance->SMPR2 |= ADC_SMPR2(ADC_SAMPLETIME_480CYCLES, channel);
    }
    else
    {
		instance->SMPR1 &= ~ADC_SMPR1(7, channel);
		instance->SMPR1 |= ADC_SMPR1(ADC_SAMPLETIME_480CYCLES, channel);
    }

    if (channel < 16)
    {    
		mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ADC, MP_HAL_PIN_PULL_NONE, 0);
    }
	
	
    Adc_Init();    
}


/******************************************************************************/
/* MicroPython bindings : adc object (single channel)                         */

static void adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_print_str(print, "<ADC on ");
}

/// \classmethod \constructor(pin)
/// Create an ADC object associated with the given pin.
/// This allows you to then read analog values on that pin.
static mp_obj_t adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) 
{
    // check number of arguments
   //mp_arg_check_num(n_args, n_kw, 1, 1, false);
   
	mp_hal_pin_input(pyb_pin_AN00);
	mp_hal_pin_input(pyb_pin_AN01);
	mp_hal_pin_input(pyb_pin_AN02);
	mp_hal_pin_input(pyb_pin_AN03);
	mp_hal_pin_input(pyb_pin_AN04);
	mp_hal_pin_input(pyb_pin_AN05);
	mp_hal_pin_input(pyb_pin_AN06);
	mp_hal_pin_input(pyb_pin_AN07);
	mp_hal_pin_input(pyb_pin_AN08);
	mp_hal_pin_input(pyb_pin_AN09);
	mp_hal_pin_input(pyb_pin_AN10);
	mp_hal_pin_input(pyb_pin_AN11);
	
	
    adc_handle.Instance = ADC2;
	
	tiger_obj_adc_t *o = &adc;

    o->base.type = &tiger_adc_type;
	
	
	o->handle = &adc_handle;
    //adc_init_single(o, ADCx);
    return MP_OBJ_FROM_PTR(o);
}

const machine_pin_obj_t * get_pin_by_channel(uint8_t channel)
{
	switch(channel)
	{
		case 0:
			return pyb_pin_AN00;
		case 1:
			return pyb_pin_AN01;
		case 2:
			return pyb_pin_AN02;
		case 3:
			return pyb_pin_AN03;
		case 4:
			return pyb_pin_AN04;
		case 5:
			return pyb_pin_AN05;
		case 6:
			return pyb_pin_AN06;
		case 7:
			return pyb_pin_AN07;
		case 8:
			return pyb_pin_AN08;
		case 9:
			return pyb_pin_AN09;
		case 10:
			return pyb_pin_AN10;
		case 11:
			return pyb_pin_AN11;
		default:
			return NULL;
	}
}

uint32_t Adc_ValGet( tiger_obj_adc_t *obj, uint8_t channel, uint32_t n_conversions, uint32_t reso)
{
	ADC_TypeDef *instance = obj->handle->Instance;     

    instance->CR1 |= ADC_CR1_SCAN;

    instance->SQR1 = 0;
    instance->SQR1 |= ADC_SQR1(n_conversions);   // 6 conversion
	instance->SQR1 |= ADC_SQR1_RK(channel, 16);
	instance->SQR1 |= ADC_SQR1_RK(channel, 15);
	instance->SQR1 |= ADC_SQR1_RK(channel, 14);
	instance->SQR1 |= ADC_SQR1_RK(channel, 13);
	
	instance->SQR2 = 0;
	instance->SQR2 |= ADC_SQR2_RK(channel, 12);
	instance->SQR2 |= ADC_SQR2_RK(channel, 11);
	instance->SQR2 |= ADC_SQR2_RK(channel, 10);
	instance->SQR2 |= ADC_SQR2_RK(channel,  9);
	instance->SQR2 |= ADC_SQR2_RK(channel,  8);
	instance->SQR2 |= ADC_SQR2_RK(channel,  7);

	instance->SQR3  = 0;
    instance->SQR3 |= ADC_SQR3_RK(channel,  6);
	instance->SQR3 |= ADC_SQR3_RK(channel,  5);
	instance->SQR3 |= ADC_SQR3_RK(channel,  4);
	instance->SQR3 |= ADC_SQR3_RK(channel,  3);
	instance->SQR3 |= ADC_SQR3_RK(channel,  2);
	instance->SQR3 |= ADC_SQR3_RK(channel,  1);
    
    instance->CR2 |=  ADC_CR2_ADON;
	instance->CR2 &= ~ADC_CR2_CONT;

    obj->n_seq_adc       = n_conversions;
    obj->avg_data        = 0;
    
    obj->n_seq_adc_temp  = obj->n_seq_adc;
    obj->avg_data        = 0;
    
	
	instance->CR1 |= (reso);
	instance->CR1 |= ADC_CR1_EOCIE;
    
    
    if (instance->SQR1 & ADC_SQR1_L)
    {    
        instance->CR2 |= (ADC_CR2_EOCS);
    }
	
    
    instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    
    while (obj->n_seq_adc_temp)  
	{
		__WFI();
	}
    
    return obj->avg_data;
}



/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
static mp_obj_t adc_read_single(mp_obj_t self_in, mp_obj_t channel)  
{
    tiger_obj_adc_t * self = MP_OBJ_TO_PTR(self_in);
	uint8_t ch = mp_obj_get_int(channel);
	const machine_pin_obj_t * pin = get_pin_by_channel(ch);
	Adc_Prepare(self, pin, ch);
	
    return mp_obj_new_int(Adc_ValGet(self,ch,1,ADC_RESOLUTION_8B));
}
static MP_DEFINE_CONST_FUN_OBJ_2(adc_read_single_obj, adc_read_single);


static const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readSingle), MP_ROM_PTR(&adc_read_single_obj) },
};

static MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    tiger_adc_type,
    MP_QSTR_ANALOG1,
    MP_TYPE_FLAG_NONE,
    make_new, adc_make_new,
    print, adc_print,
    locals_dict, &adc_locals_dict
    );
#endif // MICROPY_HW_ENABLE_ADC
