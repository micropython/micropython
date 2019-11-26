/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Emil Renner Berthing
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "gd32vf103/rcu.h"
#include "gd32vf103/mtimer.h"
#include "gd32vf103/usbfs.h"

#include "lib/eclic.h"

#include "py/mpconfig.h"

#include "usbacm.h"

#if 1
#define debug(...)
#else
#include "uart0.h"
#define debug(...) uart0_printf(__VA_ARGS__)
#endif

#define CDC_INTERFACE 0
#define CDC_ENDPOINT 2
/* up to 64 bytes for full-speed interrupt eps */
#define CDC_PACKETSIZE 8

#define ACM_INTERFACE 1
#define ACM_ENDPOINT 1
/* 8, 16, 32 or 64 bytes for full-speed bulk eps */
#define ACM_PACKETSIZE 64

#define USBFS_FIFO_RXSIZE  512
#define USBFS_FIFO_TX0SIZE 256
#define USBFS_FIFO_TX1SIZE 256
#define USBFS_FIFO_TX2SIZE 64
#define USBFS_FIFO_TX3SIZE 0

#define USB_WORD(x) ((x) & 0xFF),((x) >> 8)
#define USB_TRIPLE(x) ((x) & 0xFF),(((x) >> 8) & 0xFF),((x) >> 16)
#define USB_QUAD(x) ((x) & 0xFF),(((x) >> 8) & 0xFF),(((x) >> 16) & 0xFF),((x) >> 24)

struct usb_setup_packet {
    union {
        struct {
            uint8_t bmRequestType;
            uint8_t bRequest;
        };
        uint16_t request;
    };
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

struct usb_descriptor_device {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
};

struct usb_descriptor_device_qualifier {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
    uint8_t bReserved;
};

struct usb_descriptor_configuration {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
    uint8_t rest[];
};

struct usb_descriptor_string {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wCodepoint[];
};

struct usb_setup_handler {
    uint16_t req;
    uint8_t idx;
    uint8_t len;
    int (*fn)(const struct usb_setup_packet *p, const void **data);
};


struct acm_line_coding {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
};

static const struct usb_descriptor_device usbfs_descriptor_device = {
    .bLength = 18,
    .bDescriptorType = 0x01,        /* Device */
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,           /* 0x00 = per interface */
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x1d50,               /* OpenMoko vendor id */
    .idProduct = 0x613f,              /* GeckoBoot target product id */
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

static const struct usb_descriptor_configuration usbfs_descriptor_configuration1 = {
    .bLength = 9,
    .bDescriptorType = 0x02,          /* Configuration */
    .wTotalLength = 9 + 8 + 9 + 5 + 5 + 4 + 5 + 7 + 9 + 7 + 7,
    .bNumInterfaces = 2,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 250,
    .rest = {
        /* Interface Association */
        /* .bLength                */ 8,
        /* .bDescriptorType        */ 0x0B, /* Interface Association */
        /* .bFirstInterface        */ CDC_INTERFACE,
        /* .bInterfaceCount        */ 2,
        /* .bFunctionClass         */ 0x02, /* 0x02 = CDC */
        /* .bFunctionSubClass      */ 0x02, /* 0x02 = ACM */
        /* .bFunctionProtocol      */ 0,
        /* .iFunction              */ 0,
        /* Interface */
        /* .bLength                */ 9,
        /* .bDescriptorType        */ 0x04, /* Interface */
        /* .bInterfaceNumber       */ CDC_INTERFACE,
        /* .bAlternateSetting      */ 0,
        /* .bNumEndpoints          */ 1,
        /* .bInterfaceClass        */ 0x02, /* 0x02 = CDC */
        /* .bInterfaceSubClass     */ 0x02, /* 0x02 = ACM */
        /* .bInterfaceProtocol     */ 0x01, /* 0x00 = no protocol required, 0x01 = AT commands V.250 etc */
        /* .iInterface             */ 0,
        /* CDC Header */
        /* .bLength                */ 5,
        /* .bDescriptorType        */ 0x24, /* CS_INTERFACE */
        /* .bDescriptorSubtype     */ 0x00, /* Header */
        /* .bcdCDC                 */ USB_WORD(0x0120),
        /* CDC Call Management */
        /* .bLength                */ 5,
        /* .bDescriptorType        */ 0x24, /* CS_INTERFACE */
        /* .bDescriptorSubtype     */ 0x01, /* Call Management */
        /* .bmCapabilities         */ 0x03, /* Handles call management over data line */
        /* .bDataInterface         */ ACM_INTERFACE,
        /* CDC ACM */
        /* .bLength                */ 4,
        /* .bDescriptorType        */ 0x24, /* CS_INTERFACE */
        /* .bDescriptorSubtype     */ 0x02, /* ACM */
        /* .bmCapabilities         */ 0x02, /* 0x02 = supports line state and coding */
        /* CDC Union */
        /* .bLength                */ 5,
        /* .bDescriptorType        */ 0x24, /* CS_INTERFACE */
        /* .bDescriptorSubtype     */ 0x06, /* Union */
        /* .bControlInterface      */ CDC_INTERFACE,
        /* .bSubordinateInterface0 */ ACM_INTERFACE,
        /* Endpoint */
        /* .bLength                */ 7,
        /* .bDescriptorType        */ 0x05, /* Endpoint */
        /* .bEndpointAddress       */ 0x80 | CDC_ENDPOINT, /* in */
        /* .bmAttributes           */ 0x03, /* interrupt */
        /* .wMaxPacketSize         */ USB_WORD(CDC_PACKETSIZE),
        /* .bInterval              */ 255,  /* poll every 255ms */
        /* Interface */
        /* .bLength                */ 9,
        /* .bDescriptorType        */ 0x04, /* Interface */
        /* .bInterfaceNumber       */ ACM_INTERFACE,
        /* .bAlternateSetting      */ 0,
        /* .bNumEndpoints          */ 2,
        /* .bInterfaceClass        */ 0x0A, /* 0x0A = CDC Data */
        /* .bInterfaceSubClass     */ 0x00,
        /* .bInterfaceProtocol     */ 0x00,
        /* .iInterface             */ 0,
        /* Endpoint */
        /* .bLength                */ 7,
        /* .bDescriptorType        */ 0x05, /* Endpoint */
        /* .bEndpointAddress       */ 0x80 | ACM_ENDPOINT, /* in */
        /* .bmAttributes           */ 0x02, /* bulk */
        /* .wMaxPacketSize         */ USB_WORD(ACM_PACKETSIZE),
        /* .bInterval              */ 0,    /* unused */
        /* Endpoint */
        /* .bLength                */ 7,
        /* .bDescriptorType        */ 0x05, /* Endpoint */
        /* .bEndpointAddress       */ ACM_ENDPOINT, /* out */
        /* .bmAttributes           */ 0x02, /* bulk */
        /* .wMaxPacketSize         */ USB_WORD(ACM_PACKETSIZE),
        /* .bInterval              */ 0,    /* unused */
    }
};

static const struct usb_descriptor_string usbfs_descriptor_string0 = {
    .bLength = 4,
    .bDescriptorType = 0x03,     /* String */
    .wCodepoint = {
        0x0409,         /* English (US) */
    },
};

static const struct usb_descriptor_string usbfs_descriptor_manufacturer = {
    .bLength = 16,
    .bDescriptorType = 0x03,     /* String */
    .wCodepoint = {
        'L','a','b','i','t','a','t',
    },
};

static const struct usb_descriptor_string usbfs_descriptor_product = {
    .bLength = 20,
    .bDescriptorType = 0x03,     /* String */
    .wCodepoint = {
        'G','D','3','2','V','F','1','0','3',
    },
};

/* must be at least 12 characters long and consist of only '0'-'9','A'-'B'
 * at least according to the mass-storage bulk-only document */
static const struct usb_descriptor_string usbfs_descriptor_serial = {
    .bLength = 26,
    .bDescriptorType = 0x03,     /* String */
    .wCodepoint = {
        '0','0','0','0','0','0','0','0','0','0','0','1',
    },
};

static const struct usb_descriptor_string *const usbfs_descriptor_string[] = {
    &usbfs_descriptor_string0,
    &usbfs_descriptor_manufacturer,
    &usbfs_descriptor_product,
    &usbfs_descriptor_serial,
};

static struct {
    uint32_t *ep0out;
    const unsigned char *ep0in;
    uint32_t bytes;
    uint32_t packetsize;
} usbfs_state;

static struct {
    union {
        struct usb_setup_packet setup;
        uint32_t v[2];
    };
    uint8_t data[64];
} usbfs_outbuf;

static uint16_t usbfs_status;

static struct acm_line_coding acm_line_coding;

static volatile bool acm_inidle;
static volatile uint16_t acm_inhead;
static volatile uint16_t acm_intail;
static uint8_t acm_inbuf[1024];

static volatile uint8_t acm_outbytes;
static union {
    uint32_t word[64 / 4];
    uint8_t byte[64];
} acm_outbuf;

int mp_hal_stdin_rx_chr(void) {
    static uint8_t head;
    unsigned int bytes;
    int ret;

    #if 0
    while (1) {
        eclic_global_interrupt_disable();
        bytes = acm_outbytes;
        if (bytes > 0) {
            break;
        }
        wait_for_interrupt();
        eclic_global_interrupt_enable();
    }
    eclic_global_interrupt_enable();
    #else
    do {
        bytes = acm_outbytes;
    } while (bytes == 0);
    #endif

    ret = acm_outbuf.byte[head];
    acm_outbytes = --bytes;
    if (bytes == 0) {
        head = 0;
        USBFS->DOEP[ACM_ENDPOINT].LEN = USBFS_DOEPLEN_PCNT(1) | ACM_PACKETSIZE;
        USBFS->DOEP[ACM_ENDPOINT].CTL |= USBFS_DOEPCTL_EPEN | USBFS_DOEPCTL_CNAK;
    } else {
        head++;
    }

    return ret;
}

static void
acm_send(void) {
    unsigned int head = acm_inhead;
    unsigned int tail = acm_intail;
    unsigned int len;
    uint32_t dieplen;

    if (head == tail) {
        acm_inidle = true;
        return;
    }

    len = (tail - head) % ARRAY_SIZE(acm_inbuf);
    debug("tfstat%x=%lu, len=%u\n", ACM_ENDPOINT,
        USBFS->DIEP[ACM_ENDPOINT].TFSTAT,
        len);
    if (len >= 4 * ACM_PACKETSIZE) {
        len = 4 * ACM_PACKETSIZE;
        dieplen = USBFS_DIEPLEN_PCNT(4U);
    } else if (len >= 3 * ACM_PACKETSIZE) {
        len = 3 * ACM_PACKETSIZE;
        dieplen = USBFS_DIEPLEN_PCNT(3U);
    } else if (len >= 2 * ACM_PACKETSIZE) {
        len = 2 * ACM_PACKETSIZE;
        dieplen = USBFS_DIEPLEN_PCNT(2U);
    } else {
        if (len > ACM_PACKETSIZE) {
            len = ACM_PACKETSIZE;
        }
        dieplen = USBFS_DIEPLEN_PCNT(1U);
    }
    dieplen |= len;

    tail = (head + len) % ARRAY_SIZE(acm_inbuf);
    acm_inhead = tail;

    USBFS->DIEP[ACM_ENDPOINT].LEN = dieplen;
    USBFS->DIEP[ACM_ENDPOINT].CTL |= USBFS_DIEPCTL_EPEN | USBFS_DIEPCTL_CNAK;
    do {
        uint32_t v = acm_inbuf[head++];

        head %= ARRAY_SIZE(acm_inbuf);
        if (head != tail) {
            v |= ((uint32_t)acm_inbuf[head++]) << 8;
            head %= ARRAY_SIZE(acm_inbuf);
        }
        if (head != tail) {
            v |= ((uint32_t)acm_inbuf[head++]) << 16;
            head %= ARRAY_SIZE(acm_inbuf);
        }
        if (head != tail) {
            v |= ((uint32_t)acm_inbuf[head++]) << 24;
            head %= ARRAY_SIZE(acm_inbuf);
        }
        USBFS->DFIFO[ACM_ENDPOINT][0] = v;
    } while (head != tail);
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    unsigned int tail = acm_intail;

    for (; len > 0; len--, str++) {
        acm_inbuf[tail++] = *str;
        tail %= ARRAY_SIZE(acm_inbuf);
        while (tail == acm_inhead) {
            /* wait */
        }
    }

    acm_intail = tail;
    if (acm_inidle) {
        acm_inidle = false;
        acm_send();
    }
}

static void
usbfs_udelay(unsigned int us) {
    uint32_t zero = MTIMER->mtime_lo;
    uint32_t end = us * (CORECLOCK / 4000000) + 1;
    uint32_t now;

    do {
        now = MTIMER->mtime_lo - zero;
    } while (now < end);
}

static void
usbfs_ep0in_transfer(void) {
    uint32_t len = usbfs_state.bytes;
    const unsigned char *p = usbfs_state.ep0in;
    const unsigned char *end;

    if (len > usbfs_state.packetsize) {
        len = usbfs_state.packetsize;
    }

    end = p + len;

    debug("tfstat%x=%lu (0x%03x)\n", 0,
        USBFS->DIEP[0].TFSTAT,
        (uintptr_t)&USBFS->DIEP[0].TFSTAT - (uintptr_t)USBFS);

    USBFS->DIEP[0].LEN = USBFS_DIEPLEN_PCNT(1U) | len;
    USBFS->DIEP[0].CTL |= USBFS_DIEPCTL_EPEN | USBFS_DIEPCTL_CNAK;
    while (p < end) {
        uint32_t v = *p++;
        if (p < end) {
            v |= (*p++) << 8;
        }
        if (p < end) {
            v |= (*p++) << 16;
        }
        if (p < end) {
            v |= (*p++) << 24;
        }
        USBFS->DFIFO[0][0] = v;
    }
}

#if 1
static void
usbfs_ep0in_transfer_empty(void) {
    USBFS->DIEP[0].LEN = USBFS_DIEPLEN_PCNT(1U);
    USBFS->DIEP[0].CTL |= USBFS_DIEPCTL_EPEN | USBFS_DIEPCTL_CNAK;
}
#else
static inline void
usbfs_ep0in_transfer_empty(void) {
    usbfs_ep0in_transfer();
}
#endif

static inline void
usbfs_ep0in_stall(void) {
    USBFS->DIEP[0].CTL |= USBFS_DIEPCTL_STALL;
}

static void
usbfs_ep0out_prepare_setup(void) {
    USBFS->DOEP[0].LEN =
        USBFS_DOEPLEN_STPCNT(3U) |
        USBFS_DOEPLEN_PCNT(0U) |
        USBFS_DOEPLEN_TLEN(0U);
    USBFS->DOEP[0].CTL |= USBFS_DOEPCTL_EPEN | USBFS_DOEPCTL_STALL;
}

static void
usbfs_ep0out_prepare_out(void) {
    USBFS->DOEP[0].LEN =
        USBFS_DOEPLEN_STPCNT(3U) |
        USBFS_DOEPLEN_PCNT(1U) |
        USBFS_DOEPLEN_TLEN(usbfs_state.packetsize);
    USBFS->DOEP[0].CTL |= USBFS_DOEPCTL_EPEN | USBFS_DOEPCTL_CNAK;
}

static void
usbfs_suspend(void) {
    /*
    usbfs_phy_stop();
    usbfs_flags_enable(USBFS_GINTMSK_WUIM
                | USBFS_GINTMSK_USBRST);
    */
}

static void
usbfs_wakeup(void) {
    /*
    usbfs_phy_start();
    usbfs_flags_enable(USB_OTG_GINTMSK_ENUMDNEM
                | USB_OTG_GINTMSK_USBRST
                | USB_OTG_GINTMSK_USBSUSPM
                | USB_OTG_GINTMSK_OEPINT
                | USB_OTG_GINTMSK_IEPINT);
    */
}

static void
usbfs_txfifos_flush(void) {
    /* flush all tx fifos */
    USBFS->GRSTCTL |= USBFS_GRSTCTL_TXFNUM(0x10U) | USBFS_GRSTCTL_TXFF;
    while ((USBFS->GRSTCTL & USBFS_GRSTCTL_TXFF)) {
        /* wait */
    }
    /* wait 3 more phy clocks */
    usbfs_udelay(3);
}

static void
usbfs_ep_reset(void) {
    unsigned int i;

    USBFS->DIEP[0].CTL =
        USBFS_DIEPCTL_STALL |
        USBFS_DIEPCTL_EPTYPE_CONTROL |
        USBFS_DIEP0CTL_MPL_64B;
    USBFS->DIEP[0].INTF =
        USBFS_DIEPINTF_IEPNE |
        USBFS_DIEPINTF_EPTXFUD |
        USBFS_DIEPINTF_CITO |
        USBFS_DIEPINTF_EPDIS |
        USBFS_DIEPINTF_TF;
    for (i = 1; i < 4; i++) {
        /*
        if (USBFS->DIEP[i].CTL & USBFS_DIEPCTL_EPEN)
                USBFS->DIEP[i].CTL = USBFS_DIEPCTL_EPD | USBFS_DIEPCTL_SNAK;
        else
        */
        USBFS->DIEP[i].CTL = USBFS_DIEPCTL_SNAK;
        USBFS->DIEP[i].INTF =
            USBFS_DIEPINTF_IEPNE |
            USBFS_DIEPINTF_EPTXFUD |
            USBFS_DIEPINTF_CITO |
            USBFS_DIEPINTF_EPDIS |
            USBFS_DIEPINTF_TF;
        USBFS->DIEP[i].LEN = 0;
    }

    USBFS->DOEP[0].CTL =
        USBFS_DOEPCTL_STALL |
        USBFS_DOEPCTL_EPTYPE_CONTROL |
        USBFS_DOEP0CTL_MPL_64B;
    USBFS->DOEP[0].INTF =
        USBFS_DOEPINTF_BTBSTP |
        USBFS_DOEPINTF_EPRXFOVR |
        USBFS_DOEPINTF_STPF |
        USBFS_DOEPINTF_EPDIS |
        USBFS_DOEPINTF_TF;
    for (i = 1; i < 4; i++) {
        /*
        if (USBFS->DOEP[i].CTL & USBFS_DOEPCTL_EPEN)
                USBFS->DOEP[i].CTL = USBFS_DOEPCTL_EPD | USBFS_DOEPCTL_SNAK;
        else
        */
        USBFS->DOEP[i].CTL = USBFS_DOEPCTL_SNAK;
        USBFS->DOEP[i].INTF =
            USBFS_DOEPINTF_BTBSTP |
            USBFS_DOEPINTF_EPRXFOVR |
            USBFS_DOEPINTF_STPF |
            USBFS_DOEPINTF_EPDIS |
            USBFS_DOEPINTF_TF;
        USBFS->DOEP[i].LEN = 0;
    }
}

static void
usbfs_reset(void) {
    /* clear the remote wakeup signaling */
    USBFS->DCTL &= ~USBFS_DCTL_RWKUP;

    /* flush all tx fifos */
    usbfs_txfifos_flush();

    /* reset endpoint registers */
    usbfs_ep_reset();

    /* reset address */
    USBFS->DCFG &= ~USBFS_DCFG_DAR_Msk;

    /* enable interrupts for endpoint 0 only */
    USBFS->DAEPINTEN =
        USBFS_DAEPINTEN_OEPIE(1U) |
        USBFS_DAEPINTEN_IEPIE(1U);
    USBFS->DOEPINTEN =
        USBFS_DOEPINTEN_STPFEN |
        /* USBFS_DOEPINTEN_EPDISEN | */
        USBFS_DOEPINTEN_TFEN;
    USBFS->DIEPINTEN =
        /* USBFS_DIEPINTEN_CITOEN | */
        /* USBFS_DIEPINTEN_EPDISEN | */
        USBFS_DIEPINTEN_TFEN;

    /* reset internal state */
    usbfs_state.bytes = 0;
    usbfs_state.packetsize = 64;
}

static void
usbfs_enumdone(void) {
    USBFS->DCTL |= USBFS_DCTL_CGINAK;

    if ((USBFS->DSTAT & USBFS_DSTAT_ES_Msk) == USBFS_DSTAT_ES_FULL) {
        /* we already set 64 byte packages at reset */
        debug("full speed.. ");
    } else {
        /* use 8 byte packages */
        USBFS->DIEP[0].CTL |= USBFS_DIEP0CTL_MPL_8B;
        USBFS->DOEP[0].CTL |= USBFS_DOEP0CTL_MPL_8B;
        usbfs_state.packetsize = 8;
        debug("low speed.. ");
    }

    /* prepare to receive setup package */
    usbfs_ep0out_prepare_setup();
}

static int
usbfs_handle_get_status_device(const struct usb_setup_packet *p, const void **data) {
    debug("GET_STATUS: device\n");
    *data = &usbfs_status;
    return 2;
}

static int
usbfs_handle_set_address(const struct usb_setup_packet *p, const void **data) {
    debug("SET_ADDRESS: wValue = %hu\n", p->wValue);
    USBFS->DCFG = (USBFS->DCFG & ~USBFS_DCFG_DAR_Msk) |
        USBFS_DCFG_DAR((uint32_t)p->wValue);
    return 0;
}

static int
usbfs_handle_get_descriptor_device(const void **data, uint8_t index) {
    if (index != 0) {
        debug("GET_DESCRIPTOR: type = 0x01, but index = 0x%02x\n", index);
        return -1;
    }
    *data = &usbfs_descriptor_device;
    return sizeof(usbfs_descriptor_device);
}

static int
usbfs_handle_get_descriptor_configuration(const void **data, uint8_t index) {
    if (index != 0) {
        debug("GET_DESCRIPTOR: unknown configuration %hu\n", index);
        return -1;
    }
    *data = &usbfs_descriptor_configuration1;
    return usbfs_descriptor_configuration1.wTotalLength;
}

static int
usbfs_handle_get_descriptor_string(const void **data, uint8_t index) {
    const struct usb_descriptor_string *desc;

    if (index >= ARRAY_SIZE(usbfs_descriptor_string)) {
        debug("GET_DESCRIPTOR: unknown string %hu\n", index);
        return -1;
    }
    desc = usbfs_descriptor_string[index];
    *data = desc;
    return desc->bLength;
}

static int
usbfs_handle_get_descriptor(const struct usb_setup_packet *p, const void **data) {
    uint8_t type = p->wValue >> 8;
    uint8_t index = p->wValue & 0xFFU;

    switch (type) {
        case 0x01:
            debug("GET_DESCRIPTOR: device, %u bytes\n", p->wLength);
            return usbfs_handle_get_descriptor_device(data, index);
        case 0x02:
            debug("GET_DESCRIPTOR: configuration %u, %u bytes\n",
                index, p->wLength);
            return usbfs_handle_get_descriptor_configuration(data, index);
        case 0x03:
            debug("GET_DESCRIPTOR: string %u, %u bytes\n",
                index, p->wLength);
            return usbfs_handle_get_descriptor_string(data, index);
        #ifndef NDEBUG
        case 0x06: /* DEVICE QUALIFIER (for high-speed) */
            debug("DEVICE_QUALIFIER\n");
            break;
        default:
            debug("GET_DESCRIPTOR: unknown type 0x%02x\n", type);
            break;
        #endif
    }
    return -1;
}

static int
usbfs_handle_get_configuration(const struct usb_setup_packet *p, const void **data) {
    debug("GET_CONFIGURATION\n");
    *data = &usbfs_descriptor_configuration1.bConfigurationValue;
    return 1;
}

static int
usbfs_handle_set_configuration(const struct usb_setup_packet *p, const void **data) {
    debug("SET_CONFIGURATION: wValue = %hu\n", p->wValue);

    if (p->wValue != usbfs_descriptor_configuration1.bConfigurationValue) {
        return -1;
    }

    /* configure CDC endpoint */
    USBFS->DIEP[CDC_ENDPOINT].CTL =
        USBFS_DIEPCTL_SNAK |
        USBFS_DIEPCTL_EPTYPE_INTERRUPT |
        USBFS_DIEPCTL_EPACT |
        CDC_PACKETSIZE;

    /* configure ACM endpoints */
    USBFS->DIEP[ACM_ENDPOINT].CTL =
        USBFS_DIEPCTL_SNAK |
        USBFS_DIEPCTL_EPTYPE_BULK |
        USBFS_DIEPCTL_EPACT |
        ACM_PACKETSIZE;

    USBFS->DOEP[ACM_ENDPOINT].LEN = USBFS_DOEPLEN_PCNT(1) | ACM_PACKETSIZE;
    USBFS->DOEP[ACM_ENDPOINT].CTL =
        USBFS_DOEPCTL_EPEN |
        USBFS_DOEPCTL_CNAK |
        USBFS_DOEPCTL_EPTYPE_BULK |
        USBFS_DOEPCTL_EPACT |
        ACM_PACKETSIZE;

    USBFS->DAEPINTEN |=
        /* (1U << (CDC_ENDPOINT + USBFS_DAEPINTEN_IEPIE_Pos)) | */
        (1U << (ACM_ENDPOINT + USBFS_DAEPINTEN_IEPIE_Pos)) |
        (1U << (ACM_ENDPOINT + USBFS_DAEPINTEN_OEPIE_Pos));

    acm_line_coding.dwDTERate = 9600;
    acm_line_coding.bCharFormat = 0;
    acm_line_coding.bParityType = 0;
    acm_line_coding.bDataBits = 8;
    acm_inidle = true;
    acm_send();

    return 0;
}

static int
usbfs_handle_set_interface0(const struct usb_setup_packet *p, const void **data) {
    debug("SET_INTERFACE: wIndex = %hu, wValue = %hu\n", p->wIndex, p->wValue);

    if (p->wValue != 0) {
        return -1;
    }

    return 0;
}

static int
usbfs_handle_clear_feature_endpoint(const struct usb_setup_packet *p, const void **data) {
    debug("CLEAR_FEATURE endpoint %hu\n", p->wIndex);
    return -1;
}

static int
acm_set_control_line_state(const struct usb_setup_packet *p, const void **data) {
    debug("SET_CONTROL_LINE_STATE: wIndex = %hu wValue = 0x%04hx\r\n",
        p->wIndex, p->wValue);

    debug("  RTS %u DTR %u\r\n", !!(p->wValue & 0x2), !!(p->wValue & 0x1));
    return 0;
}

static int
acm_set_line_coding(const struct usb_setup_packet *p, const void **data) {
    const struct acm_line_coding *lc = *data;

    debug("SET_LINE_CODING: {\r\n"
        "  dwDTERate   = %lu\r\n"
        "  bCharFormat = 0x%02x\r\n"
        "  bParityType = 0x%02x\r\n"
        "  bDataBits   = 0x%02x\r\n"
        "}\r\n",
        lc->dwDTERate,
        lc->bCharFormat,
        lc->bParityType,
        lc->bDataBits);

    acm_line_coding.dwDTERate = lc->dwDTERate;
    acm_line_coding.bCharFormat = lc->bCharFormat;
    acm_line_coding.bParityType = lc->bParityType;
    acm_line_coding.bDataBits = lc->bDataBits;

    debug("GRFLEN     = 0x%08lx\n", USBFS->GRFLEN);
    debug("DIEP0TFLEN = 0x%08lx\n", USBFS->DIEP0TFLEN);
    debug("DIEP1TFLEN = 0x%08lx\n", USBFS->DIEP1TFLEN);
    debug("DIEP2TFLEN = 0x%08lx\n", USBFS->DIEP2TFLEN);
    debug("DIEP3TFLEN = 0x%08lx\n", USBFS->DIEP3TFLEN);
    return 0;
}

static int
acm_get_line_coding(const struct usb_setup_packet *p, const void **data) {
    debug("GET_LINE_CODING: {\r\n"
        "  dwDTERate   = %lu\r\n"
        "  bCharFormat = 0x%02x\r\n"
        "  bParityType = 0x%02x\r\n"
        "  bDataBits   = 0x%02x\r\n"
        "}\r\n",
        acm_line_coding.dwDTERate,
        acm_line_coding.bCharFormat,
        acm_line_coding.bParityType,
        acm_line_coding.bDataBits);
    *data = &acm_line_coding;
    return 7;
}

static const struct usb_setup_handler usbfs_setup_handlers[] = {
    { .req = 0x0080, .idx = 0, .len = -1, .fn = usbfs_handle_get_status_device },
    { .req = 0x0500, .idx = 0, .len = 0, .fn = usbfs_handle_set_address },
    { .req = 0x0680, .idx = -1, .len = -1, .fn = usbfs_handle_get_descriptor },
    { .req = 0x0880, .idx = 0, .len = -1, .fn = usbfs_handle_get_configuration },
    { .req = 0x0900, .idx = 0, .len = 0, .fn = usbfs_handle_set_configuration },
    { .req = 0x0102, .idx = 0, .len = 0, .fn = usbfs_handle_clear_feature_endpoint },
    { .req = 0x0b01, .idx = CDC_INTERFACE, .len = 0, .fn = usbfs_handle_set_interface0 },
    { .req = 0x2221, .idx = CDC_INTERFACE, .len = 0, .fn = acm_set_control_line_state },
    { .req = 0x2021, .idx = CDC_INTERFACE, .len = 7, .fn = acm_set_line_coding },
    { .req = 0x21a1, .idx = CDC_INTERFACE, .len = 7, .fn = acm_get_line_coding },
    { .req = 0x0102, .idx = CDC_ENDPOINT,  .len = 0, .fn = usbfs_handle_clear_feature_endpoint },
    { .req = 0x0b01, .idx = ACM_INTERFACE, .len = 0, .fn = usbfs_handle_set_interface0 },
    { .req = 0x0102, .idx = ACM_ENDPOINT,  .len = 0, .fn = usbfs_handle_clear_feature_endpoint },
};

static int
usbfs_setup_handler_run(const struct usb_setup_packet *p, const void **data) {
    uint8_t idx = p->wIndex;
    const struct usb_setup_handler *h;

    for (h = usbfs_setup_handlers; h < ARRAY_END(usbfs_setup_handlers); h++) {
        if (h->req == p->request && (h->idx == 0xFFU || h->idx == idx)) {
            if (h->len != 0xFFU && h->len != p->wLength) {
                break;
            }
            return h->fn(p, data);
        }
    }

    debug("unknown request:\n"
        "  bmRequestType 0x%02x\n"
        "  bRequest      0x%02x\n"
        "  wValue        0x%04x\n"
        "  wIndex        0x%04x\n"
        "  wLength       0x%04x\n",
        p->bmRequestType,
        p->bRequest,
        p->wValue,
        p->wIndex,
        p->wLength);
    return -1;
}

static void
usbfs_handle_setup(void) {
    const struct usb_setup_packet *p = &usbfs_outbuf.setup;

    usbfs_state.bytes = 0;

    if (p->bmRequestType & 0x80U) {
        const void *data;
        int ret = usbfs_setup_handler_run(p, &data);

        if (ret >= 0) {
            /* send IN data */
            if (ret > p->wLength) {
                ret = p->wLength;
            }
            usbfs_state.ep0in = data;
            usbfs_state.bytes = ret;
            usbfs_ep0in_transfer();
            /* prepare for IN ack */
            usbfs_ep0out_prepare_out();
            return;
        }
    } else if (p->wLength == 0) {
        const void *data;

        if (!usbfs_setup_handler_run(p, &data)) {
            /* send empty ack package */
            usbfs_ep0in_transfer_empty();
            /* prepare for next SETUP package */
            usbfs_ep0out_prepare_setup();
            return;
        }
    } else if (p->wLength <= ARRAY_SIZE(usbfs_outbuf.data)) {
        /* receive OUT data */
        usbfs_ep0out_prepare_out();
        usbfs_state.bytes = p->wLength;
        return;
    }

    /* stall IN endpoint */
    usbfs_ep0in_stall();
    /* prepare for next SETUP package */
    usbfs_ep0out_prepare_setup();
}

static void
usbfs_handle_rx0(bool setup, unsigned int len) {
    if (setup) {
        for (; len > 8; len -= 4) {
            (void)USBFS->DFIFO[0][0];
        }
        usbfs_state.ep0out = usbfs_outbuf.v;
        *usbfs_state.ep0out++ = USBFS->DFIFO[0][0];
        *usbfs_state.ep0out++ = USBFS->DFIFO[0][0];
    } else {
        while (1) {
            *usbfs_state.ep0out++ = USBFS->DFIFO[0][0];
            if (len <= 4) {
                break;
            }
            len -= 4;
        }
    }
}

static void
usbfs_handle_ep0(void) {
    uint32_t oflags = USBFS->DOEP[0].INTF;
    uint32_t iflags = USBFS->DIEP[0].INTF;
    uint32_t bytes;

    USBFS->DOEP[0].INTF = oflags;
    USBFS->DIEP[0].INTF = iflags;

    // debug("EP0 %04lx %04lx %lu\n", oflags, iflags, usbfs_state.bytes);

    if (oflags & USBFS_DOEPINTF_STPF) {
        usbfs_handle_setup();
        return;
    }

    bytes = usbfs_state.bytes;
    if (bytes == 0) {
        return;
    }

    if (iflags & USBFS_DIEPINTF_TF) {
        /* data IN */
        if (bytes > usbfs_state.packetsize) {
            /* send next package */
            usbfs_state.ep0in += usbfs_state.packetsize;
            usbfs_state.bytes = bytes - usbfs_state.packetsize;
            usbfs_ep0in_transfer();
        } else {
            usbfs_state.bytes = 0;
        }
    } else if (oflags & USBFS_DOEPINTF_TF) {
        /* data OUT */
        bytes = usbfs_state.packetsize - (USBFS->DOEP[0].LEN & USBFS_DOEPLEN_TLEN_Msk);
        if (usbfs_state.bytes > bytes) {
            usbfs_state.bytes -= bytes;
            /* prepare for more OUT data */
            usbfs_ep0out_prepare_out();
        } else {
            const void *data = usbfs_outbuf.data;

            usbfs_state.bytes = 0;
            if (!usbfs_setup_handler_run(&usbfs_outbuf.setup, &data)) {
                /* send empty ack package */
                usbfs_ep0in_transfer_empty();
            } else {
                usbfs_ep0in_stall();
            }
            usbfs_ep0out_prepare_setup();
        }
    }
}

static void
acm_handle_rx(bool setup, unsigned int len) {
    for (uint32_t *p = acm_outbuf.word;; p++) {
        *p = USBFS->DFIFO[ACM_ENDPOINT][0];
        if (len <= 4) {
            break;
        }
        len -= 4;
    }
}

static void
acm_handle_ep(void) {
    uint32_t flags = USBFS->DOEP[ACM_ENDPOINT].INTF;

    USBFS->DOEP[ACM_ENDPOINT].INTF = flags;

    /*
    debug("ACM OUT: 0x%02lx (0x%08lx)\n", flags,
                USBFS->DOEP[ACM_ENDPOINT].CTL);
    */

    if (flags & USBFS_DOEPINTF_TF) {
        unsigned int bytes = ACM_PACKETSIZE -
            (USBFS->DOEP[ACM_ENDPOINT].LEN & USBFS_DOEPLEN_TLEN_Msk);

        acm_outbytes = bytes;
    }

    flags = USBFS->DIEP[ACM_ENDPOINT].INTF;

    USBFS->DIEP[ACM_ENDPOINT].INTF = flags;

    /*
    debug("ACM  IN: 0x%02lx (0x%08lx)\n", flags,
                USBFS->DIEP[ACM_ENDPOINT].CTL);
    */

    if (flags & USBFS_DIEPINTF_TF) {
        acm_send();
    }
}

struct {
    void (*rx)(bool setup, unsigned int len);
    void (*ep)(void);
} usbfs_endpoints[] = {
    {
        .rx = usbfs_handle_rx0,
        .ep = usbfs_handle_ep0,
    },
    {
        .rx = acm_handle_rx,
        .ep = acm_handle_ep,
    },
};

static void
usbfs_handle_rxdata(void) {
    uint32_t grstat = USBFS->GRSTATP;
    unsigned int len = (grstat & USBFS_GRSTAT_BCOUNT_Msk) >> USBFS_GRSTAT_BCOUNT_Pos;
    unsigned int ep;
    bool setup;

    if (len == 0) {
        return;
    }

    ep = grstat & USBFS_GRSTAT_EPNUM_Msk;
    setup = (grstat & USBFS_GRSTAT_RPCKST_Msk) == USBFS_GRSTAT_RPCKST_STP;
    usbfs_endpoints[ep].rx(setup, len);
}

static void
usbfs_handle_endpoints(void) {
    uint32_t flags = USBFS->DAEPINT;
    uint32_t mask = 0x10001U;

    for (unsigned int i = 0; i < ARRAY_SIZE(usbfs_endpoints); i++, mask <<= 1) {
        if (flags & mask) {
            usbfs_endpoints[i].ep();
        }
    }
}

void
USBFS_IRQHandler(void) {
    uint32_t flags = USBFS->GINTF;

    // debug("flags = %08lx\n", flags);

    /* read all incoming packets */
    while ((flags & USBFS_GINTF_RXFNEIF)) {
        usbfs_handle_rxdata();
        flags = USBFS->GINTF;
    }

    if (flags & (USBFS_GINTF_OEPIF | USBFS_GINTF_IEPIF)) {
        usbfs_handle_endpoints();
    }

    /*
    if (!(flags & (
                USBFS_GINTF_SP |
                USBFS_GINTF_WKUPIF |
                USBFS_GINTF_RST |
                USBFS_GINTF_ENUMF)))
        return;
    */

    if (flags & USBFS_GINTF_SP) {
        debug("SUSPEND.. ");
        usbfs_suspend();
        USBFS->GINTF = USBFS_GINTF_SP;
        debug("done\n");
        return;
    }
    if (flags & USBFS_GINTF_WKUPIF) {
        debug("WAKEUP.. ");
        usbfs_wakeup();
        USBFS->GINTF = USBFS_GINTF_WKUPIF;
        debug("done\n");
    }
    if (flags & USBFS_GINTF_RST) {
        debug("RESET.. ");
        usbfs_reset();
        USBFS->GINTF = USBFS_GINTF_RST;
        debug("done\n");
    }
    if (flags & USBFS_GINTF_ENUMF) {
        debug("ENUMDONE.. ");
        usbfs_enumdone();
        USBFS->GINTF = USBFS_GINTF_ENUMF;
        debug("done\n");
    }
}

static void
usbfs_allocate_buffers(uint32_t rx,
    uint32_t tx0, uint32_t tx1, uint32_t tx2, uint32_t tx3) {
    /* round up to number of 32bit words */
    rx = (rx + 3) >> 2;
    tx0 = (tx0 + 3) >> 2;
    tx1 = (tx1 + 3) >> 2;
    tx2 = (tx2 + 3) >> 2;
    tx3 = (tx3 + 3) >> 2;
    USBFS->GRFLEN = rx;
    debug("GRFLEN     (0x%03x) = %3lu       = 0x%08lx\n",
        (uintptr_t)&USBFS->GRFLEN - (uintptr_t)USBFS,
        rx,
        USBFS->GRFLEN);
    USBFS->DIEP0TFLEN = (tx0 << 16) | rx;
    debug("DIEP0TFLEN (0x%03x) = %3lu, %4lu = 0x%08lx\n",
        (uintptr_t)&USBFS->DIEP0TFLEN - (uintptr_t)USBFS,
        tx0, rx,
        USBFS->DIEP0TFLEN);
    USBFS->DIEP1TFLEN = (tx1 << 16) | (rx + tx0);
    debug("DIEP1TFLEN (0x%03x) = %3lu, %4lu = 0x%08lx\n",
        (uintptr_t)&USBFS->DIEP1TFLEN - (uintptr_t)USBFS,
        tx1, (rx + tx0),
        USBFS->DIEP1TFLEN);
    USBFS->DIEP2TFLEN = (tx2 << 16) | (rx + tx0 + tx1);
    debug("DIEP2TFLEN (0x%03x) = %3lu, %4lu = 0x%08lx\n",
        (uintptr_t)&USBFS->DIEP2TFLEN - (uintptr_t)USBFS,
        tx2, (rx + tx0 + tx1),
        USBFS->DIEP2TFLEN);
    USBFS->DIEP3TFLEN = (tx3 << 16) | (rx + tx0 + tx1 + tx2);
    debug("DIEP3TFLEN (0x%03x) = %3lu, %4lu = 0x%08lx\n",
        (uintptr_t)&USBFS->DIEP3TFLEN - (uintptr_t)USBFS,
        tx3, (rx + tx0 + tx1 + tx2),
        USBFS->DIEP3TFLEN);
}

void
usbacm_init(void) {
    /* turn on USBFS clock */
    RCU->AHBEN |= RCU_AHBEN_USBFSEN;

    /* turn on GPIOA and AFIO */
    RCU->APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_AFEN;

    /* reset USBFS */
    RCU->AHBRST |= RCU_AHBRST_USBFSRST;
    RCU->AHBRST &= ~RCU_AHBRST_USBFSRST;

    /* disable global interrupt flag */
    USBFS->GAHBCS = 0U;

    /* disable Vbus sensing */
    USBFS->GCCFG = USBFS_GCCFG_VBUSIG;

    debug("core reset");
    USBFS->GRSTCTL = USBFS_GRSTCTL_CSRST;
    while ((USBFS->GRSTCTL & USBFS_GRSTCTL_CSRST)) {
        debug(".");
    }
    debug(" done\n");
    usbfs_udelay(3);

    /* force device mode */
    debug("switching to device mode");
    USBFS->GUSBCS |= USBFS_GUSBCS_FDM;
    while ((USBFS->GINTF & USBFS_GINTF_COPM)) {
        debug(".");
    }
    debug(" done\n");

    /* manual says: "the application must wait at
     * least 25ms for [FDM to] take effect" */
    usbfs_udelay(25000);

    /* initialize device */
    USBFS->DCFG =
        USBFS_DCFG_EOPFT_80PCT |
        USBFS_DCFG_DS_FULL;

    /* disconnect */
    USBFS->DCTL = USBFS_DCTL_SD;

    /* now that we're disconnected, power on phy */
    USBFS->GCCFG =
        USBFS_GCCFG_VBUSIG |
        /* USBFS_GCCFG_VBUSACEN | */
        USBFS_GCCFG_VBUSBCEN |
        USBFS_GCCFG_PWRON;

    /* setup fifo allocation */
    usbfs_allocate_buffers(USBFS_FIFO_RXSIZE,
        USBFS_FIFO_TX0SIZE,
        USBFS_FIFO_TX1SIZE,
        USBFS_FIFO_TX2SIZE,
        USBFS_FIFO_TX3SIZE);

    /* flush all tx fifos */
    usbfs_txfifos_flush();

    /* flush rx fifo */
    USBFS->GRSTCTL |= USBFS_GRSTCTL_RXFF;
    while ((USBFS->GRSTCTL & USBFS_GRSTCTL_RXFF)) {
        /* wait */
    }
    /* wait 3 more phy clocks */
    usbfs_udelay(3);

    USBFS->DIEPINTEN = 0U;
    USBFS->DOEPINTEN = 0U;
    USBFS->DAEPINTEN = 0U;

    /* reset endpoint registers */
    usbfs_ep_reset();

    /* clear all sticky interrupts */
    USBFS->GINTF =
        USBFS_GINTF_WKUPIF |
        USBFS_GINTF_SESIF |
        USBFS_GINTF_DISCIF |
        USBFS_GINTF_IDPSC |
        USBFS_GINTF_ISOONCIF |
        USBFS_GINTF_ISOINCIF |
        USBFS_GINTF_EOPFIF |
        USBFS_GINTF_ISOOPDIF |
        USBFS_GINTF_ENUMF |
        USBFS_GINTF_RST |
        USBFS_GINTF_SP |
        USBFS_GINTF_ESP |
        USBFS_GINTF_SOF |
        USBFS_GINTF_MFIF;

    /* enable interrupts */
    USBFS->GINTEN =
        USBFS_GINTEN_WKUPIE |
        USBFS_GINTEN_OEPIE |
        USBFS_GINTEN_IEPIE |
        USBFS_GINTEN_ENUMFIE |
        USBFS_GINTEN_RSTIE |
        USBFS_GINTEN_RXFNEIE |
        USBFS_GINTEN_SPIE;

    /* enable eclic interrupt */
    eclic_config(USBFS_IRQn, ECLIC_ATTR_TRIG_LEVEL, 4);
    eclic_enable(USBFS_IRQn);

    /* set usb global interrupt flag */
    USBFS->GAHBCS |= USBFS_GAHBCS_GINTEN;

    /* connect */
    USBFS->DCTL &= ~USBFS_DCTL_SD;
}
