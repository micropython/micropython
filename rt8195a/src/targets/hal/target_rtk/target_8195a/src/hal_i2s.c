/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "hal_i2s.h"
#include "rand.h"
#include "rtl_utility.h"


//1 need to be modified 


/*======================================================
    Local used variables
*/
SRAM_BF_DATA_SECTION
HAL_I2S_OP HalI2SOpSAL;

// TODO:
#if I2S0_USED
    SRAM_BF_DATA_SECTION
    SAL_I2S_HND         SalI2S0Hnd;

    SRAM_BF_DATA_SECTION
    HAL_I2S_INIT_DAT    HalI2S0InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE  I2S0IrqHandle;

#endif

#if I2S1_USED
    SRAM_BF_DATA_SECTION
    SAL_I2S_HND         SalI2S1Hnd;

    SRAM_BF_DATA_SECTION
    HAL_I2S_INIT_DAT    HalI2S1InitData;

    SRAM_BF_DATA_SECTION
    IRQ_HANDLE  I2S1IrqHandle;
#endif

#ifndef CONFIG_KERNEL
/* I2S TX RX page memory*/
SRAM_BF_DATA_SECTION
volatile u32  I2S_TX_PAGE[768/4*4];
SRAM_BF_DATA_SECTION
volatile u32  I2S_RX_PAGE[768/4*4];


#endif

volatile u32 I2SMstRDCmdCnt[4];


VOID
I2SISRHandle(
    IN  VOID *Data
)
{
    PSAL_I2S_HND    pSalI2SHND = (PSAL_I2S_HND) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    u8  I2SIrqIdx = pSalI2SHND->pInitDat->I2SIdx;
    u32 I2STxIsr, I2SRxIsr;
    u8  I2SPageNum = pSalI2SHND->I2SPageNum +1;
    u32 i;

    I2STxIsr = pHalI2SOP->HalI2SReadReg(pSalI2SHND->pInitDat, REG_I2S_TX_STATUS_INT);
    I2SRxIsr = pHalI2SOP->HalI2SReadReg(pSalI2SHND->pInitDat, REG_I2S_RX_STATUS_INT);

    pSalI2SHND->pInitDat->I2STxIntrClr = I2STxIsr;
    pSalI2SHND->pInitDat->I2SRxIntrClr = I2SRxIsr;
    pHalI2SOP->HalI2SClrIntr(pSalI2SHND->pInitDat);
    for (i=0 ; i<I2SPageNum ; i++) // page 0, 1, 2, 3
    {
        if (I2STxIsr & (1<<pSalI2SHND->pInitDat->I2SHWTxIdx))
        {
            pSalI2SHND->pUserCB->TXCCB(pSalI2SHND);
            I2STxIsr &= ~(1<<pSalI2SHND->pInitDat->I2SHWTxIdx);
            pSalI2SHND->pInitDat->I2SHWTxIdx += 1;
            if (pSalI2SHND->pInitDat->I2SHWTxIdx == I2SPageNum)
                pSalI2SHND->pInitDat->I2SHWTxIdx = 0;
        }
        
        if (I2SRxIsr & (1<<pSalI2SHND->pInitDat->I2SHWRxIdx))
        {
            pSalI2SHND->pUserCB->RXCCB(pSalI2SHND);
            I2SRxIsr &= ~(1<<pSalI2SHND->pInitDat->I2SHWRxIdx);
            pSalI2SHND->pInitDat->I2SHWRxIdx += 1;
            if (pSalI2SHND->pInitDat->I2SHWRxIdx == I2SPageNum)
                pSalI2SHND->pInitDat->I2SHWRxIdx = 0;
        }

    }
#if 0
    /*I2S0 General Call Intr*/
    if (pHalI2COP->HalI2CReadReg(pSalI2CHND->pInitDat,REG_DW_I2C_IC_INTR_STAT) &
                                BIT_CTRL_IC_INTR_STAT_R_GEN_CALL(1)) {
        DBG_8195A_I2C_LVL(VERI_I2C_LVL,"I2C%d INTR_GEN_CALL\n",I2CIrqIdx);
        pSalI2CHND->pInitDat->I2CIntrClr = REG_DW_I2C_IC_CLR_GEN_CALL;
        pHalI2COP->HalI2CClrIntr(pSalI2CHND->pInitDat);
        pSalI2CHND->pUserCB->GENCALLCB(pSalI2CHND);
    }
#endif
}



static RTK_STATUS
RtkI2SIrqInit(
    IN  PSAL_I2S_HND    pSalI2SHND
){
    if (RtkI2SIdxChk(pSalI2SHND->DevNum))
        return _EXIT_FAILURE;

    switch (pSalI2SHND->DevNum){
#if I2S0_USED
        case I2S0_SEL:
        {
            I2S0IrqHandle.Data = (u32) (pSalI2SHND);
            I2S0IrqHandle.IrqNum = I2S0_PCM0_IRQ;
            I2S0IrqHandle.IrqFun = (IRQ_FUN) I2SISRHandle;
            I2S0IrqHandle.Priority = 3;
            InterruptRegister(&I2S0IrqHandle);
            InterruptEn(&I2S0IrqHandle);
            break;
        }
#endif
#if I2S1_USED
        case I2S1_SEL:
        {
            I2S1IrqHandle.Data = (u32) (pSalI2SHND);
            I2S1IrqHandle.IrqNum = I2S1_PCM1_IRQ;
            I2S1IrqHandle.IrqFun = (IRQ_FUN) I2SISRHandle;
            I2S1IrqHandle.Priority = 3;
            InterruptRegister(&I2S1IrqHandle);
            InterruptEn(&I2S1IrqHandle);
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }
    return _EXIT_SUCCESS;
}

static RTK_STATUS
RtkI2SIrqDeInit(
    IN  PSAL_I2S_HND    pSalI2SHND
){
    if (RtkI2SIdxChk(pSalI2SHND->DevNum))
        return _EXIT_FAILURE;

    switch (pSalI2SHND->DevNum){
#if I2S0_USED
        case I2S0_SEL:
        {
            InterruptUnRegister(&I2S0IrqHandle);  
            break;
        }
#endif
#if I2S1_USED
        case I2S1_SEL:
        {
            InterruptUnRegister(&I2S1IrqHandle);
            break;
        }
#endif
        default:
            return _EXIT_FAILURE;
    }

    return _EXIT_SUCCESS;
}


static RTK_STATUS
RtkI2SPinMuxInit(
    IN  PSAL_I2S_HND    pSalI2SHND
){
    u32 I2Stemp;

#ifdef CONFIG_DEBUG_LOG 
    DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s\n", __func__);
#endif 


    if (RtkI2SIdxChk(pSalI2SHND->DevNum))
        return _EXIT_FAILURE;

    // enable system pll
    //I2Stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SOC_SYS_SYSPLL_CTRL1) | (1<<9) | (1<<10);
    //HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SOC_SYS_SYSPLL_CTRL1, I2Stemp);
    I2Stemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1) | (1<<9) | (1<<10);
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_SYSPLL_CTRL1, I2Stemp);

    switch (pSalI2SHND->DevNum){
#if I2S0_USED
        case I2S0_SEL:
        {
            ACTCK_I2S_CCTRL(ON);
            LXBUS_FCTRL(ON); // enable lx bus for i2s
            
            /*I2S0 Pin Mux Setting*/
            PinCtrl(I2S0, pSalI2SHND->PinMux, ON);
            if (pSalI2SHND->PinMux == I2S0_TO_S0) {
                JTAG_PIN_FCTRL(OFF);
                SPI_FLASH_PIN_FCTRL(OFF);
            }
            I2S0_MCK_CTRL(ON);
            I2S0_PIN_CTRL(ON);
            I2C0_FCTRL(ON);
            
            break;
        }
#endif
#if I2S1_USED
        case I2S1_SEL:
        {
            ACTCK_I2S_CCTRL(ON);
            LXBUS_FCTRL(ON); // enable lx bus for i2s
            
            /*I2S1 Pin Mux Setting*/
            PinCtrl(I2S1, pSalI2SHND->PinMux, ON);
            I2S1_MCK_CTRL(ON);
            I2S1_PIN_CTRL(ON);
            I2S1_FCTRL(ON);
            break;
        }
#endif

        default:
            return _EXIT_FAILURE;
    }
    
    return _EXIT_SUCCESS;
}


static RTK_STATUS
RtkI2SPinMuxDeInit(
    IN  PSAL_I2S_HND    pSalI2SHND
){
#ifdef CONFIG_DEBUG_LOG 
    DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s\n", __func__);
#endif 

    if (RtkI2SIdxChk(pSalI2SHND->DevNum))
        return _EXIT_FAILURE;

    switch (pSalI2SHND->DevNum){
#if I2S0_USED
        case I2S0_SEL:
        {
            /*I2S0 Pin Mux Setting*/
            //ACTCK_I2C0_CCTRL(OFF);
            PinCtrl(I2S0, pSalI2SHND->PinMux, OFF);
            I2S0_MCK_CTRL(OFF);
            I2S0_PIN_CTRL(OFF);
            //I2S0_FCTRL(OFF);
            
            break;
        }
#endif
#if I2S1_USED
        case I2S1_SEL:
        {
            /*I2S1 Pin Mux Setting*/
            //ACTCK_I2C1_CCTRL(OFF);
            PinCtrl(I2S1, pSalI2SHND->PinMux, OFF);
            I2S1_MCK_CTRL(OFF);
            I2S1_PIN_CTRL(OFF);
            //I2S1_FCTRL(OFF);
            break;
        }
#endif

        default:
            return _EXIT_FAILURE;
    }
    
    return _EXIT_SUCCESS;
}


RTK_STATUS
RtkI2SInit(
    IN  VOID *Data
){
    PSAL_I2S_HND    pSalI2SHND = (PSAL_I2S_HND) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    u32 I2SDatTemp;
    
    if (RtkI2SIdxChk(pSalI2SHND->DevNum))
        return _EXIT_FAILURE;

    //if (RtkI2SOpTypeChk(pSalI2SHND))
    //    return _EXIT_FAILURE;

    /*I2S Initialize HAL Operations*/
    HalI2SOpInit(pHalI2SOP);
    
    /*I2S Interrupt Initialization*/
    RtkI2SIrqInit(pSalI2SHND);

    /*I2S Pin Mux Initialization*/
    RtkI2SPinMuxInit(pSalI2SHND);
#if 1    
    /*I2S Load User Setting*/
    pSalI2SHND->pInitDat->I2SIdx        = pSalI2SHND->DevNum;
    pSalI2SHND->pInitDat->I2SMaster     = 0;
    pSalI2SHND->pInitDat->I2SChNum      = pSalI2SHND->I2SChNum;
    pSalI2SHND->pInitDat->I2SPageNum    = pSalI2SHND->I2SPageNum;
    pSalI2SHND->pInitDat->I2SPageSize   = pSalI2SHND->I2SPageSize;
    pSalI2SHND->pInitDat->I2SRate       = pSalI2SHND->I2SRate;
    pSalI2SHND->pInitDat->I2STRxAct     = pSalI2SHND->I2STRxAct;
    pSalI2SHND->pInitDat->I2SWordLen    = pSalI2SHND->I2SWordLen;
#ifndef CONFIG_KERNEL
    pSalI2SHND->pInitDat->I2STxData     = (u8 *)&I2S_TX_PAGE[0];
    pSalI2SHND->pInitDat->I2SRxData     = (u8 *)&I2S_RX_PAGE[0];
#else
    pSalI2SHND->pInitDat->I2STxData     = (u8 *)RtlMalloc(sizeof(u8)*4*4*(pSalI2SHND->I2SPageSize+1));
    pSalI2SHND->pInitDat->I2SRxData     = (u8 *)RtlMalloc(sizeof(u8)*4*4*(pSalI2SHND->I2SPageSize+1));
#endif
    /*I2S HAL Initialization*/
    pHalI2SOP->HalI2SInit(pSalI2SHND->pInitDat);

    /*I2S Device Status Update*/
    pSalI2SHND->DevSts  = I2S_STS_INITIALIZED;
    
    /*I2S Enable Module*/
    pSalI2SHND->pInitDat->I2SEn = I2S_ENABLE;
    pHalI2SOP->HalI2SEnable(pSalI2SHND->pInitDat);

#endif
    /*I2S Device Status Update*/
    pSalI2SHND->DevSts  = I2S_STS_IDLE;    
    
    return _EXIT_SUCCESS;
}


RTK_STATUS
RtkI2SDeInit(
    IN  VOID *Data
)
{
    PSAL_I2S_HND    pSalI2SHND = (PSAL_I2S_HND) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    HalI2SOpInit(pHalI2SOP);

    /*I2S Disable Module*/
    pSalI2SHND->pInitDat->I2SEn = I2S_DISABLE;
    pHalI2SOP->HalI2SEnable(pSalI2SHND->pInitDat);

    /*I2C HAL DeInitialization*/
    //pHalI2SOP->HalI2SDeInit(pSalI2SHND->pInitDat);
    
    /*I2S Interrupt DeInitialization*/
    RtkI2SIrqDeInit(pSalI2SHND);

    /*I2S Pin Mux DeInitialization*/
    RtkI2SPinMuxDeInit(pSalI2SHND);
    
#ifdef CONFIG_DEBUG_LOG 
    DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s\n", __func__);
#endif 
#ifdef CONFIG_KERNEL
    RtlMfree(pSalI2SHND->pInitDat->I2STxData, sizeof(u8)*4*4*(pSalI2SHND->I2SPageSize+1));
    RtlMfree(pSalI2SHND->pInitDat->I2SRxData, sizeof(u8)*4*4*(pSalI2SHND->I2SPageSize+1));
#endif
#ifdef CONFIG_DEBUG_LOG 
    DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s\n", __func__);
#endif 
    /*I2S HAL DeInitialization*/
    pHalI2SOP->HalI2SDeInit(pSalI2SHND->pInitDat);
#ifdef CONFIG_DEBUG_LOG 
    DBG_8195A_I2S_LVL(SAL_I2S_LVL, "%s\n", __func__);
#endif 

    /*I2S Device Status Update*/
    pSalI2SHND->DevSts  = I2S_STS_UNINITIAL;
    
    return _EXIT_SUCCESS;
}


RTK_STATUS
RtkI2SSend(
    IN  VOID *Data
){
    PSAL_I2S_HND    pSalI2SHND = (PSAL_I2S_HND) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;   
    u32     I2SLocalTemp;
    u16 I2STxIdx = pSalI2SHND->pInitDat->I2STxIdx;
    //u8  I2SPageNum = pSalI2SHND->pInitDat->I2SPageNum;
    //u16 I2SPageSize = pSalI2SHND->pInitDat->I2SPageSize;
    
    //if (RtkI2SOpTypeChk(pSalI2SHND))
    //    return _EXIT_FAILURE;
    //check page own bit
    if (pHalI2SOP->HalI2SReadReg(pSalI2SHND->pInitDat, REG_I2S_TX_PAGE0_OWN+4*I2STxIdx))
    { //own bit is i2s controller 
        return _EXIT_FAILURE;

    } else {
        I2SLocalTemp = pHalI2SOP->HalI2STx(pSalI2SHND->pInitDat, pSalI2SHND->pTXBuf);
    }
    return _EXIT_SUCCESS;
}


RTK_STATUS
RtkI2SReceive(
    IN  VOID *Data
){
    PSAL_I2S_HND    pSalI2SHND = (PSAL_I2S_HND) Data;
    PHAL_I2S_OP     pHalI2SOP  = &HalI2SOpSAL;
    u32  I2SLocalTemp;
    u16 I2SRxIdx = pSalI2SHND->pInitDat->I2SRxIdx;

    if (pHalI2SOP->HalI2SReadReg(pSalI2SHND->pInitDat, REG_I2S_RX_PAGE0_OWN+4*I2SRxIdx))
    {  //own bit is i2s controller
         return _EXIT_FAILURE;
    } else {
        I2SLocalTemp = pHalI2SOP->HalI2SRx(pSalI2SHND->pInitDat, pSalI2SHND->pRXBuf);
    }
    return _EXIT_SUCCESS;
}


RTK_STATUS
RtkI2SIoCtrl(
    IN  VOID *Data
){
    return _EXIT_SUCCESS;
}


RTK_STATUS
RtkI2SPowerCtrl(
    IN  VOID *Data
){
    return _EXIT_SUCCESS;
}


PSAL_I2S_HND
RtkI2SGetSalHnd(
    IN  u8  I2SIdx
){
    if (RtkI2SIdxChk(I2SIdx))
        return (PSAL_I2S_HND)NULL;

    switch (I2SIdx){
#if I2S0_USED
        case I2S0_SEL:
            return (&SalI2S0Hnd);
#endif
#if I2S1_USED
        case I2S1_SEL:
            return (&SalI2S1Hnd);
#endif
        default:
            break;
    }

    return (PSAL_I2S_HND)NULL;
}

RTK_STATUS
RtkI2SLoadDefault( IN  VOID *Data
){
    PSAL_I2S_HND    pSalI2SHND = (PSAL_I2S_HND) Data;

    if (RtkI2SIdxChk(pSalI2SHND->DevNum))
        return _EXIT_FAILURE;
        
    if (pSalI2SHND->DevNum == I2S0_SEL)
    {
#if I2S0_USED
        pSalI2SHND->pInitDat = &HalI2S0InitData;
#endif        
    }
    else if(pSalI2SHND->DevNum == I2S1_SEL)
    {
#if I2S1_USED    
        pSalI2SHND->pInitDat = &HalI2S1InitData;
#endif        
    }
    else
    {
        /*The I2S module is NOT presented.*/
        return _EXIT_FAILURE;           
    }

    pSalI2SHND->I2SChNum        = I2S_CH_STEREO;
    pSalI2SHND->I2SPageNum      = I2S_4PAGE;
    pSalI2SHND->I2SPageSize     = (768/4)-1;//default is 48khz 4ms page size
    pSalI2SHND->I2SRate         = I2S_SR_48KHZ;
    pSalI2SHND->I2STRxAct       = I2S_TXRX;
    pSalI2SHND->I2SWordLen      = 0;

    pSalI2SHND->PinMux          = 0;
    pSalI2SHND->DevSts          = I2S_STS_UNINITIAL;
    pSalI2SHND->ErrType         = NULL;
    pSalI2SHND->TimeOut         = 0;
    
    pSalI2SHND->pTXBuf          = NULL;
    pSalI2SHND->pRXBuf          = NULL;
    pSalI2SHND->pUserCB         = NULL;
    
    pSalI2SHND->pInitDat->I2SIdx        = pSalI2SHND->DevNum;
    pSalI2SHND->pInitDat->I2SEn         = I2S_DISABLE;
    pSalI2SHND->pInitDat->I2SMaster     = 0;
    pSalI2SHND->pInitDat->I2SWordLen    = 0;
    pSalI2SHND->pInitDat->I2SChNum      = I2S_CH_STEREO;
    pSalI2SHND->pInitDat->I2SPageNum    = I2S_4PAGE;
    pSalI2SHND->pInitDat->I2SPageSize   = (768/4)-1;
    pSalI2SHND->pInitDat->I2SRate       = I2S_SR_48KHZ;
    pSalI2SHND->pInitDat->I2STRxAct     = I2S_TXRX;
    //pSalI2SHND->pInitDat->I2STxIntrMSK  = I2S_FEATURE_DISABLED;
    //pSalI2SHND->pInitDat->I2SRxIntrMSK  = I2S_FEATURE_DISABLED; 
    pSalI2SHND->pInitDat->I2STxIntrMSK  = 0xf;
    pSalI2SHND->pInitDat->I2SRxIntrMSK  = 0xf; 

    return _EXIT_SUCCESS;
}

VOID 
SalI2SOpInit(
    IN  VOID *Data
){
    PRTK_I2S_OP pSalI2SOp = (PRTK_I2S_OP) Data;
    
    pSalI2SOp->Init         = RtkI2SInit;
    pSalI2SOp->DeInit       = RtkI2SDeInit;
    pSalI2SOp->Send         = RtkI2SSend;
    pSalI2SOp->Receive      = RtkI2SReceive;
    pSalI2SOp->IoCtrl       = RtkI2SIoCtrl;
    pSalI2SOp->PowerCtrl    = RtkI2SPowerCtrl;
}


VOID HalI2SOpInit(
    IN  VOID *Data
)
{
    PHAL_I2S_OP pHalI2SOp = (PHAL_I2S_OP) Data;

    pHalI2SOp->HalI2SInit        = HalI2SInitRtl8195a;
    pHalI2SOp->HalI2SDeInit      = HalI2SDeInitRtl8195a;
    pHalI2SOp->HalI2STx          = HalI2STxRtl8195a;
    pHalI2SOp->HalI2SRx          = HalI2SRxRtl8195a;
    pHalI2SOp->HalI2SEnable      = HalI2SEnableRtl8195a;
    pHalI2SOp->HalI2SIntrCtrl    = HalI2SIntrCtrlRtl8195a;
    pHalI2SOp->HalI2SReadReg     = HalI2SReadRegRtl8195a;
    pHalI2SOp->HalI2SSetRate     = HalI2SSetRateRtl8195a;
    pHalI2SOp->HalI2SSetWordLen  = HalI2SSetWordLenRtl8195a;
    pHalI2SOp->HalI2SSetChNum    = HalI2SSetChNumRtl8195a;
    pHalI2SOp->HalI2SSetPageNum  = HalI2SSetPageNumRtl8195a;
    pHalI2SOp->HalI2SSetPageSize = HalI2SSetPageSizeRtl8195a;
    pHalI2SOp->HalI2SClrIntr     = HalI2SClrIntrRtl8195a;
    pHalI2SOp->HalI2SClrAllIntr  = HalI2SClrAllIntrRtl8195a;
    pHalI2SOp->HalI2SDMACtrl     = HalI2SDMACtrlRtl8195a;
}


