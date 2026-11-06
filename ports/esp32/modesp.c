/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>

#include "esp_flash.h"
#include "esp_log.h"

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_ESP_OSDEBUG_REPL

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_heap_caps.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/pyexec.h"

#define ESP_OSDEBUG_REPL (-1)

// IDF logs from any task, and from the MicroPython task with the GIL released,
// so the log hook must not enter the REPL output path (GIL, scheduler, USB
// stack, dupterm).  It only queues complete lines into a ring buffer, which the
// MicroPython task writes out from a scheduled callback.
#define LOG_REPL_RING_SIZE (MICROPY_PY_ESP_OSDEBUG_REPL_BUF_SIZE)
#define LOG_REPL_LINE_MAX (256)
#define LOG_REPL_LINE_SLOTS (4)
#define LOG_REPL_WAIT_MS (MICROPY_PY_ESP_OSDEBUG_REPL_WAIT_MS)
#define LOG_REPL_DRAIN_CHUNK (128)

// Per-task line assembly: some IDF code emits one line in several calls (the
// Wi-Fi libraries and log v2 send the prefix, message and newline separately)
// without holding a lock across them.
typedef struct _log_repl_line_t {
    TaskHandle_t owner;
    size_t len;
    char buf[LOG_REPL_LINE_MAX];
} log_repl_line_t;

typedef struct _log_repl_t {
    portMUX_TYPE lock; // guards the line owners and the drop state
    RingbufHandle_t ring;
    size_t dropped;
    bool dropping;
    log_repl_line_t line[LOG_REPL_LINE_SLOTS];
} log_repl_t;

// Never freed: other tasks may be inside the hook.
static log_repl_t *log_repl;
static mp_sched_node_t log_repl_node;
static vprintf_like_t log_repl_prev_vprintf;

static void log_repl_drain(mp_sched_node_t *node) {
    (void)node;
    // In the raw REPL only a running script's output may be mixed with logs, the
    // rest is protocol.  The interrupt char is set exactly while a script runs.
    if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL && mp_interrupt_char == -1) {
        mp_sched_schedule_node(&log_repl_node, log_repl_drain);
        return;
    }
    // Only what was queued on entry: it ends on a line boundary, and fast
    // producers can't keep this running.  Later lines schedule another run.
    UBaseType_t budget;
    vRingbufferGetInfo(log_repl->ring, NULL, NULL, NULL, NULL, &budget);
    while (budget > 0) {
        // In small pieces, so the space is freed while the output is slow.
        size_t n;
        char *data = xRingbufferReceiveUpTo(log_repl->ring, &n, 0,
            MIN(budget, LOG_REPL_DRAIN_CHUNK));
        if (data == NULL) {
            break;
        }
        // IDF lines end with a bare '\n', the REPL wants "\r\n".
        mp_hal_stdout_tx_strn_cooked(data, n);
        vRingbufferReturnItem(log_repl->ring, data);
        budget -= n;
        // The output moves again, so logging tasks may wait for space.
        log_repl->dropping = false;
    }
    portENTER_CRITICAL(&log_repl->lock);
    size_t dropped = log_repl->dropped;
    log_repl->dropped = 0;
    log_repl->dropping = false;
    portEXIT_CRITICAL(&log_repl->lock);
    if (dropped != 0) {
        char buf[48];
        int len = snprintf(buf, sizeof(buf), "[esp.REPL: %u bytes of log dropped]\r\n",
            (unsigned int)dropped);
        mp_hal_stdout_tx_strn(buf, len);
    }
}

static void log_repl_schedule(void) {
    if (mp_sched_schedule_node(&log_repl_node, log_repl_drain)) {
        mp_hal_wake_main_task();
    }
}

static void log_repl_send(const char *str, size_t len) {
    if (len == 0) {
        return;
    }
    // The MicroPython task drains the buffer, so it must never wait for it.
    // While the output is stalled, drop at once until the next drain.
    TickType_t wait = 0;
    if (!log_repl->dropping
        && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING
        && xTaskGetCurrentTaskHandle() != mp_main_task_handle) {
        wait = pdMS_TO_TICKS(LOG_REPL_WAIT_MS);
    }
    if (xRingbufferSend(log_repl->ring, str, len, wait) != pdTRUE) {
        portENTER_CRITICAL(&log_repl->lock);
        log_repl->dropped += len;
        log_repl->dropping = true;
        portEXIT_CRITICAL(&log_repl->lock);
    }
    log_repl_schedule();
}

// The calling task's line slot, or a free one; NULL if all are taken.
static log_repl_line_t *log_repl_line_get(void) {
    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    log_repl_line_t *line = NULL;
    log_repl_line_t *free_line = NULL;
    portENTER_CRITICAL(&log_repl->lock);
    for (size_t i = 0; i < LOG_REPL_LINE_SLOTS && line == NULL; ++i) {
        if (log_repl->line[i].owner == self) {
            line = &log_repl->line[i];
        } else if (log_repl->line[i].owner == NULL && free_line == NULL) {
            free_line = &log_repl->line[i];
        }
    }
    if (line == NULL && free_line != NULL) {
        line = free_line;
        line->owner = self;
        line->len = 0;
    }
    portEXIT_CRITICAL(&log_repl->lock);
    return line;
}

static int log_repl_vprintf(const char *format, va_list ap) {
    if (xPortInIsrContext()) {
        return 0;
    }
    // Format straight after the task's partial line.  Without a free slot the
    // output is queued as it comes and may interleave with other tasks.
    log_repl_line_t *line = log_repl_line_get();
    char stack_buf[128];
    char *buf = stack_buf;
    size_t pos = 0;
    size_t size = sizeof(stack_buf);
    if (line != NULL) {
        buf = line->buf;
        pos = line->len;
        size = sizeof(line->buf);
    }
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int n = vsnprintf(buf + pos, size - pos, format, ap);
    char *heap_buf = NULL;
    if (n >= (int)(size - pos)) {
        // Too long for the line buffer: assemble the line on the heap.
        heap_buf = heap_caps_malloc(pos + n + 1, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (heap_buf != NULL) {
            memcpy(heap_buf, buf, pos);
            vsnprintf(heap_buf + pos, n + 1, format, ap_copy);
            buf = heap_buf;
        } else {
            n = size - pos - 1;
        }
    }
    va_end(ap_copy);
    size_t len = pos + MAX(n, 0);

    // Queue all complete lines in one piece, keep the rest for the next call.
    size_t end = len;
    if (line != NULL) {
        while (end > 0 && buf[end - 1] != '\n') {
            --end;
        }
        if (len - end >= sizeof(line->buf)) {
            end = len;
        }
    }
    log_repl_send(buf, end);
    if (line != NULL) {
        memmove(line->buf, buf + end, len - end);
        line->len = len - end;
        if (line->len == 0) {
            portENTER_CRITICAL(&log_repl->lock);
            line->owner = NULL;
            portEXIT_CRITICAL(&log_repl->lock);
        }
    }
    heap_caps_free(heap_buf);
    return n;
}

static void log_repl_restore(void) {
    if (log_repl_prev_vprintf == NULL) {
        return;
    }
    vprintf_like_t current = esp_log_set_vprintf(log_repl_prev_vprintf);
    if (current == log_repl_vprintf) {
        log_repl_prev_vprintf = NULL;
    } else {
        // Replaced since (e.g. by the raw REPL): keep it.
        esp_log_set_vprintf(current);
    }
}

#endif // MICROPY_PY_ESP_OSDEBUG_REPL

static mp_obj_t esp_osdebug(size_t n_args, const mp_obj_t *args) {
    esp_log_level_t level = LOG_LOCAL_LEVEL; // Maximum available level
    if (n_args == 2) {
        level = mp_obj_get_int(args[1]);
    }
    #if MICROPY_PY_ESP_OSDEBUG_REPL
    if (mp_obj_is_small_int(args[0]) && MP_OBJ_SMALL_INT_VALUE(args[0]) == ESP_OSDEBUG_REPL) {
        if (log_repl == NULL) {
            log_repl_t *l = heap_caps_calloc(1, sizeof(log_repl_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (l == NULL) {
                mp_raise_type(&mp_type_MemoryError);
            }
            l->ring = xRingbufferCreateWithCaps(LOG_REPL_RING_SIZE, RINGBUF_TYPE_BYTEBUF,
                MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (l->ring == NULL) {
                heap_caps_free(l);
                mp_raise_type(&mp_type_MemoryError);
            }
            portMUX_INITIALIZE(&l->lock);
            log_repl = l;
        }
        vprintf_like_t prev = esp_log_set_vprintf(log_repl_vprintf);
        if (prev != log_repl_vprintf) {
            log_repl_prev_vprintf = prev;
        }
        esp_log_level_set("*", level);
        return mp_const_none;
    }
    log_repl_restore();
    #endif
    if (args[0] == mp_const_none) {
        // Set logging back to boot default of ESP_LOG_ERROR
        esp_log_level_set("*", ESP_LOG_ERROR);
    } else {
        // Enable logging at the given level
        // TODO args[0] should set the UART to which debug is sent
        esp_log_level_set("*", level);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_osdebug_obj, 1, 2, esp_osdebug);

static mp_obj_t esp_flash_read_(mp_obj_t offset_in, mp_obj_t buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    esp_err_t res = esp_flash_read(NULL, bufinfo.buf, offset, bufinfo.len);
    if (res != ESP_OK) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_read_obj, esp_flash_read_);

static mp_obj_t esp_flash_write_(mp_obj_t offset_in, mp_obj_t buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    esp_err_t res = esp_flash_write(NULL, bufinfo.buf, offset, bufinfo.len);
    if (res != ESP_OK) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_write_obj, esp_flash_write_);

static mp_obj_t esp_flash_erase(mp_obj_t sector_in) {
    mp_int_t sector = mp_obj_get_int(sector_in);
    esp_err_t res = esp_flash_erase_region(NULL, sector * 4096, 4096);
    if (res != ESP_OK) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp_flash_erase_obj, esp_flash_erase);

static mp_obj_t esp_flash_size(void) {
    uint32_t size;
    esp_flash_get_size(NULL, &size);
    return mp_obj_new_int_from_uint(size);
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_size_obj, esp_flash_size);

static mp_obj_t esp_flash_user_start(void) {
    return MP_OBJ_NEW_SMALL_INT(0x200000);
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_user_start_obj, esp_flash_user_start);

static mp_obj_t esp_gpio_matrix_in(mp_obj_t pin, mp_obj_t sig, mp_obj_t inv) {
    esp_rom_gpio_connect_in_signal(mp_obj_get_int(pin), mp_obj_get_int(sig), mp_obj_get_int(inv));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp_gpio_matrix_in_obj, esp_gpio_matrix_in);

static mp_obj_t esp_gpio_matrix_out(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    esp_rom_gpio_connect_out_signal(mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_gpio_matrix_out_obj, 4, 4, esp_gpio_matrix_out);

static const mp_rom_map_elem_t esp_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp) },

    { MP_ROM_QSTR(MP_QSTR_osdebug), MP_ROM_PTR(&esp_osdebug_obj) },

    { MP_ROM_QSTR(MP_QSTR_flash_read), MP_ROM_PTR(&esp_flash_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_write), MP_ROM_PTR(&esp_flash_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_erase), MP_ROM_PTR(&esp_flash_erase_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_size), MP_ROM_PTR(&esp_flash_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_user_start), MP_ROM_PTR(&esp_flash_user_start_obj) },

    { MP_ROM_QSTR(MP_QSTR_gpio_matrix_in), MP_ROM_PTR(&esp_gpio_matrix_in_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio_matrix_out), MP_ROM_PTR(&esp_gpio_matrix_out_obj) },

    #if MICROPY_PY_ESP_OSDEBUG_REPL
    // Constant for first arg of osdebug()
    { MP_ROM_QSTR(MP_QSTR_REPL), MP_ROM_INT(ESP_OSDEBUG_REPL) },
    #endif

    // Constants for second arg of osdebug()
    { MP_ROM_QSTR(MP_QSTR_LOG_NONE), MP_ROM_INT((mp_uint_t)ESP_LOG_NONE)},
    { MP_ROM_QSTR(MP_QSTR_LOG_ERROR), MP_ROM_INT((mp_uint_t)ESP_LOG_ERROR)},
    { MP_ROM_QSTR(MP_QSTR_LOG_WARNING), MP_ROM_INT((mp_uint_t)ESP_LOG_WARN)},
    { MP_ROM_QSTR(MP_QSTR_LOG_INFO), MP_ROM_INT((mp_uint_t)ESP_LOG_INFO)},
    { MP_ROM_QSTR(MP_QSTR_LOG_DEBUG), MP_ROM_INT((mp_uint_t)ESP_LOG_DEBUG)},
    { MP_ROM_QSTR(MP_QSTR_LOG_VERBOSE), MP_ROM_INT((mp_uint_t)ESP_LOG_VERBOSE)},
};

static MP_DEFINE_CONST_DICT(esp_module_globals, esp_module_globals_table);

const mp_obj_module_t esp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&esp_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esp, esp_module);
