/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jim Mussared
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

/*
 * This is based on (but heavily modified from) cygmon-gmon.c from newlib.
 *
 * Copyright (c) 1991, 2000 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Also based on code from MCU on Eclipse by Erich Styger
 * See https://mcuoneclipse.com/2015/08/23/tutorial-using-gnu-profiling-gprof-with-arm-cortex-m/
 * and https://github.com/ErichStyger/mcuoneclipse/tree/master/Examples/KDS/FRDM-K64F120M/FRDM-K64F_Profiling/Profiling

 * This is a free software and is opened for education, research and commercial developments under license policy of following terms:
 * This is a free software and there is NO WARRANTY.
 * No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
 * Redistributions of source code must retain the copyright notice in the Processor Expert component code.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_ENABLE_PROFILER

#include "extmod/vfs.h"

// gmon header structure.
// The file is this header, followed by ncnt 16-bit histogram entries, followed
// by a gmon_rawarc_t for each arc.
typedef struct _gmon_hdr_t {
    uint32_t lowpc;     // base pc address of sample buffer
    uint32_t highpc;    // max pc address of sampled buffer
    int32_t ncnt;       // size of sample buffer (plus this header)
    int32_t version;    // version number
    int32_t profrate;   // profiling clock rate
    int32_t spare[3];   // reserved
} gmon_hdr_t;

// gmon file format arc structure.
typedef struct _gmon_rawarc_t {
    uint32_t raw_callerpc;
    uint32_t raw_selfpc;
    uint32_t raw_count;
} gmon_rawarc_t;

// Note this is the older BSD style format.
// But gprof from modern binutils handles it fine.
#define GMONVERSION 0x00051879

// Systick frequency.
#define INSTRUCTION_SAMPLING_HZ 1000

// PC values from sampling will be rounded to this level of granularity.
#define INSTRUCTION_SAMPLING_RESOLUTION 32

// Caller addresses will be rounded to this level of granularity.
#define CALLER_ADDRESS_RESOLUTION 128

// How many unique (source-addr, dest-fn) pairs (8 bytes each).
#define TOTAL_ARCS 512

// A single arc entry in the table.
// These form a singly-linked list for all arcs with the same source address.
// Note: The count is 16-bit. If it hits 0xffff, then a new entry is created.
// gmon will sum counts for entries for the same arc. This keeps this structure at two-words.
typedef struct _gmon_arc_t {
    uintptr_t selfpc;  // callee address/program counter. The caller address is use to calculate the index into the callers[] array which points to arcs[] array.
    uint16_t count;    // how many times it has been called.
    uint16_t link;     // link to next entry in the table. For arcs[0] this points to the last used entry.
} gmon_arc_t;

// General rounding functions.
#define ROUNDDOWN(x,y)  (((x) / (y)) * (y))
#define ROUNDUP(x,y)    ((((x) + (y) - 1) / (y)) * (y))

// Profiling states.
enum {
    GMON_PROF_OFF,   // Off (disable sampling and ignore arc events).
    GMON_PROF_ON,    // Active.
    GMON_PROF_BUSY,  // Currently processing an arc or writing to the file.
    GMON_PROF_ERROR, // The arc table was full.
};

// The profiling data structures are housed in this structure.
typedef struct _gmon_state_t {
    // Current profiling state.
    volatile int state;

    // Instruction sampling histogram.
    size_t kcount_len;
    uint16_t *kcount;

    // Arcs are stored as linked lists (the gmon_arc_t struct) where each list is
    // all the arcs for a given caller address.
    // The callers table points to the head of the list for the address corresponding to that index.
    size_t callers_len;
    uint16_t *callers;
    size_t arcs_len;
    gmon_arc_t *arcs;

    // The range of addresses that we care about (typically the bounds of the FLASH_TEXT area).
    uintptr_t lowpc;
    uintptr_t highpc;
} gmon_state_t;

STATIC volatile gmon_state_t gmon_state = { GMON_PROF_OFF, 0 };

STATIC void gmon_initialise() {
    extern char _stext; // end of text/code symbol, defined by linker.
    extern char _etext; // end of text/code symbol, defined by linker.

    // Round lowpc and highpc to multiples of the density we're using
    // so the rest of the scaling stays in ints.
    gmon_state.state = GMON_PROF_OFF;
    gmon_state.lowpc = ROUNDDOWN((uintptr_t)&_stext, INSTRUCTION_SAMPLING_RESOLUTION * sizeof(uint16_t));
    gmon_state.highpc = ROUNDUP((uintptr_t)&_etext, INSTRUCTION_SAMPLING_RESOLUTION * sizeof(uint16_t));

    size_t text_size = gmon_state.highpc - gmon_state.lowpc;
    gmon_state.kcount_len = text_size / INSTRUCTION_SAMPLING_RESOLUTION;
    gmon_state.callers_len = text_size / CALLER_ADDRESS_RESOLUTION;

    size_t total_bytes = gmon_state.callers_len * sizeof(uint16_t) + TOTAL_ARCS * sizeof(gmon_arc_t) + gmon_state.kcount_len * sizeof(uint16_t);
    mp_printf(&mp_plat_print, "Profiling: text=" UINT_FMT " bytes, allocating=" UINT_FMT " bytes (caller=" UINT_FMT " arcs=" UINT_FMT " counts=" UINT_FMT ")\n", gmon_state.highpc - gmon_state.lowpc, total_bytes, gmon_state.callers_len * sizeof(uint16_t), TOTAL_ARCS * sizeof(gmon_arc_t), gmon_state.kcount_len * sizeof(uint16_t));
    MP_STATE_PORT(profiling_buffer) = m_new0(uint8_t, total_bytes);

    uint8_t *p = MP_STATE_PORT(profiling_buffer);
    gmon_state.arcs = (gmon_arc_t*)p;
    p += TOTAL_ARCS * sizeof(gmon_arc_t);
    gmon_state.kcount = (uint16_t *)p;
    p += gmon_state.kcount_len * sizeof(uint16_t);
    gmon_state.callers = (uint16_t *)p;

    gmon_state.state = GMON_PROF_OFF;
}

// This is called (via profiler_mcount.s) at the start of every function
// compiled with -pg. callerpc is the caller (the address of the instruction
// after the 'bl'), and selfpc is is the address of the instruction after the
// injected code.
void _mcount_internal(uintptr_t callerpc, uintptr_t selfpc) {
    // Check that we are profiling and that we aren't recursively invoked.
    if (gmon_state.state != GMON_PROF_ON) {
        return;
    }

    // Check that this is an arc entirely in text space.
    // For example: signal handlers get called from the stack, not from text space.
    if (callerpc < gmon_state.lowpc || callerpc >= gmon_state.highpc) {
        return;
    }
    if (selfpc < gmon_state.lowpc || selfpc >= gmon_state.highpc) {
        return;
    }

    // Prevent recursive invocations.
    gmon_state.state = GMON_PROF_BUSY;

    // Get a pointer to the entry in gmon_state.callers.
    // The value will either be zero (we've never seen an arc from this address),
    // or non-zero (the index of the start of the chain of dest address counts).
    uint16_t *caller = &gmon_state.callers[(callerpc - gmon_state.lowpc) / (CALLER_ADDRESS_RESOLUTION * sizeof(uint16_t))];

    // Get the head of the chain (or 0 if this chain doesn't exist).
    uint16_t next = *caller;

    if (next) {
        // A chain exists for this caller, search it for the callee.
        do {
            gmon_arc_t *entry = &gmon_state.arcs[next];

            if (entry->selfpc == selfpc && entry->count < 0xffff) {
                // Already have an entry for this arc, just increment the count.
                entry->count++;

                gmon_state.state = GMON_PROF_ON;
                return;
            }

            // Will either be a new entry to check, or zero (end of chain).
            next = entry->link;
        } while (next);
    }

    // Either a chain didn't exist or we didn't find this arc.
    // Try and create a new entry.

    // Check if table is full.
    if (gmon_state.arcs[0].link >= TOTAL_ARCS) {
        // Halt further profiling, arc table is full.
        gmon_state.state = GMON_PROF_ERROR;
        return;
    }
    next = ++gmon_state.arcs[0].link;
    gmon_arc_t *entry = &gmon_state.arcs[next];

    // Initialize the entry.
    entry->selfpc = selfpc;
    entry->count = 1;
    entry->link = *caller;

    // And make it the head of the chain for this callerpc.
    *caller = next;

    gmon_state.state = GMON_PROF_ON;
}

// Called at 1000Hz. Increment the histogram entry for this PC.
void gmon_systick(size_t pc) {
    if (gmon_state.state != GMON_PROF_ON) {
        return;
    }

    if (pc >= gmon_state.lowpc && pc < gmon_state.highpc) {
        pc = ROUNDDOWN(pc, INSTRUCTION_SAMPLING_RESOLUTION);
        size_t idx = (pc - gmon_state.lowpc) / INSTRUCTION_SAMPLING_RESOLUTION;
        gmon_state.kcount[idx]++;
    }
}

STATIC void gmon_active(bool active) {
    if (active) {
        gmon_initialise();
        gmon_state.state = GMON_PROF_ON;
    } else {
        gmon_state.state = GMON_PROF_OFF;
        size_t total_bytes = gmon_state.callers_len * sizeof(uint16_t) + TOTAL_ARCS * sizeof(gmon_arc_t) + gmon_state.kcount_len * sizeof(uint16_t);
        m_del(uint8_t, MP_STATE_PORT(profiling_buffer), total_bytes);
        MP_STATE_PORT(profiling_buffer) = NULL;
    }
}

STATIC const mp_rom_map_elem_t vfs_open_write_args_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_QSTR(MP_QSTR_wb) },
};
STATIC MP_DEFINE_CONST_MAP(vfs_open_write_args, vfs_open_write_args_table);

STATIC mp_obj_t profiler_active(size_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        gmon_active(mp_obj_is_true(args[0]));
    }
    return mp_obj_new_bool(gmon_state.state == GMON_PROF_ON);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(profiler_active_obj, 0, 1, profiler_active);

STATIC mp_obj_t profiler_write() {
    if (gmon_state.state == GMON_PROF_ERROR) {
        gmon_active(false);
        mp_raise_OSError(MP_ENOMEM);
    }

    gmon_state.state = GMON_PROF_BUSY;

    int stream_error;

    STATIC const char gmon_out[] = "gmon.out";
    mp_obj_t arg = mp_obj_new_str(gmon_out, strlen(gmon_out));
    mp_obj_t f = mp_vfs_open(1, &arg, (mp_map_t *)&vfs_open_write_args);

    mp_printf(&mp_plat_print, "Profiling: Writing " UINT_FMT " arcs to %s.\n", gmon_state.arcs[0].link, gmon_out);

    // Write header.
    gmon_hdr_t hdr = {0};
    hdr.lowpc = gmon_state.lowpc;
    hdr.highpc = gmon_state.highpc;
    hdr.ncnt = gmon_state.kcount_len * sizeof(uint16_t) + sizeof(gmon_hdr_t);
    hdr.version = GMONVERSION;
    hdr.profrate = INSTRUCTION_SAMPLING_HZ;
    mp_stream_rw(f, &hdr, sizeof(gmon_hdr_t), &stream_error, MP_STREAM_RW_WRITE);

    // Write sampling histogram.
    mp_stream_rw(f, gmon_state.kcount, gmon_state.kcount_len * sizeof(uint16_t), &stream_error, MP_STREAM_RW_WRITE);

    // Write arcs.
    for (size_t i = 0; i < gmon_state.callers_len; i++) {
        if (gmon_state.callers[i] == 0) {
            // Never saw an arc from this address.
            continue;
        }

        // Map back to 32-bit caller address.
        size_t callerpc = gmon_state.lowpc + i * CALLER_ADDRESS_RESOLUTION * sizeof(uint16_t);

        // Walk the chain for this caller and emit counts for each callee.
        for (uint16_t j = gmon_state.callers[i]; j != 0; j = gmon_state.arcs[j].link) {
            gmon_rawarc_t rawarc;
            rawarc.raw_callerpc = callerpc;
            rawarc.raw_selfpc = gmon_state.arcs[j].selfpc;
            rawarc.raw_count = gmon_state.arcs[j].count;
            mp_stream_rw(f, &rawarc, sizeof(rawarc), &stream_error, MP_STREAM_RW_WRITE);
        }
    }

    mp_stream_close(f);

    gmon_active(false);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(profiler_write_obj, profiler_write);

STATIC const mp_rom_map_elem_t profiler_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_profiler) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&profiler_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&profiler_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(profiler_module_globals, profiler_module_globals_table);

const mp_obj_module_t profiler_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&profiler_module_globals,
};

#endif // MICROPY_ENABLE_PROFILER
