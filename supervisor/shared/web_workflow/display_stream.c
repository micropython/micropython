// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/web_workflow/display_stream.h"

#if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_DISPLAYIO

#include <string.h>

#include "py/mperrno.h"
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/web_workflow/websocket.h"
#include "supervisor/workflow.h"

#if CIRCUITPY_BUSDISPLAY
#include "shared-bindings/busdisplay/BusDisplay.h"
#endif
#if CIRCUITPY_FRAMEBUFFERIO
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#endif

// A Hextile rectangle is cut into tiles of this size.
#define HEXTILE_SIZE (16)
// The display is sent in blocks, each its own rectangle, so that the worst a
// rectangle can take is known before it is encoded. A block is one row of tiles.
#define BLOCK_W (64)
#define BLOCK_H (HEXTILE_SIZE)
#define BLOCK_PIXELS (BLOCK_W * BLOCK_H)
#define MAX_SIZE (1024)
#define BLOCK_ROWS (MAX_SIZE / BLOCK_H)
// Room for one message: the websocket header and the RFB data.
#define MESSAGE_SIZE (8192)
#define WS_HEADER_SIZE (4)
#define PAYLOAD_SIZE (MESSAGE_SIZE - WS_HEADER_SIZE)
#define RECT_HEADER_SIZE (12)
// The rectangle header and every tile as a mask byte and raw pixels.
#define WORST_RECT (RECT_HEADER_SIZE + BLOCK_W / HEXTILE_SIZE + BLOCK_PIXELS * 4)
#define BLOCK_MASK_WORDS (BLOCK_PIXELS / 32 + 1)
// The message, a block in viewer pixels and the mask rendering a block needs.
#define BUFFER_SIZE (MESSAGE_SIZE + BLOCK_PIXELS * 4 + BLOCK_MASK_WORDS * 4)
// How long one background pass may spend sending.
#define PASS_BUDGET_MS (8)
// How often the connection is checked and the VNC port accepts. Each can cost a
// command to the network co-processor, so it isn't done every pass.
#define POLL_MS (250)
// Passes that send start at least this far apart, so the stream takes at most
// PASS_BUDGET_MS of every PASS_INTERVAL_MS however fast the display changes or
// however slowly the viewer reads.
#define PASS_INTERVAL_MS (20)

#define ENCODING_RAW (0)
#define ENCODING_HEXTILE (5)
#define ENCODING_DESKTOP_SIZE (-223)

// What a Hextile tile's mask byte says about the tile.
#define HEXTILE_RAW (1)
#define HEXTILE_BACKGROUND (2)
#define HEXTILE_FOREGROUND (4)
#define HEXTILE_ANY_SUBRECTS (8)
#define HEXTILE_SUBRECTS_COLOURED (16)

#define SECURITY_NONE (1)

enum {
    STATE_VERSION,
    STATE_SECURITY,
    STATE_CLIENT_INIT,
    STATE_MESSAGE,
    STATE_ENCODINGS,
    STATE_SKIP,
    // Sending a refusal, then closing.
    STATE_CLOSING,
};

// Native pixel formats of the display.
enum {
    FORMAT_NONE,
    FORMAT_BGRX8888,
    FORMAT_RGB565LE,
    FORMAT_RGB565BE,
    FORMAT_RGB332,
    FORMAT_GRAY8,
    FORMAT_RGBD4,
    FORMAT_GRAY,
};

typedef struct {
    uint8_t bits_per_pixel;
    uint8_t depth;
    bool big_endian;
    bool true_colour;
    uint16_t max[3];
    uint8_t shift[3];
} pixel_format_t;

// The colours a viewer keeps from one Hextile tile to the next in a rectangle.
typedef struct {
    uint32_t background;
    uint32_t foreground;
    bool background_known;
    bool foreground_known;
} hextile_colours_t;

static socketpool_socket_obj_t vnc_listening;
static socketpool_socket_obj_t vnc_incoming;
static uint64_t last_accept_ms;
static mp_int_t vnc_port;
static bool vnc_keys;

static websocket_t ws;
static bool over_websocket;
// Set while a viewer is connected and the buffer is allocated.
static bool streaming;
static uint8_t state;
static uint8_t minor_version;
// The client message being read.
static uint8_t input[20];
static uint8_t input_len;
static uint8_t input_needed;
// Encodings or cut text bytes still to read.
static uint32_t to_read;

static uint8_t *message;
// The part of message still to send.
static size_t message_start;
static size_t message_end;
// Bytes put after the websocket header that aren't in a message yet.
static size_t out_len;
static uint32_t *block;
static uint32_t *block_mask;

// What the viewer asked for.
static pixel_format_t pf;
static pixel_format_t next_pf;
static bool next_pf_set;
static uint8_t channel_bits[3];
static uint8_t pixel_bytes;
static bool use_hextile;
static bool raw_listed;
static bool desktop_size;
static bool update_requested;
static uint16_t client_width;
static uint16_t client_height;

// Keys typed into the viewer, for the serial console.
static ringbuf_t keys;
static uint8_t keys_buf[16];
static bool control_down;

// The display being streamed. target_fb is set when it's read from a
// framebuffer, otherwise blocks are rendered.
static displayio_display_core_t *target;
#if CIRCUITPY_FRAMEBUFFERIO
static framebufferio_framebufferdisplay_obj_t *target_fb;
#endif
static uint8_t format;
static uint16_t width;
static uint16_t height;
static uint8_t depth;
static bool lsb_first;
static void *pixels;
// When the connection was last checked.
static uint64_t last_poll_ms;
// When the last pass that sent started.
static uint64_t last_pass_ms;
// Ticks left until the next pass is requested, and whether the tick is enabled for it.
static volatile uint8_t pass_countdown;
static bool tick_enabled;

// Blocks written since they were last sent, a bit per column in each row of blocks.
static uint16_t dirty[BLOCK_ROWS];
// The blocks of the update being sent, and how many are left.
static uint16_t pending[BLOCK_ROWS];
static uint16_t rects_left;
static uint16_t next_block_row;

void display_stream_init(void) {
    socketpool_socket_reset(&ws.socket);
    socketpool_socket_reset(&vnc_listening);
    socketpool_socket_reset(&vnc_incoming);
    ringbuf_init(&keys, keys_buf, sizeof(keys_buf));
}

void display_stream_set_vnc(mp_int_t port, bool allow_keys) {
    vnc_port = port;
    vnc_keys = allow_keys;
}

void display_stream_listen(socketpool_socketpool_obj_t *pool) {
    if (vnc_port <= 0 || !common_hal_socketpool_socket_get_closed(&vnc_listening)) {
        return;
    }
    #if CIRCUITPY_SOCKETPOOL_IPV6
    bool opened = socketpool_socket(pool, SOCKETPOOL_AF_INET6, SOCKETPOOL_SOCK_STREAM, 0, &vnc_listening);
    #else
    bool opened = socketpool_socket(pool, SOCKETPOOL_AF_INET, SOCKETPOOL_SOCK_STREAM, 0, &vnc_listening);
    #endif
    if (!opened) {
        return;
    }
    common_hal_socketpool_socket_settimeout(&vnc_listening, 0);
    if (common_hal_socketpool_socket_bind(&vnc_listening, "", 0, vnc_port) != 0 ||
        !common_hal_socketpool_socket_listen(&vnc_listening, 1)) {
        common_hal_socketpool_socket_close(&vnc_listening);
    }
}

static void _mark_dirty(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    if (x1 >= x2 || y1 >= y2) {
        return;
    }
    uint16_t columns = 0;
    for (uint16_t c = x1 / BLOCK_W; c <= (x2 - 1) / BLOCK_W && c < 16; c++) {
        columns |= 1 << c;
    }
    for (uint16_t r = y1 / BLOCK_H; r <= (y2 - 1) / BLOCK_H && r < BLOCK_ROWS; r++) {
        dirty[r] |= columns;
    }
}

static void _mark_all_dirty(void) {
    _mark_dirty(0, 0, width, height);
}

// Some ports only run the web workflow when a socket has data, so the tick asks
// for the next pass, PASS_INTERVAL_MS after the last one.
static void _pass_later(void) {
    if (pass_countdown > 0) {
        return;
    }
    if (!tick_enabled) {
        supervisor_enable_tick();
        tick_enabled = true;
    }
    uint64_t since = supervisor_ticks_ms64() - last_pass_ms;
    pass_countdown = since >= PASS_INTERVAL_MS ? 1 : PASS_INTERVAL_MS - since;
}

static void _no_pass_needed(void) {
    pass_countdown = 0;
    if (tick_enabled) {
        supervisor_disable_tick();
        tick_enabled = false;
    }
}

void display_stream_tick(void) {
    if (pass_countdown > 0 && --pass_countdown == 0) {
        supervisor_workflow_request_background();
    }
}

static void _stop(void) {
    _no_pass_needed();
    if (!common_hal_socketpool_socket_get_closed(&ws.socket)) {
        common_hal_socketpool_socket_close(&ws.socket);
    }
    if (message != NULL) {
        port_free(message);
        message = NULL;
    }
    streaming = false;
    target = NULL;
    #if CIRCUITPY_FRAMEBUFFERIO
    target_fb = NULL;
    #endif
    ringbuf_clear(&keys);
}

static uint8_t *_out(size_t len) {
    uint8_t *p = message + WS_HEADER_SIZE + out_len;
    out_len += len;
    return p;
}

static uint8_t *_put16(uint8_t *p, uint16_t v) {
    p[0] = v >> 8;
    p[1] = v;
    return p + 2;
}

static uint8_t *_put32(uint8_t *p, uint32_t v) {
    p = _put16(p, v >> 16);
    return _put16(p, v);
}

static uint32_t _get32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
}

static void _put_pixel_format(uint8_t *p, const pixel_format_t *f) {
    p[0] = f->bits_per_pixel;
    p[1] = f->depth;
    p[2] = f->big_endian;
    p[3] = f->true_colour;
    for (int i = 0; i < 3; i++) {
        _put16(p + 4 + 2 * i, f->max[i]);
        p[10 + i] = f->shift[i];
    }
    p[13] = p[14] = p[15] = 0;
}

// Takes a pixel format the viewer asked for. Returns false for one it can't be
// sent: colour maps, and channels that aren't a whole number of bits.
static bool _set_pixel_format(const pixel_format_t *f) {
    if (!f->true_colour ||
        (f->bits_per_pixel != 8 && f->bits_per_pixel != 16 && f->bits_per_pixel != 32)) {
        return false;
    }
    for (int i = 0; i < 3; i++) {
        uint16_t max = f->max[i];
        if (max == 0 || max > 255 || (max & (max + 1)) != 0) {
            return false;
        }
        channel_bits[i] = 32 - __builtin_clz(max);
    }
    pf = *f;
    pixel_bytes = f->bits_per_pixel / 8;
    return true;
}

static void _default_pixel_format(pixel_format_t *f) {
    if (format == FORMAT_RGB565LE || format == FORMAT_RGB565BE) {
        *f = (pixel_format_t) {16, 16, false, true, {31, 63, 31}, {11, 5, 0}};
    } else {
        *f = (pixel_format_t) {32, 24, false, true, {255, 255, 255}, {16, 8, 0}};
    }
}

void display_stream_handoff(socketpool_socket_obj_t *socket, bool websocket) {
    if (streaming && message_start < message_end) {
        // A close frame can't go in the middle of a message, so finish it if the
        // socket takes it now. A viewer that stopped reading is closed without
        // one rather than holding everything up.
        int left = message_end - message_start;
        if (socketpool_socket_send(&ws.socket, message + message_start, left) < left) {
            common_hal_socketpool_socket_close(&ws.socket);
        }
    }
    if (streaming && !over_websocket && !common_hal_socketpool_socket_get_closed(&ws.socket)) {
        // Only a websocket viewer gets a close frame.
        common_hal_socketpool_socket_close(&ws.socket);
    }
    if (message != NULL) {
        port_free(message);
        message = NULL;
    }
    streaming = false;
    websocket_take_socket(&ws, socket);
    over_websocket = websocket;
    uint32_t *buffer = port_malloc(BUFFER_SIZE, false);
    message = (uint8_t *)buffer;
    if (message == NULL) {
        _stop();
        return;
    }
    block = buffer + MESSAGE_SIZE / sizeof(uint32_t);
    block_mask = block + BLOCK_PIXELS;
    message_start = 0;
    message_end = 0;
    out_len = 0;
    target = NULL;
    #if CIRCUITPY_FRAMEBUFFERIO
    target_fb = NULL;
    #endif
    format = FORMAT_NONE;
    width = 0;
    height = 0;
    rects_left = 0;
    update_requested = false;
    use_hextile = false;
    desktop_size = false;
    next_pf_set = false;
    control_down = false;
    ringbuf_clear(&keys);
    memcpy(_out(12), "RFB 003.008\n", 12);
    state = STATE_VERSION;
    input_len = 0;
    input_needed = 12;
    last_poll_ms = supervisor_ticks_ms64();
    last_pass_ms = 0;
    streaming = true;
    _pass_later();
}

void display_stream_area_updated(displayio_display_core_t *core, const displayio_area_t *area) {
    if (!streaming) {
        return;
    }
    if (core == target) {
        _mark_dirty(area->x1, area->y1, area->x2, area->y2);
    }
    // Another display may be a new one to switch to.
    _pass_later();
}

static uint8_t _format_of(const _displayio_colorspace_t *colorspace) {
    if (colorspace->depth < 8 && !colorspace->pixels_in_byte_share_row) {
        return FORMAT_NONE;
    }
    switch (colorspace->depth) {
        case 32:
            return FORMAT_BGRX8888;
        case 16:
            return colorspace->reverse_bytes_in_word ? FORMAT_RGB565BE : FORMAT_RGB565LE;
        case 8:
            return colorspace->grayscale ? FORMAT_GRAY8 : FORMAT_RGB332;
        case 4:
            return colorspace->grayscale ? FORMAT_GRAY : FORMAT_RGBD4;
        case 2:
        case 1:
            return FORMAT_GRAY;
    }
    return FORMAT_NONE;
}

// Finds the display to stream and notices when it changes.
static void _check_display(void) {
    displayio_display_core_t *core = NULL;
    void *fb = NULL;
    void *new_pixels = NULL;
    for (size_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT && core == NULL; i++) {
        const mp_obj_type_t *type = displays[i].display_base.type;
        #if CIRCUITPY_FRAMEBUFFERIO
        if (type == &framebufferio_framebufferdisplay_type) {
            fb = &displays[i].framebuffer_display;
            core = &displays[i].framebuffer_display.core;
            new_pixels = displays[i].framebuffer_display.bufinfo.buf;
        }
        #endif
        #if CIRCUITPY_BUSDISPLAY
        if (type == &busdisplay_busdisplay_type) {
            core = &displays[i].display.core;
        }
        #endif
        (void)type;
    }
    uint8_t new_format = FORMAT_NONE;
    if (core != NULL) {
        new_format = _format_of(&core->colorspace);
        // The core's area is the panel's size, whatever the rotation.
        if (core->area.x2 > MAX_SIZE || core->area.y2 > MAX_SIZE) {
            new_format = FORMAT_NONE;
        }
        if (fb != NULL ? new_pixels == NULL : core->colorspace.depth < 8) {
            new_format = FORMAT_NONE;
        }
        if (new_format == FORMAT_NONE) {
            core = NULL;
            fb = NULL;
            new_pixels = NULL;
        }
    }
    uint16_t new_width = core != NULL ? core->area.x2 : width;
    uint16_t new_height = core != NULL ? core->area.y2 : height;
    if (core == target &&
        (core == NULL || (new_format == format && new_width == width && new_height == height &&
                          new_pixels == pixels))) {
        return;
    }
    target = core;
    #if CIRCUITPY_FRAMEBUFFERIO
    target_fb = fb;
    #endif
    format = new_format;
    // Without a display the viewer keeps its size and is shown black.
    width = new_width;
    height = new_height;
    depth = core != NULL ? core->colorspace.depth : 0;
    lsb_first = core != NULL && !core->colorspace.reverse_pixels_in_byte;
    pixels = new_pixels;
    _mark_all_dirty();
}

// The pixel at x of row as 0xRRGGBB.
static uint32_t _rgb(const uint8_t *row, uint16_t x) {
    uint32_t v;
    switch (format) {
        case FORMAT_BGRX8888:
            row += x * 4;
            return row[2] << 16 | row[1] << 8 | row[0];
        case FORMAT_RGB565LE:
        case FORMAT_RGB565BE: {
            row += x * 2;
            v = format == FORMAT_RGB565LE ? (row[1] << 8 | row[0]) : (row[0] << 8 | row[1]);
            uint32_t r = v >> 11, g = (v >> 5) & 0x3f, b = v & 0x1f;
            return ((r << 3) | (r >> 2)) << 16 | ((g << 2) | (g >> 4)) << 8 | ((b << 3) | (b >> 2));
        }
        case FORMAT_RGB332: {
            v = row[x];
            uint32_t r = v >> 5, g = (v >> 2) & 7, b = v & 3;
            return (r * 255 / 7) << 16 | (g * 255 / 7) << 8 | b * 0x55;
        }
        case FORMAT_GRAY8:
            return row[x] * 0x010101;
    }
    uint8_t per_byte = 8 / depth;
    uint8_t index = x % per_byte;
    uint8_t shift = lsb_first ? index * depth : 8 - depth - index * depth;
    v = (row[x / per_byte] >> shift) & ((1 << depth) - 1);
    if (format == FORMAT_RGBD4) {
        return ((v >> 3) & 1 ? 0xff0000 : 0) | ((v >> 2) & 1 ? 0xff00 : 0) | ((v >> 1) & 1 ? 0xff : 0);
    }
    return v * 255 / ((1 << depth) - 1) * 0x010101;
}

static uint32_t _client_pixel(uint32_t rgb) {
    return ((rgb >> (24 - channel_bits[0])) & pf.max[0]) << pf.shift[0] |
                   ((rgb >> (16 - channel_bits[1])) & pf.max[1]) << pf.shift[1] |
                   ((rgb >> (8 - channel_bits[2])) & pf.max[2]) << pf.shift[2];
}

// Fills block with the w x h pixels at x, y in the viewer's format.
static void _load_block(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (target == NULL) {
        memset(block, 0, w * h * sizeof(uint32_t));
        return;
    }
    const uint8_t *src = NULL;
    size_t stride = 0;
    uint16_t x0 = 0;
    #if CIRCUITPY_FRAMEBUFFERIO
    if (target_fb != NULL) {
        stride = target_fb->row_stride;
        src = (const uint8_t *)pixels + target_fb->first_pixel_offset + y * stride;
        x0 = x;
    }
    #endif
    #if CIRCUITPY_BUSDISPLAY
    if (src == NULL) {
        // Rendered into block itself. Converting from the end doesn't overwrite
        // a pixel before it's read, since pixels are at most 4 bytes.
        displayio_area_t area = { .x1 = x, .y1 = y, .x2 = x + w, .y2 = y + h, .next = NULL };
        stride = w * depth / 8;
        memset(block, 0, h * stride);
        memset(block_mask, 0, BLOCK_MASK_WORDS * sizeof(uint32_t));
        displayio_display_core_fill_area(target, &area, block_mask, block);
        src = (const uint8_t *)block;
    }
    #endif
    for (int j = h - 1; j >= 0; j--) {
        const uint8_t *row = src + j * stride;
        uint32_t *out = block + j * w;
        for (int i = w - 1; i >= 0; i--) {
            out[i] = _client_pixel(_rgb(row, x0 + i));
        }
    }
}

static uint8_t *_put_pixel(uint8_t *p, uint32_t v) {
    for (uint8_t i = 0; i < pixel_bytes; i++) {
        p[i] = pf.big_endian ? v >> (8 * (pixel_bytes - 1 - i)) : v >> (8 * i);
    }
    return p + pixel_bytes;
}

// A Hextile tile as plain pixels. Returns the end of it.
static uint8_t *_put_hextile_raw(uint8_t *mask, const uint32_t *tile, uint8_t w, uint8_t h, uint8_t stride, hextile_colours_t *kept) {
    *mask = HEXTILE_RAW;
    uint8_t *p = mask + 1;
    for (uint8_t y = 0; y < h; y++) {
        for (uint8_t x = 0; x < w; x++) {
            p = _put_pixel(p, tile[y * stride + x]);
        }
    }
    // Viewers differ on what these are after a raw tile.
    kept->background_known = false;
    kept->foreground_known = false;
    return p;
}

// Writes the w x h pixels as one Hextile tile, as subrectangles on a background
// unless that would take more than the pixels themselves. Returns the end of it.
static uint8_t *_put_hextile_tile(uint8_t *p, const uint32_t *tile, uint8_t w, uint8_t h, uint8_t stride, hextile_colours_t *kept) {
    // A majority vote finds the most common colour when more than half the
    // pixels have it. Otherwise any colour will do, and the tile likely ends up raw.
    uint32_t background = 0;
    int16_t votes = 0;
    for (uint8_t y = 0; y < h; y++) {
        for (uint8_t x = 0; x < w; x++) {
            uint32_t v = tile[y * stride + x];
            if (votes == 0) {
                background = v;
            }
            votes += v == background ? 1 : -1;
        }
    }
    uint32_t foreground = background;
    bool coloured = false;
    for (uint8_t y = 0; y < h; y++) {
        for (uint8_t x = 0; x < w; x++) {
            uint32_t v = tile[y * stride + x];
            if (foreground == background) {
                foreground = v;
            } else if (v != background && v != foreground) {
                coloured = true;
            }
        }
    }
    uint8_t *mask = p++;
    *mask = 0;
    if (!kept->background_known || kept->background != background) {
        *mask |= HEXTILE_BACKGROUND;
        p = _put_pixel(p, background);
        kept->background = background;
        kept->background_known = true;
    }
    if (foreground == background) {
        return p;
    }
    *mask |= HEXTILE_ANY_SUBRECTS;
    if (coloured) {
        *mask |= HEXTILE_SUBRECTS_COLOURED;
        kept->foreground_known = false;
    } else if (!kept->foreground_known || kept->foreground != foreground) {
        *mask |= HEXTILE_FOREGROUND;
        p = _put_pixel(p, foreground);
        kept->foreground = foreground;
        kept->foreground_known = true;
    }
    uint8_t *count = p++;
    *count = 0;
    const uint8_t *raw_end = mask + 1 + w * h * pixel_bytes;
    uint8_t subrect_size = 2 + (coloured ? pixel_bytes : 0);
    // Pixels already in a subrectangle, a bit per column.
    uint16_t done[HEXTILE_SIZE] = { 0 };
    for (uint8_t y = 0; y < h; y++) {
        const uint32_t *row = tile + y * stride;
        for (uint8_t x = 0; x < w; x++) {
            uint32_t v = row[x];
            if (v == background || (done[y] & (1 << x)) != 0) {
                continue;
            }
            if (p + subrect_size > raw_end) {
                return _put_hextile_raw(mask, tile, w, h, stride, kept);
            }
            uint8_t rect_w = 1;
            while (x + rect_w < w && row[x + rect_w] == v) {
                rect_w++;
            }
            uint8_t rect_h = 1;
            for (bool same = true; same && y + rect_h < h; rect_h += same) {
                const uint32_t *below = row + rect_h * stride + x;
                for (uint8_t i = 0; i < rect_w && same; i++) {
                    same = below[i] == v;
                }
            }
            if (coloured) {
                p = _put_pixel(p, v);
            }
            *p++ = x << 4 | y;
            *p++ = (rect_w - 1) << 4 | (rect_h - 1);
            *count += 1;
            for (uint8_t i = 0; i < rect_h; i++) {
                done[y + i] |= ((1 << rect_w) - 1) << x;
            }
        }
    }
    return p;
}

static void _put_rect_header(uint8_t *p, uint16_t x, uint16_t y, uint16_t w, uint16_t h, int32_t encoding) {
    p = _put16(p, x);
    p = _put16(p, y);
    p = _put16(p, w);
    p = _put16(p, h);
    _put32(p, encoding);
}

static void _put_block_rect(uint16_t x, uint16_t y) {
    uint16_t w = MIN(BLOCK_W, width - x);
    uint16_t h = MIN(BLOCK_H, height - y);
    _load_block(x, y, w, h);
    _put_rect_header(_out(RECT_HEADER_SIZE), x, y, w, h, use_hextile ? ENCODING_HEXTILE : ENCODING_RAW);
    uint8_t *start = _out(0);
    uint8_t *p = start;
    if (use_hextile) {
        hextile_colours_t kept = { 0 };
        for (uint16_t i = 0; i < w; i += HEXTILE_SIZE) {
            p = _put_hextile_tile(p, block + i, MIN(HEXTILE_SIZE, w - i), h, w, &kept);
        }
    } else {
        for (size_t i = 0; i < (size_t)w * h; i++) {
            p = _put_pixel(p, block[i]);
        }
    }
    _out(p - start);
}

// Adds the next part of an update to the message, starting an update if one
// was asked for and there is something to send. Returns false when there was
// nothing to add.
static bool _fill(uint64_t end_ms) {
    _check_display();
    if (rects_left == 0) {
        if (next_pf_set) {
            next_pf_set = false;
            if (!_set_pixel_format(&next_pf)) {
                _stop();
                return false;
            }
            _mark_all_dirty();
        }
        if (!update_requested) {
            return false;
        }
        if (width != client_width || height != client_height) {
            if (!desktop_size) {
                // It can only be told by connecting again.
                _stop();
                return false;
            }
            uint8_t *p = _out(4);
            p[0] = 0;
            p[1] = 0;
            _put16(p + 2, 1);
            _put_rect_header(_out(RECT_HEADER_SIZE), 0, 0, width, height, ENCODING_DESKTOP_SIZE);
            client_width = width;
            client_height = height;
            update_requested = false;
            _mark_all_dirty();
            return true;
        }
        uint16_t count = 0;
        uint16_t rows = (height + BLOCK_H - 1) / BLOCK_H;
        for (uint16_t r = 0; r < rows; r++) {
            pending[r] = dirty[r];
            dirty[r] = 0;
            count += __builtin_popcount(pending[r]);
        }
        if (count == 0) {
            return false;
        }
        uint8_t *p = _out(4);
        p[0] = 0;
        p[1] = 0;
        _put16(p + 2, count);
        rects_left = count;
        next_block_row = 0;
        update_requested = false;
    }
    bool first = true;
    while (rects_left > 0 && out_len + WORST_RECT <= PAYLOAD_SIZE &&
           (first || supervisor_ticks_ms64() < end_ms)) {
        while (pending[next_block_row] == 0) {
            next_block_row++;
        }
        uint8_t column = __builtin_ctz(pending[next_block_row]);
        pending[next_block_row] &= ~(1 << column);
        _put_block_rect(column * BLOCK_W, next_block_row * BLOCK_H);
        rects_left--;
        first = false;
    }
    return true;
}

static void _frame(void) {
    if (!over_websocket) {
        message_start = WS_HEADER_SIZE;
    } else if (out_len <= 125) {
        message_start = WS_HEADER_SIZE - 2;
        message[message_start + 1] = out_len;
    } else {
        message_start = 0;
        message[1] = 126;
        message[2] = out_len >> 8;
        message[3] = out_len & 0xff;
    }
    if (over_websocket) {
        // Binary
        message[message_start] = 0x80 | 0x2;
    }
    message_end = WS_HEADER_SIZE + out_len;
    out_len = 0;
}

static int _read(uint8_t *buf, size_t len) {
    if (over_websocket) {
        size_t n = 0;
        while (n < len && websocket_read_payload_byte(&ws, buf + n)) {
            n++;
        }
        return n;
    }
    int n = socketpool_socket_recv_into(&ws.socket, buf, len);
    return n < 0 ? 0 : n;
}

static void _type(const char *text) {
    for (; *text != '\0'; text++) {
        if (*text == mp_interrupt_char) {
            ringbuf_clear(&keys);
            mp_sched_keyboard_interrupt();
        } else if (ringbuf_num_empty(&keys) > 0) {
            ringbuf_put(&keys, *text);
        }
    }
}

static void _key(bool down, uint32_t keysym) {
    if (keysym == 0xffe3 || keysym == 0xffe4) {
        control_down = down;
        return;
    }
    if (!down) {
        return;
    }
    char c[2] = { 0, 0 };
    const char *text = c;
    if (keysym >= 0x20 && keysym <= 0x7e) {
        c[0] = keysym;
        if (control_down && keysym >= 0x40) {
            c[0] &= 0x1f;
        }
    } else {
        switch (keysym) {
            case 0xff0d: // Return
            case 0xff8d: // KP_Enter
                c[0] = '\r';
                break;
            case 0xff08: // BackSpace
                c[0] = '\b';
                break;
            case 0xff09: // Tab
                c[0] = '\t';
                break;
            case 0xff1b: // Escape
                c[0] = 0x1b;
                break;
            case 0xff51: // Left
                text = "\x1b[D";
                break;
            case 0xff52: // Up
                text = "\x1b[A";
                break;
            case 0xff53: // Right
                text = "\x1b[C";
                break;
            case 0xff54: // Down
                text = "\x1b[B";
                break;
            case 0xff50: // Home
                text = "\x1b[H";
                break;
            case 0xff57: // End
                text = "\x1b[F";
                break;
            case 0xffff: // Delete
                text = "\x1b[3~";
                break;
        }
    }
    _type(text);
}

static void _refuse(const char *reason) {
    // SecurityResult failed, and from 3.8 why.
    _put32(_out(4), 1);
    if (minor_version >= 8) {
        size_t len = strlen(reason);
        _put32(_out(4), len);
        memcpy(_out(len), reason, len);
    }
    state = STATE_CLOSING;
}

static void _server_init(void) {
    _check_display();
    client_width = width;
    client_height = height;
    pixel_format_t f;
    _default_pixel_format(&f);
    _set_pixel_format(&f);
    uint8_t *p = _out(4 + 16);
    // Without a display yet the viewer is told 1 x 1 until there is one.
    _put16(p, MAX(client_width, 1));
    _put16(p + 2, MAX(client_height, 1));
    _put_pixel_format(p + 4, &pf);
    const char *name = MICROPY_HW_BOARD_NAME;
    size_t len = strlen(name);
    _put32(_out(4), len);
    memcpy(_out(len), name, len);
    _mark_all_dirty();
}

// Handles input once input_needed bytes of it are read.
static void _handle_input(void) {
    uint8_t needed = 1;
    switch (state) {
        case STATE_VERSION:
            if (memcmp(input, "RFB 003.", 8) != 0) {
                _stop();
                return;
            }
            minor_version = (input[9] - '0') * 10 + (input[10] - '0');
            if (minor_version >= 8) {
                minor_version = 8;
            } else if (minor_version != 7) {
                minor_version = 3;
            }
            if (minor_version == 3) {
                // The server picks.
                _put32(_out(4), SECURITY_NONE);
                state = STATE_CLIENT_INIT;
            } else {
                uint8_t *p = _out(2);
                p[0] = 1;
                p[1] = SECURITY_NONE;
                state = STATE_SECURITY;
            }
            break;
        case STATE_SECURITY:
            if (input[0] != SECURITY_NONE) {
                _refuse("Unsupported security type");
                break;
            }
            if (minor_version >= 8) {
                _put32(_out(4), 0);
            }
            state = STATE_CLIENT_INIT;
            break;
        case STATE_CLIENT_INIT:
            // Shared or not, a new viewer takes over.
            _server_init();
            state = STATE_MESSAGE;
            break;
        case STATE_MESSAGE:
            if (input_len == 1) {
                switch (input[0]) {
                    case 0: // SetPixelFormat
                        needed = 20;
                        break;
                    case 2: // SetEncodings
                        needed = 4;
                        break;
                    case 3: // FramebufferUpdateRequest
                        needed = 10;
                        break;
                    case 4: // KeyEvent
                        needed = 8;
                        break;
                    case 5: // PointerEvent
                        needed = 6;
                        break;
                    case 6: // ClientCutText
                        needed = 8;
                        break;
                    default:
                        // Its length isn't known, so nothing after it can be read.
                        _stop();
                        return;
                }
                input_needed = needed;
                return;
            }
            switch (input[0]) {
                case 0: {
                    const uint8_t *p = input + 4;
                    next_pf = (pixel_format_t) {
                        p[0], p[1], p[2] != 0, p[3] != 0,
                        {p[4] << 8 | p[5], p[6] << 8 | p[7], p[8] << 8 | p[9]},
                        {p[10], p[11], p[12]}
                    };
                    next_pf_set = true;
                    break;
                }
                case 2:
                    to_read = input[2] << 8 | input[3];
                    use_hextile = false;
                    raw_listed = false;
                    desktop_size = false;
                    if (to_read > 0) {
                        state = STATE_ENCODINGS;
                        needed = 4;
                    }
                    break;
                case 3:
                    if (input[1] == 0) {
                        _mark_all_dirty();
                    }
                    update_requested = true;
                    break;
                case 4:
                    // The VNC port has no password, so it only reaches the REPL if asked to.
                    if (over_websocket || vnc_keys) {
                        _key(input[1] != 0, _get32(input + 4));
                    }
                    break;
                case 6:
                    to_read = _get32(input + 4);
                    if (to_read > 0) {
                        state = STATE_SKIP;
                    }
                    break;
            }
            break;
        case STATE_ENCODINGS: {
            int32_t encoding = _get32(input);
            if (encoding == ENCODING_HEXTILE && !raw_listed) {
                use_hextile = true;
            } else if (encoding == ENCODING_RAW) {
                raw_listed = true;
            } else if (encoding == ENCODING_DESKTOP_SIZE) {
                desktop_size = true;
            }
            if (--to_read > 0) {
                needed = 4;
            } else {
                state = STATE_MESSAGE;
            }
            break;
        }
    }
    input_len = 0;
    input_needed = needed;
}

static void _read_input(void) {
    while (streaming && state != STATE_CLOSING) {
        if (state == STATE_SKIP) {
            uint8_t skipped[16];
            int n = _read(skipped, MIN(sizeof(skipped), to_read));
            if (n <= 0) {
                return;
            }
            to_read -= n;
            if (to_read == 0) {
                state = STATE_MESSAGE;
            }
            continue;
        }
        int n = _read(input + input_len, input_needed - input_len);
        if (n <= 0) {
            return;
        }
        input_len += n;
        if (input_len == input_needed) {
            _handle_input();
        }
    }
}

static void _accept(void) {
    if (common_hal_socketpool_socket_get_closed(&vnc_listening)) {
        return;
    }
    int result = socketpool_socket_accept(&vnc_listening, NULL, &vnc_incoming);
    if (result == -MP_EBADF) {
        common_hal_socketpool_socket_close(&vnc_listening);
        return;
    }
    if (result > 0) {
        common_hal_socketpool_socket_settimeout(&vnc_incoming, 0);
        display_stream_handoff(&vnc_incoming, false);
    }
}

void display_stream_background(void) {
    uint64_t start = supervisor_ticks_ms64();
    if (start - last_accept_ms >= POLL_MS) {
        last_accept_ms = start;
        _accept();
    }
    if (!streaming) {
        return;
    }
    // A CLOSE from the viewer closes the socket without asking the co-processor.
    if (common_hal_socketpool_socket_get_closed(&ws.socket)) {
        _stop();
        return;
    }
    if (start - last_poll_ms >= POLL_MS) {
        last_poll_ms = start;
        if (!common_hal_socketpool_socket_get_connected(&ws.socket)) {
            _stop();
            return;
        }
    }
    // Some ports wake the web workflow until a socket's data is read, so read
    // what the viewer sent every time. Replies go in the next message, so not
    // in the middle of one.
    if (message_start >= message_end) {
        _read_input();
        if (!streaming || common_hal_socketpool_socket_get_closed(&ws.socket)) {
            _stop();
            return;
        }
    }
    if (start - last_pass_ms < PASS_INTERVAL_MS) {
        // Too soon after the last one, whatever woke the web workflow.
        _pass_later();
        return;
    }
    last_pass_ms = start;
    do {
        if (message_start < message_end) {
            int sent = socketpool_socket_send(&ws.socket, message + message_start, message_end - message_start);
            if (sent == -MP_EAGAIN) {
                break;
            }
            if (sent < 0) {
                _stop();
                return;
            }
            message_start += sent;
            continue;
        }
        if (out_len > 0) {
            _frame();
            continue;
        }
        if (state == STATE_CLOSING) {
            _stop();
            return;
        }
        if (state < STATE_MESSAGE || !_fill(start + PASS_BUDGET_MS)) {
            break;
        }
        if (!streaming) {
            return;
        }
    } while (supervisor_ticks_ms64() - start < PASS_BUDGET_MS);
    // Come back for what's left: the rest of a message the socket couldn't take
    // yet, or an update the budget ran out on.
    if (message_start < message_end || out_len > 0 || rects_left > 0) {
        _pass_later();
    } else {
        _no_pass_needed();
    }
}

bool display_stream_connected(void) {
    return streaming && state >= STATE_MESSAGE;
}

uint32_t display_stream_available(void) {
    return ringbuf_num_filled(&keys);
}

char display_stream_read_char(void) {
    if (ringbuf_num_filled(&keys) > 0) {
        return ringbuf_get(&keys);
    }
    return -1;
}

#endif
