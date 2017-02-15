/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_SDIO_H_
#define _HAL_SDIO_H_

#include "rtl8195a_sdio.h"

#if !SDIO_BOOT_DRIVER
#include "mailbox.h"
#endif
#define PURE_SDIO_INIC           0  // is a pure SDIO iNIC device or a SDIO iNIC + peripheral device

#if SDIO_BOOT_DRIVER
typedef struct _HAL_SDIO_ADAPTER_ {
	u8				*pTXBDAddr;			/* The TX_BD start address */
	PSDIO_TX_BD		pTXBDAddrAligned;	/* The TX_BD start address, it must be 4-bytes aligned */
	PSDIO_TX_BD_HANDLE	pTXBDHdl;		/* point to the allocated memory for TX_BD Handle array */
	u16				TXBDWPtr;		    /* The SDIO TX(Host->Device) BD local write index, different with HW maintained write Index. */
	u16				TXBDRPtr;		    /* The SDIO TX(Host->Device) BD read index */
	u16				TXBDRPtrReg;		/* The SDIO TX(Host->Device) BD read index has been write to HW register */
    u16             reserve1;
    
	u8				*pRXBDAddr;			/* The RX_BD start address */
	PSDIO_RX_BD		pRXBDAddrAligned;	/* The RX_BD start address, it must be 8-bytes aligned */
	PSDIO_RX_BD_HANDLE	pRXBDHdl;		/* point to the allocated memory for RX_BD Handle array */
	u16				RXBDWPtr;		    /* The SDIO RX(Device->Host) BD write index */
	u16				RXBDRPtr;		    /* The SDIO RX(Device->Host) BD local read index, different with HW maintained Read Index. */
	u16				IntMask;			/* The Interrupt Mask */
	u16				IntStatus;			/* The Interrupt Status */
	u32				Events;				/* The Event to the SDIO Task */

	u32				EventSema;			/* Semaphore for SDIO events, use to wakeup the SDIO task */	
    u8              CCPWM;              /* the value write to register CCPWM, which will sync to Host HCPWM */
    u8              reserve2;
    u16             CCPWM2;             /* the value write to register CCPWM2, which will sync to Host HCPWM2 */
    
	s8              (*Tx_Callback)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize);	/* to hook the WLan driver TX callback function to handle a Packet TX */
	VOID			*pTxCb_Adapter;		/* a pointer will be used to call the TX Callback function, 
											which is from the TX CallBack function register */
	s8			(*pTxCallback_Backup)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize);	// Use to back up the registered TX Callback function, for MP/Normal mode switch
	VOID			*pTxCb_Adapter_Backup;	// Backup the pTxCb_Adapter, for MP/Normal mode switch
	_LIST			FreeTxPktList;		/* The list to queue free Tx packets handler */
	_LIST			RxPktList;			/* The list to queue RX packets */
	_LIST			FreeRxPktList;		/* The list to queue free Rx packets handler */
	SDIO_TX_PACKET	*pTxPktHandler;		/* to store allocated TX Packet handler memory address */
	SDIO_RX_PACKET	*pRxPktHandler;		/* to store allocated RX Packet handler memory address */
	u32				RxInQCnt;			/* The packet count for Rx In Queue */
	u32				MemAllocCnt;		// Memory allocated count, for debug only
	u32				MAllocFailedCnt;	// MemAlloc Failed count, for debugging
	
//	VOID			*pHalOp;			/* point to HAL operation function table */
} HAL_SDIO_ADAPTER, *PHAL_SDIO_ADAPTER;

extern BOOL SDIO_Device_Init_Rom(
	IN PHAL_SDIO_ADAPTER pSDIODev
);
extern VOID SDIO_Device_DeInit_Rom(
	IN PHAL_SDIO_ADAPTER pSDIODev
);
extern VOID SDIO_Send_C2H_IOMsg_Rom(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 *C2HMsg
);
extern u8 SDIO_Send_C2H_PktMsg_Rom(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u8 *C2HMsg, 
	IN u16 MsgLen
);
extern VOID SDIO_Register_Tx_Callback_Rom(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN s8 (*Tx_Callback)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize),
	IN VOID *pAdapter	
);
extern s8 SDIO_Rx_Callback_Rom(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN VOID *pData,
	IN u16 Offset,
	IN u16 Length,
	IN u8 CmdType
);

#else   // else of "#if SDIO_BOOT_DRIVER"
typedef struct _HAL_SDIO_ADAPTER_ {
//	u8				*pTxBuff;			/* point to the SDIO TX Buffer */
//	u8				*pTxBuffAligned;	/* point to the SDIO TX Buffer with 4-bytes aligned */
//	u32				TXFifoRPtr;		    /* The SDIO TX(Host->Device) FIFO buffer read pointer */

	u8				*pTXBDAddr;			/* The TX_BD start address */
	PSDIO_TX_BD		pTXBDAddrAligned;	/* The TX_BD start address, it must be 4-bytes aligned */
	PSDIO_TX_BD_HANDLE	pTXBDHdl;		/* point to the allocated memory for TX_BD Handle array */
	u16				TXBDWPtr;		    /* The SDIO TX(Host->Device) BD local write index, different with HW maintained write Index. */
	u16				TXBDRPtr;		    /* The SDIO TX(Host->Device) BD read index */
	u16				TXBDRPtrReg;		/* The SDIO TX(Host->Device) BD read index has been write to HW register */
    
	u8				*pRXBDAddr;			/* The RX_BD start address */
	PSDIO_RX_BD		pRXBDAddrAligned;	/* The RX_BD start address, it must be 8-bytes aligned */
	PSDIO_RX_BD_HANDLE	pRXBDHdl;		/* point to the allocated memory for RX_BD Handle array */
	u16				RXBDWPtr;		    /* The SDIO RX(Device->Host) BD write index */
	u16				RXBDRPtr;		    /* The SDIO RX(Device->Host) BD local read index, different with HW maintained Read Index. */
	u16				IntMask;			/* The Interrupt Mask */
	u16				IntStatus;			/* The Interrupt Status */
	u32				Events;				/* The Event to the SDIO Task */

    u8              CCPWM;              /* the value write to register CCPWM, which will sync to Host HCPWM */
    u8              reserve1;
    u16             CCPWM2;             /* the value write to register CCPWM2, which will sync to Host HCPWM2 */
    u8              CRPWM;              /* sync from Host HRPWM */
    u8              reserve2;
    u16             CRPWM2;             /* sync from Host HRPWM2 */

#if !TASK_SCHEDULER_DISABLED
	_Sema			TxSema;             /* Semaphore for SDIO TX, use to wakeup the SDIO TX task */	
    _Sema           RxSema;             /* Semaphore for SDIO RX, use to wakeup the SDIO RX task */    
#else
	u32				EventSema;			/* Semaphore for SDIO events, use to wakeup the SDIO task */	
#endif
	s8              (*Tx_Callback)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize);	/* to hook the WLan driver TX callback function to handle a Packet TX */
	VOID			*pTxCb_Adapter;		/* a pointer will be used to call the TX Callback function, 
											which is from the TX CallBack function register */
	s8			(*pTxCallback_Backup)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize);	// Use to back up the registered TX Callback function, for MP/Normal mode switch
	VOID			*pTxCb_Adapter_Backup;	// Backup the pTxCb_Adapter, for MP/Normal mode switch
#if (CONFIG_INIC_EN == 0)
	_LIST			FreeTxPktList;		/* The list to queue free Tx packets handler */
	SDIO_TX_PACKET	*pTxPktHandler;		/* to store allocated TX Packet handler memory address */
#endif
	_LIST			RxPktList;			/* The list to queue RX packets */
	_LIST			FreeRxPktList;		/* The list to queue free Rx packets handler */
//	_LIST			RecyclePktList;		/* The list to queue packets handler to be recycled */
	SDIO_RX_PACKET	*pRxPktHandler;		/* to store allocated RX Packet handler memory address */
	_Mutex			RxMutex;			/* The Mutex to protect RxPktList */
	u32				RxInQCnt;			/* The packet count for Rx In Queue */
#if SDIO_DEBUG
	_Mutex			StatisticMutex;		/* The Mutex to protect Statistic data */
	u32				MemAllocCnt;		// Memory allocated count, for debug only
	u32				MAllocFailedCnt;	// MemAlloc Failed count, for debugging
#endif	
	VOID			*pHalOp;			/* point to HAL operation function table */
	RTL_MAILBOX		*pMBox;				/* the Mail box for other driver module can send message to SDIO driver */

#ifdef PLATFORM_FREERTOS
	xTaskHandle		xSDIOTxTaskHandle;	/* The handle of the SDIO Task for TX, can be used to delte the task */
    xTaskHandle     xSDIORxTaskHandle;  /* The handle of the SDIO Task speical for RX, can be used to delte the task */
#endif
    u8              RxFifoBusy;         /* is the RX BD fetch hardware busy */

#if SDIO_MP_MODE
#if !TASK_SCHEDULER_DISABLED
	u32				MP_Events;				/* The Event to the SDIO Task */
	_Sema			MP_EventSema;		/* Semaphore for SDIO events, use to wakeup the SDIO task */	
    RTL_MAILBOX     *pMP_MBox;  /* the Mail box for communication with other driver module */
#ifdef PLATFORM_FREERTOS
    xTaskHandle     MP_TaskHandle;      /* The handle of the MP loopback Task, can be used to delte the task */
#endif  // end of "#ifdef PLATFORM_FREERTOS"
#endif  // end of "#if !TASK_SCHEDULER_DISABLED"
	// for MP mode
    RTL_TIMER       *pPeriodTimer;      /* a timer to calculate throughput periodically */
	u8				MP_ModeEn;			/* is in MP mode */
	u8				MP_LoopBackEn;		/* is loop-back enabled */
	u8				MP_ContinueTx;		/* is continue TX test enabled */
	u8				MP_ContinueRx;		/* is continue RX test enabled */
	u8				MP_ContinueRxMode;  /* continue RX test mode: static RX Buf, Dyna-Allocate RX Buf, Pre-Allocate RX Buf */
	u8				MP_CRxInfinite;		/* is non-stop SDIO RX, no packet count limit */
    u16				MP_CRxSize;		    /* SDIO RX test packet size */
    u8              *pMP_CRxBuf;        // the buffer for continye RX test
    u32             MP_CRxPktCnt;       /* SDIO RX test packet count */
    u32             MP_CRxPktPendingCnt;       /* SDIO RX test packet pening count */
	u32				MP_TxPktCnt;		/* SDIO TX packet count */
	u32				MP_RxPktCnt;		/* SDIO RX packet count */
	u32				MP_TxByteCnt;		/* SDIO TX Byte count */
	u32				MP_RxByteCnt;		/* SDIO RX Byte count */
	u32				MP_TxDropCnt;		/* SDIO TX Drop packet count */
	u32				MP_RxDropCnt;		/* SDIO RX Drop packet count */

	u32				MP_TxPktCntInPeriod;    /* SDIO TX packet count in a period */
	u32				MP_RxPktCntInPeriod;	/* SDIO RX packet count in a period */
	u32				MP_TxByteCntInPeriod;	/* SDIO TX Byte count in a period */
	u32				MP_RxByteCntInPeriod;	/* SDIO RX Byte count in a period */

	u32				MP_TxAvgTPWin[SDIO_AVG_TP_WIN_SIZE];        /* a window of SDIO TX byte count history, for average throughput calculation */
	u32				MP_RxAvgTPWin[SDIO_AVG_TP_WIN_SIZE];        /* a window of SDIO RX byte count history, for average throughput calculation */
	u32				MP_TxAvgTPWinSum;        /* The sum of all byte-count in the window */
	u32				MP_RxAvgTPWinSum;        /* The sum of all byte-count in the window */
    u8              OldestTxAvgWinIdx;      /* the index of the oldest TX byte count log */
    u8              TxAvgWinCnt;            /* the number of log in the Window */
    u8              OldestRxAvgWinIdx;      /* the index of the oldest RX byte count log */
    u8              RxAvgWinCnt;            /* the number of log in the Window */

	_LIST			MP_RxPktList;		/* The list to queue RX packets, for MP loopback test */
#endif	// end of '#if SDIO_MP_MODE'
} HAL_SDIO_ADAPTER, *PHAL_SDIO_ADAPTER;
#endif  // end of "#else of "#if SDIO_BOOT_DRIVER""


typedef struct _HAL_SDIO_OP_ {
	BOOL (*HalSdioDevInit)(PHAL_SDIO_ADAPTER pSDIODev);	
	VOID (*HalSdioDevDeInit)(PHAL_SDIO_ADAPTER pSDIODev);
	VOID (*HalSdioSendC2HIOMsg)(PHAL_SDIO_ADAPTER pSDIODev, u32 *C2HMsg);
	u8   (*HalSdioSendC2HPktMsg)(PHAL_SDIO_ADAPTER pSDIODev, u8 *C2HMsg, u16 MsgLen);
	VOID (*HalSdioRegTxCallback)(PHAL_SDIO_ADAPTER pSDIODev,s8 (*CallbackFun)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize), VOID *pAdapter);
	s8   (*HalSdioRxCallback)(PHAL_SDIO_ADAPTER pSDIODev, VOID *pData, u16 Offset, u16 PktSize, u8 CmdType);
#if SDIO_MP_MODE
	VOID (*HalSdioDevMPApp)(PHAL_SDIO_ADAPTER pSDIODev, u16 argc, u8  *argv[]);
#endif
}HAL_SDIO_OP, *PHAL_SDIO_OP;


extern BOOL SDIO_Device_Init(
	IN PHAL_SDIO_ADAPTER pSDIODev
);
extern VOID SDIO_Device_DeInit(
	IN PHAL_SDIO_ADAPTER pSDIODev
);
extern VOID SDIO_Send_C2H_IOMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u32 *C2HMsg
);
extern u8 SDIO_Send_C2H_PktMsg(
	IN PHAL_SDIO_ADAPTER pSDIODev, 
	IN u8 *C2HMsg, 
	IN u16 MsgLen
);
extern VOID SDIO_Register_Tx_Callback(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN s8 (*Tx_Callback)(VOID *pAdapter, u8 *pPkt, u16 Offset, u16 PktSize),
	IN VOID *pAdapter	
);
extern s8 SDIO_Rx_Callback(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN VOID *pData,
	IN u16 Offset,
	IN u16 Length,
	IN u8 CmdType
);
#if SDIO_MP_MODE
extern VOID SDIO_DeviceMPApp(
	IN PHAL_SDIO_ADAPTER pSDIODev,
	IN u16 argc, 
    IN u8  *argv[]
);
#endif

extern PHAL_SDIO_ADAPTER pgSDIODev;
extern VOID HalSdioInit(VOID);
extern VOID HalSdioDeInit(VOID);
#endif	// #ifndef _HAL_SDIO_H_
