/*
 * SRAM Driver.
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "boardctrl.h"
#include "pin.h"
#include "pin_static_af.h"
#include "mpu.h"
#include "systick.h"
#include "sram.h"

//===========================================================================

#if defined(MICROPY_HW_SRAM_SIZE)

 #define SRAM_START_ADDRESS  (uint32_t)(0x68000000)
 
	bool sram_init(void) {  

	__HAL_RCC_FSMC_CLK_ENABLE();                //使能FSMC时钟

	#if MICROPY_HW_LCD43M
	mp_hal_pin_config(MICROPY_HW_MCULCD_CS, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
	mp_hal_pin_high(MICROPY_HW_MCULCD_CS); 
	#endif

	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_NBL0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_NBL0);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_NBL1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_NBL1);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_NOE, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_NOE);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_NWE, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_NWE);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_NE3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_NE3);
									   
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A0);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A1);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A2);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A3);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A4);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A5);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A6);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A7);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A8, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A8);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A9, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A9);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A10, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A10);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A11);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A12);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A13, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A13);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A14, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A14);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A15, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A15);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A16, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A16);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A17, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A17);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_A18, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_A18);
									   
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D0);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D1);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D2);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D3);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D4);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D5);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D6);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D7);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D8, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D8);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D9, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D9);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D10, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D10);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D11);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D12);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D13, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D13);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D14, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D14);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_FSMC_D15, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_FSMC_D15);	

	FSMC_Bank1->BTCR[4]=0X00000000;
	FSMC_Bank1->BTCR[5]=0X00000000;
	FSMC_Bank1E->BWTR[4]=0X00000000;

	FSMC_Bank1->BTCR[4]|=1<<12;//en
	FSMC_Bank1->BTCR[4]|=1<<4; //16bit 	

	//1个HCLK=6ns
	//数据保持时间
	// FSMC_Bank1->BTCR[5]|=8<<8; //HCLK 6*3=18ns
	// FSMC_Bank1->BTCR[5]|=10<<8; //HCLK 6*3=18ns
FSMC_Bank1->BTCR[5]|=7<<8; //HCLK 6*3=18ns

	FSMC_Bank1->BTCR[5]|=0<<4; //
	//地址建立时间
	// FSMC_Bank1->BTCR[5]|=1<<0; //HCLK 12ns
	// FSMC_Bank1->BTCR[5]|=16<<0; //HCLK 12ns
FSMC_Bank1->BTCR[5]|=7<<0; //HCLK 12ns

	FSMC_Bank1E->BWTR[4]=0x0FFFFFFF;
	FSMC_Bank1->BTCR[4]|=1<<0; 

	return true;	
}

void *sram_start(void) {
    return (void *)SRAM_START_ADDRESS;
}

void *sram_end(void) {
    return (void *)(SRAM_START_ADDRESS + MICROPY_HW_SRAM_SIZE);
}

//------------------------------------------------------------------------------------------
#if MICROPY_HW_SRAM_STARTUP_TEST
bool sram_test(bool fast) {
    uint8_t const pattern = 0xaa;
    //uint8_t const antipattern = 0x55;
    uint8_t *const mem_base = (uint8_t *)sram_start();
    uint32_t i = 0;
    uint8_t j =0;
    /* test data bus */
    for (j = 1; j; j <<= 1) {
        *mem_base = j;
        if (*mem_base != j) {
            // printf("data bus lines test failed! data (%d)\n", j);
            return false;
        }
    }


    /* test address bus */
    /* Check individual address lines */
    for ( i = 1; i < MICROPY_HW_SRAM_SIZE; i <<= 1) {
        mem_base[i] = pattern;
        if (mem_base[i] != pattern) {
            // printf("address bus lines test failed! address (%p)\n", &mem_base[i]);
            return false;
        }
    }	

    for (i = 0; i < MICROPY_HW_SRAM_SIZE; ++i) {
        mem_base[i] = pattern;
    }

    /* test all ram cells */
    if (!fast) {
        for (i = 0; i < MICROPY_HW_SRAM_SIZE; ++i) {
            if (mem_base[i] != pattern) {
                // printf("address bus test failed! address (%p)\n", &mem_base[i]);
                return false;
            }
        }
    } else {
        memset(mem_base, pattern, MICROPY_HW_SRAM_SIZE);
    }


    return true;
}
#endif
#endif
