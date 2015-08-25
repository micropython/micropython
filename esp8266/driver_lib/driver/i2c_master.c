/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: i2c_master.c
 *
 * Description: i2c master API
 *
 * Modification history:
 *     2014/3/12, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"

#include "driver/i2c_master.h"

LOCAL uint8 m_nLastSDA;
LOCAL uint8 m_nLastSCL;

extern void ets_isr_unmask(uint32_t);
extern void ets_delay_us(uint32_t);
extern void ets_isr_mask(uint32_t);
/******************************************************************************
 * FunctionName : i2c_master_setDC
 * Description  : Internal used function -
 *                    set i2c SDA and SCL bit value for half clk cycle
 * Parameters   : uint8 SDA
 *                uint8 SCL
 * Returns      : NONE
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
i2c_master_setDC(uint8 SDA, uint8 SCL)
{
    SDA	&= 0x01;
    SCL	&= 0x01;
    m_nLastSDA = SDA;
    m_nLastSCL = SCL;

    if ((0 == SDA) && (0 == SCL)) {
        I2C_MASTER_SDA_LOW_SCL_LOW();
    } else if ((0 == SDA) && (1 == SCL)) {
        I2C_MASTER_SDA_LOW_SCL_HIGH();
    } else if ((1 == SDA) && (0 == SCL)) {
        I2C_MASTER_SDA_HIGH_SCL_LOW();
    } else {
        I2C_MASTER_SDA_HIGH_SCL_HIGH();
    }
}

/******************************************************************************
 * FunctionName : i2c_master_getDC
 * Description  : Internal used function -
 *                    get i2c SDA bit value
 * Parameters   : NONE
 * Returns      : uint8 - SDA bit value
*******************************************************************************/
LOCAL uint8 ICACHE_FLASH_ATTR
i2c_master_getDC(void)
{
    uint8 sda_out;
    sda_out = GPIO_INPUT_GET(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO));
    return sda_out;
}

/******************************************************************************
 * FunctionName : i2c_master_init
 * Description  : initilize I2C bus to enable i2c operations
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_init(void)
{
    uint8 i;

    i2c_master_setDC(1, 0);
    i2c_master_wait(5);

    // when SCL = 0, toggle SDA to clear up
    i2c_master_setDC(0, 0) ;
    i2c_master_wait(5);
    i2c_master_setDC(1, 0) ;
    i2c_master_wait(5);

    // set data_cnt to max value
    for (i = 0; i < 28; i++) {
        i2c_master_setDC(1, 0);
        i2c_master_wait(5);	// sda 1, scl 0
        i2c_master_setDC(1, 1);
        i2c_master_wait(5);	// sda 1, scl 1
    }

    // reset all
    i2c_master_stop();
    return;
}

/******************************************************************************
 * FunctionName : i2c_master_gpio_init
 * Description  : config SDA and SCL gpio to open-drain output mode,
 *                mux and gpio num defined in i2c_master.h
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_gpio_init(void)
{
    ETS_GPIO_INTR_DISABLE() ;
//    ETS_INTR_LOCK();

    PIN_FUNC_SELECT(I2C_MASTER_SDA_MUX, I2C_MASTER_SDA_FUNC);
    PIN_FUNC_SELECT(I2C_MASTER_SCL_MUX, I2C_MASTER_SCL_FUNC);

    GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); //open drain;
    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << I2C_MASTER_SDA_GPIO));
    GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SCL_GPIO)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SCL_GPIO))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); //open drain;
    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << I2C_MASTER_SCL_GPIO));

    I2C_MASTER_SDA_HIGH_SCL_HIGH();

    ETS_GPIO_INTR_ENABLE() ;
//    ETS_INTR_UNLOCK();

    i2c_master_init();
}

/******************************************************************************
 * FunctionName : i2c_master_start
 * Description  : set i2c to send state
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_start(void)
{
    i2c_master_setDC(1, m_nLastSCL);
    i2c_master_wait(5);
    i2c_master_setDC(1, 1);
    i2c_master_wait(5);	// sda 1, scl 1
    i2c_master_setDC(0, 1);
    i2c_master_wait(5);	// sda 0, scl 1
}

/******************************************************************************
 * FunctionName : i2c_master_stop
 * Description  : set i2c to stop sending state
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_stop(void)
{
    i2c_master_wait(5);

    i2c_master_setDC(0, m_nLastSCL);
    i2c_master_wait(5);	// sda 0
    i2c_master_setDC(0, 1);
    i2c_master_wait(5);	// sda 0, scl 1
    i2c_master_setDC(1, 1);
    i2c_master_wait(5);	// sda 1, scl 1
}

/******************************************************************************
 * FunctionName : i2c_master_setAck
 * Description  : set ack to i2c bus as level value
 * Parameters   : uint8 level - 0 or 1
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_setAck(uint8 level)
{
    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);
    i2c_master_setDC(level, 0);
    i2c_master_wait(5);	// sda level, scl 0
    i2c_master_setDC(level, 1);
    i2c_master_wait(8);	// sda level, scl 1
    i2c_master_setDC(level, 0);
    i2c_master_wait(5);	// sda level, scl 0
    i2c_master_setDC(1, 0);
    i2c_master_wait(5);
}

/******************************************************************************
 * FunctionName : i2c_master_getAck
 * Description  : confirm if peer send ack
 * Parameters   : NONE
 * Returns      : uint8 - ack value, 0 or 1
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR
i2c_master_getAck(void)
{
    uint8 retVal;
    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);
    i2c_master_setDC(1, 0);
    i2c_master_wait(5);
    i2c_master_setDC(1, 1);
    i2c_master_wait(5);

    retVal = i2c_master_getDC();
    i2c_master_wait(5);
    i2c_master_setDC(1, 0);
    i2c_master_wait(5);

    return retVal;
}

/******************************************************************************
* FunctionName : i2c_master_checkAck
* Description  : get dev response
* Parameters   : NONE
* Returns      : true : get ack ; false : get nack
*******************************************************************************/
bool ICACHE_FLASH_ATTR
i2c_master_checkAck(void)
{
    if(i2c_master_getAck()){
        return FALSE;
    }else{
        return TRUE;
    }
}

/******************************************************************************
* FunctionName : i2c_master_send_ack
* Description  : response ack
* Parameters   : NONE
* Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_send_ack(void)
{
    i2c_master_setAck(0x0);
}
/******************************************************************************
* FunctionName : i2c_master_send_nack
* Description  : response nack
* Parameters   : NONE
* Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_send_nack(void)
{
    i2c_master_setAck(0x1);
}

/******************************************************************************
 * FunctionName : i2c_master_readByte
 * Description  : read Byte from i2c bus
 * Parameters   : NONE
 * Returns      : uint8 - readed value
*******************************************************************************/
uint8 ICACHE_FLASH_ATTR
i2c_master_readByte(void)
{
    uint8 retVal = 0;
    uint8 k, i;

    i2c_master_wait(5);
    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);	// sda 1, scl 0

    for (i = 0; i < 8; i++) {
        i2c_master_wait(5);
        i2c_master_setDC(1, 0);
        i2c_master_wait(5);	// sda 1, scl 0
        i2c_master_setDC(1, 1);
        i2c_master_wait(5);	// sda 1, scl 1

        k = i2c_master_getDC();
        i2c_master_wait(5);

        if (i == 7) {
            i2c_master_wait(3);   ////
        }

        k <<= (7 - i);
        retVal |= k;
    }

    i2c_master_setDC(1, 0);
    i2c_master_wait(5);	// sda 1, scl 0

    return retVal;
}

/******************************************************************************
 * FunctionName : i2c_master_writeByte
 * Description  : write wrdata value(one byte) into i2c
 * Parameters   : uint8 wrdata - write value
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
i2c_master_writeByte(uint8 wrdata)
{
    uint8 dat;
    sint8 i;

    i2c_master_wait(5);

    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);

    for (i = 7; i >= 0; i--) {
        dat = wrdata >> i;
        i2c_master_setDC(dat, 0);
        i2c_master_wait(5);
        i2c_master_setDC(dat, 1);
        i2c_master_wait(5);

        if (i == 0) {
            i2c_master_wait(3);   ////
        }

        i2c_master_setDC(dat, 0);
        i2c_master_wait(5);
    }
}
