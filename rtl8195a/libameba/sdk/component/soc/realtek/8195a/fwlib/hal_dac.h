//======================================================
//  Routines to access hardware
//
//  Copyright (c) 2013 Realtek Semiconductor Corp.
//
//  This module is a confidential and proprietary property of RealTek and
//  possession or use of this module requires written permission of RealTek.
//======================================================
#ifndef _HAL_DAC_H_
#define _HAL_DAC_H_

#include "rtl8195a.h"
#include "rtl8195a_dac.h"
#include "hal_api.h"
#include "hal_gdma.h"

//================ DAC Configuration  =========================
#define DAC_INTR_OP_TYPE    1
#define DAC_DMA_OP_TYPE     1

// DAC SAL management macros
#define SAL_DAC_USER_CB_NUM     (sizeof(SAL_DAC_USER_CB) / sizeof(PSAL_DAC_USERCB_ADPT))

// DAC SAL used module. 
// Please set the DAC module flag to 1 to enable the related DAC module functions.
#define DAC0_USED                   1
#define DAC1_USED                   1


//================ Debug MSG Definition =======================
#define DAC_PREFIX      "RTL8195A[dac]: "
#define DAC_PREFIX_LVL  "    [DAC_DBG]: "

typedef enum _DAC_DBG_LVL_ {
    HAL_DAC_LVL         =   0x00,
    SAL_DAC_LVL         =   0x02,
    VERI_DAC_LVL        =   0x04,
}DAC_DBG_LVL,*PDAC_DBG_LVL;

#ifdef CONFIG_DEBUG_LOG
#ifdef CONFIG_DEBUG_LOG_DAC_HAL

    #define DBG_8195A_DAC(...)  do{ \
        _DbgDump("\r"DAC_PREFIX __VA_ARGS__);\
    }while(0)


    #define DACDBGLVL   0xFF   
    #define DBG_8195A_DAC_LVL(LVL,...)  do{\
            if (LVL&DACDBGLVL){\
                _DbgDump("\r"DAC_PREFIX_LVL __VA_ARGS__);\
            }\
    }while(0)
#else
    #define DBG_DAC_LOG_PERD    100
    #define DBG_8195A_DAC(...)
    #define DBG_8195A_DAC_LVL(...)
#endif
#endif


//================ DAC HAL Related Enumeration ==================
// DAC Module Selection 
typedef enum _DAC_MODULE_SEL_ {
        DAC0_SEL    =   0x0,
        DAC1_SEL    =   0x1,
}DAC_MODULE_SEL,*PDAC_MODULE_SEL;

// DAC module status 
typedef enum _DAC_MODULE_STATUS_ {
    DAC_DISABLE     =   0x0,
    DAC_ENABLE      =   0x1,
}DAC_MODULE_STATUS, *PDAC_MODULE_STATUS;

// DAC Data Rate
typedef enum _DAC_DATA_RATE_ {
    DAC_DATA_RATE_10K   =   0x0,
    DAC_DATA_RATE_250K  =   0x1,
}DAC_DATA_RATE,*PDAC_DATA_RATE;

// DAC Data Endian
typedef enum _DAC_DATA_ENDIAN_ {
    DAC_DATA_ENDIAN_LITTLE      =   0x0,
    DAC_DATA_ENDIAN_BIG         =   0x1,
}DAC_DATA_ENDIAN,*PDAC_DATA_ENDIAN;

// DAC Debug Select
typedef enum _DAC_DEBUG_SEL_ {
    DAC_DBG_SEL_DISABLE         =   0x0,
    DAC_DBG_SEL_ENABLE          =   0x1,
}DAC_DEBUG_SEL,*PDAC_DEBUG_SEL;

// DAC Dsc Debug Select
typedef enum _DAC_DSC_DEBUG_SEL_ {
    DAC_DSC_DBG_SEL_DISABLE         =   0x0,
    DAC_DSC_DBG_SEL_ENABLE          =   0x1,
}DAC_DSC_DEBUG_SEL,*PDAC_DSC_DEBUG_SEL;


// DAC Bypass Dsc Debug Select
typedef enum _DAC_BYPASS_DSC_SEL_ {
    DAC_BYPASS_DSC_SEL_DISABLE          =   0x0,
    DAC_BYPASS_DSC_SEL_ENABLE           =   0x1,
}DAC_BYPASS_DSC_SEL,*PDAC_BYPASS_DSC_SEL;

// DAC feature status 
typedef enum _DAC_FEATURE_STATUS_{
    DAC_FEATURE_DISABLED    =   0,
    DAC_FEATURE_ENABLED     =   1,
}DAC_FEATURE_STATUS,*PDAC_FEATURE_STATUS;

// DAC operation type
typedef enum _DAC_OP_TYPE_ {
    DAC_POLL_TYPE   =   0x0,
    DAC_DMA_TYPE    =   0x1,
    DAC_INTR_TYPE   =   0x2,
}DAC_OP_TYPE, *PDAC_OP_TYPE;

// DAC device status 
typedef enum _DAC_Device_STATUS_ {
    DAC_STS_UNINITIAL   =   0x00,
    DAC_STS_INITIALIZED =   0x01,
    DAC_STS_IDLE        =   0x02,
    
    DAC_STS_TX_READY    =   0x03,    
    DAC_STS_TX_ING      =   0x04,
    
    DAC_STS_RX_READY    =   0x05,
    DAC_STS_RX_ING      =   0x06,

    DAC_STS_ERROR       =   0x07,
}DAC_Device_STATUS, *PDAC_Device_STATUS;

//DAC device error type
typedef enum _DAC_ERR_TYPE_ {
    DAC_ERR_FIFO_OVER       =   0x04,       //DAC FIFO overflow.
    DAC_ERR_FIFO_STOP       =   0x08,       //DAC FIFO is completely empty, and it will be stopped automatically.
    DAC_ERR_FIFO_WRFAIL     =   0x10,       //When DAC is NOT enabled, a write operation attempts to access DAC register.
    DAC_ERR_FIFO_DSC_OVER0  =   0x20,
    DAC_ERR_FIFO_DSC_OVER1  =   0x40,
}DAC_ERR_TYPE, *PDAC_ERR_TYPE;

// DAC data input method
typedef enum _DAC_INPUT_TYPE_{
    DAC_INPUT_SINGLE_WR     =   0x1,        //DAC input by using single register write
    DAC_INPUT_DMA_ONEBLK    =   0x2,        //DAC input by using single DMA block
    DAC_INPUT_DMA_LLP       =   0x3,        //DAC input by using DMA linked list mode
}DAC_INPUT_TYPE,*PDAC_INPUT_TYPE;




//======================================================
// DAC HAL initial data structure 
typedef struct _HAL_DAC_INIT_DAT_ {
    u8                  DACIdx;         //DAC index used
    u8                  DACEn;          //DAC module enable
    u8                  DACDataRate;    //DAC data rate, 1'b0:10KHz, 1'b1:250KHz
    u8                  DACEndian;      //DAC endian selection, 
                                        //but actually it's for 32-bit DAC data swap control
                                        //1'b0: no swap, 
                                        //1'b1: swap the upper 16-bit and the lower 16-bit
    u8                  DACFilterSet;   //DAC filter settle
    u8                  DACBurstSz;     //DAC burst size
    u8                  DACDbgSel;      //DAC debug sel
    u8                  DACDscDbgSel;   //DAC debug dsc sel
    
    u8                  DACBPDsc;       //DAC bypass delta sigma for loopback
    u8                  DACDeltaSig;    //DAC bypass value of delta sigma
    u16                 RSVD1;          


    
    u32                 *DACData;       //DAC data pointer
    u32                 DACPWCtrl;      //DAC0 and DAC1 power control
    u32                 DACAnaCtrl0;    //DAC anapar_da control 0
    u32                 DACAnaCtrl1;    //DAC anapar_da control 1
    u32                 DACIntrMSK;     //DAC Interrupt Mask
}HAL_DAC_INIT_DAT,*PHAL_DAC_INIT_DAT;

// DAC HAL Operations
typedef struct _HAL_DAC_OP_ {
    RTK_STATUS  (*HalDACInit)       (VOID *Data);   //HAL DAC initialization
    RTK_STATUS  (*HalDACDeInit)     (VOID *Data);   //HAL DAC de-initialization
    RTK_STATUS  (*HalDACEnable)     (VOID *Data);   //HAL DAC de-initialization
    u8          (*HalDACSend)       (VOID *Data);   //HAL DAC receive
    RTK_STATUS  (*HalDACIntrCtrl)   (VOID *Data);   //HAL DAC interrupt control
    u32         (*HalDACReadReg)    (VOID *Data, u8 DACReg);//HAL DAC read register
}HAL_DAC_OP, *PHAL_DAC_OP;

// DAC user callback adapter
typedef struct _SAL_DAC_USERCB_ADPT_ {
    VOID (*USERCB)      (VOID *Data);
    u32  USERData;
}SAL_DAC_USERCB_ADPT, *PSAL_DAC_USERCB_ADPT;

// DAC user callback structure
typedef struct _SAL_DAC_USER_CB_ {
    PSAL_DAC_USERCB_ADPT    pTXCB;          //DAC Transmit Callback
    PSAL_DAC_USERCB_ADPT    pTXCCB;         //DAC Transmit Complete Callback
    PSAL_DAC_USERCB_ADPT    pRXCB;          //DAC Receive Callback
    PSAL_DAC_USERCB_ADPT    pRXCCB;         //DAC Receive Complete Callback
    PSAL_DAC_USERCB_ADPT    pRDREQCB;       //DAC Read Request Callback
    PSAL_DAC_USERCB_ADPT    pERRCB;         //DAC Error Callback
    PSAL_DAC_USERCB_ADPT    pDMATXCB;       //DAC DMA Transmit Callback
    PSAL_DAC_USERCB_ADPT    pDMATXCCB;      //DAC DMA Transmit Complete Callback
    PSAL_DAC_USERCB_ADPT    pDMARXCB;       //DAC DMA Receive Callback
    PSAL_DAC_USERCB_ADPT    pDMARXCCB;      //DAC DMA Receive Complete Callback
}SAL_DAC_USER_CB, *PSAL_DAC_USER_CB;

// DAC Transmit Buffer
typedef struct _SAL_DAC_TRANSFER_BUF_ {
    u32     DataLen;                        //DAC Transmfer Length
    u32     *pDataBuf;                      //DAC Transfer Buffer Pointer
    u32     RSVD;                           //
}SAL_DAC_TRANSFER_BUF,*PSAL_DAC_TRANSFER_BUF;

typedef struct _SAL_DAC_DMA_USER_DEF_ {
    
    u8      TxDatSrcWdth;
    u8      TxDatDstWdth;
    u8      TxDatSrcBstSz;
    u8      TxDatDstBstSz;

    u8      TxChNo;
    u8      LlpCtrl;
    u16     RSVD0;

    u32     MaxMultiBlk;    
    u32     pLlix;
    u32     pBlockSizeList;
}SAL_DAC_DMA_USER_DEF, *PSAL_DAC_DMA_USER_DEF;

// Software API Level DAC Handler 
typedef struct _SAL_DAC_HND_ {
    u8                      DevNum;             //DAC device number
    u8                      PinMux;             //DAC pin mux seletion
    u8                      OpType;             //DAC operation type selection
    volatile u8             DevSts;             //DAC device status

    u8                      DACInType;          //DAC input type
    u8                      RSVD0;
    u16                     RSVD1;
    
    u32                     DACExd;             //DAC extended options:
                                                //bit 0: example
                                                //bit 31~bit 1: Reserved
    u32                     ErrType;            //
    u32                     TimeOut;            //DAC IO Timeout count
                                                                            
    PHAL_DAC_INIT_DAT       pInitDat;           //Pointer to DAC initial data struct
    PSAL_DAC_TRANSFER_BUF   pTXBuf;             //Pointer to DAC TX buffer
    PSAL_DAC_USER_CB        pUserCB;            //Pointer to DAC User Callback
    PSAL_DAC_DMA_USER_DEF   pDMAConf;           //Pointer to DAC User Define DMA Config
}SAL_DAC_HND, *PSAL_DAC_HND;

// DAC SAL handle private 
typedef struct _SAL_DAC_HND_PRIV_ {
    VOID            **ppSalDACHnd;              //Pointer to SAL_DAC_HND pointer
    SAL_DAC_HND     SalDACHndPriv;              //Private SAL_DAC_HND
}SAL_DAC_HND_PRIV, *PSAL_DAC_HND_PRIV;

//DAC SAL management adapter
typedef struct _SAL_DAC_MNGT_ADPT_ {
    PSAL_DAC_HND_PRIV       pSalHndPriv;                //Pointer to SAL_DAC_HND
    PHAL_DAC_INIT_DAT       pHalInitDat;                //Pointer to HAL DAC initial data( HAL_I2C_INIT_DAT )
    PHAL_DAC_OP             pHalOp;                     //Pointer to HAL DAC operation( HAL_DAC_OP )
    VOID                    (*pHalOpInit)(VOID*);       //Pointer to HAL DAC initialize function
    PIRQ_HANDLE             pIrqHnd;                    //Pointer to IRQ handler in SAL layer( IRQ_HANDLE )
    PSAL_DAC_USER_CB        pUserCB;                    //Pointer to SAL user callbacks (SAL_DAC_USER_CB )
    VOID                    (*pSalIrqFunc)(VOID*);      //Used for SAL DAC interrupt function

    PSAL_DAC_DMA_USER_DEF   pDMAConf;                   //Pointer to DAC User Define DMA config
    PHAL_GDMA_ADAPTER       pHalGdmaAdp;
    PHAL_GDMA_OP            pHalGdmaOp;
    VOID                    (*pHalGdmaOpInit)(VOID*);   //Pointer to HAL DAC initialize function    
    PIRQ_HANDLE             pIrqGdmaHnd;
    VOID                    (*pSalDMAIrqFunc)(VOID*);   //Used for SAL DAC interrupt function
}SAL_DAC_MNGT_ADPT, *PSAL_DAC_MNGT_ADPT;


//================ DAC HAL Function Prototype ===================
// DAC HAL inline function
// For checking DAC input index valid or not
static inline RTK_STATUS
RtkDACIdxChk(
    IN  u8  DACIdx
)
{
#if !DAC0_USED
    if (DACIdx == DAC0_SEL)
        return _EXIT_FAILURE;
#endif

#if !DAC1_USED
    if (DACIdx == DAC1_SEL)
        return _EXIT_FAILURE;
#endif
        
    return _EXIT_SUCCESS;
}

VOID HalDACOpInit(IN VOID *Data);
RTK_STATUS RtkDACLoadDefault(IN  VOID *Data);
RTK_STATUS RtkDACInit(IN  VOID *Data);
RTK_STATUS RtkDACDeInit(IN  VOID *Data);
RTK_STATUS RtkDACSend(IN  VOID *Data);
PSAL_DAC_HND RtkDACGetSalHnd(IN  u8  DACIdx);
RTK_STATUS RtkDACFreeSalHnd(IN  PSAL_DAC_HND    pSalDACHND);
PSAL_DAC_MNGT_ADPT RtkDACGetMngtAdpt(IN  u8  DACIdx);
RTK_STATUS RtkDACFreeMngtAdpt(IN  PSAL_DAC_MNGT_ADPT  pSalDACMngtAdpt);

#endif
