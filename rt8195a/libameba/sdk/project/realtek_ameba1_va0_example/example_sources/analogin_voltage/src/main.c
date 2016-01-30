/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "analogin_api.h"
#include <sys_api.h>

#define ADC_CALIBRATION		0
#define MBED_ADC_EXAMPLE_PIN_1    AD_1	// no pin out
#define MBED_ADC_EXAMPLE_PIN_2    AD_2	// HDK, A1
#define MBED_ADC_EXAMPLE_PIN_3    AD_3	// HDK, A2

#if defined (__ICCARM__)
analogin_t   adc0;
analogin_t   adc1;
analogin_t   adc2;
#else
volatile analogin_t   adc0;
volatile analogin_t   adc1;
volatile analogin_t   adc2;
#endif

void adc_delay(void)
{
    int i;
	for(i=0;i<1600000;i++)
		asm(" nop");
}

uint16_t adcdat0    = 0;
uint16_t adcdat1    = 0;
uint16_t adcdat2    = 0;

int32_t v_mv0;
int32_t v_mv1;
int32_t v_mv2;

/*
 * OFFSET:   value of measuring at 0.000v, value(0.000v)
 * GAIN_DIV: value(1.000v)-value(0.000v) or value(2.000v)-value(1.000v) or value(3.000v)-value(2.000v)
 *
 * MSB 12bit of value is valid, need to truncate LSB 4bit (0xABCD -> 0xABC). OFFSET and GAIN_DIV are truncated values.
 */
#define OFFSET 		0x298							
#define GAIN_DIV	0x34C							
#define AD2MV(ad,offset,gain) (((ad/16)-offset)*1000/gain)	

VOID 
main (
    VOID
)
{
	uint16_t offset, gain;
	analogin_init(&adc0, MBED_ADC_EXAMPLE_PIN_1);	// no pinout on HDK board
	analogin_init(&adc1, MBED_ADC_EXAMPLE_PIN_2);
	analogin_init(&adc2, MBED_ADC_EXAMPLE_PIN_3);
#if ADC_CALIBRATION
	sys_adc_calibration(0, &offset, &gain);
	printf("ADC:offset = 0x%x, gain = 0x%x\n", offset, gain);
	if((offset==0xFFFF) || (gain==0xFFFF))
#endif
	{
    	offset = OFFSET;
    	gain = GAIN_DIV;
		printf("ADC:offset = 0x%x, gain = 0x%x\n", offset, gain);
	}
	for (;;){
		adcdat0 = analogin_read_u16(&adc0);
		adcdat1 = analogin_read_u16(&adc1);
		adcdat2 = analogin_read_u16(&adc2);

		v_mv0 = AD2MV(adcdat0, offset, gain);
		v_mv1 = AD2MV(adcdat1, offset, gain);
		v_mv2 = AD2MV(adcdat2, offset, gain);

		printf("AD0:%x = %d mv, AD1:%x = %d mv, AD2:%x = %d mv\n", adcdat0, v_mv0, adcdat1, v_mv1, adcdat2, v_mv2); 
		adc_delay();
	}
	analogin_deinit(&adc0);
	analogin_deinit(&adc1);
	analogin_deinit(&adc2);
}
