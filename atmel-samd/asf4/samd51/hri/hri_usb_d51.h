/**
 * \file
 *
 * \brief SAM USB
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 */

#ifdef _SAMD51_USB_COMPONENT_
#ifndef _HRI_USB_D51_H_INCLUDED_
#define _HRI_USB_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_USB_CRITICAL_SECTIONS)
#define USB_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define USB_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define USB_CRITICAL_SECTION_ENTER()
#define USB_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_usbdesc_bank_ctrl_pipe_reg_t;
typedef uint16_t hri_usbdesc_bank_extreg_reg_t;
typedef uint16_t hri_usbdesc_bank_status_pipe_reg_t;
typedef uint16_t hri_usbdevice_ctrlb_reg_t;
typedef uint16_t hri_usbdevice_descriptor_extreg_reg_t;
typedef uint16_t hri_usbdevice_epintsmry_reg_t;
typedef uint16_t hri_usbdevice_fnum_reg_t;
typedef uint16_t hri_usbdevice_intenset_reg_t;
typedef uint16_t hri_usbdevice_intflag_reg_t;
typedef uint16_t hri_usbdevice_padcal_reg_t;
typedef uint16_t hri_usbhost_ctrlb_reg_t;
typedef uint16_t hri_usbhost_descriptor_ctrl_pipe_reg_t;
typedef uint16_t hri_usbhost_descriptor_extreg_reg_t;
typedef uint16_t hri_usbhost_descriptor_status_pipe_reg_t;
typedef uint16_t hri_usbhost_fnum_reg_t;
typedef uint16_t hri_usbhost_intenset_reg_t;
typedef uint16_t hri_usbhost_intflag_reg_t;
typedef uint16_t hri_usbhost_padcal_reg_t;
typedef uint16_t hri_usbhost_pintsmry_reg_t;
typedef uint32_t hri_usbdesc_bank_addr_reg_t;
typedef uint32_t hri_usbdesc_bank_pcksize_reg_t;
typedef uint32_t hri_usbdevice_descadd_reg_t;
typedef uint32_t hri_usbdevice_descriptor_addr_reg_t;
typedef uint32_t hri_usbdevice_descriptor_pcksize_reg_t;
typedef uint32_t hri_usbhost_descadd_reg_t;
typedef uint32_t hri_usbhost_descriptor_addr_reg_t;
typedef uint32_t hri_usbhost_descriptor_pcksize_reg_t;
typedef uint8_t  hri_usbdesc_bank_status_bk_reg_t;
typedef uint8_t  hri_usbdevice_ctrla_reg_t;
typedef uint8_t  hri_usbdevice_dadd_reg_t;
typedef uint8_t  hri_usbdevice_descriptor_status_bk_reg_t;
typedef uint8_t  hri_usbdevice_epcfg_reg_t;
typedef uint8_t  hri_usbdevice_epintenset_reg_t;
typedef uint8_t  hri_usbdevice_epintflag_reg_t;
typedef uint8_t  hri_usbdevice_epstatus_reg_t;
typedef uint8_t  hri_usbdevice_fsmstatus_reg_t;
typedef uint8_t  hri_usbdevice_qosctrl_reg_t;
typedef uint8_t  hri_usbdevice_status_reg_t;
typedef uint8_t  hri_usbdevice_syncbusy_reg_t;
typedef uint8_t  hri_usbendpoint_epcfg_reg_t;
typedef uint8_t  hri_usbendpoint_epintenset_reg_t;
typedef uint8_t  hri_usbendpoint_epintflag_reg_t;
typedef uint8_t  hri_usbendpoint_epstatus_reg_t;
typedef uint8_t  hri_usbhost_binterval_reg_t;
typedef uint8_t  hri_usbhost_ctrla_reg_t;
typedef uint8_t  hri_usbhost_descriptor_status_bk_reg_t;
typedef uint8_t  hri_usbhost_flenhigh_reg_t;
typedef uint8_t  hri_usbhost_fsmstatus_reg_t;
typedef uint8_t  hri_usbhost_hsofc_reg_t;
typedef uint8_t  hri_usbhost_pcfg_reg_t;
typedef uint8_t  hri_usbhost_pintenset_reg_t;
typedef uint8_t  hri_usbhost_pintflag_reg_t;
typedef uint8_t  hri_usbhost_pstatus_reg_t;
typedef uint8_t  hri_usbhost_qosctrl_reg_t;
typedef uint8_t  hri_usbhost_status_reg_t;
typedef uint8_t  hri_usbhost_syncbusy_reg_t;
typedef uint8_t  hri_usbpipe_binterval_reg_t;
typedef uint8_t  hri_usbpipe_pcfg_reg_t;
typedef uint8_t  hri_usbpipe_pintenset_reg_t;
typedef uint8_t  hri_usbpipe_pintflag_reg_t;
typedef uint8_t  hri_usbpipe_pstatus_reg_t;

static inline void hri_usbdevice_wait_for_sync(const void *const hw, hri_usbdevice_syncbusy_reg_t reg)
{
	while (((Usb *)hw)->DEVICE.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_usbdevice_is_syncing(const void *const hw, hri_usbdevice_syncbusy_reg_t reg)
{
	return ((Usb *)hw)->DEVICE.SYNCBUSY.reg & reg;
}

static inline void hri_usbhost_wait_for_sync(const void *const hw, hri_usbhost_syncbusy_reg_t reg)
{
	while (((Usb *)hw)->HOST.SYNCBUSY.reg & reg) {
	};
}

static inline bool hri_usbhost_is_syncing(const void *const hw, hri_usbhost_syncbusy_reg_t reg)
{
	return ((Usb *)hw)->HOST.SYNCBUSY.reg & reg;
}

static inline void hri_usbpipe_set_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_DTGL;
}

static inline bool hri_usbpipe_get_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_DTGL)
	       >> USB_HOST_PSTATUS_DTGL_Pos;
}

static inline void hri_usbpipe_write_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_DTGL;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_DTGL;
	}
}

static inline void hri_usbpipe_clear_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_DTGL;
}

static inline void hri_usbpipe_set_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_CURBK;
}

static inline bool hri_usbpipe_get_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_CURBK)
	       >> USB_HOST_PSTATUS_CURBK_Pos;
}

static inline void hri_usbpipe_write_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_CURBK;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_CURBK;
	}
}

static inline void hri_usbpipe_clear_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_CURBK;
}

static inline void hri_usbpipe_set_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_PFREEZE;
}

static inline bool hri_usbpipe_get_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_PFREEZE)
	       >> USB_HOST_PSTATUS_PFREEZE_Pos;
}

static inline void hri_usbpipe_write_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_PFREEZE;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_PFREEZE;
	}
}

static inline void hri_usbpipe_clear_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_PFREEZE;
}

static inline void hri_usbpipe_set_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK0RDY;
}

static inline bool hri_usbpipe_get_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_BK0RDY)
	       >> USB_HOST_PSTATUS_BK0RDY_Pos;
}

static inline void hri_usbpipe_write_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK0RDY;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK0RDY;
	}
}

static inline void hri_usbpipe_clear_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK0RDY;
}

static inline void hri_usbpipe_set_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK1RDY;
}

static inline bool hri_usbpipe_get_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_BK1RDY)
	       >> USB_HOST_PSTATUS_BK1RDY_Pos;
}

static inline void hri_usbpipe_write_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK1RDY;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK1RDY;
	}
}

static inline void hri_usbpipe_clear_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK1RDY;
}

static inline void hri_usbpipe_set_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                               hri_usbpipe_pstatus_reg_t mask)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = mask;
}

static inline hri_usbpipe_pstatus_reg_t hri_usbpipe_get_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbpipe_pstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbpipe_pstatus_reg_t hri_usbpipe_read_PSTATUS_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbHost *)hw)->HostPipe[submodule_index].PSTATUS.reg;
}

static inline void hri_usbpipe_write_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbpipe_pstatus_reg_t data)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSSET.reg = data;
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = ~data;
}

static inline void hri_usbpipe_clear_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbpipe_pstatus_reg_t mask)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PSTATUSCLR.reg = mask;
}

static inline void hri_usbpipe_set_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT0;
}

static inline bool hri_usbpipe_get_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TRCPT0)
	       >> USB_HOST_PINTENSET_TRCPT0_Pos;
}

static inline void hri_usbpipe_write_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT0;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT0;
	}
}

static inline void hri_usbpipe_clear_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT0;
}

static inline void hri_usbpipe_set_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT1;
}

static inline bool hri_usbpipe_get_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TRCPT1)
	       >> USB_HOST_PINTENSET_TRCPT1_Pos;
}

static inline void hri_usbpipe_write_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT1;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT1;
	}
}

static inline void hri_usbpipe_clear_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT1;
}

static inline void hri_usbpipe_set_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRFAIL;
}

static inline bool hri_usbpipe_get_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TRFAIL)
	       >> USB_HOST_PINTENSET_TRFAIL_Pos;
}

static inline void hri_usbpipe_write_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRFAIL;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRFAIL;
	}
}

static inline void hri_usbpipe_clear_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRFAIL;
}

static inline void hri_usbpipe_set_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_PERR;
}

static inline bool hri_usbpipe_get_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_PERR)
	       >> USB_HOST_PINTENSET_PERR_Pos;
}

static inline void hri_usbpipe_write_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_PERR;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_PERR;
	}
}

static inline void hri_usbpipe_clear_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_PERR;
}

static inline void hri_usbpipe_set_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TXSTP;
}

static inline bool hri_usbpipe_get_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TXSTP)
	       >> USB_HOST_PINTENSET_TXSTP_Pos;
}

static inline void hri_usbpipe_write_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TXSTP;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TXSTP;
	}
}

static inline void hri_usbpipe_clear_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TXSTP;
}

static inline void hri_usbpipe_set_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_STALL;
}

static inline bool hri_usbpipe_get_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_STALL)
	       >> USB_HOST_PINTENSET_STALL_Pos;
}

static inline void hri_usbpipe_write_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_STALL;
	} else {
		((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_STALL;
	}
}

static inline void hri_usbpipe_clear_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_STALL;
}

static inline void hri_usbpipe_set_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                              hri_usbpipe_pintenset_reg_t mask)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = mask;
}

static inline hri_usbpipe_pintenset_reg_t hri_usbpipe_get_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                                     hri_usbpipe_pintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbpipe_pintenset_reg_t hri_usbpipe_read_PINTEN_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg;
}

static inline void hri_usbpipe_write_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                hri_usbpipe_pintenset_reg_t data)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENSET.reg = data;
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = ~data;
}

static inline void hri_usbpipe_clear_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                hri_usbpipe_pintenset_reg_t mask)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTENCLR.reg = mask;
}

static inline bool hri_usbpipe_get_PINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT0)
	       >> USB_HOST_PINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbpipe_clear_PINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT0;
}

static inline bool hri_usbpipe_get_PINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT1)
	       >> USB_HOST_PINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbpipe_clear_PINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT1;
}

static inline bool hri_usbpipe_get_PINTFLAG_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRFAIL)
	       >> USB_HOST_PINTFLAG_TRFAIL_Pos;
}

static inline void hri_usbpipe_clear_PINTFLAG_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRFAIL;
}

static inline bool hri_usbpipe_get_PINTFLAG_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_PERR)
	       >> USB_HOST_PINTFLAG_PERR_Pos;
}

static inline void hri_usbpipe_clear_PINTFLAG_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_PERR;
}

static inline bool hri_usbpipe_get_PINTFLAG_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TXSTP)
	       >> USB_HOST_PINTFLAG_TXSTP_Pos;
}

static inline void hri_usbpipe_clear_PINTFLAG_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TXSTP;
}

static inline bool hri_usbpipe_get_PINTFLAG_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_STALL)
	       >> USB_HOST_PINTFLAG_STALL_Pos;
}

static inline void hri_usbpipe_clear_PINTFLAG_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_STALL;
}

static inline bool hri_usbpipe_get_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT0)
	       >> USB_HOST_PINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbpipe_clear_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT0;
}

static inline bool hri_usbpipe_get_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT1)
	       >> USB_HOST_PINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbpipe_clear_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT1;
}

static inline bool hri_usbpipe_get_interrupt_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRFAIL)
	       >> USB_HOST_PINTFLAG_TRFAIL_Pos;
}

static inline void hri_usbpipe_clear_interrupt_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRFAIL;
}

static inline bool hri_usbpipe_get_interrupt_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_PERR)
	       >> USB_HOST_PINTFLAG_PERR_Pos;
}

static inline void hri_usbpipe_clear_interrupt_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_PERR;
}

static inline bool hri_usbpipe_get_interrupt_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TXSTP)
	       >> USB_HOST_PINTFLAG_TXSTP_Pos;
}

static inline void hri_usbpipe_clear_interrupt_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TXSTP;
}

static inline bool hri_usbpipe_get_interrupt_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_STALL)
	       >> USB_HOST_PINTFLAG_STALL_Pos;
}

static inline void hri_usbpipe_clear_interrupt_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_STALL;
}

static inline hri_usbpipe_pintflag_reg_t hri_usbpipe_get_PINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                                      hri_usbpipe_pintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbpipe_pintflag_reg_t hri_usbpipe_read_PINTFLAG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg;
}

static inline void hri_usbpipe_clear_PINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                  hri_usbpipe_pintflag_reg_t mask)
{
	((UsbHost *)hw)->HostPipe[submodule_index].PINTFLAG.reg = mask;
}

static inline void hri_usbpipe_set_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg |= USB_HOST_PCFG_BK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbpipe_get_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_BK) >> USB_HOST_PCFG_BK_Pos;
	return (bool)tmp;
}

static inline void hri_usbpipe_write_PCFG_BK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp &= ~USB_HOST_PCFG_BK;
	tmp |= value << USB_HOST_PCFG_BK_Pos;
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_clear_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg &= ~USB_HOST_PCFG_BK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_toggle_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg ^= USB_HOST_PCFG_BK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_set_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg |= USB_HOST_PCFG_PTOKEN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_pcfg_reg_t hri_usbpipe_get_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbpipe_pcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTOKEN(mask)) >> USB_HOST_PCFG_PTOKEN_Pos;
	return tmp;
}

static inline void hri_usbpipe_write_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_usbpipe_pcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp &= ~USB_HOST_PCFG_PTOKEN_Msk;
	tmp |= USB_HOST_PCFG_PTOKEN(data);
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_clear_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg &= ~USB_HOST_PCFG_PTOKEN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_toggle_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg ^= USB_HOST_PCFG_PTOKEN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_pcfg_reg_t hri_usbpipe_read_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTOKEN_Msk) >> USB_HOST_PCFG_PTOKEN_Pos;
	return tmp;
}

static inline void hri_usbpipe_set_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg |= USB_HOST_PCFG_PTYPE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_pcfg_reg_t hri_usbpipe_get_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbpipe_pcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTYPE(mask)) >> USB_HOST_PCFG_PTYPE_Pos;
	return tmp;
}

static inline void hri_usbpipe_write_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_usbpipe_pcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp &= ~USB_HOST_PCFG_PTYPE_Msk;
	tmp |= USB_HOST_PCFG_PTYPE(data);
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_clear_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg &= ~USB_HOST_PCFG_PTYPE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_toggle_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg ^= USB_HOST_PCFG_PTYPE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_pcfg_reg_t hri_usbpipe_read_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTYPE_Msk) >> USB_HOST_PCFG_PTYPE_Pos;
	return tmp;
}

static inline void hri_usbpipe_set_PCFG_reg(const void *const hw, uint8_t submodule_index, hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_pcfg_reg_t hri_usbpipe_get_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_usbpipe_pcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbpipe_write_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                              hri_usbpipe_pcfg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_clear_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                              hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_toggle_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                               hri_usbpipe_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_pcfg_reg_t hri_usbpipe_read_PCFG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbHost *)hw)->HostPipe[submodule_index].PCFG.reg;
}

static inline void hri_usbpipe_set_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_usbpipe_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg |= USB_HOST_BINTERVAL_BITINTERVAL(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_binterval_reg_t hri_usbpipe_get_BINTERVAL_BITINTERVAL_bf(const void *const hw,
                                                                                   uint8_t           submodule_index,
                                                                                   hri_usbpipe_binterval_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg;
	tmp = (tmp & USB_HOST_BINTERVAL_BITINTERVAL(mask)) >> USB_HOST_BINTERVAL_BITINTERVAL_Pos;
	return tmp;
}

static inline void hri_usbpipe_write_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbpipe_binterval_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg;
	tmp &= ~USB_HOST_BINTERVAL_BITINTERVAL_Msk;
	tmp |= USB_HOST_BINTERVAL_BITINTERVAL(data);
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_clear_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbpipe_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg &= ~USB_HOST_BINTERVAL_BITINTERVAL(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_toggle_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                               hri_usbpipe_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg ^= USB_HOST_BINTERVAL_BITINTERVAL(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_binterval_reg_t hri_usbpipe_read_BINTERVAL_BITINTERVAL_bf(const void *const hw,
                                                                                    uint8_t           submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg;
	tmp = (tmp & USB_HOST_BINTERVAL_BITINTERVAL_Msk) >> USB_HOST_BINTERVAL_BITINTERVAL_Pos;
	return tmp;
}

static inline void hri_usbpipe_set_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbpipe_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_binterval_reg_t hri_usbpipe_get_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                                        hri_usbpipe_binterval_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbpipe_write_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbpipe_binterval_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_clear_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbpipe_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbpipe_toggle_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_usbpipe_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbpipe_binterval_reg_t hri_usbpipe_read_BINTERVAL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbHost *)hw)->HostPipe[submodule_index].BINTERVAL.reg;
}

static inline void hri_usbhost_set_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_DTGL;
}

static inline bool hri_usbhost_get_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_DTGL)
	       >> USB_HOST_PSTATUS_DTGL_Pos;
}

static inline void hri_usbhost_write_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_DTGL;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_DTGL;
	}
}

static inline void hri_usbhost_clear_PSTATUS_DTGL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_DTGL;
}

static inline void hri_usbhost_set_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_CURBK;
}

static inline bool hri_usbhost_get_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_CURBK)
	       >> USB_HOST_PSTATUS_CURBK_Pos;
}

static inline void hri_usbhost_write_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_CURBK;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_CURBK;
	}
}

static inline void hri_usbhost_clear_PSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_CURBK;
}

static inline void hri_usbhost_set_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_PFREEZE;
}

static inline bool hri_usbhost_get_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_PFREEZE)
	       >> USB_HOST_PSTATUS_PFREEZE_Pos;
}

static inline void hri_usbhost_write_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_PFREEZE;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_PFREEZE;
	}
}

static inline void hri_usbhost_clear_PSTATUS_PFREEZE_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_PFREEZE;
}

static inline void hri_usbhost_set_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK0RDY;
}

static inline bool hri_usbhost_get_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_BK0RDY)
	       >> USB_HOST_PSTATUS_BK0RDY_Pos;
}

static inline void hri_usbhost_write_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK0RDY;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK0RDY;
	}
}

static inline void hri_usbhost_clear_PSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK0RDY;
}

static inline void hri_usbhost_set_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK1RDY;
}

static inline bool hri_usbhost_get_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg & USB_HOST_PSTATUS_BK1RDY)
	       >> USB_HOST_PSTATUS_BK1RDY_Pos;
}

static inline void hri_usbhost_write_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK1RDY;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = USB_HOST_PSTATUS_BK1RDY;
	}
}

static inline void hri_usbhost_clear_PSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = USB_HOST_PSTATUS_BK1RDY;
}

static inline void hri_usbhost_set_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                               hri_usbhost_pstatus_reg_t mask)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = mask;
}

static inline hri_usbhost_pstatus_reg_t hri_usbhost_get_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbhost_pstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_pstatus_reg_t hri_usbhost_read_PSTATUS_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUS.reg;
}

static inline void hri_usbhost_write_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbhost_pstatus_reg_t data)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSSET.reg = data;
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = ~data;
}

static inline void hri_usbhost_clear_PSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbhost_pstatus_reg_t mask)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PSTATUSCLR.reg = mask;
}

static inline void hri_usbhost_set_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT0;
}

static inline bool hri_usbhost_get_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TRCPT0)
	       >> USB_HOST_PINTENSET_TRCPT0_Pos;
}

static inline void hri_usbhost_write_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT0;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT0;
	}
}

static inline void hri_usbhost_clear_PINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT0;
}

static inline void hri_usbhost_set_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT1;
}

static inline bool hri_usbhost_get_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TRCPT1)
	       >> USB_HOST_PINTENSET_TRCPT1_Pos;
}

static inline void hri_usbhost_write_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT1;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRCPT1;
	}
}

static inline void hri_usbhost_clear_PINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRCPT1;
}

static inline void hri_usbhost_set_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRFAIL;
}

static inline bool hri_usbhost_get_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TRFAIL)
	       >> USB_HOST_PINTENSET_TRFAIL_Pos;
}

static inline void hri_usbhost_write_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRFAIL;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TRFAIL;
	}
}

static inline void hri_usbhost_clear_PINTEN_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TRFAIL;
}

static inline void hri_usbhost_set_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_PERR;
}

static inline bool hri_usbhost_get_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_PERR)
	       >> USB_HOST_PINTENSET_PERR_Pos;
}

static inline void hri_usbhost_write_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_PERR;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_PERR;
	}
}

static inline void hri_usbhost_clear_PINTEN_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_PERR;
}

static inline void hri_usbhost_set_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TXSTP;
}

static inline bool hri_usbhost_get_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_TXSTP)
	       >> USB_HOST_PINTENSET_TXSTP_Pos;
}

static inline void hri_usbhost_write_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TXSTP;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_TXSTP;
	}
}

static inline void hri_usbhost_clear_PINTEN_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_TXSTP;
}

static inline void hri_usbhost_set_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_STALL;
}

static inline bool hri_usbhost_get_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg & USB_HOST_PINTENSET_STALL)
	       >> USB_HOST_PINTENSET_STALL_Pos;
}

static inline void hri_usbhost_write_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_STALL;
	} else {
		((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = USB_HOST_PINTENSET_STALL;
	}
}

static inline void hri_usbhost_clear_PINTEN_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = USB_HOST_PINTENSET_STALL;
}

static inline void hri_usbhost_set_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                              hri_usbhost_pintenset_reg_t mask)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = mask;
}

static inline hri_usbhost_pintenset_reg_t hri_usbhost_get_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                                     hri_usbhost_pintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_pintenset_reg_t hri_usbhost_read_PINTEN_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg;
}

static inline void hri_usbhost_write_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                hri_usbhost_pintenset_reg_t data)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENSET.reg = data;
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = ~data;
}

static inline void hri_usbhost_clear_PINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                hri_usbhost_pintenset_reg_t mask)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTENCLR.reg = mask;
}

static inline bool hri_usbhost_get_PINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT0)
	       >> USB_HOST_PINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbhost_clear_PINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT0;
}

static inline bool hri_usbhost_get_PINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT1)
	       >> USB_HOST_PINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbhost_clear_PINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT1;
}

static inline bool hri_usbhost_get_PINTFLAG_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRFAIL)
	       >> USB_HOST_PINTFLAG_TRFAIL_Pos;
}

static inline void hri_usbhost_clear_PINTFLAG_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRFAIL;
}

static inline bool hri_usbhost_get_PINTFLAG_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_PERR)
	       >> USB_HOST_PINTFLAG_PERR_Pos;
}

static inline void hri_usbhost_clear_PINTFLAG_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_PERR;
}

static inline bool hri_usbhost_get_PINTFLAG_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TXSTP)
	       >> USB_HOST_PINTFLAG_TXSTP_Pos;
}

static inline void hri_usbhost_clear_PINTFLAG_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TXSTP;
}

static inline bool hri_usbhost_get_PINTFLAG_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_STALL)
	       >> USB_HOST_PINTFLAG_STALL_Pos;
}

static inline void hri_usbhost_clear_PINTFLAG_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_STALL;
}

static inline bool hri_usbhost_get_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT0)
	       >> USB_HOST_PINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbhost_clear_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT0;
}

static inline bool hri_usbhost_get_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRCPT1)
	       >> USB_HOST_PINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbhost_clear_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRCPT1;
}

static inline bool hri_usbhost_get_interrupt_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TRFAIL)
	       >> USB_HOST_PINTFLAG_TRFAIL_Pos;
}

static inline void hri_usbhost_clear_interrupt_TRFAIL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TRFAIL;
}

static inline bool hri_usbhost_get_interrupt_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_PERR)
	       >> USB_HOST_PINTFLAG_PERR_Pos;
}

static inline void hri_usbhost_clear_interrupt_PERR_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_PERR;
}

static inline bool hri_usbhost_get_interrupt_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_TXSTP)
	       >> USB_HOST_PINTFLAG_TXSTP_Pos;
}

static inline void hri_usbhost_clear_interrupt_TXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_TXSTP;
}

static inline bool hri_usbhost_get_interrupt_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg & USB_HOST_PINTFLAG_STALL)
	       >> USB_HOST_PINTFLAG_STALL_Pos;
}

static inline void hri_usbhost_clear_interrupt_STALL_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = USB_HOST_PINTFLAG_STALL;
}

static inline hri_usbhost_pintflag_reg_t hri_usbhost_get_PINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                                      hri_usbhost_pintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_pintflag_reg_t hri_usbhost_read_PINTFLAG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg;
}

static inline void hri_usbhost_clear_PINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                  hri_usbhost_pintflag_reg_t mask)
{
	((Usb *)hw)->HOST.HostPipe[submodule_index].PINTFLAG.reg = mask;
}

static inline void hri_usbhost_set_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg |= USB_HOST_PCFG_BK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_BK) >> USB_HOST_PCFG_BK_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_PCFG_BK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp &= ~USB_HOST_PCFG_BK;
	tmp |= value << USB_HOST_PCFG_BK_Pos;
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg &= ~USB_HOST_PCFG_BK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PCFG_BK_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg ^= USB_HOST_PCFG_BK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg |= USB_HOST_PCFG_PTOKEN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_pcfg_reg_t hri_usbhost_get_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbhost_pcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTOKEN(mask)) >> USB_HOST_PCFG_PTOKEN_Pos;
	return tmp;
}

static inline void hri_usbhost_write_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_usbhost_pcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp &= ~USB_HOST_PCFG_PTOKEN_Msk;
	tmp |= USB_HOST_PCFG_PTOKEN(data);
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg &= ~USB_HOST_PCFG_PTOKEN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index,
                                                     hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg ^= USB_HOST_PCFG_PTOKEN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_pcfg_reg_t hri_usbhost_read_PCFG_PTOKEN_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTOKEN_Msk) >> USB_HOST_PCFG_PTOKEN_Pos;
	return tmp;
}

static inline void hri_usbhost_set_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg |= USB_HOST_PCFG_PTYPE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_pcfg_reg_t hri_usbhost_get_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_pcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTYPE(mask)) >> USB_HOST_PCFG_PTYPE_Pos;
	return tmp;
}

static inline void hri_usbhost_write_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_usbhost_pcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp &= ~USB_HOST_PCFG_PTYPE_Msk;
	tmp |= USB_HOST_PCFG_PTYPE(data);
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg &= ~USB_HOST_PCFG_PTYPE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg ^= USB_HOST_PCFG_PTYPE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_pcfg_reg_t hri_usbhost_read_PCFG_PTYPE_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp = (tmp & USB_HOST_PCFG_PTYPE_Msk) >> USB_HOST_PCFG_PTYPE_Pos;
	return tmp;
}

static inline void hri_usbhost_set_PCFG_reg(const void *const hw, uint8_t submodule_index, hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_pcfg_reg_t hri_usbhost_get_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_usbhost_pcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                              hri_usbhost_pcfg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                              hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PCFG_reg(const void *const hw, uint8_t submodule_index,
                                               hri_usbhost_pcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_pcfg_reg_t hri_usbhost_read_PCFG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Usb *)hw)->HOST.HostPipe[submodule_index].PCFG.reg;
}

static inline void hri_usbhost_set_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_usbhost_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg |= USB_HOST_BINTERVAL_BITINTERVAL(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_binterval_reg_t hri_usbhost_get_BINTERVAL_BITINTERVAL_bf(const void *const hw,
                                                                                   uint8_t           submodule_index,
                                                                                   hri_usbhost_binterval_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg;
	tmp = (tmp & USB_HOST_BINTERVAL_BITINTERVAL(mask)) >> USB_HOST_BINTERVAL_BITINTERVAL_Pos;
	return tmp;
}

static inline void hri_usbhost_write_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbhost_binterval_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg;
	tmp &= ~USB_HOST_BINTERVAL_BITINTERVAL_Msk;
	tmp |= USB_HOST_BINTERVAL_BITINTERVAL(data);
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbhost_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg &= ~USB_HOST_BINTERVAL_BITINTERVAL(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_BINTERVAL_BITINTERVAL_bf(const void *const hw, uint8_t submodule_index,
                                                               hri_usbhost_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg ^= USB_HOST_BINTERVAL_BITINTERVAL(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_binterval_reg_t hri_usbhost_read_BINTERVAL_BITINTERVAL_bf(const void *const hw,
                                                                                    uint8_t           submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg;
	tmp = (tmp & USB_HOST_BINTERVAL_BITINTERVAL_Msk) >> USB_HOST_BINTERVAL_BITINTERVAL_Pos;
	return tmp;
}

static inline void hri_usbhost_set_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbhost_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_binterval_reg_t hri_usbhost_get_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                                        hri_usbhost_binterval_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbhost_binterval_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbhost_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_BINTERVAL_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_usbhost_binterval_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_binterval_reg_t hri_usbhost_read_BINTERVAL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Usb *)hw)->HOST.HostPipe[submodule_index].BINTERVAL.reg;
}

static inline void hri_usbhost_set_INTEN_HSOF_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_HSOF;
}

static inline bool hri_usbhost_get_INTEN_HSOF_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_HSOF) >> USB_HOST_INTENSET_HSOF_Pos;
}

static inline void hri_usbhost_write_INTEN_HSOF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_HSOF;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_HSOF;
	}
}

static inline void hri_usbhost_clear_INTEN_HSOF_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_HSOF;
}

static inline void hri_usbhost_set_INTEN_RST_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_RST;
}

static inline bool hri_usbhost_get_INTEN_RST_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_RST) >> USB_HOST_INTENSET_RST_Pos;
}

static inline void hri_usbhost_write_INTEN_RST_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_RST;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_RST;
	}
}

static inline void hri_usbhost_clear_INTEN_RST_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_RST;
}

static inline void hri_usbhost_set_INTEN_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_WAKEUP;
}

static inline bool hri_usbhost_get_INTEN_WAKEUP_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_WAKEUP) >> USB_HOST_INTENSET_WAKEUP_Pos;
}

static inline void hri_usbhost_write_INTEN_WAKEUP_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_WAKEUP;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_WAKEUP;
	}
}

static inline void hri_usbhost_clear_INTEN_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_WAKEUP;
}

static inline void hri_usbhost_set_INTEN_DNRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_DNRSM;
}

static inline bool hri_usbhost_get_INTEN_DNRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_DNRSM) >> USB_HOST_INTENSET_DNRSM_Pos;
}

static inline void hri_usbhost_write_INTEN_DNRSM_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_DNRSM;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_DNRSM;
	}
}

static inline void hri_usbhost_clear_INTEN_DNRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_DNRSM;
}

static inline void hri_usbhost_set_INTEN_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_UPRSM;
}

static inline bool hri_usbhost_get_INTEN_UPRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_UPRSM) >> USB_HOST_INTENSET_UPRSM_Pos;
}

static inline void hri_usbhost_write_INTEN_UPRSM_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_UPRSM;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_UPRSM;
	}
}

static inline void hri_usbhost_clear_INTEN_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_UPRSM;
}

static inline void hri_usbhost_set_INTEN_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_RAMACER;
}

static inline bool hri_usbhost_get_INTEN_RAMACER_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_RAMACER) >> USB_HOST_INTENSET_RAMACER_Pos;
}

static inline void hri_usbhost_write_INTEN_RAMACER_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_RAMACER;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_RAMACER;
	}
}

static inline void hri_usbhost_clear_INTEN_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_RAMACER;
}

static inline void hri_usbhost_set_INTEN_DCONN_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_DCONN;
}

static inline bool hri_usbhost_get_INTEN_DCONN_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_DCONN) >> USB_HOST_INTENSET_DCONN_Pos;
}

static inline void hri_usbhost_write_INTEN_DCONN_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_DCONN;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_DCONN;
	}
}

static inline void hri_usbhost_clear_INTEN_DCONN_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_DCONN;
}

static inline void hri_usbhost_set_INTEN_DDISC_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_DDISC;
}

static inline bool hri_usbhost_get_INTEN_DDISC_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTENSET.reg & USB_HOST_INTENSET_DDISC) >> USB_HOST_INTENSET_DDISC_Pos;
}

static inline void hri_usbhost_write_INTEN_DDISC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_DDISC;
	} else {
		((Usb *)hw)->HOST.INTENSET.reg = USB_HOST_INTENSET_DDISC;
	}
}

static inline void hri_usbhost_clear_INTEN_DDISC_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTENCLR.reg = USB_HOST_INTENSET_DDISC;
}

static inline void hri_usbhost_set_INTEN_reg(const void *const hw, hri_usbhost_intenset_reg_t mask)
{
	((Usb *)hw)->HOST.INTENSET.reg = mask;
}

static inline hri_usbhost_intenset_reg_t hri_usbhost_get_INTEN_reg(const void *const          hw,
                                                                   hri_usbhost_intenset_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_intenset_reg_t hri_usbhost_read_INTEN_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.INTENSET.reg;
}

static inline void hri_usbhost_write_INTEN_reg(const void *const hw, hri_usbhost_intenset_reg_t data)
{
	((Usb *)hw)->HOST.INTENSET.reg = data;
	((Usb *)hw)->HOST.INTENCLR.reg = ~data;
}

static inline void hri_usbhost_clear_INTEN_reg(const void *const hw, hri_usbhost_intenset_reg_t mask)
{
	((Usb *)hw)->HOST.INTENCLR.reg = mask;
}

static inline bool hri_usbhost_get_INTFLAG_HSOF_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_HSOF) >> USB_HOST_INTFLAG_HSOF_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_HSOF_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_HSOF;
}

static inline bool hri_usbhost_get_INTFLAG_RST_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_RST) >> USB_HOST_INTFLAG_RST_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_RST_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RST;
}

static inline bool hri_usbhost_get_INTFLAG_WAKEUP_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_WAKEUP) >> USB_HOST_INTFLAG_WAKEUP_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_WAKEUP;
}

static inline bool hri_usbhost_get_INTFLAG_DNRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DNRSM) >> USB_HOST_INTFLAG_DNRSM_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_DNRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DNRSM;
}

static inline bool hri_usbhost_get_INTFLAG_UPRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_UPRSM) >> USB_HOST_INTFLAG_UPRSM_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_UPRSM;
}

static inline bool hri_usbhost_get_INTFLAG_RAMACER_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_RAMACER) >> USB_HOST_INTFLAG_RAMACER_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RAMACER;
}

static inline bool hri_usbhost_get_INTFLAG_DCONN_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DCONN) >> USB_HOST_INTFLAG_DCONN_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_DCONN_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DCONN;
}

static inline bool hri_usbhost_get_INTFLAG_DDISC_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DDISC) >> USB_HOST_INTFLAG_DDISC_Pos;
}

static inline void hri_usbhost_clear_INTFLAG_DDISC_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DDISC;
}

static inline bool hri_usbhost_get_interrupt_HSOF_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_HSOF) >> USB_HOST_INTFLAG_HSOF_Pos;
}

static inline void hri_usbhost_clear_interrupt_HSOF_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_HSOF;
}

static inline bool hri_usbhost_get_interrupt_RST_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_RST) >> USB_HOST_INTFLAG_RST_Pos;
}

static inline void hri_usbhost_clear_interrupt_RST_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RST;
}

static inline bool hri_usbhost_get_interrupt_WAKEUP_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_WAKEUP) >> USB_HOST_INTFLAG_WAKEUP_Pos;
}

static inline void hri_usbhost_clear_interrupt_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_WAKEUP;
}

static inline bool hri_usbhost_get_interrupt_DNRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DNRSM) >> USB_HOST_INTFLAG_DNRSM_Pos;
}

static inline void hri_usbhost_clear_interrupt_DNRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DNRSM;
}

static inline bool hri_usbhost_get_interrupt_UPRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_UPRSM) >> USB_HOST_INTFLAG_UPRSM_Pos;
}

static inline void hri_usbhost_clear_interrupt_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_UPRSM;
}

static inline bool hri_usbhost_get_interrupt_RAMACER_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_RAMACER) >> USB_HOST_INTFLAG_RAMACER_Pos;
}

static inline void hri_usbhost_clear_interrupt_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_RAMACER;
}

static inline bool hri_usbhost_get_interrupt_DCONN_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DCONN) >> USB_HOST_INTFLAG_DCONN_Pos;
}

static inline void hri_usbhost_clear_interrupt_DCONN_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DCONN;
}

static inline bool hri_usbhost_get_interrupt_DDISC_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.INTFLAG.reg & USB_HOST_INTFLAG_DDISC) >> USB_HOST_INTFLAG_DDISC_Pos;
}

static inline void hri_usbhost_clear_interrupt_DDISC_bit(const void *const hw)
{
	((Usb *)hw)->HOST.INTFLAG.reg = USB_HOST_INTFLAG_DDISC;
}

static inline hri_usbhost_intflag_reg_t hri_usbhost_get_INTFLAG_reg(const void *const         hw,
                                                                    hri_usbhost_intflag_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_intflag_reg_t hri_usbhost_read_INTFLAG_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.INTFLAG.reg;
}

static inline void hri_usbhost_clear_INTFLAG_reg(const void *const hw, hri_usbhost_intflag_reg_t mask)
{
	((Usb *)hw)->HOST.INTFLAG.reg = mask;
}

static inline void hri_usbhost_set_CTRLA_SWRST_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST);
	((Usb *)hw)->HOST.CTRLA.reg |= USB_CTRLA_SWRST;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST);
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_SWRST) >> USB_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_set_CTRLA_ENABLE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	((Usb *)hw)->HOST.CTRLA.reg |= USB_CTRLA_ENABLE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_ENABLE) >> USB_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp &= ~USB_CTRLA_ENABLE;
	tmp |= value << USB_CTRLA_ENABLE_Pos;
	((Usb *)hw)->HOST.CTRLA.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	((Usb *)hw)->HOST.CTRLA.reg &= ~USB_CTRLA_ENABLE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	((Usb *)hw)->HOST.CTRLA.reg ^= USB_CTRLA_ENABLE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->HOST.CTRLA.reg |= USB_CTRLA_RUNSTDBY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_RUNSTDBY) >> USB_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp &= ~USB_CTRLA_RUNSTDBY;
	tmp |= value << USB_CTRLA_RUNSTDBY_Pos;
	((Usb *)hw)->HOST.CTRLA.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->HOST.CTRLA.reg &= ~USB_CTRLA_RUNSTDBY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->HOST.CTRLA.reg ^= USB_CTRLA_RUNSTDBY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLA_MODE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->HOST.CTRLA.reg |= USB_CTRLA_MODE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLA_MODE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_MODE) >> USB_CTRLA_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLA_MODE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp &= ~USB_CTRLA_MODE;
	tmp |= value << USB_CTRLA_MODE_Pos;
	((Usb *)hw)->HOST.CTRLA.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLA_MODE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->HOST.CTRLA.reg &= ~USB_CTRLA_MODE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLA_MODE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbhost_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->HOST.CTRLA.reg ^= USB_CTRLA_MODE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLA_reg(const void *const hw, hri_usbhost_ctrla_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLA.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_ctrla_reg_t hri_usbhost_get_CTRLA_reg(const void *const hw, hri_usbhost_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_CTRLA_reg(const void *const hw, hri_usbhost_ctrla_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLA.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLA_reg(const void *const hw, hri_usbhost_ctrla_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLA.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLA_reg(const void *const hw, hri_usbhost_ctrla_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLA.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_ctrla_reg_t hri_usbhost_read_CTRLA_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.CTRLA.reg;
}

static inline void hri_usbhost_set_QOSCTRL_CQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg |= USB_QOSCTRL_CQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_qosctrl_reg_t hri_usbhost_get_QOSCTRL_CQOS_bf(const void *const         hw,
                                                                        hri_usbhost_qosctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_CQOS(mask)) >> USB_QOSCTRL_CQOS_Pos;
	return tmp;
}

static inline void hri_usbhost_write_QOSCTRL_CQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp &= ~USB_QOSCTRL_CQOS_Msk;
	tmp |= USB_QOSCTRL_CQOS(data);
	((Usb *)hw)->HOST.QOSCTRL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_QOSCTRL_CQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg &= ~USB_QOSCTRL_CQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_QOSCTRL_CQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg ^= USB_QOSCTRL_CQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_qosctrl_reg_t hri_usbhost_read_QOSCTRL_CQOS_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_CQOS_Msk) >> USB_QOSCTRL_CQOS_Pos;
	return tmp;
}

static inline void hri_usbhost_set_QOSCTRL_DQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg |= USB_QOSCTRL_DQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_qosctrl_reg_t hri_usbhost_get_QOSCTRL_DQOS_bf(const void *const         hw,
                                                                        hri_usbhost_qosctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_DQOS(mask)) >> USB_QOSCTRL_DQOS_Pos;
	return tmp;
}

static inline void hri_usbhost_write_QOSCTRL_DQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp &= ~USB_QOSCTRL_DQOS_Msk;
	tmp |= USB_QOSCTRL_DQOS(data);
	((Usb *)hw)->HOST.QOSCTRL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_QOSCTRL_DQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg &= ~USB_QOSCTRL_DQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_QOSCTRL_DQOS_bf(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg ^= USB_QOSCTRL_DQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_qosctrl_reg_t hri_usbhost_read_QOSCTRL_DQOS_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_DQOS_Msk) >> USB_QOSCTRL_DQOS_Pos;
	return tmp;
}

static inline void hri_usbhost_set_QOSCTRL_reg(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_qosctrl_reg_t hri_usbhost_get_QOSCTRL_reg(const void *const         hw,
                                                                    hri_usbhost_qosctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.QOSCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_QOSCTRL_reg(const void *const hw, hri_usbhost_qosctrl_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_QOSCTRL_reg(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_QOSCTRL_reg(const void *const hw, hri_usbhost_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.QOSCTRL.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_qosctrl_reg_t hri_usbhost_read_QOSCTRL_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.QOSCTRL.reg;
}

static inline void hri_usbhost_set_CTRLB_RESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_RESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_RESUME_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_RESUME) >> USB_HOST_CTRLB_RESUME_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_RESUME_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_RESUME;
	tmp |= value << USB_HOST_CTRLB_RESUME_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_RESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_RESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_RESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_RESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_AUTORESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_AUTORESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_AUTORESUME_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_AUTORESUME) >> USB_HOST_CTRLB_AUTORESUME_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_AUTORESUME_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_AUTORESUME;
	tmp |= value << USB_HOST_CTRLB_AUTORESUME_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_AUTORESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_AUTORESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_AUTORESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_AUTORESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_TSTJ_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_TSTJ;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_TSTJ_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_TSTJ) >> USB_HOST_CTRLB_TSTJ_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_TSTJ_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_TSTJ;
	tmp |= value << USB_HOST_CTRLB_TSTJ_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_TSTJ_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_TSTJ;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_TSTJ_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_TSTJ;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_TSTK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_TSTK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_TSTK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_TSTK) >> USB_HOST_CTRLB_TSTK_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_TSTK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_TSTK;
	tmp |= value << USB_HOST_CTRLB_TSTK_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_TSTK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_TSTK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_TSTK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_TSTK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_SOFE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_SOFE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_SOFE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_SOFE) >> USB_HOST_CTRLB_SOFE_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_SOFE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_SOFE;
	tmp |= value << USB_HOST_CTRLB_SOFE_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_SOFE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_SOFE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_SOFE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_SOFE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_BUSRESET_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_BUSRESET;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_BUSRESET_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_BUSRESET) >> USB_HOST_CTRLB_BUSRESET_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_BUSRESET_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_BUSRESET;
	tmp |= value << USB_HOST_CTRLB_BUSRESET_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_BUSRESET_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_BUSRESET;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_BUSRESET_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_BUSRESET;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_VBUSOK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_VBUSOK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_VBUSOK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_VBUSOK) >> USB_HOST_CTRLB_VBUSOK_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_VBUSOK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_VBUSOK;
	tmp |= value << USB_HOST_CTRLB_VBUSOK_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_VBUSOK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_VBUSOK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_VBUSOK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_VBUSOK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_L1RESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_L1RESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_CTRLB_L1RESUME_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_L1RESUME) >> USB_HOST_CTRLB_L1RESUME_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_CTRLB_L1RESUME_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_L1RESUME;
	tmp |= value << USB_HOST_CTRLB_L1RESUME_Pos;
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_L1RESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_L1RESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_L1RESUME_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_L1RESUME;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_CTRLB_SPDCONF_bf(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= USB_HOST_CTRLB_SPDCONF(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_ctrlb_reg_t hri_usbhost_get_CTRLB_SPDCONF_bf(const void *const       hw,
                                                                       hri_usbhost_ctrlb_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_SPDCONF(mask)) >> USB_HOST_CTRLB_SPDCONF_Pos;
	return tmp;
}

static inline void hri_usbhost_write_CTRLB_SPDCONF_bf(const void *const hw, hri_usbhost_ctrlb_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= ~USB_HOST_CTRLB_SPDCONF_Msk;
	tmp |= USB_HOST_CTRLB_SPDCONF(data);
	((Usb *)hw)->HOST.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_SPDCONF_bf(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~USB_HOST_CTRLB_SPDCONF(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_SPDCONF_bf(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= USB_HOST_CTRLB_SPDCONF(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_ctrlb_reg_t hri_usbhost_read_CTRLB_SPDCONF_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp = (tmp & USB_HOST_CTRLB_SPDCONF_Msk) >> USB_HOST_CTRLB_SPDCONF_Pos;
	return tmp;
}

static inline void hri_usbhost_set_CTRLB_reg(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_ctrlb_reg_t hri_usbhost_get_CTRLB_reg(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_CTRLB_reg(const void *const hw, hri_usbhost_ctrlb_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_CTRLB_reg(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_CTRLB_reg(const void *const hw, hri_usbhost_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.CTRLB.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_ctrlb_reg_t hri_usbhost_read_CTRLB_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.CTRLB.reg;
}

static inline void hri_usbhost_set_HSOFC_FLENCE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg |= USB_HOST_HSOFC_FLENCE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhost_get_HSOFC_FLENCE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HSOFC.reg;
	tmp = (tmp & USB_HOST_HSOFC_FLENCE) >> USB_HOST_HSOFC_FLENCE_Pos;
	return (bool)tmp;
}

static inline void hri_usbhost_write_HSOFC_FLENCE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.HSOFC.reg;
	tmp &= ~USB_HOST_HSOFC_FLENCE;
	tmp |= value << USB_HOST_HSOFC_FLENCE_Pos;
	((Usb *)hw)->HOST.HSOFC.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_HSOFC_FLENCE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg &= ~USB_HOST_HSOFC_FLENCE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_HSOFC_FLENCE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg ^= USB_HOST_HSOFC_FLENCE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_set_HSOFC_FLENC_bf(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg |= USB_HOST_HSOFC_FLENC(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_hsofc_reg_t hri_usbhost_get_HSOFC_FLENC_bf(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HSOFC.reg;
	tmp = (tmp & USB_HOST_HSOFC_FLENC(mask)) >> USB_HOST_HSOFC_FLENC_Pos;
	return tmp;
}

static inline void hri_usbhost_write_HSOFC_FLENC_bf(const void *const hw, hri_usbhost_hsofc_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.HSOFC.reg;
	tmp &= ~USB_HOST_HSOFC_FLENC_Msk;
	tmp |= USB_HOST_HSOFC_FLENC(data);
	((Usb *)hw)->HOST.HSOFC.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_HSOFC_FLENC_bf(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg &= ~USB_HOST_HSOFC_FLENC(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_HSOFC_FLENC_bf(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg ^= USB_HOST_HSOFC_FLENC(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_hsofc_reg_t hri_usbhost_read_HSOFC_FLENC_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HSOFC.reg;
	tmp = (tmp & USB_HOST_HSOFC_FLENC_Msk) >> USB_HOST_HSOFC_FLENC_Pos;
	return tmp;
}

static inline void hri_usbhost_set_HSOFC_reg(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_hsofc_reg_t hri_usbhost_get_HSOFC_reg(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.HSOFC.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_HSOFC_reg(const void *const hw, hri_usbhost_hsofc_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_HSOFC_reg(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_HSOFC_reg(const void *const hw, hri_usbhost_hsofc_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.HSOFC.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_hsofc_reg_t hri_usbhost_read_HSOFC_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.HSOFC.reg;
}

static inline void hri_usbhost_set_FNUM_MFNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg |= USB_HOST_FNUM_MFNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_fnum_reg_t hri_usbhost_get_FNUM_MFNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp = (tmp & USB_HOST_FNUM_MFNUM(mask)) >> USB_HOST_FNUM_MFNUM_Pos;
	return tmp;
}

static inline void hri_usbhost_write_FNUM_MFNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp &= ~USB_HOST_FNUM_MFNUM_Msk;
	tmp |= USB_HOST_FNUM_MFNUM(data);
	((Usb *)hw)->HOST.FNUM.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_FNUM_MFNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg &= ~USB_HOST_FNUM_MFNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_FNUM_MFNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg ^= USB_HOST_FNUM_MFNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_fnum_reg_t hri_usbhost_read_FNUM_MFNUM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp = (tmp & USB_HOST_FNUM_MFNUM_Msk) >> USB_HOST_FNUM_MFNUM_Pos;
	return tmp;
}

static inline void hri_usbhost_set_FNUM_FNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg |= USB_HOST_FNUM_FNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_fnum_reg_t hri_usbhost_get_FNUM_FNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp = (tmp & USB_HOST_FNUM_FNUM(mask)) >> USB_HOST_FNUM_FNUM_Pos;
	return tmp;
}

static inline void hri_usbhost_write_FNUM_FNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp &= ~USB_HOST_FNUM_FNUM_Msk;
	tmp |= USB_HOST_FNUM_FNUM(data);
	((Usb *)hw)->HOST.FNUM.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_FNUM_FNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg &= ~USB_HOST_FNUM_FNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_FNUM_FNUM_bf(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg ^= USB_HOST_FNUM_FNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_fnum_reg_t hri_usbhost_read_FNUM_FNUM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp = (tmp & USB_HOST_FNUM_FNUM_Msk) >> USB_HOST_FNUM_FNUM_Pos;
	return tmp;
}

static inline void hri_usbhost_set_FNUM_reg(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_fnum_reg_t hri_usbhost_get_FNUM_reg(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.FNUM.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_FNUM_reg(const void *const hw, hri_usbhost_fnum_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_FNUM_reg(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_FNUM_reg(const void *const hw, hri_usbhost_fnum_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.FNUM.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_fnum_reg_t hri_usbhost_read_FNUM_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.FNUM.reg;
}

static inline void hri_usbhost_set_DESCADD_DESCADD_bf(const void *const hw, hri_usbhost_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg |= USB_DESCADD_DESCADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descadd_reg_t hri_usbhost_get_DESCADD_DESCADD_bf(const void *const         hw,
                                                                           hri_usbhost_descadd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Usb *)hw)->HOST.DESCADD.reg;
	tmp = (tmp & USB_DESCADD_DESCADD(mask)) >> USB_DESCADD_DESCADD_Pos;
	return tmp;
}

static inline void hri_usbhost_write_DESCADD_DESCADD_bf(const void *const hw, hri_usbhost_descadd_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.DESCADD.reg;
	tmp &= ~USB_DESCADD_DESCADD_Msk;
	tmp |= USB_DESCADD_DESCADD(data);
	((Usb *)hw)->HOST.DESCADD.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_DESCADD_DESCADD_bf(const void *const hw, hri_usbhost_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg &= ~USB_DESCADD_DESCADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_DESCADD_DESCADD_bf(const void *const hw, hri_usbhost_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg ^= USB_DESCADD_DESCADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descadd_reg_t hri_usbhost_read_DESCADD_DESCADD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Usb *)hw)->HOST.DESCADD.reg;
	tmp = (tmp & USB_DESCADD_DESCADD_Msk) >> USB_DESCADD_DESCADD_Pos;
	return tmp;
}

static inline void hri_usbhost_set_DESCADD_reg(const void *const hw, hri_usbhost_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descadd_reg_t hri_usbhost_get_DESCADD_reg(const void *const         hw,
                                                                    hri_usbhost_descadd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Usb *)hw)->HOST.DESCADD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_DESCADD_reg(const void *const hw, hri_usbhost_descadd_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_DESCADD_reg(const void *const hw, hri_usbhost_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_DESCADD_reg(const void *const hw, hri_usbhost_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.DESCADD.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descadd_reg_t hri_usbhost_read_DESCADD_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.DESCADD.reg;
}

static inline void hri_usbhost_set_PADCAL_TRANSP_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg |= USB_PADCAL_TRANSP(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_get_PADCAL_TRANSP_bf(const void *const        hw,
                                                                        hri_usbhost_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSP(mask)) >> USB_PADCAL_TRANSP_Pos;
	return tmp;
}

static inline void hri_usbhost_write_PADCAL_TRANSP_bf(const void *const hw, hri_usbhost_padcal_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp &= ~USB_PADCAL_TRANSP_Msk;
	tmp |= USB_PADCAL_TRANSP(data);
	((Usb *)hw)->HOST.PADCAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PADCAL_TRANSP_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg &= ~USB_PADCAL_TRANSP(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PADCAL_TRANSP_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg ^= USB_PADCAL_TRANSP(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_read_PADCAL_TRANSP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSP_Msk) >> USB_PADCAL_TRANSP_Pos;
	return tmp;
}

static inline void hri_usbhost_set_PADCAL_TRANSN_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg |= USB_PADCAL_TRANSN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_get_PADCAL_TRANSN_bf(const void *const        hw,
                                                                        hri_usbhost_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSN(mask)) >> USB_PADCAL_TRANSN_Pos;
	return tmp;
}

static inline void hri_usbhost_write_PADCAL_TRANSN_bf(const void *const hw, hri_usbhost_padcal_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp &= ~USB_PADCAL_TRANSN_Msk;
	tmp |= USB_PADCAL_TRANSN(data);
	((Usb *)hw)->HOST.PADCAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PADCAL_TRANSN_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg &= ~USB_PADCAL_TRANSN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PADCAL_TRANSN_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg ^= USB_PADCAL_TRANSN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_read_PADCAL_TRANSN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSN_Msk) >> USB_PADCAL_TRANSN_Pos;
	return tmp;
}

static inline void hri_usbhost_set_PADCAL_TRIM_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg |= USB_PADCAL_TRIM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_get_PADCAL_TRIM_bf(const void *const        hw,
                                                                      hri_usbhost_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRIM(mask)) >> USB_PADCAL_TRIM_Pos;
	return tmp;
}

static inline void hri_usbhost_write_PADCAL_TRIM_bf(const void *const hw, hri_usbhost_padcal_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp &= ~USB_PADCAL_TRIM_Msk;
	tmp |= USB_PADCAL_TRIM(data);
	((Usb *)hw)->HOST.PADCAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PADCAL_TRIM_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg &= ~USB_PADCAL_TRIM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PADCAL_TRIM_bf(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg ^= USB_PADCAL_TRIM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_read_PADCAL_TRIM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRIM_Msk) >> USB_PADCAL_TRIM_Pos;
	return tmp;
}

static inline void hri_usbhost_set_PADCAL_reg(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_get_PADCAL_reg(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PADCAL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_write_PADCAL_reg(const void *const hw, hri_usbhost_padcal_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_clear_PADCAL_reg(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhost_toggle_PADCAL_reg(const void *const hw, hri_usbhost_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.PADCAL.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_padcal_reg_t hri_usbhost_read_PADCAL_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.PADCAL.reg;
}

static inline bool hri_usbhost_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.SYNCBUSY.reg & USB_SYNCBUSY_SWRST) >> USB_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_usbhost_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.SYNCBUSY.reg & USB_SYNCBUSY_ENABLE) >> USB_SYNCBUSY_ENABLE_Pos;
}

static inline hri_usbhost_syncbusy_reg_t hri_usbhost_get_SYNCBUSY_reg(const void *const          hw,
                                                                      hri_usbhost_syncbusy_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_syncbusy_reg_t hri_usbhost_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.SYNCBUSY.reg;
}

static inline hri_usbhost_fsmstatus_reg_t hri_usbhost_get_FSMSTATUS_FSMSTATE_bf(const void *const           hw,
                                                                                hri_usbhost_fsmstatus_reg_t mask)
{
	return (((Usb *)hw)->HOST.FSMSTATUS.reg & USB_FSMSTATUS_FSMSTATE(mask)) >> USB_FSMSTATUS_FSMSTATE_Pos;
}

static inline hri_usbhost_fsmstatus_reg_t hri_usbhost_read_FSMSTATUS_FSMSTATE_bf(const void *const hw)
{
	return (((Usb *)hw)->HOST.FSMSTATUS.reg & USB_FSMSTATUS_FSMSTATE_Msk) >> USB_FSMSTATUS_FSMSTATE_Pos;
}

static inline hri_usbhost_fsmstatus_reg_t hri_usbhost_get_FSMSTATUS_reg(const void *const           hw,
                                                                        hri_usbhost_fsmstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.FSMSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_fsmstatus_reg_t hri_usbhost_read_FSMSTATUS_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.FSMSTATUS.reg;
}

static inline hri_usbhost_flenhigh_reg_t hri_usbhost_get_FLENHIGH_FLENHIGH_bf(const void *const          hw,
                                                                              hri_usbhost_flenhigh_reg_t mask)
{
	return (((Usb *)hw)->HOST.FLENHIGH.reg & USB_HOST_FLENHIGH_FLENHIGH(mask)) >> USB_HOST_FLENHIGH_FLENHIGH_Pos;
}

static inline hri_usbhost_flenhigh_reg_t hri_usbhost_read_FLENHIGH_FLENHIGH_bf(const void *const hw)
{
	return (((Usb *)hw)->HOST.FLENHIGH.reg & USB_HOST_FLENHIGH_FLENHIGH_Msk) >> USB_HOST_FLENHIGH_FLENHIGH_Pos;
}

static inline hri_usbhost_flenhigh_reg_t hri_usbhost_get_FLENHIGH_reg(const void *const          hw,
                                                                      hri_usbhost_flenhigh_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.FLENHIGH.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_flenhigh_reg_t hri_usbhost_read_FLENHIGH_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.FLENHIGH.reg;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT0_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT0) >> USB_HOST_PINTSMRY_EPINT0_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT1_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT1) >> USB_HOST_PINTSMRY_EPINT1_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT2_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT2) >> USB_HOST_PINTSMRY_EPINT2_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT3_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT3) >> USB_HOST_PINTSMRY_EPINT3_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT4_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT4) >> USB_HOST_PINTSMRY_EPINT4_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT5_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT5) >> USB_HOST_PINTSMRY_EPINT5_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT6_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT6) >> USB_HOST_PINTSMRY_EPINT6_Pos;
}

static inline bool hri_usbhost_get_PINTSMRY_EPINT7_bit(const void *const hw)
{
	return (((Usb *)hw)->HOST.PINTSMRY.reg & USB_HOST_PINTSMRY_EPINT7) >> USB_HOST_PINTSMRY_EPINT7_Pos;
}

static inline hri_usbhost_pintsmry_reg_t hri_usbhost_get_PINTSMRY_reg(const void *const          hw,
                                                                      hri_usbhost_pintsmry_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->HOST.PINTSMRY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbhost_pintsmry_reg_t hri_usbhost_read_PINTSMRY_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.PINTSMRY.reg;
}

static inline hri_usbhost_status_reg_t hri_usbhost_get_STATUS_SPEED_bf(const void *const        hw,
                                                                       hri_usbhost_status_reg_t mask)
{
	return (((Usb *)hw)->HOST.STATUS.reg & USB_HOST_STATUS_SPEED(mask)) >> USB_HOST_STATUS_SPEED_Pos;
}

static inline void hri_usbhost_clear_STATUS_SPEED_bf(const void *const hw, hri_usbhost_status_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.STATUS.reg = USB_HOST_STATUS_SPEED(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_status_reg_t hri_usbhost_read_STATUS_SPEED_bf(const void *const hw)
{
	return (((Usb *)hw)->HOST.STATUS.reg & USB_HOST_STATUS_SPEED_Msk) >> USB_HOST_STATUS_SPEED_Pos;
}

static inline hri_usbhost_status_reg_t hri_usbhost_get_STATUS_LINESTATE_bf(const void *const        hw,
                                                                           hri_usbhost_status_reg_t mask)
{
	return (((Usb *)hw)->HOST.STATUS.reg & USB_HOST_STATUS_LINESTATE(mask)) >> USB_HOST_STATUS_LINESTATE_Pos;
}

static inline void hri_usbhost_clear_STATUS_LINESTATE_bf(const void *const hw, hri_usbhost_status_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.STATUS.reg = USB_HOST_STATUS_LINESTATE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_status_reg_t hri_usbhost_read_STATUS_LINESTATE_bf(const void *const hw)
{
	return (((Usb *)hw)->HOST.STATUS.reg & USB_HOST_STATUS_LINESTATE_Msk) >> USB_HOST_STATUS_LINESTATE_Pos;
}

static inline hri_usbhost_status_reg_t hri_usbhost_get_STATUS_reg(const void *const hw, hri_usbhost_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->HOST.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhost_clear_STATUS_reg(const void *const hw, hri_usbhost_status_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->HOST.STATUS.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_status_reg_t hri_usbhost_read_STATUS_reg(const void *const hw)
{
	return ((Usb *)hw)->HOST.STATUS.reg;
}

static inline void hri_usbdevicedescbank_set_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg |= USB_DEVICE_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbdevicedescbank_get_ADDR_ADDR_bf(const void *const           hw,
                                                                                 hri_usbdesc_bank_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->ADDR.reg;
	tmp = (tmp & USB_DEVICE_ADDR_ADDR(mask)) >> USB_DEVICE_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->ADDR.reg;
	tmp &= ~USB_DEVICE_ADDR_ADDR_Msk;
	tmp |= USB_DEVICE_ADDR_ADDR(data);
	((UsbDeviceDescBank *)hw)->ADDR.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg &= ~USB_DEVICE_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg ^= USB_DEVICE_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbdevicedescbank_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->ADDR.reg;
	tmp = (tmp & USB_DEVICE_ADDR_ADDR_Msk) >> USB_DEVICE_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_set_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbdevicedescbank_get_ADDR_reg(const void *const           hw,
                                                                             hri_usbdesc_bank_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->ADDR.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbdevicedescbank_read_ADDR_reg(const void *const hw)
{
	return ((UsbDeviceDescBank *)hw)->ADDR.reg;
}

static inline void hri_usbdevicedescbank_set_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg |= USB_DEVICE_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevicedescbank_get_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_AUTO_ZLP) >> USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevicedescbank_write_PCKSIZE_AUTO_ZLP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_AUTO_ZLP;
	tmp |= value << USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos;
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_set_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                   hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg |= USB_DEVICE_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbdevicedescbank_get_PCKSIZE_BYTE_COUNT_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_BYTE_COUNT(mask)) >> USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                     hri_usbdesc_bank_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk;
	tmp |= USB_DEVICE_PCKSIZE_BYTE_COUNT(data);
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                     hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                      hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbdevicedescbank_read_PCKSIZE_BYTE_COUNT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk) >> USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_set_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                          hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg |= USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbdevicedescbank_get_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask)) >> USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                            hri_usbdesc_bank_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk;
	tmp |= USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(data);
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                            hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                             hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbdevicedescbank_read_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk) >> USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_set_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg |= USB_DEVICE_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbdevicedescbank_get_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_SIZE(mask)) >> USB_DEVICE_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_PCKSIZE_SIZE_bf(const void *const              hw,
                                                               hri_usbdesc_bank_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_SIZE_Msk;
	tmp |= USB_DEVICE_PCKSIZE_SIZE(data);
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_PCKSIZE_SIZE_bf(const void *const              hw,
                                                               hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_PCKSIZE_SIZE_bf(const void *const              hw,
                                                                hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbdevicedescbank_read_PCKSIZE_SIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_SIZE_Msk) >> USB_DEVICE_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_set_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbdevicedescbank_get_PCKSIZE_reg(const void *const              hw,
                                                                                   hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->PCKSIZE.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbdevicedescbank_read_PCKSIZE_reg(const void *const hw)
{
	return ((UsbDeviceDescBank *)hw)->PCKSIZE.reg;
}

static inline void hri_usbdevicedescbank_set_EXTREG_SUBPID_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg |= USB_DEVICE_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t
hri_usbdevicedescbank_get_EXTREG_SUBPID_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_SUBPID(mask)) >> USB_DEVICE_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_EXTREG_SUBPID_bf(const void *const             hw,
                                                                hri_usbdesc_bank_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp &= ~USB_DEVICE_EXTREG_SUBPID_Msk;
	tmp |= USB_DEVICE_EXTREG_SUBPID(data);
	((UsbDeviceDescBank *)hw)->EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_EXTREG_SUBPID_bf(const void *const             hw,
                                                                hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg &= ~USB_DEVICE_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_EXTREG_SUBPID_bf(const void *const             hw,
                                                                 hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg ^= USB_DEVICE_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbdevicedescbank_read_EXTREG_SUBPID_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_SUBPID_Msk) >> USB_DEVICE_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_set_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg |= USB_DEVICE_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t
hri_usbdevicedescbank_get_EXTREG_VARIABLE_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_VARIABLE(mask)) >> USB_DEVICE_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                  hri_usbdesc_bank_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp &= ~USB_DEVICE_EXTREG_VARIABLE_Msk;
	tmp |= USB_DEVICE_EXTREG_VARIABLE(data);
	((UsbDeviceDescBank *)hw)->EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                  hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg &= ~USB_DEVICE_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                   hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg ^= USB_DEVICE_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbdevicedescbank_read_EXTREG_VARIABLE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_VARIABLE_Msk) >> USB_DEVICE_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescbank_set_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbdevicedescbank_get_EXTREG_reg(const void *const             hw,
                                                                                 hri_usbdesc_bank_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->EXTREG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescbank_write_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_clear_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescbank_toggle_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->EXTREG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbdevicedescbank_read_EXTREG_reg(const void *const hw)
{
	return ((UsbDeviceDescBank *)hw)->EXTREG.reg;
}

static inline bool hri_usbdevicedescbank_get_STATUS_BK_CRCERR_bit(const void *const hw)
{
	return (((UsbDeviceDescBank *)hw)->STATUS_BK.reg & USB_DEVICE_STATUS_BK_CRCERR) >> USB_DEVICE_STATUS_BK_CRCERR_Pos;
}

static inline void hri_usbdevicedescbank_clear_STATUS_BK_CRCERR_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->STATUS_BK.reg = USB_DEVICE_STATUS_BK_CRCERR;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevicedescbank_get_STATUS_BK_ERRORFLOW_bit(const void *const hw)
{
	return (((UsbDeviceDescBank *)hw)->STATUS_BK.reg & USB_DEVICE_STATUS_BK_ERRORFLOW)
	       >> USB_DEVICE_STATUS_BK_ERRORFLOW_Pos;
}

static inline void hri_usbdevicedescbank_clear_STATUS_BK_ERRORFLOW_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->STATUS_BK.reg = USB_DEVICE_STATUS_BK_ERRORFLOW;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_bk_reg_t
hri_usbdevicedescbank_get_STATUS_BK_reg(const void *const hw, hri_usbdesc_bank_status_bk_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDeviceDescBank *)hw)->STATUS_BK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescbank_clear_STATUS_BK_reg(const void *const                hw,
                                                             hri_usbdesc_bank_status_bk_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescBank *)hw)->STATUS_BK.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_bk_reg_t hri_usbdevicedescbank_read_STATUS_BK_reg(const void *const hw)
{
	return ((UsbDeviceDescBank *)hw)->STATUS_BK.reg;
}

static inline void hri_usbdevicedescriptor_set_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_usbdevice_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg |= USB_DEVICE_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_addr_reg_t
hri_usbdevicedescriptor_get_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                         hri_usbdevice_descriptor_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg;
	tmp = (tmp & USB_DEVICE_ADDR_ADDR(mask)) >> USB_DEVICE_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbdevice_descriptor_addr_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg;
	tmp &= ~USB_DEVICE_ADDR_ADDR_Msk;
	tmp |= USB_DEVICE_ADDR_ADDR(data);
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbdevice_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg &= ~USB_DEVICE_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                               hri_usbdevice_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg ^= USB_DEVICE_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_addr_reg_t hri_usbdevicedescriptor_read_ADDR_ADDR_bf(const void *const hw,
                                                                                            uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg;
	tmp = (tmp & USB_DEVICE_ADDR_ADDR_Msk) >> USB_DEVICE_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_set_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                        hri_usbdevice_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_addr_reg_t
hri_usbdevicedescriptor_get_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                     hri_usbdevice_descriptor_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                          hri_usbdevice_descriptor_addr_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                          hri_usbdevice_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                           hri_usbdevice_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_addr_reg_t hri_usbdevicedescriptor_read_ADDR_reg(const void *const hw,
                                                                                        uint8_t submodule_index)
{
	return ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].ADDR.reg;
}

static inline void hri_usbdevicedescriptor_set_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg |= USB_DEVICE_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevicedescriptor_get_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_AUTO_ZLP) >> USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevicedescriptor_write_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index,
                                                                      bool value)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_AUTO_ZLP;
	tmp |= value << USB_DEVICE_PCKSIZE_AUTO_ZLP_Pos;
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_set_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg |= USB_DEVICE_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_get_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_BYTE_COUNT(mask)) >> USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                       hri_usbdevice_descriptor_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk;
	tmp |= USB_DEVICE_PCKSIZE_BYTE_COUNT(data);
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                       hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                        hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_read_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk) >> USB_DEVICE_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_set_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw,
                                                                            uint8_t           submodule_index,
                                                                            hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg
	    |= USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_get_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask)) >> USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void
hri_usbdevicedescriptor_write_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                           hri_usbdevice_descriptor_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk;
	tmp |= USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(data);
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void
hri_usbdevicedescriptor_clear_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                           hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg
	    &= ~USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void
hri_usbdevicedescriptor_toggle_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg
	    ^= USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_read_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Msk) >> USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_set_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                               hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg |= USB_DEVICE_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_get_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                            hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_SIZE(mask)) >> USB_DEVICE_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_usbdevice_descriptor_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_DEVICE_PCKSIZE_SIZE_Msk;
	tmp |= USB_DEVICE_PCKSIZE_SIZE(data);
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg &= ~USB_DEVICE_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg ^= USB_DEVICE_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_read_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_DEVICE_PCKSIZE_SIZE_Msk) >> USB_DEVICE_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_set_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                           hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t
hri_usbdevicedescriptor_get_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                        hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                             hri_usbdevice_descriptor_pcksize_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                             hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_usbdevice_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_pcksize_reg_t hri_usbdevicedescriptor_read_PCKSIZE_reg(const void *const hw,
                                                                                              uint8_t submodule_index)
{
	return ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].PCKSIZE.reg;
}

static inline void hri_usbdevicedescriptor_set_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg |= USB_DEVICE_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_extreg_reg_t
hri_usbdevicedescriptor_get_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                             hri_usbdevice_descriptor_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_SUBPID(mask)) >> USB_DEVICE_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_usbdevice_descriptor_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp &= ~USB_DEVICE_EXTREG_SUBPID_Msk;
	tmp |= USB_DEVICE_EXTREG_SUBPID(data);
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg &= ~USB_DEVICE_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg ^= USB_DEVICE_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_extreg_reg_t
hri_usbdevicedescriptor_read_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_SUBPID_Msk) >> USB_DEVICE_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_set_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg |= USB_DEVICE_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_extreg_reg_t
hri_usbdevicedescriptor_get_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                               hri_usbdevice_descriptor_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_VARIABLE(mask)) >> USB_DEVICE_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbdevice_descriptor_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp &= ~USB_DEVICE_EXTREG_VARIABLE_Msk;
	tmp |= USB_DEVICE_EXTREG_VARIABLE(data);
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg &= ~USB_DEVICE_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg ^= USB_DEVICE_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_extreg_reg_t
hri_usbdevicedescriptor_read_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_DEVICE_EXTREG_VARIABLE_Msk) >> USB_DEVICE_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbdevicedescriptor_set_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                          hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_extreg_reg_t
hri_usbdevicedescriptor_get_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                       hri_usbdevice_descriptor_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescriptor_write_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                            hri_usbdevice_descriptor_extreg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_clear_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                            hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevicedescriptor_toggle_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                             hri_usbdevice_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_extreg_reg_t hri_usbdevicedescriptor_read_EXTREG_reg(const void *const hw,
                                                                                            uint8_t submodule_index)
{
	return ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].EXTREG.reg;
}

static inline bool hri_usbdevicedescriptor_get_STATUS_BK_CRCERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg & USB_DEVICE_STATUS_BK_CRCERR)
	       >> USB_DEVICE_STATUS_BK_CRCERR_Pos;
}

static inline void hri_usbdevicedescriptor_clear_STATUS_BK_CRCERR_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg = USB_DEVICE_STATUS_BK_CRCERR;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevicedescriptor_get_STATUS_BK_ERRORFLOW_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg & USB_DEVICE_STATUS_BK_ERRORFLOW)
	       >> USB_DEVICE_STATUS_BK_ERRORFLOW_Pos;
}

static inline void hri_usbdevicedescriptor_clear_STATUS_BK_ERRORFLOW_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg = USB_DEVICE_STATUS_BK_ERRORFLOW;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_status_bk_reg_t
hri_usbdevicedescriptor_get_STATUS_BK_reg(const void *const hw, uint8_t submodule_index,
                                          hri_usbdevice_descriptor_status_bk_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevicedescriptor_clear_STATUS_BK_reg(const void *const hw, uint8_t submodule_index,
                                                               hri_usbdevice_descriptor_status_bk_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descriptor_status_bk_reg_t
hri_usbdevicedescriptor_read_STATUS_BK_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbDeviceDescriptor *)hw)->DeviceDescBank[submodule_index].STATUS_BK.reg;
}

static inline void hri_usbdevice_set_INTEN_SUSPEND_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_SUSPEND;
}

static inline bool hri_usbdevice_get_INTEN_SUSPEND_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_SUSPEND) >> USB_DEVICE_INTENSET_SUSPEND_Pos;
}

static inline void hri_usbdevice_write_INTEN_SUSPEND_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_SUSPEND;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_SUSPEND;
	}
}

static inline void hri_usbdevice_clear_INTEN_SUSPEND_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_SUSPEND;
}

static inline void hri_usbdevice_set_INTEN_MSOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_MSOF;
}

static inline bool hri_usbdevice_get_INTEN_MSOF_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_MSOF) >> USB_DEVICE_INTENSET_MSOF_Pos;
}

static inline void hri_usbdevice_write_INTEN_MSOF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_MSOF;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_MSOF;
	}
}

static inline void hri_usbdevice_clear_INTEN_MSOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_MSOF;
}

static inline void hri_usbdevice_set_INTEN_SOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_SOF;
}

static inline bool hri_usbdevice_get_INTEN_SOF_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_SOF) >> USB_DEVICE_INTENSET_SOF_Pos;
}

static inline void hri_usbdevice_write_INTEN_SOF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_SOF;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_SOF;
	}
}

static inline void hri_usbdevice_clear_INTEN_SOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_SOF;
}

static inline void hri_usbdevice_set_INTEN_EORST_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
}

static inline bool hri_usbdevice_get_INTEN_EORST_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_EORST) >> USB_DEVICE_INTENSET_EORST_Pos;
}

static inline void hri_usbdevice_write_INTEN_EORST_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_EORST;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
	}
}

static inline void hri_usbdevice_clear_INTEN_EORST_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_EORST;
}

static inline void hri_usbdevice_set_INTEN_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_WAKEUP;
}

static inline bool hri_usbdevice_get_INTEN_WAKEUP_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_WAKEUP) >> USB_DEVICE_INTENSET_WAKEUP_Pos;
}

static inline void hri_usbdevice_write_INTEN_WAKEUP_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_WAKEUP;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_WAKEUP;
	}
}

static inline void hri_usbdevice_clear_INTEN_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_WAKEUP;
}

static inline void hri_usbdevice_set_INTEN_EORSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORSM;
}

static inline bool hri_usbdevice_get_INTEN_EORSM_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_EORSM) >> USB_DEVICE_INTENSET_EORSM_Pos;
}

static inline void hri_usbdevice_write_INTEN_EORSM_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_EORSM;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORSM;
	}
}

static inline void hri_usbdevice_clear_INTEN_EORSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_EORSM;
}

static inline void hri_usbdevice_set_INTEN_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_UPRSM;
}

static inline bool hri_usbdevice_get_INTEN_UPRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_UPRSM) >> USB_DEVICE_INTENSET_UPRSM_Pos;
}

static inline void hri_usbdevice_write_INTEN_UPRSM_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_UPRSM;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_UPRSM;
	}
}

static inline void hri_usbdevice_clear_INTEN_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_UPRSM;
}

static inline void hri_usbdevice_set_INTEN_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_RAMACER;
}

static inline bool hri_usbdevice_get_INTEN_RAMACER_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_RAMACER) >> USB_DEVICE_INTENSET_RAMACER_Pos;
}

static inline void hri_usbdevice_write_INTEN_RAMACER_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_RAMACER;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_RAMACER;
	}
}

static inline void hri_usbdevice_clear_INTEN_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_RAMACER;
}

static inline void hri_usbdevice_set_INTEN_LPMNYET_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_LPMNYET;
}

static inline bool hri_usbdevice_get_INTEN_LPMNYET_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_LPMNYET) >> USB_DEVICE_INTENSET_LPMNYET_Pos;
}

static inline void hri_usbdevice_write_INTEN_LPMNYET_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_LPMNYET;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_LPMNYET;
	}
}

static inline void hri_usbdevice_clear_INTEN_LPMNYET_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_LPMNYET;
}

static inline void hri_usbdevice_set_INTEN_LPMSUSP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_LPMSUSP;
}

static inline bool hri_usbdevice_get_INTEN_LPMSUSP_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTENSET.reg & USB_DEVICE_INTENSET_LPMSUSP) >> USB_DEVICE_INTENSET_LPMSUSP_Pos;
}

static inline void hri_usbdevice_write_INTEN_LPMSUSP_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_LPMSUSP;
	} else {
		((Usb *)hw)->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_LPMSUSP;
	}
}

static inline void hri_usbdevice_clear_INTEN_LPMSUSP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = USB_DEVICE_INTENSET_LPMSUSP;
}

static inline void hri_usbdevice_set_INTEN_reg(const void *const hw, hri_usbdevice_intenset_reg_t mask)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = mask;
}

static inline hri_usbdevice_intenset_reg_t hri_usbdevice_get_INTEN_reg(const void *const            hw,
                                                                       hri_usbdevice_intenset_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_intenset_reg_t hri_usbdevice_read_INTEN_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.INTENSET.reg;
}

static inline void hri_usbdevice_write_INTEN_reg(const void *const hw, hri_usbdevice_intenset_reg_t data)
{
	((Usb *)hw)->DEVICE.INTENSET.reg = data;
	((Usb *)hw)->DEVICE.INTENCLR.reg = ~data;
}

static inline void hri_usbdevice_clear_INTEN_reg(const void *const hw, hri_usbdevice_intenset_reg_t mask)
{
	((Usb *)hw)->DEVICE.INTENCLR.reg = mask;
}

static inline bool hri_usbdevice_get_INTFLAG_SUSPEND_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_SUSPEND) >> USB_DEVICE_INTFLAG_SUSPEND_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_SUSPEND_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_SUSPEND;
}

static inline bool hri_usbdevice_get_INTFLAG_MSOF_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_MSOF) >> USB_DEVICE_INTFLAG_MSOF_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_MSOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_MSOF;
}

static inline bool hri_usbdevice_get_INTFLAG_SOF_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_SOF) >> USB_DEVICE_INTFLAG_SOF_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_SOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_SOF;
}

static inline bool hri_usbdevice_get_INTFLAG_EORST_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_EORST) >> USB_DEVICE_INTFLAG_EORST_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_EORST_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
}

static inline bool hri_usbdevice_get_INTFLAG_WAKEUP_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_WAKEUP) >> USB_DEVICE_INTFLAG_WAKEUP_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_WAKEUP;
}

static inline bool hri_usbdevice_get_INTFLAG_EORSM_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_EORSM) >> USB_DEVICE_INTFLAG_EORSM_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_EORSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORSM;
}

static inline bool hri_usbdevice_get_INTFLAG_UPRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_UPRSM) >> USB_DEVICE_INTFLAG_UPRSM_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_UPRSM;
}

static inline bool hri_usbdevice_get_INTFLAG_RAMACER_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_RAMACER) >> USB_DEVICE_INTFLAG_RAMACER_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_RAMACER;
}

static inline bool hri_usbdevice_get_INTFLAG_LPMNYET_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_LPMNYET) >> USB_DEVICE_INTFLAG_LPMNYET_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_LPMNYET_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_LPMNYET;
}

static inline bool hri_usbdevice_get_INTFLAG_LPMSUSP_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_LPMSUSP) >> USB_DEVICE_INTFLAG_LPMSUSP_Pos;
}

static inline void hri_usbdevice_clear_INTFLAG_LPMSUSP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_LPMSUSP;
}

static inline bool hri_usbdevice_get_interrupt_SUSPEND_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_SUSPEND) >> USB_DEVICE_INTFLAG_SUSPEND_Pos;
}

static inline void hri_usbdevice_clear_interrupt_SUSPEND_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_SUSPEND;
}

static inline bool hri_usbdevice_get_interrupt_MSOF_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_MSOF) >> USB_DEVICE_INTFLAG_MSOF_Pos;
}

static inline void hri_usbdevice_clear_interrupt_MSOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_MSOF;
}

static inline bool hri_usbdevice_get_interrupt_SOF_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_SOF) >> USB_DEVICE_INTFLAG_SOF_Pos;
}

static inline void hri_usbdevice_clear_interrupt_SOF_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_SOF;
}

static inline bool hri_usbdevice_get_interrupt_EORST_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_EORST) >> USB_DEVICE_INTFLAG_EORST_Pos;
}

static inline void hri_usbdevice_clear_interrupt_EORST_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
}

static inline bool hri_usbdevice_get_interrupt_WAKEUP_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_WAKEUP) >> USB_DEVICE_INTFLAG_WAKEUP_Pos;
}

static inline void hri_usbdevice_clear_interrupt_WAKEUP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_WAKEUP;
}

static inline bool hri_usbdevice_get_interrupt_EORSM_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_EORSM) >> USB_DEVICE_INTFLAG_EORSM_Pos;
}

static inline void hri_usbdevice_clear_interrupt_EORSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORSM;
}

static inline bool hri_usbdevice_get_interrupt_UPRSM_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_UPRSM) >> USB_DEVICE_INTFLAG_UPRSM_Pos;
}

static inline void hri_usbdevice_clear_interrupt_UPRSM_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_UPRSM;
}

static inline bool hri_usbdevice_get_interrupt_RAMACER_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_RAMACER) >> USB_DEVICE_INTFLAG_RAMACER_Pos;
}

static inline void hri_usbdevice_clear_interrupt_RAMACER_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_RAMACER;
}

static inline bool hri_usbdevice_get_interrupt_LPMNYET_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_LPMNYET) >> USB_DEVICE_INTFLAG_LPMNYET_Pos;
}

static inline void hri_usbdevice_clear_interrupt_LPMNYET_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_LPMNYET;
}

static inline bool hri_usbdevice_get_interrupt_LPMSUSP_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.INTFLAG.reg & USB_DEVICE_INTFLAG_LPMSUSP) >> USB_DEVICE_INTFLAG_LPMSUSP_Pos;
}

static inline void hri_usbdevice_clear_interrupt_LPMSUSP_bit(const void *const hw)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_LPMSUSP;
}

static inline hri_usbdevice_intflag_reg_t hri_usbdevice_get_INTFLAG_reg(const void *const           hw,
                                                                        hri_usbdevice_intflag_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_intflag_reg_t hri_usbdevice_read_INTFLAG_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.INTFLAG.reg;
}

static inline void hri_usbdevice_clear_INTFLAG_reg(const void *const hw, hri_usbdevice_intflag_reg_t mask)
{
	((Usb *)hw)->DEVICE.INTFLAG.reg = mask;
}

static inline void hri_usbdevice_set_CTRLA_SWRST_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST);
	((Usb *)hw)->DEVICE.CTRLA.reg |= USB_CTRLA_SWRST;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST);
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_SWRST) >> USB_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_set_CTRLA_ENABLE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	((Usb *)hw)->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_ENABLE) >> USB_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp &= ~USB_CTRLA_ENABLE;
	tmp |= value << USB_CTRLA_ENABLE_Pos;
	((Usb *)hw)->DEVICE.CTRLA.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	((Usb *)hw)->DEVICE.CTRLA.reg &= ~USB_CTRLA_ENABLE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_SWRST | USB_SYNCBUSY_ENABLE);
	((Usb *)hw)->DEVICE.CTRLA.reg ^= USB_CTRLA_ENABLE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->DEVICE.CTRLA.reg |= USB_CTRLA_RUNSTDBY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_RUNSTDBY) >> USB_CTRLA_RUNSTDBY_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLA_RUNSTDBY_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp &= ~USB_CTRLA_RUNSTDBY;
	tmp |= value << USB_CTRLA_RUNSTDBY_Pos;
	((Usb *)hw)->DEVICE.CTRLA.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->DEVICE.CTRLA.reg &= ~USB_CTRLA_RUNSTDBY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLA_RUNSTDBY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->DEVICE.CTRLA.reg ^= USB_CTRLA_RUNSTDBY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLA_MODE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->DEVICE.CTRLA.reg |= USB_CTRLA_MODE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLA_MODE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp = (tmp & USB_CTRLA_MODE) >> USB_CTRLA_MODE_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLA_MODE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp &= ~USB_CTRLA_MODE;
	tmp |= value << USB_CTRLA_MODE_Pos;
	((Usb *)hw)->DEVICE.CTRLA.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLA_MODE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->DEVICE.CTRLA.reg &= ~USB_CTRLA_MODE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLA_MODE_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	hri_usbdevice_wait_for_sync(hw, USB_SYNCBUSY_MASK);
	((Usb *)hw)->DEVICE.CTRLA.reg ^= USB_CTRLA_MODE;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLA_reg(const void *const hw, hri_usbdevice_ctrla_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLA.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrla_reg_t hri_usbdevice_get_CTRLA_reg(const void *const         hw,
                                                                    hri_usbdevice_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_CTRLA_reg(const void *const hw, hri_usbdevice_ctrla_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLA.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLA_reg(const void *const hw, hri_usbdevice_ctrla_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLA.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLA_reg(const void *const hw, hri_usbdevice_ctrla_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLA.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrla_reg_t hri_usbdevice_read_CTRLA_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.CTRLA.reg;
}

static inline void hri_usbdevice_set_QOSCTRL_CQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg |= USB_QOSCTRL_CQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_qosctrl_reg_t hri_usbdevice_get_QOSCTRL_CQOS_bf(const void *const           hw,
                                                                            hri_usbdevice_qosctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_CQOS(mask)) >> USB_QOSCTRL_CQOS_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_QOSCTRL_CQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp &= ~USB_QOSCTRL_CQOS_Msk;
	tmp |= USB_QOSCTRL_CQOS(data);
	((Usb *)hw)->DEVICE.QOSCTRL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_QOSCTRL_CQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg &= ~USB_QOSCTRL_CQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_QOSCTRL_CQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg ^= USB_QOSCTRL_CQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_qosctrl_reg_t hri_usbdevice_read_QOSCTRL_CQOS_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_CQOS_Msk) >> USB_QOSCTRL_CQOS_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_QOSCTRL_DQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg |= USB_QOSCTRL_DQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_qosctrl_reg_t hri_usbdevice_get_QOSCTRL_DQOS_bf(const void *const           hw,
                                                                            hri_usbdevice_qosctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_DQOS(mask)) >> USB_QOSCTRL_DQOS_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_QOSCTRL_DQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp &= ~USB_QOSCTRL_DQOS_Msk;
	tmp |= USB_QOSCTRL_DQOS(data);
	((Usb *)hw)->DEVICE.QOSCTRL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_QOSCTRL_DQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg &= ~USB_QOSCTRL_DQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_QOSCTRL_DQOS_bf(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg ^= USB_QOSCTRL_DQOS(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_qosctrl_reg_t hri_usbdevice_read_QOSCTRL_DQOS_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp = (tmp & USB_QOSCTRL_DQOS_Msk) >> USB_QOSCTRL_DQOS_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_QOSCTRL_reg(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_qosctrl_reg_t hri_usbdevice_get_QOSCTRL_reg(const void *const           hw,
                                                                        hri_usbdevice_qosctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.QOSCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_QOSCTRL_reg(const void *const hw, hri_usbdevice_qosctrl_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_QOSCTRL_reg(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_QOSCTRL_reg(const void *const hw, hri_usbdevice_qosctrl_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.QOSCTRL.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_qosctrl_reg_t hri_usbdevice_read_QOSCTRL_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.QOSCTRL.reg;
}

static inline void hri_usbdevice_set_CTRLB_DETACH_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_DETACH;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_DETACH_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_DETACH) >> USB_DEVICE_CTRLB_DETACH_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_DETACH_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_DETACH;
	tmp |= value << USB_DEVICE_CTRLB_DETACH_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_DETACH_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_DETACH;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_DETACH_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_DETACH;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_UPRSM_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_UPRSM;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_UPRSM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_UPRSM) >> USB_DEVICE_CTRLB_UPRSM_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_UPRSM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_UPRSM;
	tmp |= value << USB_DEVICE_CTRLB_UPRSM_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_UPRSM_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_UPRSM;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_UPRSM_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_UPRSM;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_NREPLY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_NREPLY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_NREPLY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_NREPLY) >> USB_DEVICE_CTRLB_NREPLY_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_NREPLY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_NREPLY;
	tmp |= value << USB_DEVICE_CTRLB_NREPLY_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_NREPLY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_NREPLY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_NREPLY_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_NREPLY;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_TSTJ_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_TSTJ;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_TSTJ_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_TSTJ) >> USB_DEVICE_CTRLB_TSTJ_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_TSTJ_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_TSTJ;
	tmp |= value << USB_DEVICE_CTRLB_TSTJ_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_TSTJ_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_TSTJ;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_TSTJ_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_TSTJ;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_TSTK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_TSTK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_TSTK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_TSTK) >> USB_DEVICE_CTRLB_TSTK_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_TSTK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_TSTK;
	tmp |= value << USB_DEVICE_CTRLB_TSTK_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_TSTK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_TSTK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_TSTK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_TSTK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_TSTPCKT_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_TSTPCKT;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_TSTPCKT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_TSTPCKT) >> USB_DEVICE_CTRLB_TSTPCKT_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_TSTPCKT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_TSTPCKT;
	tmp |= value << USB_DEVICE_CTRLB_TSTPCKT_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_TSTPCKT_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_TSTPCKT;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_TSTPCKT_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_TSTPCKT;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_OPMODE2_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_OPMODE2;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_OPMODE2_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_OPMODE2) >> USB_DEVICE_CTRLB_OPMODE2_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_OPMODE2_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_OPMODE2;
	tmp |= value << USB_DEVICE_CTRLB_OPMODE2_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_OPMODE2_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_OPMODE2;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_OPMODE2_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_OPMODE2;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_GNAK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_GNAK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_CTRLB_GNAK_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_GNAK) >> USB_DEVICE_CTRLB_GNAK_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_CTRLB_GNAK_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_GNAK;
	tmp |= value << USB_DEVICE_CTRLB_GNAK_Pos;
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_GNAK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_GNAK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_GNAK_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_GNAK;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_CTRLB_SPDCONF_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_SPDCONF(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrlb_reg_t hri_usbdevice_get_CTRLB_SPDCONF_bf(const void *const         hw,
                                                                           hri_usbdevice_ctrlb_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_SPDCONF(mask)) >> USB_DEVICE_CTRLB_SPDCONF_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_CTRLB_SPDCONF_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_SPDCONF_Msk;
	tmp |= USB_DEVICE_CTRLB_SPDCONF(data);
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_SPDCONF_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_SPDCONF(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_SPDCONF_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_SPDCONF(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrlb_reg_t hri_usbdevice_read_CTRLB_SPDCONF_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_SPDCONF_Msk) >> USB_DEVICE_CTRLB_SPDCONF_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_CTRLB_LPMHDSK_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= USB_DEVICE_CTRLB_LPMHDSK(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrlb_reg_t hri_usbdevice_get_CTRLB_LPMHDSK_bf(const void *const         hw,
                                                                           hri_usbdevice_ctrlb_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_LPMHDSK(mask)) >> USB_DEVICE_CTRLB_LPMHDSK_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_CTRLB_LPMHDSK_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= ~USB_DEVICE_CTRLB_LPMHDSK_Msk;
	tmp |= USB_DEVICE_CTRLB_LPMHDSK(data);
	((Usb *)hw)->DEVICE.CTRLB.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_LPMHDSK_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~USB_DEVICE_CTRLB_LPMHDSK(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_LPMHDSK_bf(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= USB_DEVICE_CTRLB_LPMHDSK(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrlb_reg_t hri_usbdevice_read_CTRLB_LPMHDSK_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp = (tmp & USB_DEVICE_CTRLB_LPMHDSK_Msk) >> USB_DEVICE_CTRLB_LPMHDSK_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_CTRLB_reg(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrlb_reg_t hri_usbdevice_get_CTRLB_reg(const void *const         hw,
                                                                    hri_usbdevice_ctrlb_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.CTRLB.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_CTRLB_reg(const void *const hw, hri_usbdevice_ctrlb_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_CTRLB_reg(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_CTRLB_reg(const void *const hw, hri_usbdevice_ctrlb_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.CTRLB.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_ctrlb_reg_t hri_usbdevice_read_CTRLB_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.CTRLB.reg;
}

static inline void hri_usbdevice_set_DADD_ADDEN_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg |= USB_DEVICE_DADD_ADDEN;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_DADD_ADDEN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DADD.reg;
	tmp = (tmp & USB_DEVICE_DADD_ADDEN) >> USB_DEVICE_DADD_ADDEN_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_DADD_ADDEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.DADD.reg;
	tmp &= ~USB_DEVICE_DADD_ADDEN;
	tmp |= value << USB_DEVICE_DADD_ADDEN_Pos;
	((Usb *)hw)->DEVICE.DADD.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_DADD_ADDEN_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg &= ~USB_DEVICE_DADD_ADDEN;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_DADD_ADDEN_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg ^= USB_DEVICE_DADD_ADDEN;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_DADD_DADD_bf(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg |= USB_DEVICE_DADD_DADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_dadd_reg_t hri_usbdevice_get_DADD_DADD_bf(const void *const        hw,
                                                                      hri_usbdevice_dadd_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DADD.reg;
	tmp = (tmp & USB_DEVICE_DADD_DADD(mask)) >> USB_DEVICE_DADD_DADD_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_DADD_DADD_bf(const void *const hw, hri_usbdevice_dadd_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.DADD.reg;
	tmp &= ~USB_DEVICE_DADD_DADD_Msk;
	tmp |= USB_DEVICE_DADD_DADD(data);
	((Usb *)hw)->DEVICE.DADD.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_DADD_DADD_bf(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg &= ~USB_DEVICE_DADD_DADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_DADD_DADD_bf(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg ^= USB_DEVICE_DADD_DADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_dadd_reg_t hri_usbdevice_read_DADD_DADD_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DADD.reg;
	tmp = (tmp & USB_DEVICE_DADD_DADD_Msk) >> USB_DEVICE_DADD_DADD_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_DADD_reg(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_dadd_reg_t hri_usbdevice_get_DADD_reg(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DADD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_DADD_reg(const void *const hw, hri_usbdevice_dadd_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_DADD_reg(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_DADD_reg(const void *const hw, hri_usbdevice_dadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DADD.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_dadd_reg_t hri_usbdevice_read_DADD_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.DADD.reg;
}

static inline void hri_usbdevice_set_DESCADD_DESCADD_bf(const void *const hw, hri_usbdevice_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg |= USB_DESCADD_DESCADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descadd_reg_t hri_usbdevice_get_DESCADD_DESCADD_bf(const void *const           hw,
                                                                               hri_usbdevice_descadd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DESCADD.reg;
	tmp = (tmp & USB_DESCADD_DESCADD(mask)) >> USB_DESCADD_DESCADD_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_DESCADD_DESCADD_bf(const void *const hw, hri_usbdevice_descadd_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.DESCADD.reg;
	tmp &= ~USB_DESCADD_DESCADD_Msk;
	tmp |= USB_DESCADD_DESCADD(data);
	((Usb *)hw)->DEVICE.DESCADD.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_DESCADD_DESCADD_bf(const void *const hw, hri_usbdevice_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg &= ~USB_DESCADD_DESCADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_DESCADD_DESCADD_bf(const void *const hw, hri_usbdevice_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg ^= USB_DESCADD_DESCADD(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descadd_reg_t hri_usbdevice_read_DESCADD_DESCADD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DESCADD.reg;
	tmp = (tmp & USB_DESCADD_DESCADD_Msk) >> USB_DESCADD_DESCADD_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_DESCADD_reg(const void *const hw, hri_usbdevice_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descadd_reg_t hri_usbdevice_get_DESCADD_reg(const void *const           hw,
                                                                        hri_usbdevice_descadd_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DESCADD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_DESCADD_reg(const void *const hw, hri_usbdevice_descadd_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_DESCADD_reg(const void *const hw, hri_usbdevice_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_DESCADD_reg(const void *const hw, hri_usbdevice_descadd_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DESCADD.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_descadd_reg_t hri_usbdevice_read_DESCADD_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.DESCADD.reg;
}

static inline void hri_usbdevice_set_PADCAL_TRANSP_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg |= USB_PADCAL_TRANSP(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_get_PADCAL_TRANSP_bf(const void *const          hw,
                                                                            hri_usbdevice_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSP(mask)) >> USB_PADCAL_TRANSP_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_PADCAL_TRANSP_bf(const void *const hw, hri_usbdevice_padcal_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp &= ~USB_PADCAL_TRANSP_Msk;
	tmp |= USB_PADCAL_TRANSP(data);
	((Usb *)hw)->DEVICE.PADCAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_PADCAL_TRANSP_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg &= ~USB_PADCAL_TRANSP(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_PADCAL_TRANSP_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg ^= USB_PADCAL_TRANSP(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_read_PADCAL_TRANSP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSP_Msk) >> USB_PADCAL_TRANSP_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_PADCAL_TRANSN_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg |= USB_PADCAL_TRANSN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_get_PADCAL_TRANSN_bf(const void *const          hw,
                                                                            hri_usbdevice_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSN(mask)) >> USB_PADCAL_TRANSN_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_PADCAL_TRANSN_bf(const void *const hw, hri_usbdevice_padcal_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp &= ~USB_PADCAL_TRANSN_Msk;
	tmp |= USB_PADCAL_TRANSN(data);
	((Usb *)hw)->DEVICE.PADCAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_PADCAL_TRANSN_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg &= ~USB_PADCAL_TRANSN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_PADCAL_TRANSN_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg ^= USB_PADCAL_TRANSN(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_read_PADCAL_TRANSN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRANSN_Msk) >> USB_PADCAL_TRANSN_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_PADCAL_TRIM_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg |= USB_PADCAL_TRIM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_get_PADCAL_TRIM_bf(const void *const          hw,
                                                                          hri_usbdevice_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRIM(mask)) >> USB_PADCAL_TRIM_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_PADCAL_TRIM_bf(const void *const hw, hri_usbdevice_padcal_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp &= ~USB_PADCAL_TRIM_Msk;
	tmp |= USB_PADCAL_TRIM(data);
	((Usb *)hw)->DEVICE.PADCAL.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_PADCAL_TRIM_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg &= ~USB_PADCAL_TRIM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_PADCAL_TRIM_bf(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg ^= USB_PADCAL_TRIM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_read_PADCAL_TRIM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp = (tmp & USB_PADCAL_TRIM_Msk) >> USB_PADCAL_TRIM_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_PADCAL_reg(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_get_PADCAL_reg(const void *const          hw,
                                                                      hri_usbdevice_padcal_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.PADCAL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_PADCAL_reg(const void *const hw, hri_usbdevice_padcal_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_PADCAL_reg(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_PADCAL_reg(const void *const hw, hri_usbdevice_padcal_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.PADCAL.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_padcal_reg_t hri_usbdevice_read_PADCAL_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.PADCAL.reg;
}

static inline bool hri_usbdevice_get_SYNCBUSY_SWRST_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.SYNCBUSY.reg & USB_SYNCBUSY_SWRST) >> USB_SYNCBUSY_SWRST_Pos;
}

static inline bool hri_usbdevice_get_SYNCBUSY_ENABLE_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.SYNCBUSY.reg & USB_SYNCBUSY_ENABLE) >> USB_SYNCBUSY_ENABLE_Pos;
}

static inline hri_usbdevice_syncbusy_reg_t hri_usbdevice_get_SYNCBUSY_reg(const void *const            hw,
                                                                          hri_usbdevice_syncbusy_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.SYNCBUSY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_syncbusy_reg_t hri_usbdevice_read_SYNCBUSY_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.SYNCBUSY.reg;
}

static inline hri_usbdevice_status_reg_t hri_usbdevice_get_STATUS_SPEED_bf(const void *const          hw,
                                                                           hri_usbdevice_status_reg_t mask)
{
	return (((Usb *)hw)->DEVICE.STATUS.reg & USB_DEVICE_STATUS_SPEED(mask)) >> USB_DEVICE_STATUS_SPEED_Pos;
}

static inline hri_usbdevice_status_reg_t hri_usbdevice_read_STATUS_SPEED_bf(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.STATUS.reg & USB_DEVICE_STATUS_SPEED_Msk) >> USB_DEVICE_STATUS_SPEED_Pos;
}

static inline hri_usbdevice_status_reg_t hri_usbdevice_get_STATUS_LINESTATE_bf(const void *const          hw,
                                                                               hri_usbdevice_status_reg_t mask)
{
	return (((Usb *)hw)->DEVICE.STATUS.reg & USB_DEVICE_STATUS_LINESTATE(mask)) >> USB_DEVICE_STATUS_LINESTATE_Pos;
}

static inline hri_usbdevice_status_reg_t hri_usbdevice_read_STATUS_LINESTATE_bf(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.STATUS.reg & USB_DEVICE_STATUS_LINESTATE_Msk) >> USB_DEVICE_STATUS_LINESTATE_Pos;
}

static inline hri_usbdevice_status_reg_t hri_usbdevice_get_STATUS_reg(const void *const          hw,
                                                                      hri_usbdevice_status_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_status_reg_t hri_usbdevice_read_STATUS_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.STATUS.reg;
}

static inline hri_usbdevice_fsmstatus_reg_t hri_usbdevice_get_FSMSTATUS_FSMSTATE_bf(const void *const             hw,
                                                                                    hri_usbdevice_fsmstatus_reg_t mask)
{
	return (((Usb *)hw)->DEVICE.FSMSTATUS.reg & USB_FSMSTATUS_FSMSTATE(mask)) >> USB_FSMSTATUS_FSMSTATE_Pos;
}

static inline hri_usbdevice_fsmstatus_reg_t hri_usbdevice_read_FSMSTATUS_FSMSTATE_bf(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.FSMSTATUS.reg & USB_FSMSTATUS_FSMSTATE_Msk) >> USB_FSMSTATUS_FSMSTATE_Pos;
}

static inline hri_usbdevice_fsmstatus_reg_t hri_usbdevice_get_FSMSTATUS_reg(const void *const             hw,
                                                                            hri_usbdevice_fsmstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.FSMSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_fsmstatus_reg_t hri_usbdevice_read_FSMSTATUS_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.FSMSTATUS.reg;
}

static inline bool hri_usbdevice_get_FNUM_FNCERR_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.FNUM.reg & USB_DEVICE_FNUM_FNCERR) >> USB_DEVICE_FNUM_FNCERR_Pos;
}

static inline hri_usbdevice_fnum_reg_t hri_usbdevice_get_FNUM_MFNUM_bf(const void *const        hw,
                                                                       hri_usbdevice_fnum_reg_t mask)
{
	return (((Usb *)hw)->DEVICE.FNUM.reg & USB_DEVICE_FNUM_MFNUM(mask)) >> USB_DEVICE_FNUM_MFNUM_Pos;
}

static inline hri_usbdevice_fnum_reg_t hri_usbdevice_read_FNUM_MFNUM_bf(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.FNUM.reg & USB_DEVICE_FNUM_MFNUM_Msk) >> USB_DEVICE_FNUM_MFNUM_Pos;
}

static inline hri_usbdevice_fnum_reg_t hri_usbdevice_get_FNUM_FNUM_bf(const void *const        hw,
                                                                      hri_usbdevice_fnum_reg_t mask)
{
	return (((Usb *)hw)->DEVICE.FNUM.reg & USB_DEVICE_FNUM_FNUM(mask)) >> USB_DEVICE_FNUM_FNUM_Pos;
}

static inline hri_usbdevice_fnum_reg_t hri_usbdevice_read_FNUM_FNUM_bf(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.FNUM.reg & USB_DEVICE_FNUM_FNUM_Msk) >> USB_DEVICE_FNUM_FNUM_Pos;
}

static inline hri_usbdevice_fnum_reg_t hri_usbdevice_get_FNUM_reg(const void *const hw, hri_usbdevice_fnum_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.FNUM.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_fnum_reg_t hri_usbdevice_read_FNUM_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.FNUM.reg;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT0_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT0) >> USB_DEVICE_EPINTSMRY_EPINT0_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT1_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT1) >> USB_DEVICE_EPINTSMRY_EPINT1_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT2_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT2) >> USB_DEVICE_EPINTSMRY_EPINT2_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT3_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT3) >> USB_DEVICE_EPINTSMRY_EPINT3_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT4_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT4) >> USB_DEVICE_EPINTSMRY_EPINT4_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT5_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT5) >> USB_DEVICE_EPINTSMRY_EPINT5_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT6_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT6) >> USB_DEVICE_EPINTSMRY_EPINT6_Pos;
}

static inline bool hri_usbdevice_get_EPINTSMRY_EPINT7_bit(const void *const hw)
{
	return (((Usb *)hw)->DEVICE.EPINTSMRY.reg & USB_DEVICE_EPINTSMRY_EPINT7) >> USB_DEVICE_EPINTSMRY_EPINT7_Pos;
}

static inline hri_usbdevice_epintsmry_reg_t hri_usbdevice_get_EPINTSMRY_reg(const void *const             hw,
                                                                            hri_usbdevice_epintsmry_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Usb *)hw)->DEVICE.EPINTSMRY.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_epintsmry_reg_t hri_usbdevice_read_EPINTSMRY_reg(const void *const hw)
{
	return ((Usb *)hw)->DEVICE.EPINTSMRY.reg;
}

static inline void hri_usbendpoint_set_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
}

static inline bool hri_usbendpoint_get_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_DTGLOUT)
	       >> USB_DEVICE_EPSTATUS_DTGLOUT_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
}

static inline void hri_usbendpoint_set_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLIN;
}

static inline bool hri_usbendpoint_get_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_DTGLIN)
	       >> USB_DEVICE_EPSTATUS_DTGLIN_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLIN;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLIN;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLIN;
}

static inline void hri_usbendpoint_set_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_CURBK;
}

static inline bool hri_usbendpoint_get_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_CURBK)
	       >> USB_DEVICE_EPSTATUS_CURBK_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_CURBK;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_CURBK;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_CURBK;
}

static inline void hri_usbendpoint_set_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
}

static inline bool hri_usbendpoint_get_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_STALLRQ0)
	       >> USB_DEVICE_EPSTATUS_STALLRQ0_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index,
                                                               bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
}

static inline void hri_usbendpoint_set_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
}

static inline bool hri_usbendpoint_get_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_STALLRQ1)
	       >> USB_DEVICE_EPSTATUS_STALLRQ1_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index,
                                                               bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
}

static inline void hri_usbendpoint_set_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK0RDY;
}

static inline bool hri_usbendpoint_get_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_BK0RDY)
	       >> USB_DEVICE_EPSTATUS_BK0RDY_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK0RDY;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK0RDY;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK0RDY;
}

static inline void hri_usbendpoint_set_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK1RDY;
}

static inline bool hri_usbendpoint_get_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_BK1RDY)
	       >> USB_DEVICE_EPSTATUS_BK1RDY_Pos;
}

static inline void hri_usbendpoint_write_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK1RDY;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK1RDY;
	}
}

static inline void hri_usbendpoint_clear_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK1RDY;
}

static inline void hri_usbendpoint_set_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_usbendpoint_epstatus_reg_t mask)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = mask;
}

static inline hri_usbendpoint_epstatus_reg_t
hri_usbendpoint_get_EPSTATUS_reg(const void *const hw, uint8_t submodule_index, hri_usbendpoint_epstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbendpoint_epstatus_reg_t hri_usbendpoint_read_EPSTATUS_reg(const void *const hw,
                                                                               uint8_t           submodule_index)
{
	return ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUS.reg;
}

static inline void hri_usbendpoint_write_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                      hri_usbendpoint_epstatus_reg_t data)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSSET.reg = data;
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = ~data;
}

static inline void hri_usbendpoint_clear_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                      hri_usbendpoint_epstatus_reg_t mask)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = mask;
}

static inline void hri_usbendpoint_set_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0;
}

static inline bool hri_usbendpoint_get_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRCPT0)
	       >> USB_DEVICE_EPINTENSET_TRCPT0_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT0;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT0;
}

static inline void hri_usbendpoint_set_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1;
}

static inline bool hri_usbendpoint_get_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRCPT1)
	       >> USB_DEVICE_EPINTENSET_TRCPT1_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT1;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT1;
}

static inline void hri_usbendpoint_set_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
}

static inline bool hri_usbendpoint_get_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRFAIL0)
	       >> USB_DEVICE_EPINTENSET_TRFAIL0_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
}

static inline void hri_usbendpoint_set_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
}

static inline bool hri_usbendpoint_get_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRFAIL1)
	       >> USB_DEVICE_EPINTENSET_TRFAIL1_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
}

static inline void hri_usbendpoint_set_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
}

static inline bool hri_usbendpoint_get_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_RXSTP)
	       >> USB_DEVICE_EPINTENSET_RXSTP_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_RXSTP;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_RXSTP;
}

static inline void hri_usbendpoint_set_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL0;
}

static inline bool hri_usbendpoint_get_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_STALL0)
	       >> USB_DEVICE_EPINTENSET_STALL0_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL0;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL0;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL0;
}

static inline void hri_usbendpoint_set_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL1;
}

static inline bool hri_usbendpoint_get_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_STALL1)
	       >> USB_DEVICE_EPINTENSET_STALL1_Pos;
}

static inline void hri_usbendpoint_write_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL1;
	} else {
		((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL1;
	}
}

static inline void hri_usbendpoint_clear_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL1;
}

static inline void hri_usbendpoint_set_EPINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbendpoint_epintenset_reg_t mask)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = mask;
}

static inline hri_usbendpoint_epintenset_reg_t
hri_usbendpoint_get_EPINTEN_reg(const void *const hw, uint8_t submodule_index, hri_usbendpoint_epintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbendpoint_epintenset_reg_t hri_usbendpoint_read_EPINTEN_reg(const void *const hw,
                                                                                uint8_t           submodule_index)
{
	return ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg;
}

static inline void hri_usbendpoint_write_EPINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                     hri_usbendpoint_epintenset_reg_t data)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENSET.reg = data;
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = ~data;
}

static inline void hri_usbendpoint_clear_EPINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                     hri_usbendpoint_epintenset_reg_t mask)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTENCLR.reg = mask;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT0)
	       >> USB_DEVICE_EPINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT1)
	       >> USB_DEVICE_EPINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL0)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL0_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL0;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL1)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL1_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL1;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_RXSTP)
	       >> USB_DEVICE_EPINTFLAG_RXSTP_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL0)
	       >> USB_DEVICE_EPINTFLAG_STALL0_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
}

static inline bool hri_usbendpoint_get_EPINTFLAG_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL1)
	       >> USB_DEVICE_EPINTFLAG_STALL1_Pos;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
}

static inline bool hri_usbendpoint_get_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT0)
	       >> USB_DEVICE_EPINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
}

static inline bool hri_usbendpoint_get_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT1)
	       >> USB_DEVICE_EPINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
}

static inline bool hri_usbendpoint_get_interrupt_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL0)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL0_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL0;
}

static inline bool hri_usbendpoint_get_interrupt_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL1)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL1_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL1;
}

static inline bool hri_usbendpoint_get_interrupt_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_RXSTP)
	       >> USB_DEVICE_EPINTFLAG_RXSTP_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}

static inline bool hri_usbendpoint_get_interrupt_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL0)
	       >> USB_DEVICE_EPINTFLAG_STALL0_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
}

static inline bool hri_usbendpoint_get_interrupt_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL1)
	       >> USB_DEVICE_EPINTFLAG_STALL1_Pos;
}

static inline void hri_usbendpoint_clear_interrupt_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
}

static inline hri_usbendpoint_epintflag_reg_t
hri_usbendpoint_get_EPINTFLAG_reg(const void *const hw, uint8_t submodule_index, hri_usbendpoint_epintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbendpoint_epintflag_reg_t hri_usbendpoint_read_EPINTFLAG_reg(const void *const hw,
                                                                                 uint8_t           submodule_index)
{
	return ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg;
}

static inline void hri_usbendpoint_clear_EPINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                       hri_usbendpoint_epintflag_reg_t mask)
{
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPINTFLAG.reg = mask;
}

static inline void hri_usbendpoint_set_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg |= USB_DEVICE_EPCFG_NYETDIS;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbendpoint_get_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_NYETDIS) >> USB_DEVICE_EPCFG_NYETDIS_Pos;
	return (bool)tmp;
}

static inline void hri_usbendpoint_write_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= ~USB_DEVICE_EPCFG_NYETDIS;
	tmp |= value << USB_DEVICE_EPCFG_NYETDIS_Pos;
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_clear_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg &= ~USB_DEVICE_EPCFG_NYETDIS;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_toggle_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg ^= USB_DEVICE_EPCFG_NYETDIS;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_set_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg |= USB_DEVICE_EPCFG_EPTYPE0(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbendpoint_epcfg_reg_t
hri_usbendpoint_get_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index, hri_usbendpoint_epcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE0(mask)) >> USB_DEVICE_EPCFG_EPTYPE0_Pos;
	return tmp;
}

static inline void hri_usbendpoint_write_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_usbendpoint_epcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= ~USB_DEVICE_EPCFG_EPTYPE0_Msk;
	tmp |= USB_DEVICE_EPCFG_EPTYPE0(data);
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_clear_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg &= ~USB_DEVICE_EPCFG_EPTYPE0(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_toggle_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                           hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg ^= USB_DEVICE_EPCFG_EPTYPE0(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbendpoint_epcfg_reg_t hri_usbendpoint_read_EPCFG_EPTYPE0_bf(const void *const hw,
                                                                                uint8_t           submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE0_Msk) >> USB_DEVICE_EPCFG_EPTYPE0_Pos;
	return tmp;
}

static inline void hri_usbendpoint_set_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg |= USB_DEVICE_EPCFG_EPTYPE1(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbendpoint_epcfg_reg_t
hri_usbendpoint_get_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index, hri_usbendpoint_epcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE1(mask)) >> USB_DEVICE_EPCFG_EPTYPE1_Pos;
	return tmp;
}

static inline void hri_usbendpoint_write_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_usbendpoint_epcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= ~USB_DEVICE_EPCFG_EPTYPE1_Msk;
	tmp |= USB_DEVICE_EPCFG_EPTYPE1(data);
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_clear_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg &= ~USB_DEVICE_EPCFG_EPTYPE1(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_toggle_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                           hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg ^= USB_DEVICE_EPCFG_EPTYPE1(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbendpoint_epcfg_reg_t hri_usbendpoint_read_EPCFG_EPTYPE1_bf(const void *const hw,
                                                                                uint8_t           submodule_index)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE1_Msk) >> USB_DEVICE_EPCFG_EPTYPE1_Pos;
	return tmp;
}

static inline void hri_usbendpoint_set_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbendpoint_epcfg_reg_t hri_usbendpoint_get_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                                        hri_usbendpoint_epcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbendpoint_write_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbendpoint_epcfg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_clear_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbendpoint_toggle_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_usbendpoint_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbendpoint_epcfg_reg_t hri_usbendpoint_read_EPCFG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbDevice *)hw)->DeviceEndpoint[submodule_index].EPCFG.reg;
}

static inline void hri_usbdevice_set_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
}

static inline bool hri_usbdevice_get_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_DTGLOUT)
	       >> USB_DEVICE_EPSTATUS_DTGLOUT_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_DTGLOUT_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLOUT;
}

static inline void hri_usbdevice_set_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLIN;
}

static inline bool hri_usbdevice_get_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_DTGLIN)
	       >> USB_DEVICE_EPSTATUS_DTGLIN_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLIN;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_DTGLIN;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_DTGLIN_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_DTGLIN;
}

static inline void hri_usbdevice_set_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_CURBK;
}

static inline bool hri_usbdevice_get_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_CURBK)
	       >> USB_DEVICE_EPSTATUS_CURBK_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_CURBK;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_CURBK;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_CURBK_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_CURBK;
}

static inline void hri_usbdevice_set_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
}

static inline bool hri_usbdevice_get_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_STALLRQ0)
	       >> USB_DEVICE_EPSTATUS_STALLRQ0_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_STALLRQ0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ0;
}

static inline void hri_usbdevice_set_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
}

static inline bool hri_usbdevice_get_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_STALLRQ1)
	       >> USB_DEVICE_EPSTATUS_STALLRQ1_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_STALLRQ1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_STALLRQ1;
}

static inline void hri_usbdevice_set_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK0RDY;
}

static inline bool hri_usbdevice_get_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_BK0RDY)
	       >> USB_DEVICE_EPSTATUS_BK0RDY_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK0RDY;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK0RDY;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_BK0RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK0RDY;
}

static inline void hri_usbdevice_set_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK1RDY;
}

static inline bool hri_usbdevice_get_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg & USB_DEVICE_EPSTATUS_BK1RDY)
	       >> USB_DEVICE_EPSTATUS_BK1RDY_Pos;
}

static inline void hri_usbdevice_write_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK1RDY;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = USB_DEVICE_EPSTATUS_BK1RDY;
	}
}

static inline void hri_usbdevice_clear_EPSTATUS_BK1RDY_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = USB_DEVICE_EPSTATUS_BK1RDY;
}

static inline void hri_usbdevice_set_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                  hri_usbdevice_epstatus_reg_t mask)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = mask;
}

static inline hri_usbdevice_epstatus_reg_t hri_usbdevice_get_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                                          hri_usbdevice_epstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_epstatus_reg_t hri_usbdevice_read_EPSTATUS_reg(const void *const hw,
                                                                           uint8_t           submodule_index)
{
	return ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUS.reg;
}

static inline void hri_usbdevice_write_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_usbdevice_epstatus_reg_t data)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSSET.reg = data;
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = ~data;
}

static inline void hri_usbdevice_clear_EPSTATUS_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_usbdevice_epstatus_reg_t mask)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPSTATUSCLR.reg = mask;
}

static inline void hri_usbdevice_set_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0;
}

static inline bool hri_usbdevice_get_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRCPT0)
	       >> USB_DEVICE_EPINTENSET_TRCPT0_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT0;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT0;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT0;
}

static inline void hri_usbdevice_set_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1;
}

static inline bool hri_usbdevice_get_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRCPT1)
	       >> USB_DEVICE_EPINTENSET_TRCPT1_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT1;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRCPT1;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRCPT1;
}

static inline void hri_usbdevice_set_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
}

static inline bool hri_usbdevice_get_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRFAIL0)
	       >> USB_DEVICE_EPINTENSET_TRFAIL0_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL0;
}

static inline void hri_usbdevice_set_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
}

static inline bool hri_usbdevice_get_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_TRFAIL1)
	       >> USB_DEVICE_EPINTENSET_TRFAIL1_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_TRFAIL1;
}

static inline void hri_usbdevice_set_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
}

static inline bool hri_usbdevice_get_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_RXSTP)
	       >> USB_DEVICE_EPINTENSET_RXSTP_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_RXSTP;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_RXSTP;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_RXSTP;
}

static inline void hri_usbdevice_set_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL0;
}

static inline bool hri_usbdevice_get_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_STALL0)
	       >> USB_DEVICE_EPINTENSET_STALL0_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL0;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL0;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL0;
}

static inline void hri_usbdevice_set_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL1;
}

static inline bool hri_usbdevice_get_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg & USB_DEVICE_EPINTENSET_STALL1)
	       >> USB_DEVICE_EPINTENSET_STALL1_Pos;
}

static inline void hri_usbdevice_write_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	if (value == 0x0) {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL1;
	} else {
		((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = USB_DEVICE_EPINTENSET_STALL1;
	}
}

static inline void hri_usbdevice_clear_EPINTEN_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = USB_DEVICE_EPINTENSET_STALL1;
}

static inline void hri_usbdevice_set_EPINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbdevice_epintenset_reg_t mask)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = mask;
}

static inline hri_usbdevice_epintenset_reg_t
hri_usbdevice_get_EPINTEN_reg(const void *const hw, uint8_t submodule_index, hri_usbdevice_epintenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_epintenset_reg_t hri_usbdevice_read_EPINTEN_reg(const void *const hw,
                                                                            uint8_t           submodule_index)
{
	return ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg;
}

static inline void hri_usbdevice_write_EPINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbdevice_epintenset_reg_t data)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENSET.reg = data;
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = ~data;
}

static inline void hri_usbdevice_clear_EPINTEN_reg(const void *const hw, uint8_t submodule_index,
                                                   hri_usbdevice_epintenset_reg_t mask)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTENCLR.reg = mask;
}

static inline bool hri_usbdevice_get_EPINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT0)
	       >> USB_DEVICE_EPINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
}

static inline bool hri_usbdevice_get_EPINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT1)
	       >> USB_DEVICE_EPINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
}

static inline bool hri_usbdevice_get_EPINTFLAG_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL0)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL0_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL0;
}

static inline bool hri_usbdevice_get_EPINTFLAG_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL1)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL1_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL1;
}

static inline bool hri_usbdevice_get_EPINTFLAG_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_RXSTP)
	       >> USB_DEVICE_EPINTFLAG_RXSTP_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}

static inline bool hri_usbdevice_get_EPINTFLAG_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL0)
	       >> USB_DEVICE_EPINTFLAG_STALL0_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
}

static inline bool hri_usbdevice_get_EPINTFLAG_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL1)
	       >> USB_DEVICE_EPINTFLAG_STALL1_Pos;
}

static inline void hri_usbdevice_clear_EPINTFLAG_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
}

static inline bool hri_usbdevice_get_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT0)
	       >> USB_DEVICE_EPINTFLAG_TRCPT0_Pos;
}

static inline void hri_usbdevice_clear_interrupt_TRCPT0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
}

static inline bool hri_usbdevice_get_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRCPT1)
	       >> USB_DEVICE_EPINTFLAG_TRCPT1_Pos;
}

static inline void hri_usbdevice_clear_interrupt_TRCPT1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
}

static inline bool hri_usbdevice_get_interrupt_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL0)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL0_Pos;
}

static inline void hri_usbdevice_clear_interrupt_TRFAIL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL0;
}

static inline bool hri_usbdevice_get_interrupt_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_TRFAIL1)
	       >> USB_DEVICE_EPINTFLAG_TRFAIL1_Pos;
}

static inline void hri_usbdevice_clear_interrupt_TRFAIL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRFAIL1;
}

static inline bool hri_usbdevice_get_interrupt_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_RXSTP)
	       >> USB_DEVICE_EPINTFLAG_RXSTP_Pos;
}

static inline void hri_usbdevice_clear_interrupt_RXSTP_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}

static inline bool hri_usbdevice_get_interrupt_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL0)
	       >> USB_DEVICE_EPINTFLAG_STALL0_Pos;
}

static inline void hri_usbdevice_clear_interrupt_STALL0_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
}

static inline bool hri_usbdevice_get_interrupt_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	return (((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg & USB_DEVICE_EPINTFLAG_STALL1)
	       >> USB_DEVICE_EPINTFLAG_STALL1_Pos;
}

static inline void hri_usbdevice_clear_interrupt_STALL1_bit(const void *const hw, uint8_t submodule_index)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
}

static inline hri_usbdevice_epintflag_reg_t
hri_usbdevice_get_EPINTFLAG_reg(const void *const hw, uint8_t submodule_index, hri_usbdevice_epintflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_usbdevice_epintflag_reg_t hri_usbdevice_read_EPINTFLAG_reg(const void *const hw,
                                                                             uint8_t           submodule_index)
{
	return ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg;
}

static inline void hri_usbdevice_clear_EPINTFLAG_reg(const void *const hw, uint8_t submodule_index,
                                                     hri_usbdevice_epintflag_reg_t mask)
{
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPINTFLAG.reg = mask;
}

static inline void hri_usbdevice_set_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg |= USB_DEVICE_EPCFG_NYETDIS;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbdevice_get_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_NYETDIS) >> USB_DEVICE_EPCFG_NYETDIS_Pos;
	return (bool)tmp;
}

static inline void hri_usbdevice_write_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= ~USB_DEVICE_EPCFG_NYETDIS;
	tmp |= value << USB_DEVICE_EPCFG_NYETDIS_Pos;
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg &= ~USB_DEVICE_EPCFG_NYETDIS;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_EPCFG_NYETDIS_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg ^= USB_DEVICE_EPCFG_NYETDIS;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_set_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg |= USB_DEVICE_EPCFG_EPTYPE0(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_epcfg_reg_t
hri_usbdevice_get_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index, hri_usbdevice_epcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE0(mask)) >> USB_DEVICE_EPCFG_EPTYPE0_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_usbdevice_epcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= ~USB_DEVICE_EPCFG_EPTYPE0_Msk;
	tmp |= USB_DEVICE_EPCFG_EPTYPE0(data);
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg &= ~USB_DEVICE_EPCFG_EPTYPE0(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_EPCFG_EPTYPE0_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg ^= USB_DEVICE_EPCFG_EPTYPE0(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_epcfg_reg_t hri_usbdevice_read_EPCFG_EPTYPE0_bf(const void *const hw,
                                                                            uint8_t           submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE0_Msk) >> USB_DEVICE_EPCFG_EPTYPE0_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg |= USB_DEVICE_EPCFG_EPTYPE1(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_epcfg_reg_t
hri_usbdevice_get_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index, hri_usbdevice_epcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE1(mask)) >> USB_DEVICE_EPCFG_EPTYPE1_Pos;
	return tmp;
}

static inline void hri_usbdevice_write_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_usbdevice_epcfg_reg_t data)
{
	uint8_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= ~USB_DEVICE_EPCFG_EPTYPE1_Msk;
	tmp |= USB_DEVICE_EPCFG_EPTYPE1(data);
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                        hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg &= ~USB_DEVICE_EPCFG_EPTYPE1(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_EPCFG_EPTYPE1_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg ^= USB_DEVICE_EPCFG_EPTYPE1(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_epcfg_reg_t hri_usbdevice_read_EPCFG_EPTYPE1_bf(const void *const hw,
                                                                            uint8_t           submodule_index)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp = (tmp & USB_DEVICE_EPCFG_EPTYPE1_Msk) >> USB_DEVICE_EPCFG_EPTYPE1_Pos;
	return tmp;
}

static inline void hri_usbdevice_set_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                               hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_epcfg_reg_t hri_usbdevice_get_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbdevice_epcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbdevice_write_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbdevice_epcfg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_clear_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                 hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbdevice_toggle_EPCFG_reg(const void *const hw, uint8_t submodule_index,
                                                  hri_usbdevice_epcfg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdevice_epcfg_reg_t hri_usbdevice_read_EPCFG_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Usb *)hw)->DEVICE.DeviceEndpoint[submodule_index].EPCFG.reg;
}

static inline void hri_usbhostdescbank_set_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg |= USB_HOST_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbhostdescbank_get_ADDR_ADDR_bf(const void *const           hw,
                                                                               hri_usbdesc_bank_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->ADDR.reg;
	tmp = (tmp & USB_HOST_ADDR_ADDR(mask)) >> USB_HOST_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->ADDR.reg;
	tmp &= ~USB_HOST_ADDR_ADDR_Msk;
	tmp |= USB_HOST_ADDR_ADDR(data);
	((UsbHostDescBank *)hw)->ADDR.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg &= ~USB_HOST_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_ADDR_ADDR_bf(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg ^= USB_HOST_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbhostdescbank_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->ADDR.reg;
	tmp = (tmp & USB_HOST_ADDR_ADDR_Msk) >> USB_HOST_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbhostdescbank_get_ADDR_reg(const void *const           hw,
                                                                           hri_usbdesc_bank_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescbank_write_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_ADDR_reg(const void *const hw, hri_usbdesc_bank_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->ADDR.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_addr_reg_t hri_usbhostdescbank_read_ADDR_reg(const void *const hw)
{
	return ((UsbHostDescBank *)hw)->ADDR.reg;
}

static inline void hri_usbhostdescbank_set_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg |= USB_HOST_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescbank_get_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_AUTO_ZLP) >> USB_HOST_PCKSIZE_AUTO_ZLP_Pos;
	return (bool)tmp;
}

static inline void hri_usbhostdescbank_write_PCKSIZE_AUTO_ZLP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_AUTO_ZLP;
	tmp |= value << USB_HOST_PCKSIZE_AUTO_ZLP_Pos;
	((UsbHostDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg &= ~USB_HOST_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_PCKSIZE_AUTO_ZLP_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg ^= USB_HOST_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_set_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                 hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg |= USB_HOST_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbhostdescbank_get_PCKSIZE_BYTE_COUNT_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_BYTE_COUNT(mask)) >> USB_HOST_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                   hri_usbdesc_bank_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_BYTE_COUNT_Msk;
	tmp |= USB_HOST_PCKSIZE_BYTE_COUNT(data);
	((UsbHostDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                   hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg &= ~USB_HOST_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_PCKSIZE_BYTE_COUNT_bf(const void *const              hw,
                                                                    hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg ^= USB_HOST_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbhostdescbank_read_PCKSIZE_BYTE_COUNT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_BYTE_COUNT_Msk) >> USB_HOST_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                        hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg |= USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbhostdescbank_get_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask)) >> USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                          hri_usbdesc_bank_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk;
	tmp |= USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(data);
	((UsbHostDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                          hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg &= ~USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const              hw,
                                                                           hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg ^= USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbhostdescbank_read_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk) >> USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg |= USB_HOST_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t
hri_usbhostdescbank_get_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_SIZE(mask)) >> USB_HOST_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_SIZE_Msk;
	tmp |= USB_HOST_PCKSIZE_SIZE(data);
	((UsbHostDescBank *)hw)->PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg &= ~USB_HOST_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_PCKSIZE_SIZE_bf(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg ^= USB_HOST_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbhostdescbank_read_PCKSIZE_SIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_SIZE_Msk) >> USB_HOST_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbhostdescbank_get_PCKSIZE_reg(const void *const              hw,
                                                                                 hri_usbdesc_bank_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescBank *)hw)->PCKSIZE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescbank_write_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_PCKSIZE_reg(const void *const hw, hri_usbdesc_bank_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->PCKSIZE.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_pcksize_reg_t hri_usbhostdescbank_read_PCKSIZE_reg(const void *const hw)
{
	return ((UsbHostDescBank *)hw)->PCKSIZE.reg;
}

static inline void hri_usbhostdescbank_set_EXTREG_SUBPID_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg |= USB_HOST_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbhostdescbank_get_EXTREG_SUBPID_bf(const void *const             hw,
                                                                                     hri_usbdesc_bank_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_SUBPID(mask)) >> USB_HOST_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_EXTREG_SUBPID_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp &= ~USB_HOST_EXTREG_SUBPID_Msk;
	tmp |= USB_HOST_EXTREG_SUBPID(data);
	((UsbHostDescBank *)hw)->EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_EXTREG_SUBPID_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg &= ~USB_HOST_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_EXTREG_SUBPID_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg ^= USB_HOST_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbhostdescbank_read_EXTREG_SUBPID_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_SUBPID_Msk) >> USB_HOST_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_EXTREG_VARIABLE_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg |= USB_HOST_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t
hri_usbhostdescbank_get_EXTREG_VARIABLE_bf(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_VARIABLE(mask)) >> USB_HOST_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                hri_usbdesc_bank_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp &= ~USB_HOST_EXTREG_VARIABLE_Msk;
	tmp |= USB_HOST_EXTREG_VARIABLE(data);
	((UsbHostDescBank *)hw)->EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg &= ~USB_HOST_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_EXTREG_VARIABLE_bf(const void *const             hw,
                                                                 hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg ^= USB_HOST_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbhostdescbank_read_EXTREG_VARIABLE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_VARIABLE_Msk) >> USB_HOST_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbhostdescbank_get_EXTREG_reg(const void *const             hw,
                                                                               hri_usbdesc_bank_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->EXTREG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescbank_write_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_EXTREG_reg(const void *const hw, hri_usbdesc_bank_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->EXTREG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_extreg_reg_t hri_usbhostdescbank_read_EXTREG_reg(const void *const hw)
{
	return ((UsbHostDescBank *)hw)->EXTREG.reg;
}

static inline void hri_usbhostdescbank_set_CTRL_PIPE_PDADDR_bf(const void *const                hw,
                                                               hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg |= USB_HOST_CTRL_PIPE_PDADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t
hri_usbhostdescbank_get_CTRL_PIPE_PDADDR_bf(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PDADDR(mask)) >> USB_HOST_CTRL_PIPE_PDADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_CTRL_PIPE_PDADDR_bf(const void *const                hw,
                                                                 hri_usbdesc_bank_ctrl_pipe_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp &= ~USB_HOST_CTRL_PIPE_PDADDR_Msk;
	tmp |= USB_HOST_CTRL_PIPE_PDADDR(data);
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_CTRL_PIPE_PDADDR_bf(const void *const                hw,
                                                                 hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg &= ~USB_HOST_CTRL_PIPE_PDADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_CTRL_PIPE_PDADDR_bf(const void *const                hw,
                                                                  hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg ^= USB_HOST_CTRL_PIPE_PDADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t hri_usbhostdescbank_read_CTRL_PIPE_PDADDR_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PDADDR_Msk) >> USB_HOST_CTRL_PIPE_PDADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_CTRL_PIPE_PEPNUM_bf(const void *const                hw,
                                                               hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg |= USB_HOST_CTRL_PIPE_PEPNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t
hri_usbhostdescbank_get_CTRL_PIPE_PEPNUM_bf(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PEPNUM(mask)) >> USB_HOST_CTRL_PIPE_PEPNUM_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_CTRL_PIPE_PEPNUM_bf(const void *const                hw,
                                                                 hri_usbdesc_bank_ctrl_pipe_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp &= ~USB_HOST_CTRL_PIPE_PEPNUM_Msk;
	tmp |= USB_HOST_CTRL_PIPE_PEPNUM(data);
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_CTRL_PIPE_PEPNUM_bf(const void *const                hw,
                                                                 hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg &= ~USB_HOST_CTRL_PIPE_PEPNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_CTRL_PIPE_PEPNUM_bf(const void *const                hw,
                                                                  hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg ^= USB_HOST_CTRL_PIPE_PEPNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t hri_usbhostdescbank_read_CTRL_PIPE_PEPNUM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PEPNUM_Msk) >> USB_HOST_CTRL_PIPE_PEPNUM_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_CTRL_PIPE_PERMAX_bf(const void *const                hw,
                                                               hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg |= USB_HOST_CTRL_PIPE_PERMAX(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t
hri_usbhostdescbank_get_CTRL_PIPE_PERMAX_bf(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PERMAX(mask)) >> USB_HOST_CTRL_PIPE_PERMAX_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_write_CTRL_PIPE_PERMAX_bf(const void *const                hw,
                                                                 hri_usbdesc_bank_ctrl_pipe_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp &= ~USB_HOST_CTRL_PIPE_PERMAX_Msk;
	tmp |= USB_HOST_CTRL_PIPE_PERMAX(data);
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_CTRL_PIPE_PERMAX_bf(const void *const                hw,
                                                                 hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg &= ~USB_HOST_CTRL_PIPE_PERMAX(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_CTRL_PIPE_PERMAX_bf(const void *const                hw,
                                                                  hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg ^= USB_HOST_CTRL_PIPE_PERMAX(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t hri_usbhostdescbank_read_CTRL_PIPE_PERMAX_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PERMAX_Msk) >> USB_HOST_CTRL_PIPE_PERMAX_Pos;
	return tmp;
}

static inline void hri_usbhostdescbank_set_CTRL_PIPE_reg(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t
hri_usbhostdescbank_get_CTRL_PIPE_reg(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescbank_write_CTRL_PIPE_reg(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_clear_CTRL_PIPE_reg(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescbank_toggle_CTRL_PIPE_reg(const void *const hw, hri_usbdesc_bank_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->CTRL_PIPE.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_ctrl_pipe_reg_t hri_usbhostdescbank_read_CTRL_PIPE_reg(const void *const hw)
{
	return ((UsbHostDescBank *)hw)->CTRL_PIPE.reg;
}

static inline bool hri_usbhostdescbank_get_STATUS_BK_CRCERR_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_BK.reg & USB_HOST_STATUS_BK_CRCERR) >> USB_HOST_STATUS_BK_CRCERR_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_BK_CRCERR_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_BK.reg = USB_HOST_STATUS_BK_CRCERR;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescbank_get_STATUS_BK_ERRORFLOW_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_BK.reg & USB_HOST_STATUS_BK_ERRORFLOW) >> USB_HOST_STATUS_BK_ERRORFLOW_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_BK_ERRORFLOW_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_BK.reg = USB_HOST_STATUS_BK_ERRORFLOW;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_bk_reg_t
hri_usbhostdescbank_get_STATUS_BK_reg(const void *const hw, hri_usbdesc_bank_status_bk_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHostDescBank *)hw)->STATUS_BK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescbank_clear_STATUS_BK_reg(const void *const hw, hri_usbdesc_bank_status_bk_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_BK.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_bk_reg_t hri_usbhostdescbank_read_STATUS_BK_reg(const void *const hw)
{
	return ((UsbHostDescBank *)hw)->STATUS_BK.reg;
}

static inline bool hri_usbhostdescbank_get_STATUS_PIPE_DTGLER_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_DTGLER) >> USB_HOST_STATUS_PIPE_DTGLER_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_DTGLER_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_DTGLER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescbank_get_STATUS_PIPE_DAPIDER_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_DAPIDER)
	       >> USB_HOST_STATUS_PIPE_DAPIDER_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_DAPIDER_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_DAPIDER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescbank_get_STATUS_PIPE_PIDER_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_PIDER) >> USB_HOST_STATUS_PIPE_PIDER_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_PIDER_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_PIDER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescbank_get_STATUS_PIPE_TOUTER_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_TOUTER) >> USB_HOST_STATUS_PIPE_TOUTER_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_TOUTER_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_TOUTER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescbank_get_STATUS_PIPE_CRC16ER_bit(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_CRC16ER)
	       >> USB_HOST_STATUS_PIPE_CRC16ER_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_CRC16ER_bit(const void *const hw)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_CRC16ER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_pipe_reg_t
hri_usbhostdescbank_get_STATUS_PIPE_ERCNT_bf(const void *const hw, hri_usbdesc_bank_status_pipe_reg_t mask)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_ERCNT(mask))
	       >> USB_HOST_STATUS_PIPE_ERCNT_Pos;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_ERCNT_bf(const void *const                  hw,
                                                                  hri_usbdesc_bank_status_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_ERCNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_pipe_reg_t hri_usbhostdescbank_read_STATUS_PIPE_ERCNT_bf(const void *const hw)
{
	return (((UsbHostDescBank *)hw)->STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_ERCNT_Msk)
	       >> USB_HOST_STATUS_PIPE_ERCNT_Pos;
}

static inline hri_usbdesc_bank_status_pipe_reg_t
hri_usbhostdescbank_get_STATUS_PIPE_reg(const void *const hw, hri_usbdesc_bank_status_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescBank *)hw)->STATUS_PIPE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescbank_clear_STATUS_PIPE_reg(const void *const                  hw,
                                                             hri_usbdesc_bank_status_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescBank *)hw)->STATUS_PIPE.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbdesc_bank_status_pipe_reg_t hri_usbhostdescbank_read_STATUS_PIPE_reg(const void *const hw)
{
	return ((UsbHostDescBank *)hw)->STATUS_PIPE.reg;
}

static inline void hri_usbhostdescriptor_set_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                          hri_usbhost_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg |= USB_HOST_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_addr_reg_t
hri_usbhostdescriptor_get_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                       hri_usbhost_descriptor_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg;
	tmp = (tmp & USB_HOST_ADDR_ADDR(mask)) >> USB_HOST_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_usbhost_descriptor_addr_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg;
	tmp &= ~USB_HOST_ADDR_ADDR_Msk;
	tmp |= USB_HOST_ADDR_ADDR(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                            hri_usbhost_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg &= ~USB_HOST_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_ADDR_ADDR_bf(const void *const hw, uint8_t submodule_index,
                                                             hri_usbhost_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg ^= USB_HOST_ADDR_ADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_addr_reg_t hri_usbhostdescriptor_read_ADDR_ADDR_bf(const void *const hw,
                                                                                        uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg;
	tmp = (tmp & USB_HOST_ADDR_ADDR_Msk) >> USB_HOST_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                      hri_usbhost_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_addr_reg_t
hri_usbhostdescriptor_get_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                   hri_usbhost_descriptor_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                        hri_usbhost_descriptor_addr_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                        hri_usbhost_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_ADDR_reg(const void *const hw, uint8_t submodule_index,
                                                         hri_usbhost_descriptor_addr_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_addr_reg_t hri_usbhostdescriptor_read_ADDR_reg(const void *const hw,
                                                                                    uint8_t           submodule_index)
{
	return ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].ADDR.reg;
}

static inline void hri_usbhostdescriptor_set_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg |= USB_HOST_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescriptor_get_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_AUTO_ZLP) >> USB_HOST_PCKSIZE_AUTO_ZLP_Pos;
	return (bool)tmp;
}

static inline void hri_usbhostdescriptor_write_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index,
                                                                    bool value)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_AUTO_ZLP;
	tmp |= value << USB_HOST_PCKSIZE_AUTO_ZLP_Pos;
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg &= ~USB_HOST_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_PCKSIZE_AUTO_ZLP_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg ^= USB_HOST_PCKSIZE_AUTO_ZLP;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_set_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg |= USB_HOST_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t
hri_usbhostdescriptor_get_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                hri_usbhost_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_BYTE_COUNT(mask)) >> USB_HOST_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_usbhost_descriptor_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_BYTE_COUNT_Msk;
	tmp |= USB_HOST_PCKSIZE_BYTE_COUNT(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                     hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg &= ~USB_HOST_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index,
                                                                      hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg ^= USB_HOST_PCKSIZE_BYTE_COUNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t
hri_usbhostdescriptor_read_PCKSIZE_BYTE_COUNT_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_BYTE_COUNT_Msk) >> USB_HOST_PCKSIZE_BYTE_COUNT_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                                          hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg |= USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t
hri_usbhostdescriptor_get_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                       hri_usbhost_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask)) >> USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw,
                                                                            uint8_t           submodule_index,
                                                                            hri_usbhost_descriptor_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk;
	tmp |= USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw,
                                                                            uint8_t           submodule_index,
                                                                            hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg &= ~USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw,
                                                                             uint8_t           submodule_index,
                                                                             hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg ^= USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t
hri_usbhostdescriptor_read_PCKSIZE_MULTI_PACKET_SIZE_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Msk) >> USB_HOST_PCKSIZE_MULTI_PACKET_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                             hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg |= USB_HOST_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t
hri_usbhostdescriptor_get_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                          hri_usbhost_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_SIZE(mask)) >> USB_HOST_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                               hri_usbhost_descriptor_pcksize_reg_t data)
{
	uint32_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp &= ~USB_HOST_PCKSIZE_SIZE_Msk;
	tmp |= USB_HOST_PCKSIZE_SIZE(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                               hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg &= ~USB_HOST_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_PCKSIZE_SIZE_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg ^= USB_HOST_PCKSIZE_SIZE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t hri_usbhostdescriptor_read_PCKSIZE_SIZE_bf(const void *const hw,
                                                                                              uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp = (tmp & USB_HOST_PCKSIZE_SIZE_Msk) >> USB_HOST_PCKSIZE_SIZE_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                         hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t
hri_usbhostdescriptor_get_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                      hri_usbhost_descriptor_pcksize_reg_t mask)
{
	uint32_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                           hri_usbhost_descriptor_pcksize_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                           hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_PCKSIZE_reg(const void *const hw, uint8_t submodule_index,
                                                            hri_usbhost_descriptor_pcksize_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_pcksize_reg_t hri_usbhostdescriptor_read_PCKSIZE_reg(const void *const hw,
                                                                                          uint8_t submodule_index)
{
	return ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].PCKSIZE.reg;
}

static inline void hri_usbhostdescriptor_set_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                              hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg |= USB_HOST_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_extreg_reg_t
hri_usbhostdescriptor_get_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                           hri_usbhost_descriptor_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_SUBPID(mask)) >> USB_HOST_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_usbhost_descriptor_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp &= ~USB_HOST_EXTREG_SUBPID_Msk;
	tmp |= USB_HOST_EXTREG_SUBPID(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg &= ~USB_HOST_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_EXTREG_SUBPID_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg ^= USB_HOST_EXTREG_SUBPID(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_extreg_reg_t hri_usbhostdescriptor_read_EXTREG_SUBPID_bf(const void *const hw,
                                                                                              uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_SUBPID_Msk) >> USB_HOST_EXTREG_SUBPID_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg |= USB_HOST_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_extreg_reg_t
hri_usbhostdescriptor_get_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                             hri_usbhost_descriptor_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_VARIABLE(mask)) >> USB_HOST_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_usbhost_descriptor_extreg_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp &= ~USB_HOST_EXTREG_VARIABLE_Msk;
	tmp |= USB_HOST_EXTREG_VARIABLE(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg &= ~USB_HOST_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_EXTREG_VARIABLE_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg ^= USB_HOST_EXTREG_VARIABLE(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_extreg_reg_t hri_usbhostdescriptor_read_EXTREG_VARIABLE_bf(const void *const hw,
                                                                                                uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp = (tmp & USB_HOST_EXTREG_VARIABLE_Msk) >> USB_HOST_EXTREG_VARIABLE_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                        hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_extreg_reg_t
hri_usbhostdescriptor_get_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                     hri_usbhost_descriptor_extreg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                          hri_usbhost_descriptor_extreg_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                          hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_EXTREG_reg(const void *const hw, uint8_t submodule_index,
                                                           hri_usbhost_descriptor_extreg_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_extreg_reg_t hri_usbhostdescriptor_read_EXTREG_reg(const void *const hw,
                                                                                        uint8_t submodule_index)
{
	return ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].EXTREG.reg;
}

static inline void hri_usbhostdescriptor_set_CTRL_PIPE_PDADDR_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg |= USB_HOST_CTRL_PIPE_PDADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_get_CTRL_PIPE_PDADDR_bf(const void *const hw, uint8_t submodule_index,
                                              hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PDADDR(mask)) >> USB_HOST_CTRL_PIPE_PDADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_CTRL_PIPE_PDADDR_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_ctrl_pipe_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp &= ~USB_HOST_CTRL_PIPE_PDADDR_Msk;
	tmp |= USB_HOST_CTRL_PIPE_PDADDR(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_CTRL_PIPE_PDADDR_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg &= ~USB_HOST_CTRL_PIPE_PDADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_CTRL_PIPE_PDADDR_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg ^= USB_HOST_CTRL_PIPE_PDADDR(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_read_CTRL_PIPE_PDADDR_bf(const void *const hw, uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PDADDR_Msk) >> USB_HOST_CTRL_PIPE_PDADDR_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_CTRL_PIPE_PEPNUM_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg |= USB_HOST_CTRL_PIPE_PEPNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_get_CTRL_PIPE_PEPNUM_bf(const void *const hw, uint8_t submodule_index,
                                              hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PEPNUM(mask)) >> USB_HOST_CTRL_PIPE_PEPNUM_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_CTRL_PIPE_PEPNUM_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_ctrl_pipe_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp &= ~USB_HOST_CTRL_PIPE_PEPNUM_Msk;
	tmp |= USB_HOST_CTRL_PIPE_PEPNUM(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_CTRL_PIPE_PEPNUM_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg &= ~USB_HOST_CTRL_PIPE_PEPNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_CTRL_PIPE_PEPNUM_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg ^= USB_HOST_CTRL_PIPE_PEPNUM(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_read_CTRL_PIPE_PEPNUM_bf(const void *const hw, uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PEPNUM_Msk) >> USB_HOST_CTRL_PIPE_PEPNUM_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_CTRL_PIPE_PERMAX_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg |= USB_HOST_CTRL_PIPE_PERMAX(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_get_CTRL_PIPE_PERMAX_bf(const void *const hw, uint8_t submodule_index,
                                              hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PERMAX(mask)) >> USB_HOST_CTRL_PIPE_PERMAX_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_CTRL_PIPE_PERMAX_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_ctrl_pipe_reg_t data)
{
	uint16_t tmp;
	USB_CRITICAL_SECTION_ENTER();
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp &= ~USB_HOST_CTRL_PIPE_PERMAX_Msk;
	tmp |= USB_HOST_CTRL_PIPE_PERMAX(data);
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg = tmp;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_CTRL_PIPE_PERMAX_bf(const void *const hw, uint8_t submodule_index,
                                                                   hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg &= ~USB_HOST_CTRL_PIPE_PERMAX(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_CTRL_PIPE_PERMAX_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg ^= USB_HOST_CTRL_PIPE_PERMAX(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_read_CTRL_PIPE_PERMAX_bf(const void *const hw, uint8_t submodule_index)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp = (tmp & USB_HOST_CTRL_PIPE_PERMAX_Msk) >> USB_HOST_CTRL_PIPE_PERMAX_Pos;
	return tmp;
}

static inline void hri_usbhostdescriptor_set_CTRL_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                                           hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg |= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t
hri_usbhostdescriptor_get_CTRL_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                        hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescriptor_write_CTRL_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                                             hri_usbhost_descriptor_ctrl_pipe_reg_t data)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg = data;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_clear_CTRL_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                                             hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg &= ~mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline void hri_usbhostdescriptor_toggle_CTRL_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_usbhost_descriptor_ctrl_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg ^= mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_ctrl_pipe_reg_t hri_usbhostdescriptor_read_CTRL_PIPE_reg(const void *const hw,
                                                                                              uint8_t submodule_index)
{
	return ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].CTRL_PIPE.reg;
}

static inline bool hri_usbhostdescriptor_get_STATUS_BK_CRCERR_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg & USB_HOST_STATUS_BK_CRCERR)
	       >> USB_HOST_STATUS_BK_CRCERR_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_BK_CRCERR_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg = USB_HOST_STATUS_BK_CRCERR;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescriptor_get_STATUS_BK_ERRORFLOW_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg & USB_HOST_STATUS_BK_ERRORFLOW)
	       >> USB_HOST_STATUS_BK_ERRORFLOW_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_BK_ERRORFLOW_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg = USB_HOST_STATUS_BK_ERRORFLOW;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_status_bk_reg_t
hri_usbhostdescriptor_get_STATUS_BK_reg(const void *const hw, uint8_t submodule_index,
                                        hri_usbhost_descriptor_status_bk_reg_t mask)
{
	uint8_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescriptor_clear_STATUS_BK_reg(const void *const hw, uint8_t submodule_index,
                                                             hri_usbhost_descriptor_status_bk_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_status_bk_reg_t hri_usbhostdescriptor_read_STATUS_BK_reg(const void *const hw,
                                                                                              uint8_t submodule_index)
{
	return ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_BK.reg;
}

static inline bool hri_usbhostdescriptor_get_STATUS_PIPE_DTGLER_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_DTGLER)
	       >> USB_HOST_STATUS_PIPE_DTGLER_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_DTGLER_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_DTGLER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescriptor_get_STATUS_PIPE_DAPIDER_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_DAPIDER)
	       >> USB_HOST_STATUS_PIPE_DAPIDER_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_DAPIDER_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_DAPIDER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescriptor_get_STATUS_PIPE_PIDER_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_PIDER)
	       >> USB_HOST_STATUS_PIPE_PIDER_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_PIDER_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_PIDER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescriptor_get_STATUS_PIPE_TOUTER_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_TOUTER)
	       >> USB_HOST_STATUS_PIPE_TOUTER_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_TOUTER_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_TOUTER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_usbhostdescriptor_get_STATUS_PIPE_CRC16ER_bit(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_CRC16ER)
	       >> USB_HOST_STATUS_PIPE_CRC16ER_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_CRC16ER_bit(const void *const hw, uint8_t submodule_index)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_CRC16ER;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_status_pipe_reg_t
hri_usbhostdescriptor_get_STATUS_PIPE_ERCNT_bf(const void *const hw, uint8_t submodule_index,
                                               hri_usbhost_descriptor_status_pipe_reg_t mask)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_ERCNT(mask))
	       >> USB_HOST_STATUS_PIPE_ERCNT_Pos;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_ERCNT_bf(const void *const hw, uint8_t submodule_index,
                                                                    hri_usbhost_descriptor_status_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = USB_HOST_STATUS_PIPE_ERCNT(mask);
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_status_pipe_reg_t
hri_usbhostdescriptor_read_STATUS_PIPE_ERCNT_bf(const void *const hw, uint8_t submodule_index)
{
	return (((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg & USB_HOST_STATUS_PIPE_ERCNT_Msk)
	       >> USB_HOST_STATUS_PIPE_ERCNT_Pos;
}

static inline hri_usbhost_descriptor_status_pipe_reg_t
hri_usbhostdescriptor_get_STATUS_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                          hri_usbhost_descriptor_status_pipe_reg_t mask)
{
	uint16_t tmp;
	tmp = ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_usbhostdescriptor_clear_STATUS_PIPE_reg(const void *const hw, uint8_t submodule_index,
                                                               hri_usbhost_descriptor_status_pipe_reg_t mask)
{
	USB_CRITICAL_SECTION_ENTER();
	((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg = mask;
	USB_CRITICAL_SECTION_LEAVE();
}

static inline hri_usbhost_descriptor_status_pipe_reg_t
hri_usbhostdescriptor_read_STATUS_PIPE_reg(const void *const hw, uint8_t submodule_index)
{
	return ((UsbHostDescriptor *)hw)->HostDescBank[submodule_index].STATUS_PIPE.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_USB_D51_H_INCLUDED */
#endif /* _SAMD51_USB_COMPONENT_ */
