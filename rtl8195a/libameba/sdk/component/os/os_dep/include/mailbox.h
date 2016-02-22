/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef __MAILBOX_H_
#define __MAILBOX_H_

#include "hal_api.h"
#include "osdep_api.h"
#include "hal_util.h"
#ifdef CONFIG_FREERTOS
#include "queue.h"
#endif

#define MBOX_WAIT_NO_TIMEOUT        0xffffffff  // waiting for send/receive message with no timeout
#define MBOX_WAIT_NONE              0           // No wait for send/receive message

typedef enum _MAILBOX_ID_ {
	MBOX_ID_WLAN =	    0,
	MBOX_ID_UART =	    1,
	MBOX_ID_I2C =	    2,
	MBOX_ID_I2S =	    3,
	MBOX_ID_SPI =	    4,
	MBOX_ID_SDIO =	    5,
    MBOX_ID_SDIO_MP =   6,

	MBOX_ID_MAX =	0xff	
} MAILBOX_ID;

#if defined(CONFIG_SDIO_DEVICE_EN) && defined(CONFIG_SDIO_DEVICE_NORMAL)
typedef enum _MSG_TYPE_SDIO {
	MSG_SDIO_RX_PKT=1,		// request to send a SDIO RX packet to the host side
	MSG_SDIO_C2H=2,			// request to send a C2H message
	MSG_SDIO_RPWM=3,		// request to set the RPWM
    MSG_SDIO_MP_LOOP_TXPKT=4,      // request to loopback this TX packet

	MSG_SDIO_MAX=0xff
} MSG_TYPE_SDIO;
#endif  // end  of "#ifdef CONFIG_SDIO_DEVICE_EN"

/* the data structure of a MailBox to deliver message blocks */
typedef struct _RTL_MAILBOX_ {
    void    *mbox_hdl;      // the mailbox handle which return from OS create queue API
	_Sema	*pWakeSema;     // the semaphore to wakeup the message receiving task
	_LIST	mbox_list;      // the link list to chain all created mailbox
	u8		mbox_id;        /* the ID of this Mailbox, this ID is 
                                used to locate the MBox for send/get message */
} RTL_MAILBOX, *PRTL_MAILBOX;

/* the data structure of a message block */
typedef struct _RTL_MSG_BLK {
    u8      MsgType;		// the message type
    u8      Reserved;       // reserved
    u16     DateLen;        // the vaild data length of the pBuf
    u32     Para;           // the optional parameters associated with this message type
    u8      *pBuf;          // point to a data buffer associated with this message type
} MSG_BLK, *PMSG_BLK;

/* the data structure for system level message block management */
typedef struct _RTL_MBOX_ROOT_ {
	_LIST 		mbox_list;			// the link list of all created mailbox
	_Mutex		Mutex;				// the Mutex to protect the mailbox create/delete procedure
	u8          isInitialed;        // is this Mailbox link-list initialed
} RTL_MBOX_ROOT, *PRTL_MBOX_ROOT;

// Export Funcction API
extern PRTL_MAILBOX RtlMailboxCreate(
    IN u8 MboxID, 
    IN u32 MboxSize, 
    IN _Sema *pWakeSema
);

extern VOID RtlMailboxDel(
    IN PRTL_MAILBOX MboxHdl
);

extern u8 RtlMailboxSendToBack(
    IN u8 MboxID, 
    IN MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

extern u8 RtlMailboxSendToFront(
    IN u8 MboxID, 
    IN MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

extern u8 RtlMailboxReceive(
    IN u8 MboxID, 
    OUT MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

extern u8 RtlMailboxPeek(
    IN u8 MboxID, 
    OUT MSG_BLK *pMsg, 
    IN u32 MSToWait, 
    IN u8 IsFromISR
);

extern u32 RtlMailboxMsgWaiting(
    IN u8 MboxID, 
    IN u8 IsFromISR
);


#endif	// #ifndef __MAILBOX_H_

