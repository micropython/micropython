/*
 * @brief LPC18xx/43xx High-Speed USB driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __USBHS_18XX_43XX_H_
#define __USBHS_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup USBHS_18XX_43XX CHIP: LPC18xx/43xx USBHS Device, Host, & OTG driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief USB High-Speed register block structure
 */
typedef struct {							/*!< USB Structure         */
	__I  uint32_t  RESERVED0[64];
	__I  uint32_t  CAPLENGTH;				/*!< Capability register length */
	__I  uint32_t  HCSPARAMS;				/*!< Host controller structural parameters */
	__I  uint32_t  HCCPARAMS;				/*!< Host controller capability parameters */
	__I  uint32_t  RESERVED1[5];
	__I  uint32_t  DCIVERSION;				/*!< Device interface version number */
	__I  uint32_t  RESERVED2[7];
	union {
		__IO uint32_t  USBCMD_H;			/*!< USB command (host mode) */
		__IO uint32_t  USBCMD_D;			/*!< USB command (device mode) */
	};

	union {
		__IO uint32_t  USBSTS_H;			/*!< USB status (host mode) */
		__IO uint32_t  USBSTS_D;			/*!< USB status (device mode) */
	};

	union {
		__IO uint32_t  USBINTR_H;			/*!< USB interrupt enable (host mode) */
		__IO uint32_t  USBINTR_D;			/*!< USB interrupt enable (device mode) */
	};

	union {
		__IO uint32_t  FRINDEX_H;			/*!< USB frame index (host mode) */
		__I  uint32_t  FRINDEX_D;			/*!< USB frame index (device mode) */
	};

	__I  uint32_t  RESERVED3;
	union {
		__IO uint32_t  PERIODICLISTBASE;	/*!< Frame list base address */
		__IO uint32_t  DEVICEADDR;			/*!< USB device address     */
	};

	union {
		__IO uint32_t  ASYNCLISTADDR;		/*!< Address of endpoint list in memory (host mode) */
		__IO uint32_t  ENDPOINTLISTADDR;	/*!< Address of endpoint list in memory (device mode) */
	};

	__IO uint32_t  TTCTRL;					/*!< Asynchronous buffer status for embedded TT (host mode) */
	__IO uint32_t  BURSTSIZE;				/*!< Programmable burst size */
	__IO uint32_t  TXFILLTUNING;			/*!< Host transmit pre-buffer packet tuning (host mode) */
	__I  uint32_t  RESERVED4[2];
	__IO uint32_t  ULPIVIEWPORT;			/*!< ULPI viewport          */
	__IO uint32_t  BINTERVAL;				/*!< Length of virtual frame */
	__IO uint32_t  ENDPTNAK;				/*!< Endpoint NAK (device mode) */
	__IO uint32_t  ENDPTNAKEN;				/*!< Endpoint NAK Enable (device mode) */
	__I  uint32_t  RESERVED5;
	union {
		__IO uint32_t  PORTSC1_H;			/*!< Port 1 status/control (host mode) */
		__IO uint32_t  PORTSC1_D;			/*!< Port 1 status/control (device mode) */
	};

	__I  uint32_t  RESERVED6[7];
	__IO uint32_t  OTGSC;					/*!< OTG status and control */
	union {
		__IO uint32_t  USBMODE_H;			/*!< USB mode (host mode)   */
		__IO uint32_t  USBMODE_D;			/*!< USB mode (device mode) */
	};

	__IO uint32_t  ENDPTSETUPSTAT;			/*!< Endpoint setup status  */
	__IO uint32_t  ENDPTPRIME;				/*!< Endpoint initialization */
	__IO uint32_t  ENDPTFLUSH;				/*!< Endpoint de-initialization */
	__I  uint32_t  ENDPTSTAT;				/*!< Endpoint status        */
	__IO uint32_t  ENDPTCOMPLETE;			/*!< Endpoint complete      */
	__IO uint32_t  ENDPTCTRL[6];			/*!< Endpoint control 0     */
} LPC_USBHS_T;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __USBHS_18XX_43XX_H_ */






