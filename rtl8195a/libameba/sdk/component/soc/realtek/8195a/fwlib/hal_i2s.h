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

#define I2S_MAX_ID                  1   // valid I2S index 0 ~ I2S_MAX_ID

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
#if 0
typedef enum _I2S0_PINMUX_ {
    I2S0_TO_S0      =   0x0,
    I2S0_TO_S1      =   0x1,
    I2S0_TO_S2      =   0x2,
}I2S0_PINMUX, *PI2S0_PINMUX;

typedef enum _I2S1_PINMUX_ {
    I2S1_TO_S0      =   0x0,
    I2S1_TO_S1      =   0x1,
}I2S1_PINMUX, *PI2S1_PINMUX;
#endif

typedef enum _I2S_PINMUX_ {
    I2S_S0      =   0,
    I2S_S1      =   1,
    I2S_S2      =   2,
    I2S_S3      =   3
}I2S_PINMUX, *PI2S_PINMUX;


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
	I2S_MASTER_MODE =   0x0,
    I2S_SLAVE_MODE  =   0x1
}I2S_DEV_MODE, *PI2S_DEV_MODE;

/* I2S Word Length */
typedef enum _I2S_WORD_LEN_ {
	I2S_WL_16 = 0x0,
    I2S_WL_24 = 0x1,    
}I2S_WORD_LEN, *PI2S_WORD_LEN;

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
    I2S_SR_8KHZ     =   0x00,	// /12
    I2S_SR_16KHZ    =   0x01,	// /6
    I2S_SR_24KHZ    =   0x02,	// /4
	I2S_SR_32KHZ	= 	0x03,	// /3
    I2S_SR_48KHZ    =   0x05,	// /2
	I2S_SR_96KHZ	=	0x06,	// x1, base 96kHz
	I2S_SR_7p35KHZ	= 	0x10,
	I2S_SR_11p02KHZ	= 	0x11,
	I2S_SR_22p05KHZ	= 	0x12,
	I2S_SR_29p4KHZ	= 	0x13,
	I2S_SR_44p1KHZ	= 	0x15,	
	I2S_SR_88p2KHZ	= 	0x16	// x1, base 88200Hz
}I2S_SAMPLE_RATE, *PI2S_SAMPLE_RATE;

/* I2S TX interrupt mask/status */
typedef enum _I2S_TX_IMR_ {
    I2S_TX_INT_PAGE0_OK = (1<<0),
    I2S_TX_INT_PAGE1_OK = (1<<1),
    I2S_TX_INT_PAGE2_OK = (1<<2),
    I2S_TX_INT_PAGE3_OK = (1<<3),
    I2S_TX_INT_FULL     = (1<<4),
    I2S_TX_INT_EMPTY    = (1<<5)    
} I2S_TX_IMR, *PI2S_TX_IMR;

/* I2S RX interrupt mask/status */
typedef enum _I2S_RX_IMR_ {
    I2S_RX_INT_PAGE0_OK = (1<<0),
    I2S_RX_INT_PAGE1_OK = (1<<1),
    I2S_RX_INT_PAGE2_OK = (1<<2),
    I2S_RX_INT_PAGE3_OK = (1<<3),
    I2S_RX_INT_EMPTY    = (1<<4),
    I2S_RX_INT_FULL     = (1<<5)    
} I2S_RX_IMR, *PI2S_RX_IMR;

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

typedef struct _I2S_USER_CB_{
    VOID (*TxCCB)(uint32_t id, char *pbuf);
    u32 TxCBId;
    VOID (*RxCCB)(uint32_t id, char *pbuf);
    u32 RxCBId;
}I2S_USER_CB,*PI2S_USER_CB;

/* Software API Level I2S Handler */
typedef struct _HAL_I2S_ADAPTER_{
    u8                      DevNum;             //I2S device number
    u8                      PinMux;             //I2S pin mux seletion
    u8                      RSVD0;              //Reserved
    volatile u8             DevSts;             //I2S device status
    
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
    I2S_USER_CB             UserCB;            //Pointer to I2S User Callback
    IRQ_HANDLE              IrqHandle;          // Irq Handler

    u32* TxPageList[4];       // The Tx DAM buffer: pointer of each page
    u32* RxPageList[4];       // The Tx DAM buffer: pointer of each page
}HAL_I2S_ADAPTER, *PHAL_I2S_ADAPTER;

typedef struct _HAL_I2S_DEF_SETTING_{
    u8 I2SMaster;           // Master or Slave mode
    u8 DevSts;             //I2S device status
    u8 I2SChNum;           //I2S Channel number mono or stereo
    u8 I2SPageNum;         //I2S Page number 2~4
    u8  I2STRxAct;          //I2S tx rx act, tx only or rx only or tx+rx
    u8  I2SWordLen;         //I2S Word length 16bit or 24bit
    u16 I2SPageSize;        //I2S Page size 1~4096 word
                                                
    u16 I2SRate;            //I2S sample rate 8k ~ 96khz
    
    u32 I2STxIntrMSK;   /*I2S Tx Interrupt Mask*/
    u32 I2SRxIntrMSK;   /*I2S Rx Interrupt Mask*/
}HAL_I2S_DEF_SETTING, *PHAL_I2S_DEF_SETTING;



/**********************************************************************/
HAL_Status
RtkI2SLoadDefault(IN  VOID *Adapter, IN  VOID *Setting);

HAL_Status
RtkI2SInit(IN  VOID *Data);

HAL_Status
RtkI2SDeInit(IN  VOID *Data);

HAL_Status
RtkI2SEnable(IN  VOID *Data);

HAL_Status
RtkI2SDisable(IN  VOID *Data);

extern HAL_Status 
HalI2SInit( IN VOID *Data);

extern VOID 
HalI2SDeInit( IN VOID *Data);

extern HAL_Status 
HalI2SDisable( IN VOID *Data);

extern HAL_Status 
HalI2SEnable( IN VOID *Data);




/**********************************************************************/


VOID I2S0ISRHandle(VOID *Data);
VOID I2S1ISRHandle(VOID *Data);


/**********************************************************************/

VOID HalI2SOpInit(
    IN  VOID *Data
);


#endif

