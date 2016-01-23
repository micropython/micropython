/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "rtl_utility.h"
//#include "osdep_api.h"
#include "hal_dac.h"
#include "hal_gdma.h"

//NeoJou
//#include "cmsis_os.h"

/* DAC SAL global variables declaration when kernel disabled */
#ifndef CONFIG_KERNEL
    SRAM_BF_DATA_SECTION
    HAL_DAC_OP HalDACOpSAL;
#endif

#if DAC0_USED   /*#if DAC0_USED*/    
#ifndef CONFIG_KERNEL
    SRAM_BF_DATA_SECTION
    SAL_DAC_MNGT_ADPT   SalDAC0MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_HND_PRIV    SalDAC0HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_DAC_INIT_DAT    HalDAC0InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC0IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalDAC0GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalDAC0GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC0GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USER_CB     SalDAC0UserCB;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USERCB_ADPT SalDAC0UserCBAdpt[SAL_DAC_USER_CB_NUM];
#endif    
#endif          /*#if DAC0_USED*/

#if DAC1_USED   /*#if DAC1_USED*/    
#ifndef CONFIG_KERNEL
    SRAM_BF_DATA_SECTION
    SAL_DAC_MNGT_ADPT   SalDAC1MngtAdpt;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_HND_PRIV    SalDAC1HndPriv;

    SRAM_BF_DATA_SECTION
    HAL_DAC_INIT_DAT    HalDAC1InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC1IrqHandleDat;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_ADAPTER    HalDAC1GdmaAdpt;

    SRAM_BF_DATA_SECTION
    HAL_GDMA_OP         HalDAC1GdmaOp;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE          DAC1GDMAIrqHandleDat;
    
    SRAM_BF_DATA_SECTION
    SAL_DAC_USER_CB     SalDAC1UserCB;

    SRAM_BF_DATA_SECTION
    SAL_DAC_USERCB_ADPT SalDAC1UserCBAdpt[SAL_DAC_USER_CB_NUM];
#endif    
#endif          /*#if DAC1_USED*/


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetMngtAdpt
//
// Description:
//		According to the input index, all the memory space are allocated and all the 
//          related pointers are assigned. The management adapter pointer will be 
//          returned.
//
// Arguments:
//		[in] u8  I2CIdx -
//			I2C module index
//
// Return:
//		PSAL_I2C_MNGT_ADPT
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//--------------------------------------------------------------------------------------------------- 
VOID HalDACOpInit(
    IN  VOID *Data
)
{
    PHAL_DAC_OP pHalDacOp = (PHAL_DAC_OP) Data;

    pHalDacOp->HalDACInit       =   HalDACInit8195a;
    pHalDacOp->HalDACDeInit     =   HalDACDeInit8195a;
    pHalDacOp->HalDACEnable     =   HalDACEnableRtl8195a;
    pHalDacOp->HalDACSend       =   HalDACSendRtl8195a;
    pHalDacOp->HalDACIntrCtrl   =   HalDACIntrCtrl8195a;
    pHalDacOp->HalDACReadReg    =   HalDACReadRegRtl8195a;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetMngtAdpt
//
// Description:
//		According to the input index, all the memory space are allocated and all the 
//          related pointers are assigned. The management adapter pointer will be 
//          returned.
//
// Arguments:
//		[in] u8  I2CIdx -
//			I2C module index
//
// Return:
//		PSAL_I2C_MNGT_ADPT
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//--------------------------------------------------------------------------------------------------- 
PSAL_DAC_MNGT_ADPT
RtkDACGetMngtAdpt(
    IN  u8  DACIdx
){
    PSAL_DAC_MNGT_ADPT      pSalDACMngtAdpt     = NULL;
    PSAL_DAC_USERCB_ADPT    pSalDACUserCBAdpt   = NULL;

    /* If the kernel is available, Memory-allocation is used. */
#ifdef CONFIG_KERNEL
    pSalDACMngtAdpt = (PSAL_DAC_MNGT_ADPT)RtlZmalloc(sizeof(SAL_DAC_MNGT_ADPT));
    pSalDACMngtAdpt->pSalHndPriv    = (PSAL_DAC_HND_PRIV)RtlZmalloc(sizeof(SAL_DAC_HND_PRIV));
    pSalDACMngtAdpt->pHalInitDat    = (PHAL_DAC_INIT_DAT)RtlZmalloc(sizeof(HAL_DAC_INIT_DAT));
    pSalDACMngtAdpt->pHalOp         = (PHAL_DAC_OP)RtlZmalloc(sizeof(HAL_DAC_OP));
    pSalDACMngtAdpt->pIrqHnd        = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalDACMngtAdpt->pUserCB        = (PSAL_DAC_USER_CB)RtlZmalloc(sizeof(SAL_DAC_USER_CB));
    pSalDACMngtAdpt->pHalGdmaAdp    = (PHAL_GDMA_ADAPTER)RtlZmalloc(sizeof(HAL_GDMA_ADAPTER));
    pSalDACMngtAdpt->pHalGdmaOp     = (PHAL_GDMA_OP)RtlZmalloc(sizeof(HAL_GDMA_OP));
    pSalDACMngtAdpt->pIrqGdmaHnd    = (PIRQ_HANDLE)RtlZmalloc(sizeof(IRQ_HANDLE));
    pSalDACUserCBAdpt               = (PSAL_DAC_USERCB_ADPT)RtlZmalloc((sizeof(SAL_DAC_USERCB_ADPT)*SAL_DAC_USER_CB_NUM));
#else
    switch (DACIdx){
        case DAC0_SEL:
        {
            pSalDACMngtAdpt = &SalDAC0MngtAdpt;
            pSalDACMngtAdpt->pSalHndPriv    = &SalDAC0HndPriv;
            pSalDACMngtAdpt->pHalInitDat    = &HalDAC0InitData;
            pSalDACMngtAdpt->pHalOp         = &HalDACOpSAL;
            pSalDACMngtAdpt->pIrqHnd        = &DAC0IrqHandleDat;
            pSalDACMngtAdpt->pHalGdmaAdp    = &HalDAC0GdmaAdpt;
            pSalDACMngtAdpt->pHalGdmaOp     = &HalDAC0GdmaOp;
            pSalDACMngtAdpt->pIrqGdmaHnd    = &DAC0IrqHandleDat;
            pSalDACMngtAdpt->pUserCB        = &SalDAC0UserCB;
            pSalDACUserCBAdpt               = &SalDAC0UserCBAdpt;
            break;
        }
        
        case DAC1_SEL:
        {
            pSalDACMngtAdpt = &SalDAC1MngtAdpt;
            pSalDACMngtAdpt->pSalHndPriv    = &SalDAC1HndPriv;
            pSalDACMngtAdpt->pHalInitDat    = &HalDAC1InitData;
            pSalDACMngtAdpt->pHalOp         = &HalDACOpSAL;
            pSalDACMngtAdpt->pIrqHnd        = &DAC1IrqHandleDat;
            pSalDACMngtAdpt->pHalGdmaAdp    = &HalDAC1GdmaAdpt;
            pSalDACMngtAdpt->pHalGdmaOp     = &HalDAC1GdmaOp;
            pSalDACMngtAdpt->pIrqGdmaHnd    = &DAC1IrqHandleDat;
            pSalDACMngtAdpt->pUserCB        = &SalDAC1UserCB;
            pSalDACUserCBAdpt               = &SalDAC1UserCBAdpt;
            break;
        }
        
        default:
            break;
    }
#endif

    /*To assign user callback pointers*/
    pSalDACMngtAdpt->pUserCB->pTXCB     = pSalDACUserCBAdpt;
    pSalDACMngtAdpt->pUserCB->pTXCCB    = (pSalDACUserCBAdpt+1);
    pSalDACMngtAdpt->pUserCB->pRXCB     = (pSalDACUserCBAdpt+2);
    pSalDACMngtAdpt->pUserCB->pRXCCB    = (pSalDACUserCBAdpt+3);
    pSalDACMngtAdpt->pUserCB->pRDREQCB  = (pSalDACUserCBAdpt+4);
    pSalDACMngtAdpt->pUserCB->pERRCB    = (pSalDACUserCBAdpt+5);
    pSalDACMngtAdpt->pUserCB->pDMATXCB  = (pSalDACUserCBAdpt+6);
    pSalDACMngtAdpt->pUserCB->pDMATXCCB = (pSalDACUserCBAdpt+7);
    pSalDACMngtAdpt->pUserCB->pDMARXCB  = (pSalDACUserCBAdpt+8);
    pSalDACMngtAdpt->pUserCB->pDMARXCCB = (pSalDACUserCBAdpt+9);
    
    /*To assign the rest pointers*/
    pSalDACMngtAdpt->pSalHndPriv->ppSalDACHnd = (void**)&(pSalDACMngtAdpt->pSalHndPriv);
    
    return pSalDACMngtAdpt;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CFreeMngtAdpt
//
// Description:
//		Free all the previous allocated memory space.
//
// Arguments:
//		[in] PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt -
//			I2C SAL management adapter pointer
//         
//
// Return:
//		The status of the enable process.
//          _EXIT_SUCCESS if the RtkI2CFreeMngtAdpt succeeded.
//          _EXIT_FAILURE if the RtkI2CFreeMngtAdpt failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//---------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACFreeMngtAdpt(
    IN  PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt
){
#ifdef CONFIG_KERNEL
    RtlMfree((u8 *)pSalDACMngtAdpt->pUserCB->pTXCB, (sizeof(SAL_DAC_USERCB_ADPT)*SAL_DAC_USER_CB_NUM));
    RtlMfree((u8 *)pSalDACMngtAdpt->pIrqGdmaHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalGdmaOp, sizeof(HAL_GDMA_OP));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalGdmaAdp, sizeof(HAL_GDMA_ADAPTER));
    RtlMfree((u8 *)pSalDACMngtAdpt->pUserCB, sizeof(SAL_DAC_USER_CB));
    RtlMfree((u8 *)pSalDACMngtAdpt->pIrqHnd, sizeof(IRQ_HANDLE));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalOp, sizeof(HAL_DAC_OP));
    RtlMfree((u8 *)pSalDACMngtAdpt->pHalInitDat, sizeof(HAL_DAC_INIT_DAT));
    RtlMfree((u8 *)pSalDACMngtAdpt->pSalHndPriv, sizeof(SAL_DAC_HND_PRIV));    
    RtlMfree((u8 *)pSalDACMngtAdpt, sizeof(SAL_DAC_MNGT_ADPT));
#else
    ;
#endif

    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		I2CISRHandle
//
// Description:
//         I2C Interrupt Service Routine.
//		According to the input pointer to SAL_I2C_HND, all the rest pointers will be 
//         found and be used to the rest part of this servie routine.
//         The following types of interrupt will be taken care:
//              - General Call  (providing General Call Callback). Slave receives a general call.
//              - STOP Bit       (NOT providing General Call Callback)
//              - START Bit     (NOTproviding General Call Callback)
//              - I2C Activity   (NOTproviding General Call Callback)
//              - RX Done       (providing Error Callback). The slave transmitter does NOT
//                                     receive a proper NACK for the end of whole transfer.
//              - TX Abort       (providing Error Call Callback). The Master/Slave 
//                                    transmitting is terminated.
//              - RD Req         (providing TX and TXC Callback). Slave gets a Read Request
//                                    and starts a slave-transmitter operation. The slave transmit
//                                    data will be written into slave TX FIFO from user data buffer.
//              - TX Empty      (providing TX and TXC Callback). Master TX FIFO is empty.
//                                    The user transmit data will be written into master TX FIFO
//                                    from user data buffer.
//              - TX Over        (providing Error Callback). Master TX FIFO is Overflow.
//              - RX Full         (providing RX and RXC Callback). Master/Slave RX FIFO contains
//                                    data. And the received data will be put into Master/Slave user
//                                    receive data buffer.
//              - RX Over        (providing Error Callback). Master/Slave RX FIFO is Overflow.
//              - RX Under      (providing Error Callback). Master/Slave RX FIFO is Underflow.
//
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		NA
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-02.
//
//----------------------------------------------------------------------------------------------------
VOID
DACISRHandle(
    IN  VOID *Data
){
#ifdef CONFIG_DEBUG_LOG_DAC_HAL
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    
    PSAL_DAC_USER_CB    pSalDACUserCB       = NULL;
    u8  DACIrqIdx;
    
    /* To get the SAL_I2C_MNGT_ADPT pointer, and parse the rest pointers */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;
    DACIrqIdx       = pHalDACInitDat->DACIdx;
    pSalDACUserCB   = pSalDACHND->pUserCB;

    
    //DBG_8195A_DAC_LVL(HAL_DAC_LVL,"DAC INTR STS:%x\n",pHalDACOP->HalDACReadReg(pHalDACInitDat, REG_DAC_INTR_STS));
    DiagPrintf("DAC INTR STS:%x\n",pHalDACOP->HalDACReadReg(pHalDACInitDat, REG_DAC_INTR_STS));
#else
    /* To reduce warning */
    Data = Data;
#endif
}

VOID
DACGDMAISRHandle(
    IN  VOID *Data
){
    
    /* DBG_ENTRANCE; */
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = (PSAL_DAC_MNGT_ADPT)Data;
    PHAL_GDMA_ADAPTER   pHalDACGdmaAdapter  = pSalDACMngtAdpt->pHalGdmaAdp;
    PHAL_GDMA_OP        pHalDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;

    u8  IsrTypeMap = 0;
    DBG_8195A_DAC_LVL(HAL_DAC_LVL,"%s\n",__func__);
    
    switch (pHalDACGdmaAdapter->TestItem) {
        /* case ConSarAutoDar: { */
        case 2: {
                //4 Clean Auto Reload Bit
                if ((pHalDACGdmaAdapter->MaxMuliBlock) == pHalDACGdmaAdapter->MuliBlockCunt+1) {
                    pHalDACGdmaOp->HalGdmaChCleanAutoDst(pHalDACGdmaAdapter);
					//osSignalSet(pSalDACMngtAdpt->thread_id, 0x00010dac);					
                }
                /*
                pSrc = (u8*)(pHalDACGdmaAdapter->ChSar +
                                pHalDACGdmaAdapter->BlockLen*(pHalDACGdmaAdapter->MuliBlockCunt-1));
                pDst = (u8*)pHalDACGdmaAdapter->ChDar;
                        */
            }
            break;
        /* case AutoSarConDar: { */
        case 3: {
                //4 Clean Auto Reload Bit
                if ((pHalDACGdmaAdapter->MaxMuliBlock) == pHalDACGdmaAdapter->MuliBlockCunt+1) {
                    pHalDACGdmaOp->HalGdmaChCleanAutoSrc(pHalDACGdmaAdapter);
					//osSignalSet(pSalDACMngtAdpt->thread_id, 0x00010dac);					
                }
                /*
                pSrc = (u8*)pHalDACGdmaAdapter->ChSar;
                pDst = (u8*)(pHalDACGdmaAdapter->ChDar +
                                pHalDACGdmaAdapter->BlockLen*(pHalDACGdmaAdapter->MuliBlockCunt-1));
                        */
            }
            break;
    }

    //3 Clear Pending ISR
    IsrTypeMap = pHalDACGdmaOp->HalGdmaChIsrClean((VOID*)pHalDACGdmaAdapter);

    //3 Maintain Block Count
    if (IsrTypeMap & BlockType) {
        pHalDACGdmaAdapter->MuliBlockCunt++;
    }

}

static RTK_STATUS
RtkDACPinMuxInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    u32 DACLocalTemp;

    /* Check the I2C index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    DACLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    DACLocalTemp |= BIT26;

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,DACLocalTemp);
    
    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(ON);

            /* Enable DAC0 module */
            DAC0_FCTRL(ON);            
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(ON);

            /* Enable DAC1 module */
            DAC1_FCTRL(ON);            
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }
    
    return _EXIT_SUCCESS;
}

static RTK_STATUS
RtkDACPinMuxDeInit(
    IN  PSAL_DAC_HND    pSalDACHND
){

    u32 DACLocalTemp;

    /* Check the I2C index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(OFF);

            /* Enable DAC0 module */
            DAC0_FCTRL(OFF);            
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            /* Turn on DAC active clock */
            ACTCK_DAC_CCTRL(OFF);

            /* Enable DAC1 module */
            DAC1_FCTRL(OFF);            
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }


    DACLocalTemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL2);
    DACLocalTemp &= (~BIT26);

    /* To release DAC delta sigma clock gating */
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_SYS_SYSPLL_CTRL2,DACLocalTemp);
    
    return _EXIT_SUCCESS;
}


#if DAC_INTR_OP_TYPE
//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqInit
//
// Description:
//         I2C interrupt initialization function.
//         For I2C interrupt operation mode, I2C module MUST register itself to the platform
//         by providing the interrupt handler which contains interrupt input data (arguments),
//         interrupt service routine, interrupt number, interrupt priority. And then the interrupt
//         should be enabled.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACIrqInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PIRQ_HANDLE         pIrqHandle          = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pIrqHandle      = pSalDACMngtAdpt->pIrqHnd;
    
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    //NeoJou
    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {
            pIrqHandle->Data = (u32) (pSalDACHND);
            pIrqHandle->IrqNum = DAC0_IRQ;
            pIrqHandle->IrqFun = (IRQ_FUN) DACISRHandle;
            pIrqHandle->Priority = 5;
			//NeoJou
            //InterruptRegister(pIrqHandle);
            //InterruptEn(pIrqHandle);
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            pIrqHandle->Data = (u32) (pSalDACHND);
            pIrqHandle->IrqNum = DAC1_IRQ;
            pIrqHandle->IrqFun = (IRQ_FUN) DACISRHandle;
            pIrqHandle->Priority = 5;
			//NeoJou
            //InterruptRegister(pIrqHandle);
            //InterruptEn(pIrqHandle);
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }

    
    
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqDeInit
//
// Description:
//         I2C interrupt de-initialization function.
//         According to the given I2C device number, the I2C interrupt will be unreigster 
//         from the platform and the relative interrupt handler will be cleared.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt de-initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqDeInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqDeInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACIrqDeInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PIRQ_HANDLE         pIrqHandle          = NULL;
    
    /*To Get the SAL_I2C_MNGT_ADPT Pointer*/
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    pIrqHandle      = pSalDACMngtAdpt->pIrqHnd;
    
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    InterruptUnRegister(pIrqHandle);
    return _EXIT_SUCCESS;
}

#endif


#if DAC_DMA_OP_TYPE
//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqInit
//
// Description:
//         I2C interrupt initialization function.
//         For I2C interrupt operation mode, I2C module MUST register itself to the platform
//         by providing the interrupt handler which contains interrupt input data (arguments),
//         interrupt service routine, interrupt number, interrupt priority. And then the interrupt
//         should be enabled.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACDMAInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt    = NULL;
    PHAL_GDMA_OP        pHALDACGdmaOp      = NULL;
    PIRQ_HANDLE         pIrqHandleDACGdma   = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHALDACGdmaAdpt     = pSalDACMngtAdpt->pHalGdmaAdp;
    pHALDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    pIrqHandleDACGdma   = pSalDACMngtAdpt->pIrqGdmaHnd;
    
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    HalGdmaOpInit(pHALDACGdmaOp);
    
    _memset((void *)pHALDACGdmaAdpt, 0, sizeof(HAL_GDMA_ADAPTER));
    
    pHALDACGdmaAdpt->GdmaCtl.IntEn = 1;

    //DAC TX DMA
    pHALDACGdmaAdpt->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
    pHALDACGdmaAdpt->GdmaCtl.DstTrWidth = TrWidthFourBytes;
    pHALDACGdmaAdpt->GdmaCtl.SrcMsize   = MsizeFour;
    pHALDACGdmaAdpt->GdmaCtl.DestMsize  = MsizeFour;

    pHALDACGdmaAdpt->GdmaCtl.Sinc       = IncType;
    pHALDACGdmaAdpt->GdmaCtl.Dinc       = NoChange;	

    pHALDACGdmaAdpt->GdmaCtl.Done       = 1;
    pHALDACGdmaAdpt->GdmaCtl.TtFc       = (GDMA_CTL_TT_FC_TYPE)0x01;
    
    pHALDACGdmaAdpt->GdmaCfg.DestPer    = 13;
    pHALDACGdmaAdpt->GdmaCfg.ReloadSrc  = 1;

    pHALDACGdmaAdpt->MuliBlockCunt      = 1;
    pHALDACGdmaAdpt->MaxMuliBlock       = 50000;//MaxLlp;
    
    pHALDACGdmaAdpt->GdmaIsrType        = (BlockType|TransferType|ErrType);
    pHALDACGdmaAdpt->IsrCtrl            = ENABLE;
    pHALDACGdmaAdpt->GdmaOnOff          = ON;

    pHALDACGdmaAdpt->ChNum              = 4;
    pHALDACGdmaAdpt->ChEn               = GdmaCh4;

    
    pHALDACGdmaAdpt->TestItem = 3;
    //DBG_8195A("pSalDACHND->DevNum:%x\n",pSalDACHND->DevNum);
    switch (pSalDACHND->DevNum){
#if DAC0_USED
        case DAC0_SEL:
        {   
            pHALDACGdmaAdpt->GdmaIndex          = 0;
            
            pIrqHandleDACGdma->IrqNum   = GDMA0_CHANNEL4_IRQ;
            break;
        }
#endif
#if DAC1_USED
        case DAC1_SEL:
        {
            pHALDACGdmaAdpt->GdmaIndex          = 1;
            
            pIrqHandleDACGdma->IrqNum   = GDMA1_CHANNEL4_IRQ;
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }

    /* GDMA interrupt register */
    pIrqHandleDACGdma->Data     = (u32) (pSalDACMngtAdpt);  
    pIrqHandleDACGdma->IrqFun   = (IRQ_FUN) DACGDMAISRHandle;
    pIrqHandleDACGdma->Priority = 2;
    InterruptRegister(pIrqHandleDACGdma);
    InterruptEn(pIrqHandleDACGdma);

    /* GDMA initialization */
    /* Enable the whole GDMA module first */
    if (pHALDACGdmaAdpt->GdmaIndex == 0) {        
        ACTCK_GDMA0_CCTRL(ON);
        SLPCK_GDMA0_CCTRL(ON);
        GDMA0_FCTRL(ON);
    }
    else {        
        ACTCK_GDMA1_CCTRL(ON);
        SLPCK_GDMA1_CCTRL(ON);
        GDMA1_FCTRL(ON);
    }

    pHALDACGdmaOp->HalGdmaOnOff((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChIsrEnAndDis((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChSeting((VOID*)pHALDACGdmaAdpt);
      
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CIrqDeInit
//
// Description:
//         I2C interrupt de-initialization function.
//         According to the given I2C device number, the I2C interrupt will be unreigster 
//         from the platform and the relative interrupt handler will be cleared.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C interrupt de-initialization process.
//          _EXIT_SUCCESS if the RtkI2CIrqDeInit succeeded.
//          _EXIT_FAILURE if the RtkI2CIrqDeInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
static RTK_STATUS
RtkDACDMADeInit(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;

    PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt     = NULL;
    PHAL_GDMA_OP        pHALDACGdmaOp       = NULL;
    PIRQ_HANDLE         pIrqHandleDACGdma   = NULL;
    
    /*To Get the SAL_I2C_MNGT_ADPT Pointer*/
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHALDACGdmaAdpt     = pSalDACMngtAdpt->pHalGdmaAdp;
    pHALDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    pIrqHandleDACGdma   = pSalDACMngtAdpt->pIrqGdmaHnd;

    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    HalGdmaOpInit(pHALDACGdmaOp);
        
    pHALDACGdmaAdpt->IsrCtrl    = DISABLE;
    pHALDACGdmaOp->HalGdmaChIsrEnAndDis((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChIsrClean((VOID*)pHALDACGdmaAdpt);
    pHALDACGdmaOp->HalGdmaChDis((VOID*)pHALDACGdmaAdpt);
     
    InterruptUnRegister(pIrqHandleDACGdma);
#if 0
    _memset((void *)pIrqHandleDACGdma , 0, sizeof(IRQ_HANDLE));
    _memset((void *)pHALDACGdmaOp     , 0, sizeof(HAL_GDMA_OP));
    _memset((void *)pHALDACGdmaAdpt   , 0, sizeof(HAL_GDMA_ADAPTER));
#endif    
    return _EXIT_SUCCESS;
}

#endif

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInit
//
// Description:
//         According to the given I2C index, the related SAL_I2C_MNGT_ADPT pointer used 
//         for retrieving each I2C data sturcture pointer will be reversely parsed first.
//         Then, initializing I2C HAL operation, initializing I2C interrupt (if needed), 
//         initializing I2C DMA (if needed) and initializing I2C pinmux will be done.
//         User specified I2C configuration will be assigned to I2C initial data structure
//         (PHAL_I2C_INIT_DAT   pHalI2CInitDat). I2C HAL initialization is executed after
//         all the configuration data taken.
//         In the end, I2C module is enabled as a final step of the whole initialization.
//         For a slave ack General Call support, an additional step may be followed after
//         the above steps.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          _EXIT_SUCCESS if the RtkI2CInit succeeded.
//          _EXIT_FAILURE if the RtkI2CInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACInit(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    //PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    


    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    //pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;

    /* Check the input I2C index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

#if 0
    /* Check the input I2C operation type */
    if (RtkI2COpTypeChk(pSalI2CHND))
        return _EXIT_FAILURE;
#endif

    /* DAC Initialize HAL Operations */
    HalDACOpInit(pHalDACOP);
    
    /* DAC Interrupt Initialization */
#if DAC_INTR_OP_TYPE
    RtkDACIrqInit(pSalDACHND);
#endif

    /* DAC DMA Initialization */
#if DAC_DMA_OP_TYPE
    RtkDACDMAInit(pSalDACHND);
#endif

    
    /* DAC Function and Clock Enable*/
    RtkDACPinMuxInit(pSalDACHND);

    pHalDACOP->HalDACInit(pSalDACHND->pInitDat);
    
   #if 1 
        HAL_DAC_WRITE32(pSalDACHND->DevNum, REG_DAC_INTR_CTRL, 
                                        (BIT_DAC_FIFO_FULL_EN       |
                                         BIT_DAC_FIFO_OVERFLOW_EN   |
                                         BIT_DAC_FIFO_STOP_EN       |
                                         BIT_DAC__WRITE_ERROR_EN    |
                                         BIT_DAC_DSC_OVERFLOW0_EN   |
                                         BIT_DAC_DSC_OVERFLOW1_EN));
    #else
        HAL_DAC_WRITE32(pSalDACHND->DevNum, REG_DAC_INTR_CTRL,                                      
                                         (BIT_DAC_FIFO_FULL_EN|
                                          BIT_DAC_FIFO_OVERFLOW_EN|                                          
                                          BIT_DAC_FIFO_STOP_EN|
                                          BIT_DAC__WRITE_ERROR_EN|
                                          BIT_DAC_DSC_OVERFLOW0_EN|
                                          BIT_DAC_DSC_OVERFLOW1_EN));
    #endif
    DBG_8195A("intr msk:%x\n", HAL_DAC_READ32(pSalDACHND->DevNum,REG_DAC_INTR_CTRL));
    
    pSalDACHND->pInitDat->DACEn = DAC_ENABLE;
    pHalDACOP->HalDACEnable(pSalDACHND->pInitDat);
    
    /* DAC Device Status Update */
    pSalDACHND->DevSts  = DAC_STS_IDLE;    
    
    return _EXIT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInit
//
// Description:
//         According to the given I2C index, the related SAL_I2C_MNGT_ADPT pointer used 
//         for retrieving each I2C data sturcture pointer will be reversely parsed first.
//         Then, initializing I2C HAL operation, initializing I2C interrupt (if needed), 
//         initializing I2C DMA (if needed) and initializing I2C pinmux will be done.
//         User specified I2C configuration will be assigned to I2C initial data structure
//         (PHAL_I2C_INIT_DAT   pHalI2CInitDat). I2C HAL initialization is executed after
//         all the configuration data taken.
//         In the end, I2C module is enabled as a final step of the whole initialization.
//         For a slave ack General Call support, an additional step may be followed after
//         the above steps.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          _EXIT_SUCCESS if the RtkI2CInit succeeded.
//          _EXIT_FAILURE if the RtkI2CInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACDeInit(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_DAC_INIT_DAT   pHalDACInitDat      = NULL;
    PHAL_DAC_OP         pHalDACOP           = NULL;    

    /* To Get the SAL_DAC_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHalDACInitDat  = pSalDACMngtAdpt->pHalInitDat;
    pHalDACOP       = pSalDACMngtAdpt->pHalOp;

    /* Check the input DAC index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;
        
    {
        /* DAC Initialize HAL Operations */
        HalDACOpInit(pHalDACOP);

        /* DAC Interrupt Initialization */
#if DAC_INTR_OP_TYPE
        RtkDACIrqDeInit(pSalDACHND);
#endif

        /* DAC DMA Initialization */
#if DAC_DMA_OP_TYPE
        RtkDACDMADeInit(pSalDACHND);
#endif

        pHalDACInitDat->DACEn   =   DAC_DISABLE;
        pHalDACOP->HalDACEnable(pHalDACInitDat);
        pHalDACOP->HalDACDeInit(pHalDACInitDat);
        
        /* DAC Function and Clock Enable*/
        RtkDACPinMuxDeInit(pSalDACHND);       
    }
    
    return _EXIT_SUCCESS;
}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CInit
//
// Description:
//         According to the given I2C index, the related SAL_I2C_MNGT_ADPT pointer used 
//         for retrieving each I2C data sturcture pointer will be reversely parsed first.
//         Then, initializing I2C HAL operation, initializing I2C interrupt (if needed), 
//         initializing I2C DMA (if needed) and initializing I2C pinmux will be done.
//         User specified I2C configuration will be assigned to I2C initial data structure
//         (PHAL_I2C_INIT_DAT   pHalI2CInitDat). I2C HAL initialization is executed after
//         all the configuration data taken.
//         In the end, I2C module is enabled as a final step of the whole initialization.
//         For a slave ack General Call support, an additional step may be followed after
//         the above steps.
//		
// Arguments:
//		[in] VOID *Data -
//			I2C SAL handle
//
// Return:
//		The status of the I2C initialization process.
//          _EXIT_SUCCESS if the RtkI2CInit succeeded.
//          _EXIT_FAILURE if the RtkI2CInit failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACSend(
    IN  VOID *Data
){
    PSAL_DAC_HND        pSalDACHND          = (PSAL_DAC_HND) Data;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PHAL_GDMA_ADAPTER   pHALDACGdmaAdpt    = NULL;
    PHAL_GDMA_OP        pHALDACGdmaOp      = NULL;
    //PIRQ_HANDLE         pIrqHandleDACGdma   = NULL;
    
    /* To Get the SAL_I2C_MNGT_ADPT Pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);
    
    pHALDACGdmaAdpt     = pSalDACMngtAdpt->pHalGdmaAdp;
    pHALDACGdmaOp       = pSalDACMngtAdpt->pHalGdmaOp;
    //pIrqHandleDACGdma   = pSalDACMngtAdpt->pIrqGdmaHnd;

	//NeoJou
	//pSalDACMngtAdpt->thread_id = osThreadGetId();
#if 0
    while (1)
    {
        if (!(HAL_DAC_READ32(0,REG_DAC_INTR_STS) & BIT0) )
            HAL_DAC_WRITE32(0,REG_DAC0_FIFO_WR,0xFFFF);
    }
#endif    
#if 1
    HalGdmaOpInit(pHALDACGdmaOp);

    pHALDACGdmaAdpt->GdmaCtl.BlockSize  = pSalDACHND->pTXBuf->DataLen;
    pHALDACGdmaAdpt->ChSar              =   (u32)pSalDACHND->pTXBuf->pDataBuf;
    pHALDACGdmaAdpt->ChDar              =   (u32)(DAC_REG_BASE+(pSalDACHND->DevNum*0x800));  

    //DBG_8195A("src addr:%x\n", pHALDACGdmaAdpt->ChSar);
    //DBG_8195A("dst addr:%x\n", pHALDACGdmaAdpt->ChDar);
    pHALDACGdmaOp->HalGdmaChSeting(pHALDACGdmaAdpt);

    pHALDACGdmaOp->HalGdmaChEn(pHALDACGdmaAdpt);
#endif

    return _EXIT_SUCCESS;
}

#if 0
osEvent RtkDACWait(void){
	 return osSignalWait(0x00010dac, 0xFFFFFFFF);
}
#endif


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetSalHnd
//
// Description:
//         Allocation of lower layer memory spaces will be done by invoking RtkI2CGetMngtAdpt
//         in this function and return a SAL_I2C_HND pointer to upper layer.
//         According to the given I2C index, RtkI2CGetMngtAdpt will allocate all the memory
//         space such as SAL_I2C_HND, HAL_I2C_INIT_DAT, SAL_I2C_USER_CB etc.
//         
//
// Arguments:
//		[in] u8 I2CIdx -
//			I2C Index
//
// Return:
//		PSAL_I2C_HND
//               A pointer to SAL_I2C_HND which is allocated in the lower layer.
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
PSAL_DAC_HND
RtkDACGetSalHnd(
    IN  u8  DACIdx
){
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt = NULL;
    PSAL_DAC_HND        pSalDACHND      = NULL;

    /* Check the user define setting and the given index */
    if (RtkDACIdxChk(DACIdx)) {
        return (PSAL_DAC_HND)NULL;
    }

    /* Invoke RtkI2CGetMngtAdpt to get the I2C SAL management adapter pointer */
    pSalDACMngtAdpt = RtkDACGetMngtAdpt(DACIdx);

    /* Assign the private SAL handle to public SAL handle */
    pSalDACHND      = &(pSalDACMngtAdpt->pSalHndPriv->SalDACHndPriv);

    /* Assign the internal HAL initial data pointer to the SAL handle */
    pSalDACHND->pInitDat    = pSalDACMngtAdpt->pHalInitDat;

    /* Assign the internal user callback pointer to the SAL handle */
    pSalDACHND->pUserCB     = pSalDACMngtAdpt->pUserCB;
    
    return &(pSalDACMngtAdpt->pSalHndPriv->SalDACHndPriv);

}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CGetSalHnd
//
// Description:
//         Allocation of lower layer memory spaces will be done by invoking RtkI2CGetMngtAdpt
//         in this function and return a SAL_I2C_HND pointer to upper layer.
//         According to the given I2C index, RtkI2CGetMngtAdpt will allocate all the memory
//         space such as SAL_I2C_HND, HAL_I2C_INIT_DAT, SAL_I2C_USER_CB etc.
//         
//
// Arguments:
//		[in] u8 I2CIdx -
//			I2C Index
//
// Return:
//		PSAL_I2C_HND
//               A pointer to SAL_I2C_HND which is allocated in the lower layer.
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACFreeSalHnd(
    IN  PSAL_DAC_HND    pSalDACHND
){
    PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt     = NULL;
    PSAL_DAC_HND_PRIV   pSalDACHNDPriv      = NULL;

    /* To get the SAL_DAC_MNGT_ADPT pointer */
    pSalDACHNDPriv  = CONTAINER_OF(pSalDACHND, SAL_DAC_HND_PRIV, SalDACHndPriv);
    pSalDACMngtAdpt = CONTAINER_OF(pSalDACHNDPriv->ppSalDACHnd, SAL_DAC_MNGT_ADPT, pSalHndPriv);

    /* Invoke RtkDACFreeMngtAdpt to free all the lower layer memory space */
    return (RtkDACFreeMngtAdpt(pSalDACMngtAdpt));

}


//---------------------------------------------------------------------------------------------------
//Function Name:
//		RtkI2CLoadDefault
//
// Description:
//         Accrording the given I2C index, the default I2C configuration is done.
//         
//
// Arguments:
//		[in] PSAL_I2C_HND    pSalI2CHND -
//			SAL I2C handle
//
// Return:
//		The status of the loading I2C default configuration.
//          _EXIT_SUCCESS if the RtkI2CLoadDefault succeeded.
//          _EXIT_FAILURE if the RtkI2CLoadDefault failed.
//
// Note:
//		NA
//
// See Also:
//		NA
//
// Author:
// 		By Jason Deng, 2014-04-03.
//
//----------------------------------------------------------------------------------------------------
RTK_STATUS
RtkDACLoadDefault(
    IN  VOID *Data
){
    PSAL_DAC_HND    pSalDACHND = (PSAL_DAC_HND) Data;

    /* Check the input DAC index first */
    if (RtkDACIdxChk(pSalDACHND->DevNum))
        return _EXIT_FAILURE;

    /* Load SAL handle default value */
    pSalDACHND->PinMux          =   0;
    pSalDACHND->OpType          =   DAC_POLL_TYPE;    
    pSalDACHND->DevSts          =   DAC_STS_UNINITIAL;
    pSalDACHND->DACExd          =   0;
    pSalDACHND->ErrType         =   (u32)NULL;    

    /* Load HAL initial data structure default value */
    pSalDACHND->pInitDat->DACIdx        =   pSalDACHND->DevNum;
    pSalDACHND->pInitDat->DACEn         =   DAC_DISABLE;
    pSalDACHND->pInitDat->DACDataRate   =   DAC_DATA_RATE_250K;
    pSalDACHND->pInitDat->DACEndian     =   DAC_DATA_ENDIAN_LITTLE;
    pSalDACHND->pInitDat->DACBurstSz    =   7;
    pSalDACHND->pInitDat->DACDbgSel     =   DAC_DBG_SEL_DISABLE;
    pSalDACHND->pInitDat->DACDscDbgSel  =   DAC_DSC_DBG_SEL_DISABLE;
    pSalDACHND->pInitDat->DACBPDsc      =   DAC_BYPASS_DSC_SEL_DISABLE;
    pSalDACHND->pInitDat->DACDeltaSig   =   0;
    pSalDACHND->pInitDat->DACAnaCtrl0   =   0;
    pSalDACHND->pInitDat->DACAnaCtrl1   =   0;
    pSalDACHND->pInitDat->DACIntrMSK    =   DAC_FEATURE_DISABLED;

    return _EXIT_SUCCESS;
}
