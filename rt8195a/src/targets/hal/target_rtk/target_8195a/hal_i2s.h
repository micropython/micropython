/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_I2S_H_
#define _HAL_I2S_H_

#include "rtl8195a_i2s.h"

/* User Define Flags */

#define I2S0_USED                   1
#define I2S1_USED                   1

/*
typedef struct _GDMA_CH_LLI_ELE_ {
    u32                   Sarx;
    u32                   Darx;
    u32                   Llpx;
    u32                   CtlxLow;
    u32                   CtlxUp;
    u32                   Temp;
}GDMA_CH_LLI_ELE, *PGDMA_CH_LLI_ELE;
#if 1
#if 0
typedef struct _GDMA_CH_LLI_ {
    PGDMA_CH_LLI_ELE      pLliEle;
    PGDMA_CH_LLI          pNextLli;
}GDMA_CH_LLI, *PGDMA_CH_LLI;

typedef struct _BLOCK_SIZE_LIST_ {
    u32                  BlockSize;
    PBLOCK_SIZE_LIST     pNextBlockSiz;
}BLOCK_SIZE_LIST, *PBLOCK_SIZE_LIST;
#else
struct GDMA_CH_LLI {
    PGDMA_CH_LLI_ELE                pLliEle;
    struct GDMA_CH_LLI             *pNextLli;
};

struct BLOCK_SIZE_LIST {
    u32                             BlockSize;
    struct BLOCK_SIZE_LIST          *pNextBlockSiz;
};

#endif

#endif
typedef struct _HAL_I2S_ADAPTER_ {
    u32                   ChSar;
    u32                   ChDar;
    GDMA_CHANNEL_NUM      ChEn;
    GDMA_CTL_REG          GdmaCtl;
    GDMA_CFG_REG          GdmaCfg;
    u32                   PacketLen;
    u32                   BlockLen;
    u32                   MuliBlockCunt;
    u32                   MaxMuliBlock;
    struct GDMA_CH_LLI          *pLlix;
    struct BLOCK_SIZE_LIST      *pBlockSizeList;

    PGDMA_CH_LLI_ELE            pLli;
    u32                         NextPlli;
    u8                    TestItem;
    u8                          ChNum;
    u8                          GdmaIndex;
    u8                          IsrCtrl:1;
    u8                          GdmaOnOff:1;
    u8                          Llpctrl:1;
    u8                          Lli0:1;
    u8                          Rsvd4to7:4;
    u8                          GdmaIsrType;
}HAL_I2S_ADAPTER, *PHAL_I2S_ADAPTER;

*/

/**********************************************************************/
/* I2S HAL initial data structure */
typedef struct _HAL_I2S_INIT_DAT_ {
    u8                  I2SIdx;         /*I2S index used*/
    u8                  I2SEn;          /*I2S module enable tx/rx/tx+rx*/
    u8                  I2SMaster;      /*I2S Master or Slave mode*/
    u8                  I2SWordLen;     /*I2S Word length 16 or 24bits*/

    u8                  I2SChNum;       /*I2S Channel number mono or stereo*/
    u8                  I2SPageNum;     /*I2S Page Number 2~4*/
    u16                 I2SPageSize;    /*I2S page Size 1~4096 word*/

    u8                  *I2STxData;     /*I2S Tx data pointer*/

    u8                  *I2SRxData;     /*I2S Rx data pointer*/

    u32                 I2STxIntrMSK;   /*I2S Tx Interrupt Mask*/
    u32                 I2STxIntrClr;   /*I2S Tx Interrupt register to clear */

    u32                 I2SRxIntrMSK;   /*I2S Rx Interrupt Mask*/
    u32                 I2SRxIntrClr;   /*I2S Rx Interrupt register to clear*/

	u16                 I2STxIdx;       /*I2S TX page index */
	u16                 I2SRxIdx;       /*I2S RX page index */

	u16                 I2SHWTxIdx;       /*I2S HW TX page index */
	u16                 I2SHWRxIdx;       /*I2S HW RX page index */

	
    u16                 I2SRate;        /*I2S sample rate*/
    u8                  I2STRxAct;      /*I2S tx rx act*/	
}HAL_I2S_INIT_DAT, *PHAL_I2S_INIT_DAT;

/**********************************************************************/
/* I2S Data Structures */
/* I2S Module Selection */
typedef enum _I2S_MODULE_SEL_ {
        I2S0_SEL    =   0x0,
        I2S1_SEL    =   0x1,
}I2S_MODULE_SEL,*PI2S_MODULE_SEL;
/*
typedef struct _HAL_I2S_ADAPTER_ {
    u32                       Enable:1;
    I2S_CTL_REG               I2sCtl;
    I2S_SETTING_REG           I2sSetting;
    u32                       abc;
    u8                        I2sIndex;
}HAL_I2S_ADAPTER, *PHAL_I2S_ADAPTER;
*/
/* I2S HAL Operations */
typedef struct _HAL_I2S_OP_ {
    RTK_STATUS  (*HalI2SInit)       (VOID *Data);
    RTK_STATUS  (*HalI2SDeInit)     (VOID *Data);
    RTK_STATUS  (*HalI2STx)       (VOID *Data, u8 *pBuff);
    RTK_STATUS  (*HalI2SRx)       (VOID *Data, u8 *pBuff);
    RTK_STATUS  (*HalI2SEnable)     (VOID *Data);
    RTK_STATUS  (*HalI2SIntrCtrl)   (VOID *Data);
    u32         (*HalI2SReadReg)    (VOID *Data, u8 I2SReg);
    RTK_STATUS  (*HalI2SSetRate)    (VOID *Data);
    RTK_STATUS  (*HalI2SSetWordLen) (VOID *Data);
    RTK_STATUS  (*HalI2SSetChNum)   (VOID *Data);
    RTK_STATUS  (*HalI2SSetPageNum) (VOID *Data);
    RTK_STATUS  (*HalI2SSetPageSize) (VOID *Data);

    RTK_STATUS  (*HalI2SClrIntr)    (VOID *Data);
    RTK_STATUS  (*HalI2SClrAllIntr) (VOID *Data);
    RTK_STATUS  (*HalI2SDMACtrl)    (VOID *Data); 
/*
    VOID (*HalI2sOnOff)(VOID *Data);
    BOOL (*HalI2sInit)(VOID *Data);
    BOOL (*HalI2sSetting)(VOID *Data);
    BOOL (*HalI2sEn)(VOID *Data);
    BOOL (*HalI2sIsrEnAndDis) (VOID *Data);
    BOOL (*HalI2sDumpReg)(VOID *Data);
    BOOL (*HalI2s)(VOID *Data);
*/
}HAL_I2S_OP, *PHAL_I2S_OP;


/**********************************************************************/

/* I2S Pinmux Selection */
typedef enum _I2S0_PINMUX_ {
    I2S0_TO_S0      =   0x0,
    I2S0_TO_S1      =   0x1,
    I2S0_TO_S2      =   0x2,
}I2S0_PINMUX, *PI2S0_PINMUX;

typedef enum _I2S1_PINMUX_ {
    I2S1_TO_S0      =   0x0,
    I2S1_TO_S1      =   0x1,
}I2S1_PINMUX, *PI2S1_PINMUX;


/* I2S Module Status */
typedef enum _I2S_MODULE_STATUS_ {
    I2S_DISABLE     =   0x0,
    I2S_ENABLE      =   0x1,
}I2S_MODULE_STATUS, *PI2S_MODULE_STATUS;


/* I2S Device Status */
typedef enum _I2S_Device_STATUS_ {
    I2S_STS_UNINITIAL   =   0x00,
    I2S_STS_INITIALIZED =   0x01,
    I2S_STS_IDLE        =   0x02,
    
    I2S_STS_TX_READY    =   0x03,    
    I2S_STS_TX_ING      =   0x04,
    
    I2S_STS_RX_READY    =   0x05,
    I2S_STS_RX_ING      =   0x06,

    I2S_STS_TRX_READY   =   0x07,    
    I2S_STS_TRX_ING     =   0x08,
    
    I2S_STS_ERROR       =   0x09,
}I2S_Device_STATUS, *PI2S_Device_STATUS;


/* I2S Feature Status */
typedef enum _I2S_FEATURE_STATUS_{
    I2S_FEATURE_DISABLED    =   0,
    I2S_FEATURE_ENABLED     =   1,
}I2S_FEATURE_STATUS,*PI2S_FEATURE_STATUS;

/* I2S Device Mode */
typedef enum _I2S_DEV_MODE_ {
    I2S_SLAVE_MODE  =   0x0,
    I2S_MASTER_MODE =   0x1
}I2S_DEV_MODE, *PI2S_DEV_MODE;

/* I2S Bus Transmit/Receive */
typedef enum _I2S_DIRECTION_ {
    I2S_ONLY_RX     =   0x0,
    I2S_ONLY_TX     =   0x1,
    I2S_TXRX        =   0x2
}I2S_DIRECTION, *PI2S_DIRECTION;

/* I2S Channel number */
typedef enum _I2S_CH_NUM_ {
    I2S_CH_STEREO   =   0x0,
    I2S_CH_RSVD     =   0x1,
    I2S_CH_MONO     =   0x2
}I2S_CH_NUM, *PI2S_CH_NUM;

/* I2S Page number */
typedef enum _I2S_PAGE_NUM_ {
    I2S_1PAGE       =   0x0,
    I2S_2PAGE       =   0x1,
    I2S_3PAGE       =   0x2,
    I2S_4PAGE       =   0x3
}I2S_PAGE_NUM, *PI2S_PAGE_NUM;

/* I2S Sample rate*/
typedef enum _I2S_SAMPLE_RATE_ {
    I2S_SR_8KHZ     =   0x00,
    I2S_SR_16KHZ    =   0x01,
    I2S_SR_24KHZ    =   0x02,
    I2S_SR_48KHZ    =   0x05
}I2S_SAMPLE_RATE, *PI2S_SAMPLE_RATE;

/* I2S User Callbacks */
typedef struct _SAL_I2S_USER_CB_{
    VOID (*TXCB)        (VOID *Data);
    VOID (*TXCCB)       (VOID *Data);
    VOID (*RXCB)        (VOID *Data);
    VOID (*RXCCB)       (VOID *Data);
    VOID (*RDREQCB)     (VOID *Data);
    VOID (*ERRCB)       (VOID *Data);
    VOID (*GENCALLCB)   (VOID *Data);
}SAL_I2S_USER_CB,*PSAL_I2S_USER_CB;

/* I2S Transmit Buffer */
typedef struct _SAL_I2S_TRANSFER_BUF_{
    u16     DataLen;
    u16     TargetAddr;
    u32     RegAddr;
    u32     RSVD;
    u8      *pDataBuf;
}SAL_I2S_TRANSFER_BUF,*PSAL_I2S_TRANSFER_BUF;

/* RTK I2S OP */
typedef struct _RTK_I2S_OP_ {
    RTK_STATUS (*Init)      (VOID *Data);
    RTK_STATUS (*DeInit)    (VOID *Data);
    RTK_STATUS (*Send)      (VOID *Data);
    RTK_STATUS (*Receive)   (VOID *Data);
    RTK_STATUS (*IoCtrl)    (VOID *Data);
    RTK_STATUS (*PowerCtrl) (VOID *Data);    
}RTK_I2S_OP, *PRTK_I2S_OP;


/* Software API Level I2S Handler */
typedef struct _SAL_I2S_HND_{
    u8                      DevNum;             //I2S device number
    u8                      PinMux;             //I2S pin mux seletion
    u8                      RSVD0;              //Reserved
    volatile u8             DevSts;             //I2S device status
    
    u8                      I2SChNum;           //I2S Channel number mono or stereo
    u8                      I2SPageNum;         //I2S Page number 2~4
    u16                     I2SPageSize;        //I2S Page size 1~4096 word
                                                
    u16                     I2SRate;            //I2S sample rate 8k ~ 96khz
    u8                      I2STRxAct;          //I2S tx rx act, tx only or rx only or tx+rx
    u8                      I2SWordLen;         //I2S Word length 16bit or 24bit
    u8                      RSVD1;              //Reserved
    
    u32                     RSVD2;              //Reserved
    u32                     I2SExd;             //I2S extended options:
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
                                                //bit 31~bit 8: Reserved
    u32                     ErrType;         //
    u32                     TimeOut;            //I2S IO Timeout count
                                                                            
    PHAL_I2S_INIT_DAT       pInitDat;           //Pointer to I2S initial data struct
    u8                      *pTXBuf;            //Pointer to I2S TX buffer
    u8                      *pRXBuf;            //Pointer to I2S RX buffer
    PSAL_I2S_USER_CB        pUserCB;            //Pointer to I2S User Callback
}SAL_I2S_HND, *PSAL_I2S_HND;


/**********************************************************************/
// Global Parameters
#if I2S0_USED
extern SAL_I2S_HND         SalI2S0Hnd;
#endif

#if I2S1_USED
extern SAL_I2S_HND         SalI2S1Hnd;
#endif

/**********************************************************************/
// Function Prototypes
static inline RTK_STATUS
RtkI2SIdxChk(
    IN  u8  I2SIdx
)
{
#if !I2S0_USED
    if (I2SIdx == I2S0_SEL)
        return _EXIT_FAILURE;
#endif

#if !I2S1_USED
    if (I2SIdx == I2S1_SEL)
        return _EXIT_FAILURE;
#endif

    return _EXIT_SUCCESS;
}


/**********************************************************************/
PSAL_I2S_HND
RtkI2SGetSalHnd(IN  u8  I2SIdx);

RTK_STATUS
RtkI2SLoadDefault(IN  VOID *Data);

RTK_STATUS
RtkI2SInit(IN  VOID *Data);

RTK_STATUS
RtkI2SDeInit(IN  VOID *Data);

RTK_STATUS
RtkI2SSend(IN  VOID *Data);

RTK_STATUS
RtkI2SReceive(IN  VOID *Data);

VOID SalI2SOpInit(IN  VOID *Data);

/**********************************************************************/


VOID I2S0ISRHandle(VOID *Data);
VOID I2S1ISRHandle(VOID *Data);


/**********************************************************************/

VOID HalI2SOpInit(
    IN  VOID *Data
);


#endif

