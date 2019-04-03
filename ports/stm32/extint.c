/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stddef.h>
#include <string.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "pin.h"
#include "extint.h"
#include "irq.h"

/// \moduleref pyb
/// \class ExtInt - configure I/O pins to interrupt on external events
///
/// There are a total of 22 interrupt lines. 16 of these can come from GPIO pins
/// and the remaining 6 are from internal sources.
///
/// For lines 0 thru 15, a given line can map to the corresponding line from an
/// arbitrary port. So line 0 can map to Px0 where x is A, B, C, ... and
/// line 1 can map to Px1 where x is A, B, C, ...
///
///     def callback(line):
///         print("line =", line)
///
/// Note: ExtInt will automatically configure the gpio line as an input.
///
///     extint = pyb.ExtInt(pin, pyb.ExtInt.IRQ_FALLING, pyb.Pin.PULL_UP, callback)
///
/// Now every time a falling edge is seen on the X1 pin, the callback will be
/// called. Caution: mechanical pushbuttons have "bounce" and pushing or
/// releasing a switch will often generate multiple edges.
/// See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
/// explanation, along with various techniques for debouncing.
///
/// Trying to register 2 callbacks onto the same pin will throw an exception.
///
/// If pin is passed as an integer, then it is assumed to map to one of the
/// internal interrupt sources, and must be in the range 16 thru 22.
///
/// All other pin objects go through the pin mapper to come up with one of the
/// gpio pins.
///
///     extint = pyb.ExtInt(pin, mode, pull, callback)
///
/// Valid modes are pyb.ExtInt.IRQ_RISING, pyb.ExtInt.IRQ_FALLING,
/// pyb.ExtInt.IRQ_RISING_FALLING, pyb.ExtInt.EVT_RISING,
/// pyb.ExtInt.EVT_FALLING, and pyb.ExtInt.EVT_RISING_FALLING.
///
/// Only the IRQ_xxx modes have been tested. The EVT_xxx modes have
/// something to do with sleep mode and the WFE instruction.
///
/// Valid pull values are pyb.Pin.PULL_UP, pyb.Pin.PULL_DOWN, pyb.Pin.PULL_NONE.
///
/// There is also a C API, so that drivers which require EXTI interrupt lines
/// can also use this code. See extint.h for the available functions and
/// usrsw.h for an example of using this.

// TODO Add python method to change callback object.

#define EXTI_OFFSET (EXTI_BASE - PERIPH_BASE)

// Macro used to set/clear the bit corresponding to the line in the IMR/EMR
// register in an atomic fashion by using bitband addressing.
#define EXTI_MODE_BB(mode, line) (*(__IO uint32_t *)(PERIPH_BB_BASE + ((EXTI_OFFSET + (mode)) * 32) + ((line) * 4)))

#if defined(STM32L4)
// The L4 MCU supports 40 Events/IRQs lines of the type configurable and direct.
// Here we only support configurable line types.  Details, see page 330 of RM0351, Rev 1.
// The USB_FS_WAKUP event is a direct type and there is no support for it.
#define EXTI_Mode_Interrupt   offsetof(EXTI_TypeDef, IMR1)
#define EXTI_Mode_Event       offsetof(EXTI_TypeDef, EMR1)
#define EXTI_Trigger_Rising   offsetof(EXTI_TypeDef, RTSR1)
#define EXTI_Trigger_Falling  offsetof(EXTI_TypeDef, FTSR1)
#define EXTI_RTSR EXTI->RTSR1
#define EXTI_FTSR EXTI->FTSR1
#elif defined(STM32H7)
#define EXTI_Mode_Interrupt   offsetof(EXTI_Core_TypeDef, IMR1)
#define EXTI_Mode_Event       offsetof(EXTI_Core_TypeDef, EMR1)
#define EXTI_Trigger_Rising   offsetof(EXTI_Core_TypeDef, RTSR1)
#define EXTI_Trigger_Falling  offsetof(EXTI_Core_TypeDef, FTSR1)
#define EXTI_RTSR EXTI->RTSR1
#define EXTI_FTSR EXTI->FTSR1
#else
#define EXTI_Mode_Interrupt   offsetof(EXTI_TypeDef, IMR)
#define EXTI_Mode_Event       offsetof(EXTI_TypeDef, EMR)
#define EXTI_Trigger_Rising   offsetof(EXTI_TypeDef, RTSR)
#define EXTI_Trigger_Falling  offsetof(EXTI_TypeDef, FTSR)
#define EXTI_RTSR EXTI->RTSR
#define EXTI_FTSR EXTI->FTSR
#endif

#define EXTI_SWIER_BB(line) (*(__IO uint32_t *)(PERIPH_BB_BASE + ((EXTI_OFFSET + offsetof(EXTI_TypeDef, SWIER)) * 32) + ((line) * 4)))

typedef struct {
    mp_obj_base_t base;
    mp_int_t line;
} extint_obj_t;

STATIC uint8_t pyb_extint_mode[EXTI_NUM_VECTORS];
STATIC bool pyb_extint_hard_irq[EXTI_NUM_VECTORS];

// The callback arg is a small-int or a ROM Pin object, so no need to scan by GC
STATIC mp_obj_t pyb_extint_callback_arg[EXTI_NUM_VECTORS];

#if !defined(ETH)
#define ETH_WKUP_IRQn   62  // Some MCUs don't have ETH, but we want a value to put in our table
#endif
#if !defined(OTG_HS_WKUP_IRQn)
#define OTG_HS_WKUP_IRQn 76  // Some MCUs don't have HS, but we want a value to put in our table
#endif
#if !defined(OTG_FS_WKUP_IRQn)
#define OTG_FS_WKUP_IRQn 42  // Some MCUs don't have FS IRQ, but we want a value to put in our table
#endif

STATIC const uint8_t nvic_irq_channel[EXTI_NUM_VECTORS] = {
    #if defined(STM32F0)
    EXTI0_1_IRQn,  EXTI0_1_IRQn,  EXTI2_3_IRQn,  EXTI2_3_IRQn,
    EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn,
    EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn,
    EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn, EXTI4_15_IRQn,
    PVD_VDDIO2_IRQn,
    RTC_IRQn,
    0, // internal USB wakeup event
    RTC_IRQn,
    RTC_IRQn,
    ADC1_COMP_IRQn,
    ADC1_COMP_IRQn,
    #else
    EXTI0_IRQn,     EXTI1_IRQn,     EXTI2_IRQn,     EXTI3_IRQn,     EXTI4_IRQn,
    EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,
    EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,
    EXTI15_10_IRQn,
    #if defined(STM32L4)
    PVD_PVM_IRQn,
    #else
    PVD_IRQn,
    #endif
    #if defined(STM32L4)
    OTG_FS_WKUP_IRQn,
    RTC_Alarm_IRQn,
    #else
    RTC_Alarm_IRQn,
    OTG_FS_WKUP_IRQn,
    #endif
    ETH_WKUP_IRQn,
    OTG_HS_WKUP_IRQn,
    TAMP_STAMP_IRQn,
    RTC_WKUP_IRQn,
    #endif
};

// Set override_callback_obj to true if you want to unconditionally set the
// callback function.
uint extint_register(mp_obj_t pin_obj, uint32_t mode, uint32_t pull, mp_obj_t callback_obj, bool override_callback_obj) {
    const pin_obj_t *pin = NULL;
    uint v_line;

    if (mp_obj_is_int(pin_obj)) {
        // If an integer is passed in, then use it to identify lines 16 thru 22
        // We expect lines 0 thru 15 to be passed in as a pin, so that we can
        // get both the port number and line number.
        v_line = mp_obj_get_int(pin_obj);
        if (v_line < 16) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d < 16, use a Pin object", v_line));
        }
        if (v_line >= EXTI_NUM_VECTORS) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d >= max of %d", v_line, EXTI_NUM_VECTORS));
        }
    } else {
        pin = pin_find(pin_obj);
        v_line = pin->pin;
    }
    if (mode != GPIO_MODE_IT_RISING &&
        mode != GPIO_MODE_IT_FALLING &&
        mode != GPIO_MODE_IT_RISING_FALLING &&
        mode != GPIO_MODE_EVT_RISING &&
        mode != GPIO_MODE_EVT_FALLING &&
        mode != GPIO_MODE_EVT_RISING_FALLING) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ExtInt Mode: %d", mode));
    }
    if (pull != GPIO_NOPULL &&
        pull != GPIO_PULLUP &&
        pull != GPIO_PULLDOWN) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ExtInt Pull: %d", pull));
    }

    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[v_line];
    if (!override_callback_obj && *cb != mp_const_none && callback_obj != mp_const_none) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d is already in use", v_line));
    }

    // We need to update callback atomically, so we disable the line
    // before we update anything.

    extint_disable(v_line);

    *cb = callback_obj;
    pyb_extint_mode[v_line] = (mode & 0x00010000) ? // GPIO_MODE_IT == 0x00010000
        EXTI_Mode_Interrupt : EXTI_Mode_Event;

    if (*cb != mp_const_none) {
        pyb_extint_hard_irq[v_line] = true;
        pyb_extint_callback_arg[v_line] = MP_OBJ_NEW_SMALL_INT(v_line);

        if (pin == NULL) {
            // pin will be NULL for non GPIO EXTI lines
            extint_trigger_mode(v_line, mode);
            extint_enable(v_line);
        } else {
            mp_hal_gpio_clock_enable(pin->gpio);
            GPIO_InitTypeDef exti;
            exti.Pin = pin->pin_mask;
            exti.Mode = mode;
            exti.Pull = pull;
            exti.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(pin->gpio, &exti);

            // Calling HAL_GPIO_Init does an implicit extint_enable
        }

        /* Enable and set NVIC Interrupt to the lowest priority */
        NVIC_SetPriority(IRQn_NONNEG(nvic_irq_channel[v_line]), IRQ_PRI_EXTINT);
        HAL_NVIC_EnableIRQ(nvic_irq_channel[v_line]);
    }
    return v_line;
}

// This function is intended to be used by the Pin.irq() method
void extint_register_pin(const pin_obj_t *pin, uint32_t mode, bool hard_irq, mp_obj_t callback_obj) {
    uint32_t line = pin->pin;

    // Check if the ExtInt line is already in use by another Pin/ExtInt
    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[line];
    if (*cb != mp_const_none && MP_OBJ_FROM_PTR(pin) != pyb_extint_callback_arg[line]) {
        if (mp_obj_is_small_int(pyb_extint_callback_arg[line])) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                "ExtInt vector %d is already in use", line));
        } else {
            const pin_obj_t *other_pin = MP_OBJ_TO_PTR(pyb_extint_callback_arg[line]);
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                "IRQ resource already taken by Pin('%q')", other_pin->name));
        }
    }

    extint_disable(line);

    *cb = callback_obj;
    pyb_extint_mode[line] = (mode & 0x00010000) ? // GPIO_MODE_IT == 0x00010000
        EXTI_Mode_Interrupt : EXTI_Mode_Event;

    if (*cb != mp_const_none) {
        // Configure and enable the callback

        pyb_extint_hard_irq[line] = hard_irq;
        pyb_extint_callback_arg[line] = MP_OBJ_FROM_PTR(pin);

        // Route the GPIO to EXTI
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        SYSCFG->EXTICR[line >> 2] =
            (SYSCFG->EXTICR[line >> 2] & ~(0x0f << (4 * (line & 0x03))))
            | ((uint32_t)(GPIO_GET_INDEX(pin->gpio)) << (4 * (line & 0x03)));

        extint_trigger_mode(line, mode);

        // Configure the NVIC
        NVIC_SetPriority(IRQn_NONNEG(nvic_irq_channel[line]), IRQ_PRI_EXTINT);
        HAL_NVIC_EnableIRQ(nvic_irq_channel[line]);

        // Enable the interrupt
        extint_enable(line);
    }
}

void extint_set(const pin_obj_t *pin, uint32_t mode) {
    uint32_t line = pin->pin;

    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[line];

    extint_disable(line);

    *cb = MP_OBJ_SENTINEL;

    pyb_extint_mode[line] = (mode & 0x00010000) ? // GPIO_MODE_IT == 0x00010000
        EXTI_Mode_Interrupt : EXTI_Mode_Event;

    {
        // Configure and enable the callback

        pyb_extint_hard_irq[line] = 1;
        pyb_extint_callback_arg[line] = MP_OBJ_FROM_PTR(pin);

        // Route the GPIO to EXTI
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        SYSCFG->EXTICR[line >> 2] =
            (SYSCFG->EXTICR[line >> 2] & ~(0x0f << (4 * (line & 0x03))))
            | ((uint32_t)(GPIO_GET_INDEX(pin->gpio)) << (4 * (line & 0x03)));

        // Enable or disable the rising detector
        if ((mode & GPIO_MODE_IT_RISING) == GPIO_MODE_IT_RISING) {
            EXTI_RTSR |= 1 << line;
        } else {
            EXTI_RTSR &= ~(1 << line);
        }

        // Enable or disable the falling detector
        if ((mode & GPIO_MODE_IT_FALLING) == GPIO_MODE_IT_FALLING) {
            EXTI_FTSR |= 1 << line;
        } else {
            EXTI_FTSR &= ~(1 << line);
        }

        // Configure the NVIC
        NVIC_SetPriority(IRQn_NONNEG(nvic_irq_channel[line]), IRQ_PRI_EXTINT);
        HAL_NVIC_EnableIRQ(nvic_irq_channel[line]);

        // Enable the interrupt
        extint_enable(line);
    }
}

void extint_enable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    #if defined(STM32F0) || defined(STM32F7) || defined(STM32H7)
    // The Cortex-M7 doesn't have bitband support.
    mp_uint_t irq_state = disable_irq();
    if (pyb_extint_mode[line] == EXTI_Mode_Interrupt) {
        #if defined(STM32H7)
        EXTI_D1->IMR1 |= (1 << line);
        #else
        EXTI->IMR |= (1 << line);
        #endif
    } else {
        #if defined(STM32H7)
        EXTI_D1->EMR1 |= (1 << line);
        #else
        EXTI->EMR |= (1 << line);
        #endif
    }
    enable_irq(irq_state);
    #else
    // Since manipulating IMR/EMR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(pyb_extint_mode[line], line) = 1;
    #endif
}

void extint_disable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }

    #if defined(STM32F0) || defined(STM32F7) || defined(STM32H7)
    // The Cortex-M7 doesn't have bitband support.
    mp_uint_t irq_state = disable_irq();
    #if defined(STM32H7)
    EXTI_D1->IMR1 &= ~(1 << line);
    EXTI_D1->EMR1 &= ~(1 << line);
    #else
    EXTI->IMR &= ~(1 << line);
    EXTI->EMR &= ~(1 << line);
    #endif
    enable_irq(irq_state);
    #else
    // Since manipulating IMR/EMR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(EXTI_Mode_Interrupt, line) = 0;
    EXTI_MODE_BB(EXTI_Mode_Event, line) = 0;
    #endif
}

void extint_swint(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    // we need 0 to 1 transition to trigger the interrupt
#if defined(STM32L4) || defined(STM32H7)
    EXTI->SWIER1 &= ~(1 << line);
    EXTI->SWIER1 |= (1 << line);
#else
    EXTI->SWIER &= ~(1 << line);
    EXTI->SWIER |= (1 << line);
#endif
}

void extint_trigger_mode(uint line, uint32_t mode) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    #if defined(STM32F0) || defined(STM32F7) || defined(STM32H7)
    // The Cortex-M7 doesn't have bitband support.
    mp_uint_t irq_state = disable_irq();
    // Enable or disable the rising detector
    if ((mode & GPIO_MODE_IT_RISING) == GPIO_MODE_IT_RISING) {
        EXTI_RTSR |= (1 << line);
    } else {
        EXTI_RTSR &= ~(1 << line);
    }
    // Enable or disable the falling detector
    if ((mode & GPIO_MODE_IT_FALLING) == GPIO_MODE_IT_FALLING) {
        EXTI_FTSR |= 1 << line;
    } else {
        EXTI_FTSR &= ~(1 << line);
    }
    enable_irq(irq_state);
    #else
    // Since manipulating FTSR/RTSR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(EXTI_Trigger_Rising, line) = (mode & GPIO_MODE_IT_RISING) == GPIO_MODE_IT_RISING;
    EXTI_MODE_BB(EXTI_Trigger_Falling, line) = (mode & GPIO_MODE_IT_FALLING) == GPIO_MODE_IT_FALLING;
    #endif
}

/// \method line()
/// Return the line number that the pin is mapped to.
STATIC mp_obj_t extint_obj_line(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->line);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_line_obj, extint_obj_line);

/// \method enable()
/// Enable a disabled interrupt.
STATIC mp_obj_t extint_obj_enable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    extint_enable(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);

/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
STATIC mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    extint_disable(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

/// \method swint()
/// Trigger the callback from software.
STATIC mp_obj_t extint_obj_swint(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    extint_swint(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_swint_obj,  extint_obj_swint);

// TODO document as a staticmethod
/// \classmethod regs()
/// Dump the values of the EXTI registers.
STATIC mp_obj_t extint_regs(void) {
    #if defined(STM32L4)
    printf("EXTI_IMR1   %08x\n", (unsigned int)EXTI->IMR1);
    printf("EXTI_IMR2   %08x\n", (unsigned int)EXTI->IMR2);
    printf("EXTI_EMR1   %08x\n", (unsigned int)EXTI->EMR1);
    printf("EXTI_EMR2   %08x\n", (unsigned int)EXTI->EMR2);
    printf("EXTI_RTSR1  %08x\n", (unsigned int)EXTI->RTSR1);
    printf("EXTI_RTSR2  %08x\n", (unsigned int)EXTI->RTSR2);
    printf("EXTI_FTSR1  %08x\n", (unsigned int)EXTI->FTSR1);
    printf("EXTI_FTSR2  %08x\n", (unsigned int)EXTI->FTSR2);
    printf("EXTI_SWIER1 %08x\n", (unsigned int)EXTI->SWIER1);
    printf("EXTI_SWIER2 %08x\n", (unsigned int)EXTI->SWIER2);
    printf("EXTI_PR1    %08x\n", (unsigned int)EXTI->PR1);
    printf("EXTI_PR2    %08x\n", (unsigned int)EXTI->PR2);
    #elif defined(STM32H7)
    printf("EXTI_IMR1   %08x\n", (unsigned int)EXTI_D1->IMR1);
    printf("EXTI_IMR2   %08x\n", (unsigned int)EXTI_D1->IMR2);
    printf("EXTI_IMR3   %08x\n", (unsigned int)EXTI_D1->IMR3);
    printf("EXTI_EMR1   %08x\n", (unsigned int)EXTI_D1->EMR1);
    printf("EXTI_EMR2   %08x\n", (unsigned int)EXTI_D1->EMR2);
    printf("EXTI_EMR3   %08x\n", (unsigned int)EXTI_D1->EMR3);
    printf("EXTI_RTSR1  %08x\n", (unsigned int)EXTI->RTSR1);
    printf("EXTI_RTSR2  %08x\n", (unsigned int)EXTI->RTSR2);
    printf("EXTI_RTSR3  %08x\n", (unsigned int)EXTI->RTSR3);
    printf("EXTI_FTSR1  %08x\n", (unsigned int)EXTI->FTSR1);
    printf("EXTI_FTSR2  %08x\n", (unsigned int)EXTI->FTSR2);
    printf("EXTI_FTSR3  %08x\n", (unsigned int)EXTI->FTSR3);
    printf("EXTI_SWIER1 %08x\n", (unsigned int)EXTI->SWIER1);
    printf("EXTI_SWIER2 %08x\n", (unsigned int)EXTI->SWIER2);
    printf("EXTI_SWIER3 %08x\n", (unsigned int)EXTI->SWIER3);
    printf("EXTI_PR1    %08x\n", (unsigned int)EXTI_D1->PR1);
    printf("EXTI_PR2    %08x\n", (unsigned int)EXTI_D1->PR2);
    printf("EXTI_PR3    %08x\n", (unsigned int)EXTI_D1->PR3);
    #else
    printf("EXTI_IMR   %08x\n", (unsigned int)EXTI->IMR);
    printf("EXTI_EMR   %08x\n", (unsigned int)EXTI->EMR);
    printf("EXTI_RTSR  %08x\n", (unsigned int)EXTI->RTSR);
    printf("EXTI_FTSR  %08x\n", (unsigned int)EXTI->FTSR);
    printf("EXTI_SWIER %08x\n", (unsigned int)EXTI->SWIER);
    printf("EXTI_PR    %08x\n", (unsigned int)EXTI->PR);
    #endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(extint_regs_fun_obj, extint_regs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(extint_regs_obj, MP_ROM_PTR(&extint_regs_fun_obj));

/// \classmethod \constructor(pin, mode, pull, callback)
/// Create an ExtInt object:
///
///   - `pin` is the pin on which to enable the interrupt (can be a pin object or any valid pin name).
///   - `mode` can be one of:
///     - `ExtInt.IRQ_RISING` - trigger on a rising edge;
///     - `ExtInt.IRQ_FALLING` - trigger on a falling edge;
///     - `ExtInt.IRQ_RISING_FALLING` - trigger on a rising or falling edge.
///   - `pull` can be one of:
///     - `pyb.Pin.PULL_NONE` - no pull up or down resistors;
///     - `pyb.Pin.PULL_UP` - enable the pull-up resistor;
///     - `pyb.Pin.PULL_DOWN` - enable the pull-down resistor.
///   - `callback` is the function to call when the interrupt triggers.  The
///   callback function must accept exactly 1 argument, which is the line that
///   triggered the interrupt.
STATIC const mp_arg_t pyb_extint_make_new_args[] = {
    { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pull,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
#define PYB_EXTINT_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(pyb_extint_make_new_args)

STATIC mp_obj_t extint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // type_in == extint_obj_type

    // parse args
    mp_arg_val_t vals[PYB_EXTINT_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, PYB_EXTINT_MAKE_NEW_NUM_ARGS, pyb_extint_make_new_args, vals);

    extint_obj_t *self = m_new_obj(extint_obj_t);
    self->base.type = type;
    self->line = extint_register(vals[0].u_obj, vals[1].u_int, vals[2].u_int, vals[3].u_obj, false);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void extint_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ExtInt line=%u>", self->line);
}

STATIC const mp_rom_map_elem_t extint_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_line),    MP_ROM_PTR(&extint_obj_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),  MP_ROM_PTR(&extint_obj_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&extint_obj_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_swint),   MP_ROM_PTR(&extint_obj_swint_obj) },
    { MP_ROM_QSTR(MP_QSTR_regs),    MP_ROM_PTR(&extint_regs_obj) },

    // class constants
    /// \constant IRQ_RISING - interrupt on a rising edge
    /// \constant IRQ_FALLING - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING - interrupt on a rising or falling edge
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),         MP_ROM_INT(GPIO_MODE_IT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING),        MP_ROM_INT(GPIO_MODE_IT_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING_FALLING), MP_ROM_INT(GPIO_MODE_IT_RISING_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_EVT_RISING),         MP_ROM_INT(GPIO_MODE_EVT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_EVT_FALLING),        MP_ROM_INT(GPIO_MODE_EVT_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_EVT_RISING_FALLING), MP_ROM_INT(GPIO_MODE_EVT_RISING_FALLING) },
};

STATIC MP_DEFINE_CONST_DICT(extint_locals_dict, extint_locals_dict_table);

const mp_obj_type_t extint_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtInt,
    .print = extint_obj_print,
    .make_new = extint_make_new,
    .locals_dict = (mp_obj_dict_t*)&extint_locals_dict,
};

void extint_init0(void) {
    for (int i = 0; i < PYB_EXTI_NUM_VECTORS; i++) {
        if (MP_STATE_PORT(pyb_extint_callback)[i] == MP_OBJ_SENTINEL) {
            continue;
        }
        MP_STATE_PORT(pyb_extint_callback)[i] = mp_const_none;
        pyb_extint_mode[i] = EXTI_Mode_Interrupt;
   }
}

// Interrupt handler
void Handle_EXTI_Irq(uint32_t line) {
    if (__HAL_GPIO_EXTI_GET_FLAG(1 << line)) {
        __HAL_GPIO_EXTI_CLEAR_FLAG(1 << line);
        if (line < EXTI_NUM_VECTORS) {
            mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[line];
            if (*cb != mp_const_none) {
                // If it's a soft IRQ handler then just schedule callback for later
                if (!pyb_extint_hard_irq[line]) {
                    mp_sched_schedule(*cb, pyb_extint_callback_arg[line]);
                    return;
                }

                mp_sched_lock();
                // When executing code within a handler we must lock the GC to prevent
                // any memory allocations.  We must also catch any exceptions.
                gc_lock();
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    mp_call_function_1(*cb, pyb_extint_callback_arg[line]);
                    nlr_pop();
                } else {
                    // Uncaught exception; disable the callback so it doesn't run again.
                    *cb = mp_const_none;
                    extint_disable(line);
                    printf("Uncaught exception in ExtInt interrupt handler line %u\n", (unsigned int)line);
                    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
                }
                gc_unlock();
                mp_sched_unlock();
            }
        }
    }
}
