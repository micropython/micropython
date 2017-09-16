/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
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

/*
 * Includes code from ASF sample code adc_temp.h and adc_temp.c,
 * and so includes this license:
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "common-hal/microcontroller/Processor.h"

// Don't reorder these includes because they are dependencies of adc_feature.h.
// They should really be included by adc_feature.h.
#include <compiler.h>
#include "asf/sam0/drivers/system/clock/gclk.h"
#include "asf/sam0/utils/cmsis/samd21/include/component/adc.h"

#include "asf/sam0/drivers/adc/adc_sam_d_r/adc_feature.h"
#include "asf/sam0/drivers/adc/adc.h"

#define ADC_TEMP_SAMPLE_LENGTH    4
#define INT1V_VALUE_FLOAT         1.0
#define INT1V_DIVIDER_1000	  1000.0
#define ADC_12BIT_FULL_SCALE_VALUE_FLOAT        4095.0

typedef struct nvm_calibration_data_t {
    float tempR;       // Production Room temperature
    float tempH;       // Production Hot temperature
    float INT1VR;      // Room temp 2's complement of the internal 1V reference value
    float INT1VH;      // Hot temp 2's complement of the internal 1V reference value
    uint16_t ADCR;     // Production Room temperature ADC value
    uint16_t ADCH;     // Production Hot temperature ADC value
    float VADCR;       // Room temperature ADC voltage
    float VADCH;       // Hot temperature ADC voltage
} nvm_calibration_data_t;


// Decimal to fraction conversion. (adapted from ASF sample).
STATIC float convert_dec_to_frac(uint8_t val) {
    float float_val = (float)val;
    if (val < 10) {
        return (float_val/10.0);
    } else if (val < 100) {
        return (float_val/100.0);
    } else {
        return (float_val/1000.0);
    }
}

STATIC void configure_adc_temp(struct adc_module *adc_instance) {
    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);

    // The parameters chosen here are from the temperature example in:
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // That note also recommends in general:
    // "Discard the first conversion result whenever there is a change
    // in ADC configuration like voltage reference / ADC channel change."

    config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
    config_adc.reference = ADC_REFERENCE_INT1V;
    config_adc.positive_input = ADC_POSITIVE_INPUT_TEMP;
    config_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
    config_adc.sample_length = ADC_TEMP_SAMPLE_LENGTH;

    adc_init(adc_instance, ADC, &config_adc);

    // Oversample and decimate. A higher samplenum produces a more stable result.
    ADC->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(2) | ADC_AVGCTRL_SAMPLENUM_4;
    //ADC->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(4) | ADC_AVGCTRL_SAMPLENUM_16;
}

// Extract the production calibration data information from NVM (adapted from ASF sample).
//
STATIC void load_calibration_data(nvm_calibration_data_t *cal) {
    volatile uint32_t val1;    /* Temperature Log Row Content first 32 bits */
    volatile uint32_t val2;    /* Temperature Log Row Content another 32 bits */
    uint8_t room_temp_val_int; /* Integer part of room temperature in °C */
    uint8_t room_temp_val_dec; /* Decimal part of room temperature in °C */
    uint8_t hot_temp_val_int;  /* Integer part of hot temperature in °C */
    uint8_t hot_temp_val_dec;  /* Decimal part of hot temperature in °C */
    int8_t room_int1v_val;     /* internal 1V reference drift at room temperature */
    int8_t hot_int1v_val;      /* internal 1V reference drift at hot temperature*/
    
    uint32_t *temp_log_row_ptr = (uint32_t *)NVMCTRL_TEMP_LOG;
    
    val1 = *temp_log_row_ptr;
    temp_log_row_ptr++;
    val2 = *temp_log_row_ptr;
        
    room_temp_val_int = (uint8_t)((val1 & NVMCTRL_FUSES_ROOM_TEMP_VAL_INT_Msk) >> NVMCTRL_FUSES_ROOM_TEMP_VAL_INT_Pos);
    room_temp_val_dec = (uint8_t)((val1 & NVMCTRL_FUSES_ROOM_TEMP_VAL_DEC_Msk) >> NVMCTRL_FUSES_ROOM_TEMP_VAL_DEC_Pos);
        
    hot_temp_val_int = (uint8_t)((val1 & NVMCTRL_FUSES_HOT_TEMP_VAL_INT_Msk) >> NVMCTRL_FUSES_HOT_TEMP_VAL_INT_Pos);
    hot_temp_val_dec = (uint8_t)((val1 & NVMCTRL_FUSES_HOT_TEMP_VAL_DEC_Msk) >> NVMCTRL_FUSES_HOT_TEMP_VAL_DEC_Pos);
        
    room_int1v_val = (int8_t)((val1 & NVMCTRL_FUSES_ROOM_INT1V_VAL_Msk) >> NVMCTRL_FUSES_ROOM_INT1V_VAL_Pos);
    hot_int1v_val = (int8_t)((val2 & NVMCTRL_FUSES_HOT_INT1V_VAL_Msk) >> NVMCTRL_FUSES_HOT_INT1V_VAL_Pos);
    
    cal->ADCR = (uint16_t)((val2 & NVMCTRL_FUSES_ROOM_ADC_VAL_Msk) >> NVMCTRL_FUSES_ROOM_ADC_VAL_Pos);
        
    cal->ADCH = (uint16_t)((val2 & NVMCTRL_FUSES_HOT_ADC_VAL_Msk) >> NVMCTRL_FUSES_HOT_ADC_VAL_Pos);
        
    cal->tempR = room_temp_val_int + convert_dec_to_frac(room_temp_val_dec);
    cal->tempH = hot_temp_val_int + convert_dec_to_frac(hot_temp_val_dec);
        
    cal->INT1VR = 1 - ((float)room_int1v_val/INT1V_DIVIDER_1000);
    cal->INT1VH = 1 - ((float)hot_int1v_val/INT1V_DIVIDER_1000);
        
    cal->VADCR = ((float)cal->ADCR * cal->INT1VR)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
    cal->VADCH = ((float)cal->ADCH * cal->INT1VH)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
}

/*
 * Calculate fine temperature using Equation1 and Equation
 * 1b as mentioned in data sheet section "Temperature Sensor Characteristics"
 * of Electrical Characteristics. (adapted from ASF sample code).
 */
STATIC float calculate_temperature(uint16_t raw_code, nvm_calibration_data_t *cal)
{
    float VADC;      /* Voltage calculation using ADC result for Coarse Temp calculation */
    float VADCM;     /* Voltage calculation using ADC result for Fine Temp calculation. */
    float INT1VM;    /* Voltage calculation for reality INT1V value during the ADC conversion */
	
    VADC = ((float)raw_code * INT1V_VALUE_FLOAT)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
	
    // Hopefully compiler will remove common subepxressions here.
    
    /* Coarse Temp Calculation by assume INT1V=1V for this ADC conversion */
    float coarse_temp = cal->tempR + (((cal->tempH - cal->tempR)/(cal->VADCH - cal->VADCR)) * (VADC - cal->VADCR));
	
    /* Calculation to find the real INT1V value during the ADC conversion */
    INT1VM = cal->INT1VR + (((cal->INT1VH - cal->INT1VR) * (coarse_temp - cal->tempR))/(cal->tempH - cal->tempR));
	
    VADCM = ((float)raw_code * INT1VM)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
	
    /* Fine Temp Calculation by replace INT1V=1V by INT1V = INT1Vm for ADC conversion */
    float fine_temp = cal->tempR + (((cal->tempH - cal->tempR)/(cal->VADCH - cal->VADCR)) * (VADCM - cal->VADCR));
	
    return fine_temp;
}


// External interface.
//
float common_hal_mcu_processor_get_temperature(void) {
    struct adc_module adc_instance_struct;

    system_voltage_reference_enable(SYSTEM_VOLTAGE_REFERENCE_TEMPSENSE);
    configure_adc_temp(&adc_instance_struct);
    nvm_calibration_data_t nvm_calibration_data;
    load_calibration_data(&nvm_calibration_data);
    
    adc_enable(&adc_instance_struct);

    uint16_t data;
    enum status_code status;

    // Read twice and discard first result, as recommended in section 14 of
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // "Discard the first conversion result whenever there is a change in ADC configuration
    // like voltage reference / ADC channel change"
    // Empirical observation shows the first reading is quite different than subsequent ones.

    adc_start_conversion(&adc_instance_struct);
    do {
        status = adc_read(&adc_instance_struct, &data);
    } while (status == STATUS_BUSY);

    adc_start_conversion(&adc_instance_struct);
    do {
        status = adc_read(&adc_instance_struct, &data);
    } while (status == STATUS_BUSY);

    // Disable so that someone else can use the adc with different settings.
    adc_disable(&adc_instance_struct);
    return calculate_temperature(data, &nvm_calibration_data);
}


uint32_t common_hal_mcu_processor_get_frequency(void) {
    return system_cpu_clock_get_hz();
}
