/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Host controller specific parts of USB host support.
//
// For the Synopsys DWC2 controller, the isochronous IN stream runs on the highest host
// channel, which TinyUSB never uses as it allocates channels from the lowest up.  The
// channel is programmed directly and serviced from the IRQ before TinyUSB's handler.
// It needs the controller's buffer DMA mode, which also lets the hardware move up to
// 3KiB per microframe for high-bandwidth endpoints without the CPU emptying the FIFO.

#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_HW_USB_HOST

#include "mp_usbh.h"

#if defined(TUP_USBIP_DWC2)

#include "portable/synopsys/dwc2/dwc2_common.h"

#define ISO_BUF_SIZE (3 * 1024)

static struct {
    dwc2_channel_t *channel;
    uint8_t ch_id;
    uint8_t pid;
    volatile bool allocated;
    volatile bool running;
    uint32_t hcchar;
    uint32_t hctsiz;
    mp_usbh_iso_in_cb_t cb;
    void *cb_arg;
} iso;

// Written by the controller's DMA each (micro)frame.
CFG_TUH_MEM_SECTION static uint8_t iso_buf[ISO_BUF_SIZE] TU_ATTR_ALIGNED(CFG_TUH_MEM_DCACHE_LINE_SIZE);

static void iso_arm(dwc2_regs_t *dwc2) {
    dwc2_channel_t *channel = iso.channel;
    channel->hctsiz = iso.hctsiz;
    channel->hcdma = (uint32_t)(uintptr_t)iso_buf;
    // Schedule the transfer for the next (micro)frame.
    uint32_t hcchar = iso.hcchar;
    if ((dwc2->hfnum & 1) == 0) {
        hcchar |= HCCHAR_ODDFRM;
    }
    channel->hcchar = hcchar | HCCHAR_CHENA;
}

static void iso_irq(dwc2_regs_t *dwc2) {
    if (!(dwc2->haint & (1 << iso.ch_id))) {
        return;
    }
    dwc2_channel_t *channel = iso.channel;
    const uint32_t hcint = channel->hcint;
    channel->hcint = hcint;
    if (!(hcint & HCINT_HALTED)) {
        return;
    }

    if (hcint & HCINT_XFER_COMPLETE) {
        // In DMA mode the remaining transfer size gives the number of bytes received.
        const uint32_t len = (iso.hctsiz & HCTSIZ_XFRSIZ_Msk) - (channel->hctsiz & HCTSIZ_XFRSIZ_Msk);
        if (len > 0) {
            hcd_dcache_invalidate(iso_buf, len);
        }
        iso.cb(iso.cb_arg, iso_buf, len, true);
    } else {
        // A bus error, so this (micro)frame's data is lost but the stream continues.
        iso.cb(iso.cb_arg, NULL, 0, false);
    }

    if (iso.running && (dwc2->hprt & HPRT_ENABLE)) {
        iso_arm(dwc2);
    }
}

void mp_usbh_int_handler(uint8_t rhport) {
    dwc2_regs_t *dwc2 = DWC2_REG(rhport);

    // TinyUSB enables the OTG interrupt but doesn't service it, and doesn't clear an
    // over-current change, so either would retrigger the IRQ forever.
    const uint32_t gotgint = dwc2->gotgint;
    if (gotgint) {
        dwc2->gotgint = gotgint;
    }
    uint32_t hprt = dwc2->hprt;
    if (hprt & HPRT_OVER_CURRENT_CHANGE) {
        // Don't write 1 to the other write-1-to-clear bits.
        hprt &= ~(HPRT_CONN_DETECT | HPRT_ENABLE | HPRT_ENABLE_CHANGE | HPRT_SUSPEND);
        dwc2->hprt = hprt;
    }

    if (!iso.allocated) {
        tuh_int_handler(rhport, true);
        return;
    }

    iso_irq(dwc2);

    // TinyUSB services every channel flagged in HAINT, so hide the stream's channel.
    // An event that arrives meanwhile raises the IRQ again once it's unmasked.
    const uint32_t hcintmsk = iso.channel->hcintmsk;
    iso.channel->hcintmsk = 0;
    tuh_int_handler(rhport, true);
    iso.channel->hcintmsk = hcintmsk;
}

// Reset value of GDFIFOCFG, which gives the start of the per-channel DMA state at the
// top of the FIFO RAM.
static uint32_t dwc2_gdfifocfg_reset;

void mp_usbh_hcd_pre_init(uint8_t rhport) {
    dwc2_gdfifocfg_reset = DWC2_REG(rhport)->gdfifocfg;
}

void mp_usbh_hcd_post_init(uint8_t rhport) {
    dwc2_regs_t *dwc2 = DWC2_REG(rhport);

    // TinyUSB's host FIFO layout overlaps the TX FIFOs with the RX FIFO, which always
    // starts at 0, and takes the FIFO RAM size from a table that's wrong for some parts
    // (eg STM32N6), so the DMA state can land outside it.  So keep TinyUSB's RX and
    // non-periodic TX sizes, but lay the FIFOs out below the depth the core reports.
    const uint32_t top = dwc2->ghwcfg3 >> 16;
    const uint32_t rx = dwc2->grxfsiz & 0xffff;
    const uint32_t nptx = dwc2->gnptxfsiz >> 16;
    const uint32_t ptx = top - rx - nptx;
    dwc2->gdfifocfg = dwc2_gdfifocfg_reset;
    dwc2->hptxfsiz = (ptx << 16) | rx;
    dwc2->gnptxfsiz = (nptx << 16) | (rx + ptx);
    dfifo_flush_tx(dwc2, 0x10); // all TX FIFOs
    dfifo_flush_rx(dwc2);
}

int mp_usbh_iso_in_start(uint8_t daddr, const tusb_desc_endpoint_t *desc_ep, mp_usbh_iso_in_cb_t cb, void *cb_arg) {
    const uint8_t rhport = TUH_OPT_RHPORT;
    dwc2_regs_t *dwc2 = DWC2_REG(rhport);

    if (iso.allocated) {
        return -MP_EBUSY;
    }
    if (!(dwc2->gahbcfg & GAHBCFG_DMAEN)) {
        // Eg a full speed only core, which has no DMA.
        return -MP_EOPNOTSUPP;
    }
    if (desc_ep->bmAttributes.xfer != TUSB_XFER_ISOCHRONOUS || tu_edpt_dir(desc_ep->bEndpointAddress) != TUSB_DIR_IN) {
        return -MP_EINVAL;
    }
    // A request is made every (micro)frame, and split transactions aren't supported.
    tuh_bus_info_t bus_info;
    if (!tuh_bus_info_get(daddr, &bus_info)) {
        return -MP_ENODEV;
    }
    if (desc_ep->bInterval != 1 || bus_info.speed != hcd_port_speed_get(rhport)) {
        return -MP_EOPNOTSUPP;
    }

    const uint16_t wMaxPacketSize = tu_le16toh(desc_ep->wMaxPacketSize);
    const uint32_t mps = wMaxPacketSize & 0x7ff;
    uint32_t mult = 1;
    if (bus_info.speed == TUSB_SPEED_HIGH) {
        mult = MIN(((wMaxPacketSize >> 11) & 3) + 1, 3);
    }
    if (mps == 0 || mps * mult > ISO_BUF_SIZE) {
        return -MP_EINVAL;
    }

    // Use the highest channel, if TinyUSB isn't using it.
    const dwc2_ghwcfg2_t ghwcfg2 = {.value = dwc2->ghwcfg2};
    const uint8_t ch_id = ghwcfg2.num_host_ch;
    dwc2_channel_t *channel = &dwc2->channel[ch_id];
    if (channel->hcchar & HCCHAR_CHENA) {
        return -MP_EBUSY;
    }

    // For high-bandwidth IN, the starting PID gives the number of packets per microframe.
    static const uint8_t start_pid[] = { HCTSIZ_PID_DATA0, HCTSIZ_PID_DATA1, HCTSIZ_PID_DATA2 };
    iso.channel = channel;
    iso.ch_id = ch_id;
    iso.cb = cb;
    iso.cb_arg = cb_arg;
    iso.hctsiz = ((uint32_t)start_pid[mult - 1] << HCTSIZ_PID_Pos) | (mult << HCTSIZ_PKTCNT_Pos) | (mps * mult);
    iso.hcchar = (mps << HCCHAR_MPSIZ_Pos)
        | ((uint32_t)tu_edpt_number(desc_ep->bEndpointAddress) << HCCHAR_EPNUM_Pos)
        | HCCHAR_EPDIR
        | ((uint32_t)HCCHAR_EPTYPE_ISOCHRONOUS << HCCHAR_EPTYP_Pos)
        | (mult << HCCHAR_MC_Pos)
        | ((uint32_t)daddr << HCCHAR_DAD_Pos)
        | (bus_info.speed == TUSB_SPEED_LOW ? HCCHAR_LSDEV : 0);

    channel->hcsplt = 0;
    channel->hcint = 0xffffffff;
    // In DMA mode the channel halts after every transfer, with the reason in HCINT.
    channel->hcintmsk = HCINT_HALTED;

    hcd_int_disable(rhport);
    dwc2->haintmsk |= 1 << ch_id;
    iso.allocated = true;
    iso.running = true;
    iso_arm(dwc2);
    hcd_int_enable(rhport);
    return 0;
}

void mp_usbh_iso_in_stop(void) {
    if (!iso.allocated) {
        return;
    }
    const uint8_t rhport = TUH_OPT_RHPORT;
    dwc2_regs_t *dwc2 = DWC2_REG(rhport);
    dwc2_channel_t *channel = iso.channel;

    // A periodic channel halts by itself at the end of the (micro)frame, and in DMA mode
    // it mustn't be disabled while active, so let the last transfer finish.
    iso.running = false;
    mp_uint_t t0 = mp_hal_ticks_ms();
    while ((channel->hcchar & HCCHAR_CHENA) && mp_hal_ticks_ms() - t0 < 5) {
    }
    if (channel->hcchar & HCCHAR_CHENA) {
        // It didn't halt, eg because the device was unplugged, so force it.
        channel->hcchar |= HCCHAR_CHDIS | HCCHAR_CHENA;
        t0 = mp_hal_ticks_ms();
        while ((channel->hcchar & HCCHAR_CHENA) && mp_hal_ticks_ms() - t0 < 5) {
        }
    }

    hcd_int_disable(rhport);
    dwc2->haintmsk &= ~(1 << iso.ch_id);
    channel->hcintmsk = 0;
    channel->hcint = 0xffffffff;
    iso.allocated = false;
    hcd_int_enable(rhport);
}

#else

// Other host controllers don't have an isochronous IN stream yet.

void mp_usbh_int_handler(uint8_t rhport) {
    tuh_int_handler(rhport, true);
}

void mp_usbh_hcd_pre_init(uint8_t rhport) {
    (void)rhport;
}

void mp_usbh_hcd_post_init(uint8_t rhport) {
    (void)rhport;
}

int mp_usbh_iso_in_start(uint8_t daddr, const tusb_desc_endpoint_t *desc_ep, mp_usbh_iso_in_cb_t cb, void *cb_arg) {
    return -MP_EOPNOTSUPP;
}

void mp_usbh_iso_in_stop(void) {
}

#endif // defined(TUP_USBIP_DWC2)

#endif // MICROPY_HW_USB_HOST
