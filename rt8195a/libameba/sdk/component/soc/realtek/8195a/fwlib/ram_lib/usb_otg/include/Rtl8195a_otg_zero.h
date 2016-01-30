/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef RTL8195A_OTG_ZERO_H
#define RTL8195A_OTG_ZERO_H

#include "usb_ch9.h"
#include "usb_gadget.h"

struct zero_dev {
	//ModifiedByJD    spinlock_t		lock;
	struct usb_gadget	*gadget;
	struct usb_request	*req;		/* for control responses */

	/* when configured, we have one of two configs:
	 * - source data (in to host) and sink it (out from host)
	 * - or loop it back (out from host back in to host)
	 */
	u8			config;
	struct usb_ep		*in_ep, *out_ep, *status_ep;//ModifiedByJD    

    const struct usb_endpoint_descriptor
				*in, *out, *status;                 //ModifiedByJD    
	/* autoresume timer */
	//ModifiedByJD    struct timer_list	resume;
};


#endif
