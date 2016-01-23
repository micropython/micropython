/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

//#include "mbed_assert.h"
#include "objects.h"
#include "PinNames.h"
//#include <osdep_api.h>
#include "hal_i2c.h"
#include "i2c_api.h"



#if CONFIG_I2C_EN

//#include "cmsis.h"
#include "pinmap.h"


static const PinMap PinMap_I2C_SDA[] = {
    {PD_4,  RTL_PIN_PERI(I2C0, 0, S0), RTL_PIN_FUNC(I2C0, S0)},
    {PH_1,  RTL_PIN_PERI(I2C0, 0, S1), RTL_PIN_FUNC(I2C0, S1)},
    {PC_8,  RTL_PIN_PERI(I2C0, 0, S2), RTL_PIN_FUNC(I2C0, S2)},
    {PE_7,  RTL_PIN_PERI(I2C0, 0, S3), RTL_PIN_FUNC(I2C0, S3)},
    
    {PC_4,  RTL_PIN_PERI(I2C1, 1, S0), RTL_PIN_FUNC(I2C1, S0)},
    {PH_3,  RTL_PIN_PERI(I2C1, 1, S1), RTL_PIN_FUNC(I2C1, S1)},
    {PD_7,  RTL_PIN_PERI(I2C1, 1, S2), RTL_PIN_FUNC(I2C1, S2)},

    {PB_7,  RTL_PIN_PERI(I2C2, 2, S0), RTL_PIN_FUNC(I2C2, S0)},
    {PE_1,  RTL_PIN_PERI(I2C2, 2, S1), RTL_PIN_FUNC(I2C2, S1)},
    {PC_7,  RTL_PIN_PERI(I2C2, 2, S2), RTL_PIN_FUNC(I2C2, S2)},

    {PB_3,  RTL_PIN_PERI(I2C3, 3, S0), RTL_PIN_FUNC(I2C3, S0)},
    {PE_3,  RTL_PIN_PERI(I2C3, 3, S1), RTL_PIN_FUNC(I2C3, S1)},
    {PE_5,  RTL_PIN_PERI(I2C3, 3, S2), RTL_PIN_FUNC(I2C3, S2)},
    {PD_9,  RTL_PIN_PERI(I2C3, 3, S3), RTL_PIN_FUNC(I2C3, S3)},

    {NC,    NC,     0}
};

static const PinMap PinMap_I2C_SCL[] = {
    {PD_5,  RTL_PIN_PERI(I2C0, 0, S0), RTL_PIN_FUNC(I2C0, S0)},
    {PH_0,  RTL_PIN_PERI(I2C0, 0, S1), RTL_PIN_FUNC(I2C0, S1)},
    {PC_9,  RTL_PIN_PERI(I2C0, 0, S2), RTL_PIN_FUNC(I2C0, S2)},
    {PE_6,  RTL_PIN_PERI(I2C0, 0, S3), RTL_PIN_FUNC(I2C0, S3)},

    {PC_5,  RTL_PIN_PERI(I2C1, 1, S0), RTL_PIN_FUNC(I2C1, S0)},
    {PH_2,  RTL_PIN_PERI(I2C1, 1, S1), RTL_PIN_FUNC(I2C1, S1)},
    {PD_6,  RTL_PIN_PERI(I2C1, 1, S2), RTL_PIN_FUNC(I2C1, S2)},

    {PB_6,  RTL_PIN_PERI(I2C2, 2, S0), RTL_PIN_FUNC(I2C2, S0)},
    {PE_0,  RTL_PIN_PERI(I2C2, 2, S1), RTL_PIN_FUNC(I2C2, S1)},
    {PC_6,  RTL_PIN_PERI(I2C2, 2, S2), RTL_PIN_FUNC(I2C2, S2)},
    
    {PB_2,  RTL_PIN_PERI(I2C3, 3, S0), RTL_PIN_FUNC(I2C3, S0)},
    {PE_2,  RTL_PIN_PERI(I2C3, 3, S1), RTL_PIN_FUNC(I2C3, S1)},
    {PE_4,  RTL_PIN_PERI(I2C3, 3, S2), RTL_PIN_FUNC(I2C3, S2)},
    {PD_8,  RTL_PIN_PERI(I2C3, 3, S3), RTL_PIN_FUNC(I2C3, S3)},

    {NC,    NC,     0}
};

static uint16_t i2c_target_addr[4];
static SAL_I2C_TRANSFER_BUF    i2ctxtranbuf[4];
static SAL_I2C_TRANSFER_BUF    i2crxtranbuf[4];
extern u32 ConfigDebugErr;
extern u32 ConfigDebuginfo;
void i2c_init(i2c_t *obj, PinName sda, PinName scl) {
    
    uint32_t i2c_sel;
    uint32_t i2c_idx;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_USERCB_ADPT    pSalI2CUserCBAdpt   = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    
    // Determine the I2C to use
    uint32_t i2c_sda;
    uint32_t i2c_scl;


    i2c_sda = (uint32_t)pinmap_peripheral(sda, PinMap_I2C_SDA);
    i2c_scl = (uint32_t)pinmap_peripheral(scl, PinMap_I2C_SCL);


	ConfigDebugErr &= (~(_DBG_I2C_|_DBG_GDMA_));
    ConfigDebugInfo&= (~(_DBG_I2C_|_DBG_GDMA_));
	
    i2c_sel = (uint32_t)pinmap_merge(i2c_sda, i2c_scl);
    i2c_idx = RTL_GET_PERI_IDX(i2c_sel);
    if (unlikely(i2c_idx == NC)) {
        DBG_8195A("%s: Cannot find matched UART\n", __FUNCTION__);
        return;
    }
    
    /* Get I2C device handler */
    pSalI2CMngtAdpt     = &(obj->SalI2CMngtAdpt);
    pSalI2CUserCBAdpt   = (PSAL_I2C_USERCB_ADPT)&(obj->SalI2CUserCBAdpt);
    
    

    /*To assign the rest pointers*/
    pSalI2CMngtAdpt->MstRDCmdCnt    = 0;
    pSalI2CMngtAdpt->InnerTimeOut   = 2000; // inner time-out count, 2000 ms
    pSalI2CMngtAdpt->pSalHndPriv    = &(obj->SalI2CHndPriv);
    pSalI2CMngtAdpt->pSalHndPriv->ppSalI2CHnd = (void**)&(pSalI2CMngtAdpt->pSalHndPriv);

    /* To assign the default (ROM) HAL OP initialization function */
    pSalI2CMngtAdpt->pHalOpInit     = &HalI2COpInit;

    /* To assign the default (ROM) HAL GDMA OP initialization function */
    pSalI2CMngtAdpt->pHalGdmaOpInit = &HalGdmaOpInit;

    /* To assign the default (ROM) SAL interrupt function */
    pSalI2CMngtAdpt->pSalIrqFunc    = &I2CISRHandle;

    /* To assign the default (ROM) SAL DMA TX interrupt function */
    pSalI2CMngtAdpt->pSalDMATxIrqFunc   = &I2CTXGDMAISRHandle;

    /* To assign the default (ROM) SAL DMA RX interrupt function */
    pSalI2CMngtAdpt->pSalDMARxIrqFunc   = &I2CRXGDMAISRHandle;

    pSalI2CMngtAdpt->pHalInitDat        = &(obj->HalI2CInitData);
    pSalI2CMngtAdpt->pHalOp             = &(obj->HalI2COp);
    pSalI2CMngtAdpt->pIrqHnd            = &(obj->I2CIrqHandleDat);
    pSalI2CMngtAdpt->pHalTxGdmaAdp      = &(obj->HalI2CTxGdmaAdpt);
    pSalI2CMngtAdpt->pHalRxGdmaAdp      = &(obj->HalI2CRxGdmaAdpt);
    pSalI2CMngtAdpt->pHalGdmaOp         = &(obj->HalI2CGdmaOp);
    pSalI2CMngtAdpt->pIrqTxGdmaHnd      = &(obj->I2CTxGdmaIrqHandleDat);
    pSalI2CMngtAdpt->pIrqRxGdmaHnd      = &(obj->I2CRxGdmaIrqHandleDat);
    pSalI2CMngtAdpt->pUserCB            = &(obj->SalI2CUserCB);
    pSalI2CMngtAdpt->pDMAConf           = &(obj->SalI2CDmaUserDef);
    
    /* Assign the private SAL handle to public SAL handle */
    pSalI2CHND      = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalI2CHND->pInitDat    = pSalI2CMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalI2CHND->pUserCB     = pSalI2CMngtAdpt->pUserCB;

    /* Assign the internal user define DMA configuration to the SAL handle */
    pSalI2CHND->pDMAConf    = pSalI2CMngtAdpt->pDMAConf;

    /*To assign user callback pointers*/
    pSalI2CMngtAdpt->pUserCB->pTXCB     = pSalI2CUserCBAdpt;
    pSalI2CMngtAdpt->pUserCB->pTXCCB    = (pSalI2CUserCBAdpt+1);
    pSalI2CMngtAdpt->pUserCB->pRXCB     = (pSalI2CUserCBAdpt+2);
    pSalI2CMngtAdpt->pUserCB->pRXCCB    = (pSalI2CUserCBAdpt+3);
    pSalI2CMngtAdpt->pUserCB->pRDREQCB  = (pSalI2CUserCBAdpt+4);
    pSalI2CMngtAdpt->pUserCB->pERRCB    = (pSalI2CUserCBAdpt+5);
    pSalI2CMngtAdpt->pUserCB->pDMATXCB  = (pSalI2CUserCBAdpt+6);
    pSalI2CMngtAdpt->pUserCB->pDMATXCCB = (pSalI2CUserCBAdpt+7);
    pSalI2CMngtAdpt->pUserCB->pDMARXCB  = (pSalI2CUserCBAdpt+8);
    pSalI2CMngtAdpt->pUserCB->pDMARXCCB = (pSalI2CUserCBAdpt+9);
    pSalI2CMngtAdpt->pUserCB->pGENCALLCB= (pSalI2CUserCBAdpt+10);
    
    /* Set I2C Device Number */
    pSalI2CHND->DevNum = i2c_idx;

    /* Load I2C default value */
    RtkI2CLoadDefault(pSalI2CHND);

    /* Assign I2C Pin Mux */
    pSalI2CHND->PinMux        = RTL_GET_PERI_SEL(i2c_sel);
    pSalI2CHND->OpType        = I2C_INTR_TYPE;
    pSalI2CHND->I2CMaster     = I2C_MASTER_MODE;
    pSalI2CHND->I2CSpdMod     = I2C_SS_MODE;
    pSalI2CHND->I2CClk        = 100;
    pSalI2CHND->I2CAckAddr    = 0;    
    pSalI2CHND->TimeOut       = 300;
    pSalI2CHND->I2CExd     	|= (I2C_EXD_MTR_ADDR_RTY);

    pSalI2CMngtAdpt->InnerTimeOut   = pSalI2CHND->TimeOut;

    /* Deinit I2C first */
    i2c_reset(obj);


    /* Init I2C now */
    RtkI2CInit(pSalI2CHND); 
}

void i2c_frequency(i2c_t *obj, int hz) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt     = &(obj->SalI2CMngtAdpt);
    pSalI2CHND          = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    uint16_t i2c_default_clk    = (uint16_t) pSalI2CHND->I2CClk;
    uint16_t i2c_user_clk       = (uint16_t) (hz/1000);

    
    if (i2c_default_clk != i2c_user_clk) {
        /* Deinit I2C first */
        i2c_reset(obj);
        if (i2c_user_clk <= 100) {
            pSalI2CHND->I2CSpdMod = I2C_SS_MODE;
        }
        else if ((i2c_user_clk > 100) && (i2c_user_clk <= 400)) {
            pSalI2CHND->I2CSpdMod = I2C_FS_MODE;
        }
        else if (i2c_user_clk > 400) {
            pSalI2CHND->I2CSpdMod = I2C_HS_MODE;
        }
        else {
            pSalI2CHND->I2CSpdMod = I2C_SS_MODE;
        }
        
        /* Load the user defined I2C clock */
        pSalI2CHND->I2CClk = i2c_user_clk;

        /* Init I2C now */
        RtkI2CInit(pSalI2CHND);        
    }
}

inline int i2c_start(i2c_t *obj) {
    return 0;
}

inline int i2c_stop(i2c_t *obj) {
    return 0;
}

extern u32 
HalDelayUs(
    IN  u32 us
);

int i2c_read(i2c_t *obj, int address, char *data, int length, int stop) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    u32 I2CInTOTcnt     = 0;
    u32 InTimeoutCount  = 0;
    u32 InStartCount    = 0;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);


    if (i2c_target_addr[pSalI2CHND->DevNum] != address) {
		
		DiagPrintf("i2c_read NO address before \n");
		
        /* Deinit I2C first */
        i2c_reset(obj);

        /* Load the user defined I2C target slave address */
        i2c_target_addr[pSalI2CHND->DevNum] = address;
        pSalI2CHND->I2CAckAddr = address;

        /* Init I2C now */
        RtkI2CInit(pSalI2CHND); 
    }

    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!stop) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }

    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = length;
    pSalI2CHND->pRXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pRXBuf->RegAddr   = 0;
    pSalI2CHND->pRXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CReceive(pSalI2CHND) != HAL_OK) {
	  //NeoJou
	  DiagPrintf("i2c_read() : RtkI2CReceive failed \n");
	  return -2;
//        length = length - pSalI2CHND->pRXBuf->DataLen;
//        return ((int)length);
    }
    else {
        //DBG_8195A(">\n");
        /* Calculate user time out parameters */
        I2CInTOTcnt = 300;
        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
            InStartCount = HalTimerOp.HalTimerReadCount(1);
        }
        while((pSalI2CHND->DevSts != I2C_STS_IDLE) && 
              (pSalI2CHND->DevSts != I2C_STS_ERROR) &&
              (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)) {

			 HalDelayUs(1);
			  
             /* Time-Out check */
            if (InTimeoutCount > 0) {
				//DiagPrintf("i2c_read(): InStartCount=0x%x, InTimeoutCount=0x%x\n", InStartCount, InTimeoutCount);
                if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                    
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    //HalDelayUs(1000);
                    //RtkI2CInit(pSalI2CHND);

					//NeoJou
					DiagPrintf("i2c_read(): while timeout -3\n");
					return -3;
                    //return ((int)(length));
                }
            }
            else {
                if (I2CInTOTcnt == 0) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_RX_ADD_TO;
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    
                    //RtkI2CInit(pSalI2CHND);

					//NeoJou
					DiagPrintf("i2c_read(): I2CInTocnt -4\n");
					return -4;
                    //return ((int)(length));
                }
            }  
        }
        //DBG_8195A("<\n");

		if ( pSalI2CHND->pRXBuf->DataLen != 0 ) {
			DiagPrintf("i2c_read() : strange : didn't receive all \n");
			return -5;
		}
		
        if (pSalI2CHND->DevSts != I2C_STS_TIMEOUT) {			
			//DiagPrintf("i2c_read(): return length=%d, datalen=%d\n", length, pSalI2CHND->pRXBuf->DataLen);
            //return ((int)(length - pSalI2CHND->pRXBuf->DataLen));
			return (int)length;
        } else {
        	DiagPrintf("i2c_read() : timeout failed\n");
			//DiagPrintf("i2c_read(): return length=%d\n", length);
            return -6;
        }
    }
}

int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    u32 I2CInTOTcnt     = 0;
    u32 InTimeoutCount  = 0;
    u32 InStartCount    = 0;


    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);

    if (i2c_target_addr[pSalI2CHND->DevNum] != address) {
		
		DBG_8195A("i2c_write NO address before \n");

        /* Deinit I2C first */
        i2c_reset(obj);

        /* Load the user defined I2C target slave address */
        i2c_target_addr[pSalI2CHND->DevNum] = address;
        pSalI2CHND->I2CAckAddr = address;

        /* Init I2C now */
        RtkI2CInit(pSalI2CHND); 
    }

    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!stop) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }
    
    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = length;
    pSalI2CHND->pTXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        length = length - pSalI2CHND->pTXBuf->DataLen;
        return ((int)length);
    }
    else {
        //DBG_8195A("(\n");
        /* Calculate user time out parameters */
        I2CInTOTcnt = 300;
        if ((I2CInTOTcnt != 0) && (I2CInTOTcnt != I2C_TIMEOOUT_ENDLESS)) {
            InTimeoutCount = (I2CInTOTcnt*1000/TIMER_TICK_US);
            InStartCount = HalTimerOp.HalTimerReadCount(1);
        }
        while((pSalI2CHND->DevSts != I2C_STS_IDLE) && 
              (pSalI2CHND->DevSts != I2C_STS_ERROR) &&
              (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)) {
            /* Time-Out check */
            if (InTimeoutCount > 0) {
                if (HAL_TIMEOUT == I2CIsTimeout(InStartCount, InTimeoutCount)) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    
                    //RtkI2CInit(pSalI2CHND);
                    return ((int)(length));
                }
            }
            else {
                if (I2CInTOTcnt == 0) {
                    pSalI2CHND->DevSts  = I2C_STS_TIMEOUT;
                    pSalI2CHND->ErrType = I2C_ERR_TX_ADD_TO;
                    /* DeInit I2C, Init I2C */
                    //RtkI2CDeInit(pSalI2CHND);
                    
                    //RtkI2CInit(pSalI2CHND);
                    return ((int)(length));
                }
            }         
        }

        if (pSalI2CHND->DevSts != I2C_STS_TIMEOUT)
            return ((int)(length - pSalI2CHND->pTXBuf->DataLen));
        else
            return ((int)(length));
    }
}

int i2c_byte_read(i2c_t *obj, int last) {
    uint8_t i2cdatlocal;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Check if the it's the last byte or not */
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    if (!last) {
        pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;
    }

    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = 1;
    pSalI2CHND->pRXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pRXBuf->RegAddr   = 0;
    pSalI2CHND->pRXBuf->pDataBuf  = &i2cdatlocal;
    RtkI2CReceive(pSalI2CHND);

    return (int)i2cdatlocal;
}

int i2c_byte_write(i2c_t *obj, int data) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->I2CExd &= (~I2C_EXD_MTR_HOLD_BUS);
    pSalI2CHND->I2CExd |= I2C_EXD_MTR_HOLD_BUS;

    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = 1;
    pSalI2CHND->pTXBuf->TargetAddr= pSalI2CHND->I2CAckAddr;
    pSalI2CHND->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (unsigned char*)&data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        return 0;
    }

    return 1;
}

void i2c_reset(i2c_t *obj) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Deinit I2C directly */
    RtkI2CDeInit(pSalI2CHND);
}

#if DEVICE_I2CSLAVE

void i2c_slave_address(i2c_t *obj, int idx, uint32_t address, uint32_t mask) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    uint16_t i2c_default_addr   = (uint16_t) pSalI2CHND->I2CAckAddr;
    uint16_t i2c_user_addr      = (uint16_t) address;

    if (i2c_default_addr != i2c_user_addr) {
        /* Deinit I2C first */
        i2c_reset(obj);

        /* Load the user defined I2C clock */
        pSalI2CHND->I2CAckAddr    = i2c_user_addr;

        /* Init I2C now */
        RtkI2CInit(pSalI2CHND);
    }
}

void i2c_slave_mode(i2c_t *obj, int enable_slave) {

    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    /* Deinit I2C first */
    i2c_reset(obj);

    /* Load the user defined I2C clock */
    pSalI2CHND->I2CMaster = I2C_MASTER_MODE;
    if (enable_slave) 
        pSalI2CHND->I2CMaster = I2C_SLAVE_MODE;

    /* Init I2C now */
    RtkI2CInit(pSalI2CHND);
}

// See I2CSlave.h
#define NoData         0 // the slave has not been addressed
#define ReadAddressed  1 // the master has requested a read from this slave (slave = transmitter)
#define WriteGeneral   2 // the master is writing to all slave
#define WriteAddressed 3 // the master is writing to this slave (slave = receiver)

int i2c_slave_receive(i2c_t *obj) {

    int i2cslvrevsts = NoData;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    i2cslvrevsts = RtkSalI2CSts(pSalI2CHND);
    return i2cslvrevsts;
}

int i2c_slave_read(i2c_t *obj, char *data, int length) {

    //uint8_t i2cdatlocal;
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->pRXBuf            = &i2crxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pRXBuf->DataLen   = length;
    //obj->i2c->pRXBuf->TargetAddr= obj->i2c->I2CAckAddr;
    //obj->i2c->pRXBuf->RegAddr   = 0;
    pSalI2CHND->pRXBuf->pDataBuf  = (u8 *)data;
    if (RtkI2CReceive(pSalI2CHND) != HAL_OK) {
        return 0;   //error
    }

    return length;
}

int i2c_slave_write(i2c_t *obj, const char *data, int length) {
    PSAL_I2C_MNGT_ADPT      pSalI2CMngtAdpt     = NULL;
    PSAL_I2C_HND            pSalI2CHND          = NULL;
    pSalI2CMngtAdpt         = &(obj->SalI2CMngtAdpt);
    pSalI2CHND              = &(pSalI2CMngtAdpt->pSalHndPriv->SalI2CHndPriv);
    
    pSalI2CHND->pTXBuf            = &i2ctxtranbuf[pSalI2CHND->DevNum];
    pSalI2CHND->pTXBuf->DataLen   = length;
    //obj->i2c->pTXBuf->TargetAddr= obj->i2c->I2CAckAddr;
    //obj->i2c->pTXBuf->RegAddr   = 0;
    pSalI2CHND->pTXBuf->pDataBuf  = (u8 *)data;

    if (RtkI2CSend(pSalI2CHND) != HAL_OK) {
        return 0;   //error
    }

    return 1;
}


#endif // CONFIG_I2C_SLAVE_EN

#endif // CONFIG_I2C_EN

