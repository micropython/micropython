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


// USB Video Class (UVC) host driver.
//
// Binds to the first camera attached, keeping a copy of its video function descriptors
// so formats can be listed and streaming set up on demand from Python.  The video
// stream itself is received over an isochronous endpoint by mp_usbh_iso_in_start(), and
// frames are assembled in the IRQ handler into a set of rotating frame buffers.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "mp_usbh_uvc.h"

#if MICROPY_HW_USB_HOST && MICROPY_HW_USB_HOST_UVC

#include "class/video/video.h"

#define UVC_CTRL_TIMEOUT_MS (1000)

// Video probe and commit control lengths, which depend on the UVC version.
#define UVC_PROBE_LEN_1_0 (26)
#define UVC_PROBE_LEN_1_1 (34)
#define UVC_PROBE_LEN_1_5 (48)

// Offsets of the fields used in the video probe and commit control.
#define UVC_PROBE_BMHINT (0)
#define UVC_PROBE_FORMAT_INDEX (2)
#define UVC_PROBE_FRAME_INDEX (3)
#define UVC_PROBE_FRAME_INTERVAL (4)
#define UVC_PROBE_MAX_FRAME_SIZE (18)
#define UVC_PROBE_MAX_PAYLOAD_SIZE (22)

// Payload header bmHeaderInfo bits.
#define UVC_PAYLOAD_FID (0x01)
#define UVC_PAYLOAD_EOF (0x02)
#define UVC_PAYLOAD_ERR (0x40)

typedef struct {
    uint8_t *buf[MP_USBH_UVC_NUM_BUFS];
    size_t bufsize;
    uint8_t fill;           // buffer being written by the IRQ
    uint8_t ready;          // latest complete frame
    uint8_t user;           // buffer handed out by mp_usbh_uvc_frame_take()
    size_t fill_len;
    size_t ready_len;
    uint32_t ready_seq;
    uint32_t user_seq;
    uint32_t expected_len;  // exact frame size for uncompressed formats, otherwise 0
    bool mjpeg;
    bool synced;            // the fill buffer started at a frame boundary
    bool fill_err;
    bool fill_overflow;
    uint8_t fid;
    mp_usbh_uvc_stats_t stats;
} uvc_stream_t;

typedef struct {
    uint8_t daddr;
    uint8_t vc_itf;
    uint8_t vs_itf;
    uint16_t bcd_uvc;
    uint16_t desc_len;
    uint16_t probe_len;
    volatile bool streaming;
    uvc_stream_t stream;
} uvc_state_t;

static uvc_state_t uvc;

// Descriptors of the camera's video function, from the VideoControl interface onwards.
static uint8_t uvc_desc[CFG_TUH_ENUMERATION_BUFSIZE];

static inline bool uvc_desc_valid(const uint8_t *p, const uint8_t *end) {
    return p + 2 <= end && p[0] >= 2 && p + p[0] <= end;
}

// Frames handed out by mp_usbh_uvc_frame_take() keep their buffer alive if referenced.
static void uvc_release_bufs(void) {
    for (size_t i = 0; i < MP_USBH_UVC_NUM_BUFS; ++i) {
        MP_STATE_VM(usbh_uvc_bufs)[i] = NULL;
    }
}

/******************************************************************************/
// Frame assembly, called from the host controller IRQ.

static bool uvc_frame_valid(const uvc_stream_t *s) {
    const uint8_t *buf = s->buf[s->fill];
    if (s->expected_len) {
        return s->fill_len == s->expected_len;
    }
    if (s->mjpeg) {
        // Must start with a JPEG start of image marker.
        return s->fill_len >= 2 && buf[0] == 0xff && buf[1] == 0xd8;
    }
    return true;
}

static void uvc_frame_end(uvc_stream_t *s) {
    if (!s->synced) {
        // Streaming may have started part way through this frame, so discard it.
        s->synced = true;
    } else if (s->fill_len == 0 && !s->fill_err) {
        // Nothing received.
    } else if (s->fill_err || !uvc_frame_valid(s)) {
        s->stats.dropped++;
    } else {
        uint8_t done = s->fill;
        s->fill = s->ready;
        s->ready = done;
        s->ready_len = s->fill_len;
        s->ready_seq++;
        s->stats.frames++;
    }
    s->fill_len = 0;
    s->fill_err = false;
    s->fill_overflow = false;
}

static void uvc_iso_cb(void *arg, const uint8_t *data, size_t len, bool ok) {
    uvc_stream_t *s = arg;

    if (!ok) {
        s->stats.errors++;
        s->fill_err = true;
        return;
    }

    if (len < 2) {
        // No payload in this (micro)frame.
        return;
    }

    const uint8_t header_len = data[0];
    const uint8_t info = data[1];
    if (header_len < 2 || header_len > len) {
        s->fill_err = true;
        return;
    }

    // A change of frame ID marks the start of a new frame, in case the previous frame
    // wasn't terminated with EOF.
    const uint8_t fid = info & UVC_PAYLOAD_FID;
    if (fid != s->fid) {
        if (s->fid != 0xff) {
            uvc_frame_end(s);
        }
        s->fid = fid;
    }

    if (info & UVC_PAYLOAD_ERR) {
        s->fill_err = true;
    }

    size_t n = len - header_len;
    if (n > 0) {
        if (s->fill_len + n > s->bufsize) {
            if (!s->fill_overflow) {
                s->fill_overflow = true;
                if (s->synced) {
                    s->stats.overflows++;
                }
            }
            s->fill_err = true;
        } else {
            memcpy(s->buf[s->fill] + s->fill_len, data + header_len, n);
            s->fill_len += n;
        }
    }

    if (info & UVC_PAYLOAD_EOF) {
        uvc_frame_end(s);
    }
}

/******************************************************************************/
// TinyUSB class driver.

static bool uvch_init(void) {
    memset(&uvc, 0, sizeof(uvc));
    return true;
}

static bool uvch_deinit(void) {
    // The isochronous stream has already been stopped by mp_usbh_deinit().
    memset(&uvc, 0, sizeof(uvc));
    return true;
}

static uint16_t uvch_open(uint8_t rhport, uint8_t daddr, const tusb_desc_interface_t *itf_desc, uint16_t max_len) {
    (void)rhport;

    if (itf_desc->bInterfaceClass != TUSB_CLASS_VIDEO || itf_desc->bInterfaceSubClass != VIDEO_SUBCLASS_CONTROL) {
        return 0;
    }
    if (uvc.daddr != 0) {
        // Only one camera is supported at a time.
        return 0;
    }

    // Claim the VideoControl interface along with the VideoStreaming interfaces that follow it.
    const uint8_t *start = (const uint8_t *)itf_desc;
    const uint8_t *end = start + max_len;
    const uint8_t *p = start + itf_desc->bLength;
    uint16_t bcd_uvc = 0;
    uint8_t vs_itf = 0xff;
    while (uvc_desc_valid(p, end)) {
        if (p[1] == TUSB_DESC_INTERFACE_ASSOCIATION) {
            break;
        }
        if (p[1] == TUSB_DESC_INTERFACE) {
            const tusb_desc_interface_t *itf = (const tusb_desc_interface_t *)p;
            if (itf->bInterfaceClass != TUSB_CLASS_VIDEO || itf->bInterfaceSubClass != VIDEO_SUBCLASS_STREAMING) {
                break;
            }
            if (vs_itf == 0xff) {
                vs_itf = itf->bInterfaceNumber;
            }
        } else if (p[1] == TUSB_DESC_CS_INTERFACE && vs_itf == 0xff && p[2] == VIDEO_CS_ITF_VC_HEADER && p[0] >= 5) {
            bcd_uvc = tu_le16toh(tu_unaligned_read16(p + 3));
        }
        p += p[0];
    }

    uint16_t len = p - start;
    if (vs_itf == 0xff || len > sizeof(uvc_desc)) {
        return 0;
    }

    memcpy(uvc_desc, start, len);
    uvc.desc_len = len;
    uvc.daddr = daddr;
    uvc.vc_itf = itf_desc->bInterfaceNumber;
    uvc.vs_itf = vs_itf;
    uvc.bcd_uvc = bcd_uvc;
    return len;
}

static bool uvch_set_config(uint8_t daddr, uint8_t itf_num) {
    // Nothing to configure until streaming is started.
    usbh_driver_set_config_complete(daddr, itf_num);
    return true;
}

static bool uvch_xfer_cb(uint8_t daddr, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes) {
    // No transfers are submitted through TinyUSB.
    (void)daddr;
    (void)ep_addr;
    (void)result;
    (void)xferred_bytes;
    return true;
}

static void uvch_close(uint8_t daddr) {
    if (daddr != uvc.daddr) {
        return;
    }
    if (uvc.streaming) {
        // Called from the host task, so leave the buffers to stream_stop() or deinit().
        mp_usbh_iso_in_stop();
        uvc.streaming = false;
    }
    uvc.daddr = 0;
    uvc.desc_len = 0;
}

const usbh_class_driver_t mp_usbh_uvc_driver = {
    .name = "UVC",
    .init = uvch_init,
    .deinit = uvch_deinit,
    .open = uvch_open,
    .set_config = uvch_set_config,
    .xfer_cb = uvch_xfer_cb,
    .close = uvch_close,
};

/******************************************************************************/
// Public API, called from thread context.

uint8_t mp_usbh_uvc_daddr(void) {
    return uvc.daddr;
}

void mp_usbh_uvc_iter_init(mp_usbh_uvc_iter_t *iter) {
    memset(iter, 0, sizeof(*iter));
}

static void uvc_set_fourcc(char *fourcc, const uint8_t *src) {
    for (int i = 0; i < 4; ++i) {
        fourcc[i] = (src[i] >= 0x20 && src[i] < 0x7f) ? src[i] : '?';
    }
}

const mp_usbh_uvc_frame_t *mp_usbh_uvc_iter_next(mp_usbh_uvc_iter_t *iter) {
    const uint8_t *end = uvc_desc + uvc.desc_len;
    mp_usbh_uvc_frame_t *frame = &iter->frame;

    // Walk the class-specific descriptors of the VideoStreaming interface's default alternate
    // setting, where each format descriptor is followed by its frame descriptors.
    while (uvc_desc_valid(uvc_desc + iter->pos, end)) {
        const uint8_t *d = uvc_desc + iter->pos;
        iter->pos += d[0];

        if (d[1] == TUSB_DESC_INTERFACE) {
            const tusb_desc_interface_t *itf = (const tusb_desc_interface_t *)d;
            iter->in_vs = itf->bInterfaceNumber == uvc.vs_itf && itf->bAlternateSetting == 0;
            continue;
        }
        if (!iter->in_vs || d[1] != TUSB_DESC_CS_INTERFACE) {
            continue;
        }

        const uint8_t subtype = d[2];
        if (subtype == VIDEO_CS_ITF_VS_FORMAT_UNCOMPRESSED && d[0] >= 27) {
            uvc_set_fourcc(frame->fourcc, d + 5);
            frame->format_index = d[3];
            frame->bits_per_pixel = d[21];
            iter->default_frame_index = d[22];
            iter->frame_subtype = VIDEO_CS_ITF_VS_FRAME_UNCOMPRESSED;
        } else if (subtype == VIDEO_CS_ITF_VS_FORMAT_MJPEG && d[0] >= 11) {
            memcpy(frame->fourcc, "MJPG", 4);
            frame->format_index = d[3];
            frame->bits_per_pixel = 0;
            iter->default_frame_index = d[6];
            iter->frame_subtype = VIDEO_CS_ITF_VS_FRAME_MJPEG;
        } else if (subtype == VIDEO_CS_ITF_VS_FORMAT_FRAME_BASED && d[0] >= 28) {
            uvc_set_fourcc(frame->fourcc, d + 5);
            frame->format_index = d[3];
            frame->bits_per_pixel = 0;
            iter->default_frame_index = d[22];
            iter->frame_subtype = VIDEO_CS_ITF_VS_FRAME_FRAME_BASED;
        } else if (subtype >= VIDEO_CS_ITF_VS_FORMAT_UNCOMPRESSED && subtype != VIDEO_CS_ITF_VS_COLORFORMAT
                   && subtype != iter->frame_subtype) {
            // Some other format (or frame of one) that isn't supported.
            iter->frame_subtype = 0;
        } else if (subtype == iter->frame_subtype && iter->frame_subtype != 0 && d[0] >= 26) {
            uint8_t interval_type;
            if (subtype == VIDEO_CS_ITF_VS_FRAME_FRAME_BASED) {
                frame->max_frame_size = 0;
                frame->default_interval = tu_le32toh(tu_unaligned_read32(d + 17));
                interval_type = d[21];
            } else {
                frame->max_frame_size = tu_le32toh(tu_unaligned_read32(d + 17));
                frame->default_interval = tu_le32toh(tu_unaligned_read32(d + 21));
                interval_type = d[25];
            }
            if (d[0] < 26 + 4 * (interval_type ? interval_type : 3)) {
                continue;
            }
            frame->frame_index = d[3];
            frame->is_default = d[3] == iter->default_frame_index;
            frame->width = tu_le16toh(tu_unaligned_read16(d + 5));
            frame->height = tu_le16toh(tu_unaligned_read16(d + 7));
            frame->interval_type = interval_type;
            frame->intervals = d + 26;
            return frame;
        }
    }

    return NULL;
}

static int uvc_vs_request(uint8_t request, uint8_t control, uint8_t *data, uint16_t len) {
    uint8_t bmRequestType = (request & 0x80) ? 0xa1 : 0x21; // class, interface, IN or OUT
    return mp_usbh_control_xfer(uvc.daddr, bmRequestType, request, control << 8, uvc.vs_itf, data, len, UVC_CTRL_TIMEOUT_MS);
}

static int uvc_set_interface(uint8_t alt) {
    return mp_usbh_control_xfer(uvc.daddr, 0x01, TUSB_REQ_SET_INTERFACE, alt, uvc.vs_itf, NULL, 0, UVC_CTRL_TIMEOUT_MS);
}

int mp_usbh_uvc_negotiate(const mp_usbh_uvc_frame_t *frame, uint32_t interval, mp_usbh_uvc_commit_t *commit) {
    if (uvc.daddr == 0) {
        return -MP_ENODEV;
    }
    if (uvc.streaming) {
        return -MP_EBUSY;
    }

    uint16_t len = UVC_PROBE_LEN_1_0;
    if (uvc.bcd_uvc >= 0x0150) {
        len = UVC_PROBE_LEN_1_5;
    } else if (uvc.bcd_uvc >= 0x0110) {
        len = UVC_PROBE_LEN_1_1;
    }

    // Like Linux, select alternate setting 0 before anything else.  Cameras can misbehave
    // without it, eg if they were left streaming by a host that was then reset.
    int ret = uvc_set_interface(0);
    if (ret < 0) {
        return ret;
    }

    uint8_t probe[UVC_PROBE_LEN_1_5];
    for (;;) {
        // The camera fills in defaults for the fields left as zero.
        memset(probe, 0, sizeof(probe));
        tu_unaligned_write16(probe + UVC_PROBE_BMHINT, tu_htole16(1)); // keep dwFrameInterval fixed
        probe[UVC_PROBE_FORMAT_INDEX] = frame->format_index;
        probe[UVC_PROBE_FRAME_INDEX] = frame->frame_index;
        tu_unaligned_write32(probe + UVC_PROBE_FRAME_INTERVAL, tu_htole32(interval));
        ret = uvc_vs_request(VIDEO_REQUEST_SET_CUR, VIDEO_VS_CTL_PROBE, probe, len);

        // Some cameras only accept the UVC 1.0 length, regardless of their version.
        if (ret == -MP_EPIPE && len > UVC_PROBE_LEN_1_0) {
            len = UVC_PROBE_LEN_1_0;
            continue;
        }
        break;
    }
    if (ret < 0) {
        return ret;
    }

    // Read back what the camera agreed to, then commit it.
    ret = uvc_vs_request(VIDEO_REQUEST_GET_CUR, VIDEO_VS_CTL_PROBE, probe, len);
    if (ret < 0) {
        return ret;
    }
    if (ret < UVC_PROBE_LEN_1_0) {
        return -MP_EIO;
    }
    ret = uvc_vs_request(VIDEO_REQUEST_SET_CUR, VIDEO_VS_CTL_COMMIT, probe, len);
    if (ret < 0) {
        return ret;
    }

    uvc.probe_len = len;
    commit->interval = tu_le32toh(tu_unaligned_read32(probe + UVC_PROBE_FRAME_INTERVAL));
    commit->max_frame_size = tu_le32toh(tu_unaligned_read32(probe + UVC_PROBE_MAX_FRAME_SIZE));
    commit->max_payload_size = tu_le32toh(tu_unaligned_read32(probe + UVC_PROBE_MAX_PAYLOAD_SIZE));
    return 0;
}

// Find the alternate setting of the VideoStreaming interface with the least bandwidth
// that can carry max_payload_size bytes per (micro)frame, or the most if none can.
static const tusb_desc_endpoint_t *uvc_find_alt_setting(uint32_t max_payload_size, uint8_t *alt_out, uint32_t *xfer_size_out) {
    const uint8_t *end = uvc_desc + uvc.desc_len;
    const bool high_speed = tuh_speed_get(uvc.daddr) == TUSB_SPEED_HIGH;

    const tusb_desc_endpoint_t *best = NULL;
    uint8_t best_alt = 0;
    uint32_t best_size = 0;
    const tusb_desc_endpoint_t *largest = NULL;
    uint8_t largest_alt = 0;
    uint32_t largest_size = 0;

    bool in_vs = false;
    uint8_t alt = 0;
    for (const uint8_t *p = uvc_desc; uvc_desc_valid(p, end); p += p[0]) {
        if (p[1] == TUSB_DESC_INTERFACE) {
            const tusb_desc_interface_t *itf = (const tusb_desc_interface_t *)p;
            in_vs = itf->bInterfaceNumber == uvc.vs_itf;
            alt = itf->bAlternateSetting;
        } else if (p[1] == TUSB_DESC_ENDPOINT && in_vs && alt > 0) {
            const tusb_desc_endpoint_t *ep = (const tusb_desc_endpoint_t *)p;
            if (ep->bmAttributes.xfer != TUSB_XFER_ISOCHRONOUS || tu_edpt_dir(ep->bEndpointAddress) != TUSB_DIR_IN) {
                continue;
            }
            uint16_t wMaxPacketSize = tu_le16toh(ep->wMaxPacketSize);
            uint32_t size = wMaxPacketSize & 0x7ff;
            if (high_speed) {
                size *= MIN(((wMaxPacketSize >> 11) & 3) + 1, 3);
            }
            if (size >= max_payload_size && (best == NULL || size < best_size)) {
                best = ep;
                best_alt = alt;
                best_size = size;
            }
            if (size > largest_size) {
                largest = ep;
                largest_alt = alt;
                largest_size = size;
            }
        }
    }

    if (best == NULL || max_payload_size == 0) {
        best = largest;
        best_alt = largest_alt;
        best_size = largest_size;
    }
    *alt_out = best_alt;
    *xfer_size_out = best_size;
    return best;
}

int mp_usbh_uvc_stream_start(const mp_usbh_uvc_frame_t *frame, const mp_usbh_uvc_commit_t *commit, size_t bufsize) {
    if (uvc.daddr == 0) {
        return -MP_ENODEV;
    }
    if (uvc.streaming) {
        return -MP_EBUSY;
    }

    uint8_t alt;
    uint32_t xfer_size;
    const tusb_desc_endpoint_t *ep = uvc_find_alt_setting(commit->max_payload_size, &alt, &xfer_size);
    if (ep == NULL) {
        // Cameras that stream over a bulk endpoint aren't supported yet.
        return -MP_EOPNOTSUPP;
    }

    // The buffers are root pointers, so they're kept while the IRQ writes to them.
    uvc_stream_t *s = &uvc.stream;
    memset(s, 0, sizeof(*s));
    uvc_release_bufs();
    for (size_t i = 0; i < MP_USBH_UVC_NUM_BUFS; ++i) {
        s->buf[i] = MP_STATE_VM(usbh_uvc_bufs)[i] = m_new(uint8_t, bufsize);
    }
    s->bufsize = bufsize;
    s->fill = 0;
    s->ready = 1;
    s->user = 2;
    s->fid = 0xff;
    s->mjpeg = memcmp(frame->fourcc, "MJPG", 4) == 0;
    if (frame->bits_per_pixel) {
        s->expected_len = (uint32_t)frame->width * frame->height * frame->bits_per_pixel / 8;
    }
    s->stats.alt_setting = alt;
    s->stats.xfer_size = xfer_size;

    int ret = uvc_set_interface(alt);
    if (ret < 0) {
        uvc_release_bufs();
        return ret;
    }

    ret = mp_usbh_iso_in_start(uvc.daddr, ep, uvc_iso_cb, s);
    if (ret < 0) {
        uvc_set_interface(0);
        uvc_release_bufs();
        return ret;
    }

    uvc.streaming = true;
    return 0;
}

void mp_usbh_uvc_stream_stop(void) {
    if (!uvc.streaming) {
        return;
    }
    mp_usbh_iso_in_stop();
    uvc.streaming = false;
    uvc_release_bufs();

    // Tell the camera to stop streaming and release its bandwidth.
    if (uvc.daddr != 0) {
        uvc_set_interface(0);
    }
}

void mp_usbh_uvc_deinit(void) {
    // Called on soft reset, so mustn't raise.
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_usbh_uvc_stream_stop();
        nlr_pop();
    }
    mp_usbh_iso_in_stop();
    uvc.streaming = false;
    uvc_release_bufs();
}

bool mp_usbh_uvc_is_streaming(void) {
    return uvc.streaming;
}

bool mp_usbh_uvc_frame_take(uint8_t **buf, size_t *len) {
    uvc_stream_t *s = &uvc.stream;
    bool new_frame = false;

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (s->ready_seq != s->user_seq) {
        uint8_t ready = s->ready;
        s->ready = s->user;
        s->user = ready;
        s->user_seq = s->ready_seq;
        *len = s->ready_len;
        new_frame = true;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    if (new_frame) {
        *buf = s->buf[s->user];
    }
    return new_frame;
}

/******************************************************************************/
// Camera controls, in the camera terminal and processing unit.

#define UVC_ENTITY_CT (0) // camera terminal
#define UVC_ENTITY_PU (1) // processing unit

#define UVC_CTRL_SIGNED (0x01)
#define UVC_CTRL_AE_MODE (0x02) // auto-exposure mode bitmap, exposed as a bool

typedef struct {
    const char *name;
    uint8_t entity;
    uint8_t selector;
    uint8_t bit;  // bit in the entity's bmControls
    uint8_t size; // bytes
    uint8_t flags;
} uvc_control_t;

static const uvc_control_t uvc_controls[] = {
    {"exposure_auto", UVC_ENTITY_CT, 0x02, 1, 1, UVC_CTRL_AE_MODE},
    {"exposure_priority", UVC_ENTITY_CT, 0x03, 2, 1, 0},
    {"exposure", UVC_ENTITY_CT, 0x04, 3, 4, 0},
    {"focus", UVC_ENTITY_CT, 0x06, 5, 2, 0},
    {"focus_auto", UVC_ENTITY_CT, 0x08, 17, 1, 0},
    {"iris", UVC_ENTITY_CT, 0x09, 7, 2, 0},
    {"zoom", UVC_ENTITY_CT, 0x0b, 9, 2, 0},
    {"privacy", UVC_ENTITY_CT, 0x11, 18, 1, 0},
    {"backlight_compensation", UVC_ENTITY_PU, 0x01, 8, 2, 0},
    {"brightness", UVC_ENTITY_PU, 0x02, 0, 2, UVC_CTRL_SIGNED},
    {"contrast", UVC_ENTITY_PU, 0x03, 1, 2, 0},
    {"gain", UVC_ENTITY_PU, 0x04, 9, 2, 0},
    {"power_line_frequency", UVC_ENTITY_PU, 0x05, 10, 1, 0},
    {"hue", UVC_ENTITY_PU, 0x06, 2, 2, UVC_CTRL_SIGNED},
    {"saturation", UVC_ENTITY_PU, 0x07, 3, 2, 0},
    {"sharpness", UVC_ENTITY_PU, 0x08, 4, 2, 0},
    {"gamma", UVC_ENTITY_PU, 0x09, 5, 2, 0},
    {"white_balance", UVC_ENTITY_PU, 0x0a, 6, 2, 0},
    {"white_balance_auto", UVC_ENTITY_PU, 0x0b, 12, 1, 0},
    {"hue_auto", UVC_ENTITY_PU, 0x10, 11, 1, 0},
};

// Find the ID and supported controls of the camera terminal or processing unit.
static bool uvc_find_entity(uint8_t entity, uint8_t *id, uint32_t *controls) {
    const uint8_t *end = uvc_desc + uvc.desc_len;
    for (const uint8_t *p = uvc_desc; uvc_desc_valid(p, end); p += p[0]) {
        if (p[1] == TUSB_DESC_INTERFACE && p != uvc_desc) {
            // The end of the VideoControl interface.
            break;
        }
        if (p[1] != TUSB_DESC_CS_INTERFACE) {
            continue;
        }
        const uint8_t *bm;
        size_t bm_size;
        if (entity == UVC_ENTITY_CT && p[2] == VIDEO_CS_ITF_VC_INPUT_TERMINAL && p[0] >= 15
            && tu_le16toh(tu_unaligned_read16(p + 4)) == VIDEO_ITT_CAMERA) {
            bm_size = p[14];
            bm = p + 15;
        } else if (entity == UVC_ENTITY_PU && p[2] == VIDEO_CS_ITF_VC_PROCESSING_UNIT && p[0] >= 8) {
            bm_size = p[7];
            bm = p + 8;
        } else {
            continue;
        }
        *id = p[3];
        *controls = 0;
        for (size_t i = 0; i < MIN(bm_size, 4) && bm + i < p + p[0]; ++i) {
            *controls |= (uint32_t)bm[i] << (8 * i);
        }
        return true;
    }
    return false;
}

const char *mp_usbh_uvc_control_name(size_t idx) {
    return idx < MP_ARRAY_SIZE(uvc_controls) ? uvc_controls[idx].name : NULL;
}

bool mp_usbh_uvc_control_available(size_t idx) {
    uint8_t id;
    uint32_t controls;
    const uvc_control_t *c = &uvc_controls[idx];
    return uvc.daddr != 0 && uvc_find_entity(c->entity, &id, &controls) && (controls & (1 << c->bit));
}

static int uvc_control_request(const uvc_control_t *c, uint8_t request, uint8_t *data) {
    uint8_t id;
    uint32_t controls;
    if (uvc.daddr == 0) {
        return -MP_ENODEV;
    }
    if (!uvc_find_entity(c->entity, &id, &controls) || !(controls & (1 << c->bit))) {
        return -MP_EOPNOTSUPP;
    }
    uint8_t bmRequestType = (request & 0x80) ? 0xa1 : 0x21; // class, interface, IN or OUT
    int ret = mp_usbh_control_xfer(uvc.daddr, bmRequestType, request, c->selector << 8, id << 8 | uvc.vc_itf,
            data, c->size, UVC_CTRL_TIMEOUT_MS);
    return ret < 0 ? ret : 0;
}

int mp_usbh_uvc_control_get(size_t idx, uint8_t request, int32_t *value) {
    const uvc_control_t *c = &uvc_controls[idx];
    uint8_t data[4] = {0};
    if (c->flags & UVC_CTRL_AE_MODE) {
        // Auto-exposure on (any mode except manual), with a range of 0 to 1.
        if (request == VIDEO_REQUEST_GET_MIN || request == VIDEO_REQUEST_GET_RES) {
            *value = request == VIDEO_REQUEST_GET_RES;
            return 0;
        } else if (request == VIDEO_REQUEST_GET_MAX) {
            *value = 1;
            return 0;
        }
    }
    int ret = uvc_control_request(c, request, data);
    if (ret < 0) {
        return ret;
    }
    uint32_t v = data[0] | data[1] << 8 | data[2] << 16 | (uint32_t)data[3] << 24;
    if ((c->flags & UVC_CTRL_SIGNED) && c->size < 4 && (v & (1u << (8 * c->size - 1)))) {
        v |= ~0u << (8 * c->size);
    }
    if (c->flags & UVC_CTRL_AE_MODE) {
        v = v != 1;
    }
    *value = v;
    return 0;
}

int mp_usbh_uvc_control_set(size_t idx, int32_t value) {
    const uvc_control_t *c = &uvc_controls[idx];
    if (c->flags & UVC_CTRL_AE_MODE) {
        if (value) {
            // Use aperture priority mode if supported, which is what most webcams use.
            uint8_t modes = 0;
            int ret = uvc_control_request(c, VIDEO_REQUEST_GET_RES, &modes);
            if (ret < 0) {
                return ret;
            }
            value = (modes & 0x08) ? 0x08 : (modes & 0x02) ? 0x02 : 0x04;
        } else {
            value = 0x01;
        }
    }
    uint8_t data[4] = {value, value >> 8, value >> 16, value >> 24};
    return uvc_control_request(c, VIDEO_REQUEST_SET_CUR, data);
}

void mp_usbh_uvc_get_stats(mp_usbh_uvc_stats_t *stats) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    *stats = uvc.stream.stats;
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

MP_STATIC_ASSERT(MP_USBH_UVC_NUM_BUFS == 3);
MP_REGISTER_ROOT_POINTER(uint8_t * usbh_uvc_bufs[3]);

#endif // MICROPY_HW_USB_HOST && MICROPY_HW_USB_HOST_UVC
