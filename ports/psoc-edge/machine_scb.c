/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "mpconfigport.h"

#if MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_UART

#include "py/runtime.h"
#include "genhdr/pins_af.h"
#include "machine_scb.h"

#define scb_irq_assert_raise_val(msg, ret)   if (ret != CY_SYSINT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

/* Forward declaration */
static void machine_scb_irq_handler(uint8_t scb);

#define DEFINE_SCB_IRQ_HANDLER(scb) \
    void SCB##scb##_IRQ_Handler(void) { \
        machine_scb_irq_handler(scb); \
    }

/**
 * The file build-<board>/genhdr/pins_af.h contains the macro
 *
 *  MICROPY_PY_MACHINE_FOR_ALL_SCB(DO)
 *
 *  which uses the X-macro (as argument) pattern to pass a worker
 *  macro DO(port) for the list of all user available ports.
 *
 * The available (not hidden) user SCBs are those alternate
 * functions defined in the boards/pse8x_af.csv file, for which
 * the corresponding pin are available for the user.
 * The available pins are those defined in the
 * boards/<board>/pins.csv file, which are not prefixed
 * with a hyphen(-).
 * See tools/boardgen.py and psoc-edge/boards/make-pins.py
 * for more information.
 */

MICROPY_PY_MACHINE_FOR_ALL_SCB(DEFINE_SCB_IRQ_HANDLER)

#define MAP_SCB_IRQ_CONFIG(scb) \
    [scb] = { \
        SCB##scb, \
        { \
            scb_##scb##_interrupt_IRQn, \
            SYS_INT_IRQ_LOWEST_PRIORITY, \
            SCB##scb##_IRQ_Handler \
        }, \
        PCLK_SCB##scb##_CLOCK_SCB_EN, \
        NULL, \
        NULL, \
    },

static machine_scb_obj_t machine_scb_obj[MICROPY_PY_MACHINE_SCB_NUM_ENTRIES] = {
    MICROPY_PY_MACHINE_FOR_ALL_SCB(MAP_SCB_IRQ_CONFIG)
};

static void machine_scb_irq_handler(uint8_t scb) {
    machine_scb_obj_t *scb_obj = &machine_scb_obj[scb];
    if (scb_obj->parent != NULL && scb_obj->parent_handler != NULL) {
        scb_obj->parent_handler(scb_obj->parent);
    }
}

machine_scb_obj_t *machine_scb_obj_alloc(uint8_t scb, mp_obj_t parent, machine_scb_parent_irq_handler_t handler) {
    machine_scb_obj_t *obj = &machine_scb_obj[scb];
    if (obj->parent != NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCB %u is already in use by another I2C, SPI or UART instance."), scb);
    }
    obj->parent = parent;
    obj->parent_handler = handler;
    return &machine_scb_obj[scb];
}

void machine_scb_obj_free(machine_scb_obj_t *scb) {
    scb->parent = NULL;
    scb->parent_handler = NULL;
}

#endif // MICROPY_PY_MACHINE_I2C || MICROPY_PY_MACHINE_I2C_TARGET || MICROPY_PY_MACHINE_SPI || MICROPY_PY_MACHINE_UART
