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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

// Per-task line assembly: IDF often emits one line in several calls.
typedef struct _log_repl_line_t {
    TaskHandle_t owner;
    size_t len;
    char buf[LOG_REPL_LINE_MAX];
} log_repl_line_t;

typedef struct _log_repl_t {
    portMUX_TYPE lock;
    size_t head; // free-running
    size_t tail; // free-running
    size_t dropped;
    bool dropping;
    log_repl_line_t line[LOG_REPL_LINE_SLOTS];
    char ring[LOG_REPL_RING_SIZE];
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
    char buf[128];
    portENTER_CRITICAL(&log_repl->lock);
    size_t end = log_repl->head;
    portEXIT_CRITICAL(&log_repl->lock);
    // Only what was queued on entry, so fast producers can't keep this running.
    for (;;) {
        portENTER_CRITICAL(&log_repl->lock);
        size_t n = MIN(end - log_repl->tail, sizeof(buf));
        for (size_t i = 0; i < n; ++i) {
            buf[i] = log_repl->ring[(log_repl->tail + i) % LOG_REPL_RING_SIZE];
        }
        log_repl->tail += n;
        log_repl->dropping = false;
        size_t dropped = 0;
        if (n == 0) {
            dropped = log_repl->dropped;
            log_repl->dropped = 0;
        }
        bool more = log_repl->head != log_repl->tail;
        portEXIT_CRITICAL(&log_repl->lock);
        if (n != 0) {
            mp_hal_stdout_tx_strn(buf, n);
            continue;
        }
        if (dropped != 0) {
            int len = snprintf(buf, sizeof(buf), "[esp.REPL: %u bytes of log dropped]\r\n",
                (unsigned int)dropped);
            mp_hal_stdout_tx_strn(buf, len);
        }
        if (more) {
            mp_sched_schedule_node(&log_repl_node, log_repl_drain);
        }
        break;
    }
}

static void log_repl_schedule(void) {
    if (mp_sched_schedule_node(&log_repl_node, log_repl_drain)) {
        mp_hal_wake_main_task();
    }
}

static void log_repl_queue(const char *str, size_t len, bool newline) {
    size_t need = len + (newline ? 2 : 0);
    // The MicroPython task drains the buffer, so it must never wait for it.
    bool may_wait = xTaskGetSchedulerState() == taskSCHEDULER_RUNNING
        && xTaskGetCurrentTaskHandle() != mp_main_task_handle;
    TickType_t start = xTaskGetTickCount();
    for (;;) {
        portENTER_CRITICAL(&log_repl->lock);
        if (LOG_REPL_RING_SIZE - (log_repl->head - log_repl->tail) >= need) {
            for (size_t i = 0; i < len; ++i) {
                log_repl->ring[(log_repl->head + i) % LOG_REPL_RING_SIZE] = str[i];
            }
            if (newline) {
                log_repl->ring[(log_repl->head + len) % LOG_REPL_RING_SIZE] = '\r';
                log_repl->ring[(log_repl->head + len + 1) % LOG_REPL_RING_SIZE] = '\n';
            }
            log_repl->head += need;
            portEXIT_CRITICAL(&log_repl->lock);
            break;
        }
        bool give_up = !may_wait || log_repl->dropping
            || xTaskGetTickCount() - start >= pdMS_TO_TICKS(LOG_REPL_WAIT_MS);
        if (give_up) {
            // Output is stalled: drop without waiting until the next drain.
            log_repl->dropped += need;
            log_repl->dropping = true;
        }
        portEXIT_CRITICAL(&log_repl->lock);
        if (give_up) {
            break;
        }
        log_repl_schedule();
        vTaskDelay(1);
    }
    log_repl_schedule();
}

static void log_repl_write(const char *str, size_t len) {
    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    log_repl_line_t *line = NULL;
    portENTER_CRITICAL(&log_repl->lock);
    for (size_t i = 0; i < LOG_REPL_LINE_SLOTS && line == NULL; ++i) {
        if (log_repl->line[i].owner == self) {
            line = &log_repl->line[i];
        }
    }
    for (size_t i = 0; i < LOG_REPL_LINE_SLOTS && line == NULL; ++i) {
        if (log_repl->line[i].owner == NULL) {
            line = &log_repl->line[i];
            line->owner = self;
            line->len = 0;
        }
    }
    portEXIT_CRITICAL(&log_repl->lock);

    if (line == NULL) {
        // No free slot: queue piecewise, may interleave with other tasks.
        size_t start = 0;
        for (size_t i = 0; i < len; ++i) {
            if (str[i] == '\n') {
                log_repl_queue(str + start, i - start, true);
                start = i + 1;
            }
        }
        if (start < len) {
            log_repl_queue(str + start, len - start, false);
        }
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '\n') {
            log_repl_queue(line->buf, line->len, true);
            line->len = 0;
        } else if (str[i] != '\r') {
            if (line->len == LOG_REPL_LINE_MAX) {
                log_repl_queue(line->buf, line->len, false);
                line->len = 0;
            }
            line->buf[line->len++] = str[i];
        }
    }
    if (line->len == 0) {
        portENTER_CRITICAL(&log_repl->lock);
        line->owner = NULL;
        portEXIT_CRITICAL(&log_repl->lock);
    }
}

static int log_repl_vprintf(const char *format, va_list ap) {
    if (xPortInIsrContext()) {
        return 0;
    }
    char stack_buf[128];
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int n = vsnprintf(stack_buf, sizeof(stack_buf), format, ap);
    const char *str = stack_buf;
    char *heap_buf = NULL;
    if (n >= (int)sizeof(stack_buf)) {
        heap_buf = heap_caps_malloc(n + 1, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (heap_buf != NULL) {
            vsnprintf(heap_buf, n + 1, format, ap_copy);
            str = heap_buf;
        } else {
            n = sizeof(stack_buf) - 1;
        }
    }
    va_end(ap_copy);
    if (n > 0) {
        log_repl_write(str, n);
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
