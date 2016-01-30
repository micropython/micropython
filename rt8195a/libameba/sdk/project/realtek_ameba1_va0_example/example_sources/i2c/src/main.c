/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "PinNames.h"
#include "basic_types.h"
#include "diag.h"
#include <osdep_api.h>

#include "i2c_api.h"
#include "pinmap.h"
#include "ex_api.h"

#define I2C_SINGLE_BOARD
#ifndef I2C_SINGLE_BOARD
	#define I2C_DUAL_BOARD
#endif
#ifdef I2C_SINGLE_BOARD
	#define I2C_MASTER_DEVICE
	#define I2C_SLAVE_DEVICE
#endif
#ifdef I2C_DUAL_BOARD
	#define I2C_MASTER_DEVICE
	#ifndef I2C_MASTER_DEVICE
		#define I2C_SLAVE_DEVICE
	#endif
#endif

#define MBED_I2C_MTR_SDA    PB_3
#define MBED_I2C_MTR_SCL    PB_2

#define MBED_I2C_SLV_SDA    PC_4
#define MBED_I2C_SLV_SCL    PC_5

#define MBED_I2C_SLAVE_ADDR0    0xAA
#define MBED_I2C_BUS_CLK        100000  //hz

#define I2C_DATA_LENGTH         125
char	i2cdatasrc[I2C_DATA_LENGTH];
char	i2cdatadst[I2C_DATA_LENGTH];

#if defined (__ICCARM__)
i2c_t   i2cmaster;
i2c_t   i2cslave;
#else
volatile i2c_t   i2cmaster;
volatile i2c_t   i2cslave;
#endif

void i2c_callback(void *userdata)
{

	int     i2clocalcnt;
	int     result = 0;

	DBG_8195A("show slave received data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt+=2) {
		DBG_8195A("i2c data: %02x \t %02x\n",i2cdatadst[i2clocalcnt],i2cdatadst[i2clocalcnt+1]);
	}

	// verify result
	result = 1;
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
	DBG_8195A("\r\nResult is %s\r\n", (result) ? "success" : "fail");

}

void main(void)
{
    int     i2clocalcnt;

    int     result = 0;

    // prepare for transmission
    _memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
    _memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);

    for (i2clocalcnt=0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++){
        i2cdatasrc[i2clocalcnt] = i2clocalcnt+1;
    }
#ifdef I2C_MASTER_DEVICE
    i2c_init(&i2cmaster, MBED_I2C_MTR_SDA ,MBED_I2C_MTR_SCL);
    i2c_frequency(&i2cmaster,MBED_I2C_BUS_CLK);
#endif
	
#ifdef I2C_SLAVE_DEVICE
    i2c_init(&i2cslave, MBED_I2C_SLV_SDA ,MBED_I2C_SLV_SCL);
    i2c_frequency(&i2cslave,MBED_I2C_BUS_CLK);
    i2c_slave_address(&i2cslave, 0, MBED_I2C_SLAVE_ADDR0, 0xFF);
    i2c_slave_mode(&i2cslave, 1);
    i2c_set_user_callback(&i2cslave, I2C_RX_COMPLETE, i2c_callback);
    
    DBG_8195A("slave read\n");
    i2c_slave_read(&i2cslave, &i2cdatadst[0], I2C_DATA_LENGTH);
#endif

#ifdef I2C_MASTER_DEVICE
    DBG_8195A("master write...\n");
    i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[0], I2C_DATA_LENGTH, 1);
#endif
    while(1){;}
}
