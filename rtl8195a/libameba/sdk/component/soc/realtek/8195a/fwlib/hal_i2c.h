/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_I2C_H_     //#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_

#include "rtl8195a_i2c.h"
#include "hal_gdma.h"

//================= I2C CONFIGURATION START ==================
// I2C SAL User Configuration Flags

// I2C SAL operation types
#define I2C_POLL_OP_TYPE            1
#define I2C_INTR_OP_TYPE            1
#define I2C_DMA_OP_TYPE             1

// I2C supports user register address
#define I2C_USER_REG_ADDR           1       //I2C User specific register address by using 
                                            //the first I2C data as the register 
                                            //address
                                            
// I2C SAL used module. Please set the I2C module flag to 1 to enable the related 
// I2C module functions.
#define I2C0_USED                   1
#define I2C1_USED                   1
#define I2C2_USED                   1
#define I2C3_USED                   1
//================= I2C CONFIGURATION END  ===================


//================= I2C HAL START  ==========================
// I2C debug output
#define I2C_PREFIX      "RTL8195A[i2c]: "
#define I2C_PREFIX_LVL  "    [i2c_DBG]: "

typedef enum _I2C_DBG_LVL_ {
    HAL_I2C_LVL         =   0x01,
    SAL_I2C_LVL         =   0x02,
    VERI_I2C_LVL        =   0x03,
}I2C_DBG_LVL,*PI2C_DBG_LVL;

#ifdef CONFIG_DEBUG_LOG
#ifdef CONFIG_DEBUG_LOG_I2C_HAL
#define DBG_I2C_LOG_PERD    100

    #define I2CDBGLVL   0xFF
    #define DBG_8195A_I2C(...)  do{ \
                                    _DbgDump("\r"I2C_PREFIX __VA_ARGS__);\
                                }while(0)
   
    #define DBG_8195A_I2C_LVL(LVL,...)  do{\
                                            if (LVL&I2CDBGLVL){\
                                                _DbgDump("\r"I2C_PREFIX_LVL __VA_ARGS__);\
                                            }\
                                        }while(0)
#else
    #define DBG_I2C_LOG_PERD    100
    #define DBG_8195A_I2C(...)
    #define DBG_8195A_I2C_LVL(...)
#endif
#else
    #define DBG_I2C_LOG_PERD    100
    #define DBG_8195A_I2C(...)
    #define DBG_8195A_I2C_LVL(...)
#endif

//======================================================
// I2C HAL related enumeration
// I2C Module Selection 
typedef enum _I2C_MODULE_SEL_ {
        I2C0_SEL    =   0x0,
        I2C1_SEL    =   0x1,
        I2C2_SEL    =   0x2,
        I2C3_SEL    =   0x3,
}I2C_MODULE_SEL,*PI2C_MODULE_SEL;

// I2C HAL initial data structure 
typedef struct _HAL_I2C_INIT_DAT_ {
    u8                  I2CIdx;         //I2C index used
    u8                  I2CEn;          //I2C module enable
    u8                  I2CMaster;      //Master or Slave mode
    u8                  I2CAddrMod;     //I2C addressing mode(7-bit, 10-bit)

    u8                  I2CSpdMod;      //I2C  speed mode(Standard, Fast, High)
    u8                  I2CSetup;       //I2C SDA setup time
    u8                  I2CRXTL;        //I2C RX FIFO Threshold
    u8                  I2CTXTL;        //I2C TX FIFO Threshold

    u8                  I2CBusLd;       //I2C bus load (pf) for high speed mode
    u8                  I2CReSTR;       //I2C restart support
    u8                  I2CGC;          //I2C general support
    u8                  I2CStartB;      //I2C start byte support

    u8                  I2CSlvNoAck;    //I2C slave no ack support
    u8                  I2CDMACtrl;     //I2C DMA feature support
    u8                  I2CCmd;         //I2C Command
    u8                  I2CDataLen;     //I2C Data Length

    u8                  I2CSlvAckGC;    //I2C slave acks to General Call
    u8                  I2CStop;        //I2C issues STOP bit or not
    u16                 RSVD0;
    
    u8                  *I2CRWData;     //I2C Read/Write data pointer

    u16                 I2CIntrMSK;     //I2C Interrupt Mask
    u16                 I2CIntrClr;     //I2C Interrupt register to clear 

    u16                 I2CAckAddr;     //I2C target address in I2C Master mode,
                                        //ack address in I2C Slave mode    
    u16                 I2CSdaHd;       //I2C SDA hold time

    u32                 I2CClk;         //I2C bus clock (in kHz)

    u8                  I2CTxDMARqLv;   //I2C TX DMA Empty Level
    u8                  I2CRxDMARqLv;   //I2C RX DMA Full Level
    u16                 RSVD1;          //Reserved
}HAL_I2C_INIT_DAT,*PHAL_I2C_INIT_DAT;

// I2C HAL Operations
typedef struct _HAL_I2C_OP_ {
    HAL_Status  (*HalI2CInit)       (VOID *Data);   //HAL I2C initialization
    HAL_Status  (*HalI2CDeInit)     (VOID *Data);   //HAL I2C de-initialization
    HAL_Status  (*HalI2CSend)       (VOID *Data);   //HAL I2C send
    u8          (*HalI2CReceive)    (VOID *Data);   //HAL I2C receive
    HAL_Status  (*HalI2CEnable)     (VOID *Data);   //HAL I2C enable module
    HAL_Status  (*HalI2CIntrCtrl)   (VOID *Data);   //HAL I2C interrupt control
    u32         (*HalI2CReadReg)    (VOID *Data, u8 I2CReg);//HAL I2C read register
    HAL_Status  (*HalI2CWriteReg)   (VOID *Data, u8 I2CReg, u32 RegVal);//HAL I2C write register
    HAL_Status  (*HalI2CSetCLK)     (VOID *Data);   //HAL I2C set bus clock
    HAL_Status  (*HalI2CMassSend)   (VOID *Data);   //HAL I2C mass send
    HAL_Status  (*HalI2CClrIntr)    (VOID *Data);   //HAL I2C clear interrupts
    HAL_Status  (*HalI2CClrAllIntr) (VOID *Data);   //HAL I2C clear all interrupts
    HAL_Status  (*HalI2CDMACtrl)    (VOID *Data);   //HAL I2C DMA control
}HAL_I2C_OP, *PHAL_I2C_OP;
//================= I2C HAL END   ===========================


//================= I2C SAL START  ==========================
//I2C SAL Macros

//======================================================
// I2C SAL related enumerations
// I2C Extend Features
typedef enum _I2C_EXD_SUPPORT_{
    I2C_EXD_RESTART     =   0x1,            //BIT_0, RESTART bit
    I2C_EXD_GENCALL     =   0x2,            //BIT_1, Master generates General Call. All "send" operations generate General Call addresss
    I2C_EXD_STARTB      =   0x4,            //BIT_2, Using START BYTE, instead of START Bit
    I2C_EXD_SLVNOACK    =   0x8,            //BIT_3, Slave no ack to master
    I2C_EXD_BUS400PF    =   0x10,           //BIT_4, I2C bus loading is 400pf
    I2C_EXD_SLVACKGC    =   0x20,           //BIT_5, Slave acks to a General Call
    I2C_EXD_USER_REG    =   0x40,           //BIT_6, Using User Register Address
    I2C_EXD_USER_TWOB   =   0x80,           //BIT_7, User Register Address is 2-byte
    I2C_EXD_MTR_ADDR_RTY=   0x100,          //BIT_8, Master retries to send start condition and Slave address when the slave doesn't ack 
                                            //         the address.
    I2C_EXD_MTR_ADDR_UPD=   0x200,         //BIT_9, Master dynamically updates slave address
    I2C_EXD_MTR_HOLD_BUS=   0x400,         //BIT_10, Master doesn't generate STOP when the FIFO is empty. This would make Master hold
                                            //         the bus.
}I2C_EXD_SUPPORT,*PI2C_EXD_SUPPORT;

// I2C operation type
typedef enum _I2C_OP_TYPE_ {
    I2C_POLL_TYPE   =   0x0,
    I2C_DMA_TYPE    =   0x1,
    I2C_INTR_TYPE   =   0x2,
}I2C_OP_TYPE, *PI2C_OP_TYPE;

// I2C pinmux selection 
typedef enum _I2C_PINMUX_ {
    I2C_PIN_S0      =   0x0,
    I2C_PIN_S1      =   0x1,
    I2C_PIN_S2      =   0x2,
    I2C_PIN_S3      =   0x3,    //Only valid for I2C0 and I2C3
}I2C_PINMUX, *PI2C_PINMUX;

// I2C module status 
typedef enum _I2C_MODULE_STATUS_ {
    I2C_DISABLE     =   0x0,
    I2C_ENABLE      =   0x1,
}I2C_MODULE_STATUS, *PI2C_MODULE_STATUS;

// I2C device status 
typedef enum _I2C_Device_STATUS_ {
    I2C_STS_UNINITIAL   =   0x00,
    I2C_STS_INITIALIZED =   0x01,
    I2C_STS_IDLE        =   0x02,
    
    I2C_STS_TX_READY    =   0x03,    
    I2C_STS_TX_ING      =   0x04,
    
    I2C_STS_RX_READY    =   0x05,
    I2C_STS_RX_ING      =   0x06,

    I2C_STS_ERROR       =   0x10,
    I2C_STS_TIMEOUT     =   0x11, 
}I2C_Device_STATUS, *PI2C_Device_STATUS;

// I2C feature status 
typedef enum _I2C_FEATURE_STATUS_{
    I2C_FEATURE_DISABLED    =   0,
    I2C_FEATURE_ENABLED     =   1,
}I2C_FEATURE_STATUS,*PI2C_FEATURE_STATUS;

// I2C device mode
typedef enum _I2C_DEV_MODE_ {
    I2C_SLAVE_MODE  =   0x0,
    I2C_MASTER_MODE =   0x1,
}I2C_DEV_MODE, *PI2C_DEV_MODE;

// I2C Bus Transmit/Receive
typedef enum _I2C_DIRECTION_ {
    I2C_ONLY_TX     =   0x1,
    I2C_ONLY_RX     =   0x2,
    I2C_TXRX        =   0x3,
}I2C_DIRECTION, *PI2C_DIRECTION;

//I2C DMA module number
typedef enum _I2C_DMA_MODULE_SEL_ {
    I2C_DMA_MODULE_0    =   0x0,
    I2C_DMA_MODULE_1    =   0x1
}I2C_DMA_MODULE_SEL, *PI2C_DMA_MODULE_SEL;

// I2C0 DMA peripheral number
typedef enum _I2C0_DMA_PERI_NUM_ {
    I2C0_DMA_TX_NUM =   0x8,
    I2C0_DMA_RX_NUM =   0x9,
}I2C0_DMA_PERI_NUM,*PI2C0_DMA_PERI_NUM;

// I2C1 DMA peripheral number
typedef enum _I2C1_DMA_PERI_NUM_ {
    I2C1_DMA_TX_NUM =   0xA,
    I2C1_DMA_RX_NUM =   0xB,
}I2C1_DMA_PERI_NUM,*PI2C1_DMA_PERI_NUM;

// I2C0 DMA module used
typedef enum _I2C0_DMA_MODULE_ {
    I2C0_DMA0   =   0x0,
    I2C0_DMA1   =   0x1,
}I2C0_DMA_MODULE,*PI2C0_DMA_MODULE;

// I2C0 DMA module used
typedef enum _I2C1_DMA_MODULE_ {
    I2C1_DMA0   =   0x0,
    I2C1_DMA1   =   0x1,
}I2C1_DMA_MODULE,*PI2C1_DMA_MODULE;

// I2C command type 
typedef enum _I2C_COMMAND_TYPE_ {
    I2C_WRITE_CMD   =   0x0,
    I2C_READ_CMD    =   0x1,
}I2C_COMMAND_TYPE,*PI2C_COMMAND_TYPE;

// I2C STOP BIT 
typedef enum _I2C_STOP_TYPE_ {
    I2C_STOP_DIS    =   0x0,
    I2C_STOP_EN     =   0x1,
}I2C_STOP_TYPE, *PI2C_STOP_TYPE;

// I2C error type 
typedef enum _I2C_ERR_TYPE_ {
    I2C_ERR_RX_UNDER    =   0x01,           //I2C RX FIFO Underflow
    I2C_ERR_RX_OVER     =   0x02,           //I2C RX FIFO Overflow
    I2C_ERR_TX_OVER     =   0x04,           //I2C TX FIFO Overflow
    I2C_ERR_TX_ABRT     =   0x08,           //I2C TX terminated
    I2C_ERR_SLV_TX_NACK =   0x10,           //I2C slave transmission terminated by master NACK, 
                                            //but there are data in slave TX FIFO
    I2C_ERR_USER_REG_TO =   0x20,

    I2C_ERR_RX_CMD_TO   =   0x21,
    I2C_ERR_RX_FF_TO    =   0x22,
    I2C_ERR_TX_CMD_TO   =   0x23,
    I2C_ERR_TX_FF_TO    =   0x24,

    I2C_ERR_TX_ADD_TO   =   0x25,
    I2C_ERR_RX_ADD_TO   =   0x26,
}I2C_ERR_TYPE, *PI2C_ERR_TYPE;

// I2C Time Out type
typedef enum _I2C_TIMEOUT_TYPE_ {
    I2C_TIMEOOUT_DISABLE    =   0x00,           
    I2C_TIMEOOUT_ENDLESS    =   0xFFFFFFFF,
}I2C_TIMEOUT_TYPE, *PI2C_TIMEOUT_TYPE;

//======================================================
// SAL I2C related data structures
// I2C user callback adapter
typedef struct _SAL_I2C_USERCB_ADPT_ {
    VOID (*USERCB)      (VOID *Data);
    u32  USERData;
}SAL_I2C_USERCB_ADPT, *PSAL_I2C_USERCB_ADPT;

// I2C user callback structure
typedef struct _SAL_I2C_USER_CB_ {
    PSAL_I2C_USERCB_ADPT    pTXCB;          //I2C Transmit Callback
    PSAL_I2C_USERCB_ADPT    pTXCCB;         //I2C Transmit Complete Callback
    PSAL_I2C_USERCB_ADPT    pRXCB;          //I2C Receive Callback
    PSAL_I2C_USERCB_ADPT    pRXCCB;         //I2C Receive Complete Callback
    PSAL_I2C_USERCB_ADPT    pRDREQCB;       //I2C Read Request Callback
    PSAL_I2C_USERCB_ADPT    pERRCB;         //I2C Error Callback
    PSAL_I2C_USERCB_ADPT    pDMATXCB;       //I2C DMA Transmit Callback
    PSAL_I2C_USERCB_ADPT    pDMATXCCB;      //I2C DMA Transmit Complete Callback
    PSAL_I2C_USERCB_ADPT    pDMARXCB;       //I2C DMA Receive Callback
    PSAL_I2C_USERCB_ADPT    pDMARXCCB;      //I2C DMA Receive Complete Callback
    PSAL_I2C_USERCB_ADPT    pGENCALLCB;     //I2C General Call Callback
}SAL_I2C_USER_CB, *PSAL_I2C_USER_CB;

// I2C Transmit Buffer
typedef struct _SAL_I2C_TRANSFER_BUF_ {
    u16     DataLen;                        //I2C Transmfer Length
    u16     TargetAddr;                     //I2C Target Address. It's only valid in Master Mode.
    u32     RegAddr;                        //I2C Register Address. It's only valid in Master Mode.
    u32     RSVD;                           //
    u8      *pDataBuf;                      //I2C Transfer Buffer Pointer
}SAL_I2C_TRANSFER_BUF,*PSAL_I2C_TRANSFER_BUF;

typedef struct _SAL_I2C_DMA_USER_DEF_ {
    u8      TxDatSrcWdth;
    u8      TxDatDstWdth;
    u8      TxDatSrcBstSz;
    u8      TxDatDstBstSz;
    u8      TxChNo;
    u8      RSVD0;
    u16     RSVD1;
    u8      RxDatSrcWdth;
    u8      RxDatDstWdth;
    u8      RxDatSrcBstSz;
    u8      RxDatDstBstSz;
    u8      RxChNo;
    u8      RSVD2;
    u16     RSVD3;
}SAL_I2C_DMA_USER_DEF, *PSAL_I2C_DMA_USER_DEF;

// RTK I2C OP
typedef struct _RTK_I2C_OP_ {
    HAL_Status (*Init)      (VOID *Data);
    HAL_Status (*DeInit)    (VOID *Data);
    HAL_Status (*Send)      (VOID *Data);
    HAL_Status (*Receive)   (VOID *Data);
    HAL_Status (*IoCtrl)    (VOID *Data);
    HAL_Status (*PowerCtrl) (VOID *Data);    
}RTK_I2C_OP, *PRTK_I2C_OP;

// Software API Level I2C Handler 
typedef struct _SAL_I2C_HND_ {
    u8                      DevNum;             //I2C device number
    u8                      PinMux;             //I2C pin mux seletion
    u8                      OpType;             //I2C operation type selection
    volatile u8             DevSts;             //I2C device status
    
    u8                      I2CMaster;          //I2C Master or Slave mode
    u8                      I2CAddrMod;         //I2C 7-bit or 10-bit mode
    u8                      I2CSpdMod;          //I2C SS/ FS/ HS speed mode
    u8                      I2CAckAddr;         //I2C target address in Master 
                                                //mode or ack address in Slave
                                                //mode
                                                
    u16                     I2CClk;             //I2C bus clock
    u8                      MasterRead;         //I2C Master Read Supported,
                                                //An Address will be sent before
                                                //read data back.
                                                
    u8                      I2CDmaSel;          //I2C DMA module select
                                                //         0 for DMA0,
                                                //         1 for DMA1
    u8                      I2CTxDMARqLv;       //I2C TX DMA Empty Level
    u8                      I2CRxDMARqLv;       //I2C RX DMA Full Level
    u16                     RSVD0;              //Reserved

    u32                     AddRtyTimeOut;      //I2C TimeOut Value for master send address retry
                                                //(Originally Reserved.)
    
    u32                     I2CExd;             //I2C extended options:
                                                //bit 0: I2C RESTART supported,
                                                //          0 for NOT supported,
                                                //          1 for supported
                                                //bit 1: I2C General Call supported
                                                //          0 for NOT supported,
                                                //          1 for supported
                                                //bit 2: I2C START Byte supported
                                                //          0 for NOT supported,
                                                //          1 for supported
                                                //bit 3: I2C Slave-No-Ack
                                                //         supported
                                                //          0 for NOT supported,
                                                //          1 for supported
                                                //bit 4: I2C bus loading,
                                                //          0 for 100pf, 
                                                //          1  for 400pf
                                                //bit 5: I2C slave ack to General
                                                //         Call
                                                //bit 6: I2C User register address
                                                //bit 7: I2C 2-Byte User register
                                                //         address
                                                //bit 8: I2C slave address no ack retry,
                                                //        It's only for Master mode,
                                                //        when slave doesn't ack the
                                                //        address
                                                //bit 31~bit 8: Reserved
    u32                     ErrType;            //
    u32                     TimeOut;            //I2C IO Timeout count,  in ms
                                                                            
    PHAL_I2C_INIT_DAT       pInitDat;           //Pointer to I2C initial data struct
    PSAL_I2C_TRANSFER_BUF   pTXBuf;             //Pointer to I2C TX buffer
    PSAL_I2C_TRANSFER_BUF   pRXBuf;             //Pointer to I2C RX buffer
    PSAL_I2C_USER_CB        pUserCB;            //Pointer to I2C User Callback
    PSAL_I2C_DMA_USER_DEF   pDMAConf;           //Pointer to I2C User Define DMA config
}SAL_I2C_HND, *PSAL_I2C_HND;



//======================================================
// I2C SAL Function Prototypes

// For checking I2C input index valid or not
static inline HAL_Status
RtkI2CIdxChk(
    IN  u8  I2CIdx
)
{
    if (I2CIdx > I2C3_SEL)
        return HAL_ERR_UNKNOWN;
        
    return HAL_OK;
}
#if 0
//For checking I2C operation type valid or not
static inline HAL_Status
RtkI2COpTypeChk(
    IN  VOID *Data
)
{
    PSAL_I2C_HND    pSalI2CHND = (PSAL_I2C_HND) Data;

    if (pSalI2CHND->OpType == I2C_POLL_TYPE)
        return HAL_ERR_UNKNOWN;

    if (pSalI2CHND->OpType == I2C_DMA_TYPE)
        return HAL_ERR_UNKNOWN;

    if (pSalI2CHND->OpType == I2C_INTR_TYPE)
        return HAL_ERR_UNKNOWN;

    pSalI2CHND = pSalI2CHND;
    
    return HAL_OK;
}
#endif
//For checking I2C DMA available or not
static inline HAL_Status
RtkI2CDMAChk(
    IN  VOID *Data
)
{
    PSAL_I2C_HND    pSalI2CHND = (PSAL_I2C_HND) Data;

    if (pSalI2CHND->OpType == I2C_DMA_TYPE) {
        if (pSalI2CHND->DevNum >= I2C2_SEL)
            return HAL_ERR_UNKNOWN;
    }
    else {
        return HAL_ERR_UNKNOWN;
    }

    return HAL_OK;
}

//For checking I2C DMA available or not
static inline HAL_Status
RtkI2CDMAInitChk(
    IN  VOID *Data
)
{
    PSAL_I2C_HND    pSalI2CHND = (PSAL_I2C_HND) Data;

    if (pSalI2CHND->OpType != I2C_DMA_TYPE) {
        return HAL_ERR_UNKNOWN;
    }
    else {
        return HAL_OK;
    }
    
}

//======================================================
//SAL I2C management function prototype
_LONG_CALL_ROM_ HAL_Status RtkI2CLoadDefault(IN VOID *Data);
_LONG_CALL_ROM_ HAL_Status RtkI2CInit(IN  VOID *Data);
_LONG_CALL_ROM_ HAL_Status RtkI2CDeInit(IN  VOID *Data);
_LONG_CALL_ROM_ HAL_Status RtkI2CSend(IN  VOID *Data);
_LONG_CALL_ROM_ HAL_Status RtkI2CReceive(IN  VOID *Data);
_LONG_CALL_ROM_ VOID RtkSalI2COpInit(IN  VOID *Data);
_LONG_CALL_ROM_ HAL_Status RtkI2CSendUserAddr(IN  VOID *Data,IN  u8  MtrWr);
_LONG_CALL_ROM_ HAL_Status RtkI2CIoCtrl(IN  VOID *Data);
_LONG_CALL_ROM_ HAL_Status RtkI2CPowerCtrl(IN  VOID *Data);
_LONG_CALL_ HAL_Status RtkI2CInitForPS(IN  VOID *Data);
_LONG_CALL_ HAL_Status RtkI2CDeInitForPS(IN  VOID *Data);
_LONG_CALL_ HAL_Status RtkI2CDisablePS(IN  VOID *Data);
_LONG_CALL_ HAL_Status RtkI2CEnablePS(IN  VOID *Data);
//================= I2C SAL END   ===========================


//================= I2C SAL MANAGEMENT START  =================
// I2C SAL management macros
#define SAL_USER_CB_NUM     (sizeof(SAL_I2C_USER_CB) / sizeof(PSAL_I2C_USERCB_ADPT))

//======================================================
// I2C SAL management data structures
// I2C SAL handle private 
typedef struct _SAL_I2C_HND_PRIV_ {
    VOID            **ppSalI2CHnd;              //Pointer to SAL_I2C_HND pointer
    SAL_I2C_HND     SalI2CHndPriv;              //Private SAL_I2C_HND
}SAL_I2C_HND_PRIV, *PSAL_I2C_HND_PRIV;

//I2C SAL management adapter
typedef struct _SAL_I2C_MNGT_ADPT_ {
    PSAL_I2C_HND_PRIV       pSalHndPriv;                //Pointer to SAL_I2C_HND
    PHAL_I2C_INIT_DAT       pHalInitDat;                //Pointer to HAL I2C initial data( HAL_I2C_INIT_DAT )
    PHAL_I2C_OP             pHalOp;                     //Pointer to HAL I2C operation( HAL_I2C_OP )
    VOID                    (*pHalOpInit)(VOID*);       //Pointer to HAL I2C initialize function
    PIRQ_HANDLE             pIrqHnd;                    //Pointer to IRQ handler in SAL layer( IRQ_HANDLE )
    PSAL_I2C_USER_CB        pUserCB;                    //Pointer to SAL user callbacks (SAL_I2C_USER_CB )
    volatile u32            MstRDCmdCnt;                //Used for Master Read command count
    volatile u32            InnerTimeOut;               //Used for SAL internal timeout count
    VOID                    (*pSalIrqFunc)(VOID*);      //Used for SAL I2C interrupt function
    
    PSAL_I2C_DMA_USER_DEF   pDMAConf;                   //Pointer to I2C User Define DMA config
    PHAL_GDMA_ADAPTER       pHalTxGdmaAdp;              //Pointer to HAL_GDMA_ADAPTER
    PHAL_GDMA_ADAPTER       pHalRxGdmaAdp;              //Pointer to HAL_GDMA_ADAPTER
    PHAL_GDMA_OP            pHalGdmaOp;                 //Pointer to HAL_GDMA_OP
    VOID                    (*pHalGdmaOpInit)(VOID*);   //Pointer to HAL I2C initialize function
    PIRQ_HANDLE             pIrqTxGdmaHnd;              //Pointer to IRQ handler for Tx GDMA
    PIRQ_HANDLE             pIrqRxGdmaHnd;              //Pointer to IRQ handler for Rx GDMA
    VOID                    (*pSalDMATxIrqFunc)(VOID*); //Used for SAL I2C interrupt function
    VOID                    (*pSalDMARxIrqFunc)(VOID*); //Used for SAL I2C interrupt function
    u32                     RSVD;                       //Reserved
}SAL_I2C_MNGT_ADPT, *PSAL_I2C_MNGT_ADPT;

//======================================================
//SAL I2C management function prototype
PSAL_I2C_MNGT_ADPT RtkI2CGetMngtAdpt(IN u8 I2CIdx);
HAL_Status RtkI2CFreeMngtAdpt(IN  PSAL_I2C_MNGT_ADPT  pSalI2CMngtAdpt);
PSAL_I2C_HND RtkI2CGetSalHnd(IN  u8  I2CIdx);
HAL_Status RtkI2CFreeSalHnd(IN  PSAL_I2C_HND    pSalI2CHND);
u32 RtkSalI2CSts(IN  VOID *Data);

extern _LONG_CALL_ VOID I2CISRHandle(IN  VOID *Data);
extern _LONG_CALL_ VOID I2CTXGDMAISRHandle(IN  VOID *Data);
extern _LONG_CALL_ VOID I2CRXGDMAISRHandle(IN  VOID *Data);
extern HAL_Status I2CIsTimeout (IN  u32 StartCount, IN  u32 TimeoutCnt);
extern HAL_TIMER_OP        HalTimerOp;
//======================================================
// Function Prototypes
_LONG_CALL_ VOID HalI2COpInit(IN VOID *Data);
//================= I2C SAL MANAGEMENT END   ==================

//================= Rtl8195a I2C V02 function prototype  ============
_LONG_CALL_ VOID HalI2COpInitV02(IN  VOID *Data);
_LONG_CALL_ VOID I2CISRHandleV02(IN  VOID *Data);
_LONG_CALL_ HAL_Status RtkI2CSendV02(IN  VOID *Data);
_LONG_CALL_ HAL_Status RtkI2CReceiveV02(IN  VOID *Data);
_LONG_CALL_ VOID RtkSalI2COpInitV02(IN  VOID *Data);
//================= Rtl8195a I2C V02 function prototype  END==========

//======================================================
//SAL I2C patch function prototype
HAL_Status RtkI2CSend_Patch(IN  VOID *Data);
HAL_Status RtkI2CReceive_Patch(IN  VOID *Data);
VOID HalI2COpInit_Patch(IN VOID *Data);
VOID I2CISRHandle_Patch(IN  VOID *Data);

#ifndef CONFIG_RELEASE_BUILD_LIBRARIES
#define RtkI2CSend RtkI2CSend_Patch
#define RtkI2CReceive RtkI2CReceive_Patch
#endif
HAL_Status RtkI2CSend_Patch(IN  VOID *Data);
HAL_Status RtkI2CReceive_Patch(IN  VOID *Data);
//================= I2C SAL END   ===========================

#endif                  //#ifndef _HAL_I2C_H_
