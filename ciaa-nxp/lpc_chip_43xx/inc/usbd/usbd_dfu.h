/***********************************************************************
* $Id:: mw_usbd_dfu.h 331 2012-08-09 18:54:34Z usb10131                       $
*
* Project: USB device ROM Stack
*
* Description:
*     Device Firmware Upgrade (DFU) module.
*
***********************************************************************
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#ifndef __MW_USBD_DFU_H__
#define __MW_USBD_DFU_H__

#include "usbd.h"

/** \file
 *  \brief Device Firmware Upgrade (DFU) class descriptors.
 *
 *  Definition of DFU class descriptors and their bit defines.
 *
 */

/**
 * If USB device is only DFU capable, DFU Interface number is always 0.
 * if USB device is (DFU + Other Class (Audio/Mass Storage/HID), DFU 
 * Interface number should also be 0 in this implementation.
 */ 
#define USB_DFU_IF_NUM	0x0

#define USB_DFU_DESCRIPTOR_TYPE     0x21
#define USB_DFU_DESCRIPTOR_SIZE     9
#define USB_DFU_SUBCLASS            0x01

/* DFU class-specific requests (Section 3, DFU Rev 1.1) */
#define USB_REQ_DFU_DETACH          0x00
#define USB_REQ_DFU_DNLOAD          0x01
#define USB_REQ_DFU_UPLOAD          0x02
#define USB_REQ_DFU_GETSTATUS       0x03
#define USB_REQ_DFU_CLRSTATUS       0x04
#define USB_REQ_DFU_GETSTATE        0x05
#define USB_REQ_DFU_ABORT           0x06

#define DFU_STATUS_OK               0x00
#define DFU_STATUS_errTARGET        0x01
#define DFU_STATUS_errFILE          0x02
#define DFU_STATUS_errWRITE         0x03
#define DFU_STATUS_errERASE         0x04
#define DFU_STATUS_errCHECK_ERASED  0x05
#define DFU_STATUS_errPROG          0x06
#define DFU_STATUS_errVERIFY        0x07
#define DFU_STATUS_errADDRESS       0x08
#define DFU_STATUS_errNOTDONE       0x09
#define DFU_STATUS_errFIRMWARE      0x0a
#define DFU_STATUS_errVENDOR        0x0b
#define DFU_STATUS_errUSBR          0x0c
#define DFU_STATUS_errPOR           0x0d
#define DFU_STATUS_errUNKNOWN       0x0e
#define DFU_STATUS_errSTALLEDPKT    0x0f

enum dfu_state {
  DFU_STATE_appIDLE             = 0,
  DFU_STATE_appDETACH           = 1,
  DFU_STATE_dfuIDLE             = 2,
  DFU_STATE_dfuDNLOAD_SYNC      = 3,
  DFU_STATE_dfuDNBUSY           = 4,
  DFU_STATE_dfuDNLOAD_IDLE      = 5,
  DFU_STATE_dfuMANIFEST_SYNC    = 6,
  DFU_STATE_dfuMANIFEST         = 7,
  DFU_STATE_dfuMANIFEST_WAIT_RST= 8,
  DFU_STATE_dfuUPLOAD_IDLE      = 9,
  DFU_STATE_dfuERROR            = 10
};

#define DFU_EP0_NONE            0
#define DFU_EP0_UNHANDLED       1
#define DFU_EP0_STALL           2
#define DFU_EP0_ZLP             3
#define DFU_EP0_DATA            4

#define USB_DFU_CAN_DOWNLOAD    (1 << 0)
#define USB_DFU_CAN_UPLOAD      (1 << 1)
#define USB_DFU_MANIFEST_TOL    (1 << 2)
#define USB_DFU_WILL_DETACH     (1 << 3)

PRE_PACK struct POST_PACK _USB_DFU_FUNC_DESCRIPTOR {
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bmAttributes;
  uint16_t  wDetachTimeOut;
  uint16_t  wTransferSize;
  uint16_t  bcdDFUVersion;
};
typedef struct _USB_DFU_FUNC_DESCRIPTOR USB_DFU_FUNC_DESCRIPTOR;

PRE_PACK struct POST_PACK _DFU_STATUS {
  uint8_t bStatus;
  uint8_t bwPollTimeout[3];
  uint8_t bState;
  uint8_t iString;
};
typedef struct _DFU_STATUS DFU_STATUS_T;

#define DFU_FUNC_DESC_SIZE    sizeof(USB_DFU_FUNC_DESCRIPTOR)
#define DFU_GET_STATUS_SIZE   0x6 


#endif  /* __MW_USBD_DFU_H__ */
