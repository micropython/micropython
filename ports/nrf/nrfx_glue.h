/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Glenn Ruben Bakke
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

#ifndef NRFX_GLUE_H
#define NRFX_GLUE_H

#include "py/mpconfig.h"
#include "py/misc.h"

#include <soc/nrfx_irqs.h>
#include <soc/nrfx_atomic.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE MP_ARRAY_SIZE
#endif

#define NRFX_STATIC_ASSERT(expression)

#define NRFX_ASSERT(expression)  do { bool res = expression; (void)res; } while (0)

void mp_hal_delay_us(mp_uint_t us);
#define NRFX_DELAY_US            mp_hal_delay_us

#define nrfx_atomic_t nrfx_atomic_u32_t
#define NRFX_ATOMIC_FETCH_STORE(p_data, value) nrfx_atomic_u32_fetch_store(p_data, value)
#define NRFX_ATOMIC_FETCH_OR(p_data, value)    nrfx_atomic_u32_fetch_or(p_data, value)
#define NRFX_ATOMIC_FETCH_AND(p_data, value)   nrfx_atomic_u32_fetch_and(p_data, value)
#define NRFX_ATOMIC_FETCH_XOR(p_data, value)   nrfx_atomic_u32_fetch_xor(p_data, value)
#define NRFX_ATOMIC_FETCH_ADD(p_data, value)   nrfx_atomic_u32_fetch_add(p_data, value)
#define NRFX_ATOMIC_FETCH_SUB(p_data, value)   nrfx_atomic_u32_fetch_sub(p_data, value)
#define NRFX_ATOMIC_CAS(p_data, old_value, new_value) \
    nrfx_atomic_u32_cmp_exch(p_data, &(old_value), new_value)

#define NRFX_CLZ(value) __CLZ(value)
#define NRFX_CTZ(value) __CLZ(__RBIT(value))

// Event readback is required on all nRF51/nRF52 devices
#define NRFX_EVENT_READBACK_ENABLED 1

// Override NRFY_CACHE_INV to suppress unused variable warnings
// on targets without data cache (nRF51/nRF52).
#define NRFY_CACHE_INV(p_buffer, size) do { (void)(p_buffer); (void)(size); } while (0)
#define NRFY_CACHE_WB(p_buffer, size) do { (void)(p_buffer); (void)(size); } while (0)
#define NRFY_CACHE_WBINV(p_buffer, size) do { (void)(p_buffer); (void)(size); } while (0)

#define NRFX_DPPI_CHANNELS_USED   0
#define NRFX_DPPI_GROUPS_USED     0
#define NRFX_PPI_CHANNELS_USED    0
#define NRFX_PPI_GROUPS_USED      0
#define NRFX_EGUS_USED            0
#define NRFX_GPIOTE_CHANNELS_USED 0
#define NRFX_TIMERS_USED          0

#if BLUETOOTH_SD

#if NRF51
#include "nrf_soc.h"
#else
#include "nrf_nvic.h"
#endif

#include "ble_drv.h"

#if (BLUETOOTH_SD == 110)
#define NRFX_IRQ_ENABLE(irq_number) \
    do { \
        if (ble_drv_stack_enabled() == 1) \
        { \
            sd_nvic_EnableIRQ(irq_number); \
        } else { \
            NVIC_EnableIRQ(irq_number); \
        } \
    } while (0)
#else
#define NRFX_IRQ_ENABLE(irq_number) sd_nvic_EnableIRQ(irq_number)
#endif

#if (BLUETOOTH_SD == 110)
#define NRFX_IRQ_DISABLE(irq_number) \
    do { \
        if (ble_drv_stack_enabled() == 1) \
        { \
            sd_nvic_DisableIRQ(irq_number);  \
        } else { \
            NVIC_DisableIRQ(irq_number); \
        } \
    } while (0)
#else
#define NRFX_IRQ_DISABLE(irq_number) sd_nvic_DisableIRQ(irq_number)
#endif

#if (BLUETOOTH_SD == 110)
#define NRFX_IRQ_PRIORITY_SET(irq_number, priority) \
    do { \
        if (ble_drv_stack_enabled() == 1) \
        { \
            sd_nvic_SetPriority(irq_number, priority); \
        } else { \
            NVIC_SetPriority(irq_number, priority); \
        } \
    } while (0)
#else
#define NRFX_IRQ_PRIORITY_SET(irq_number, priority) sd_nvic_SetPriority(irq_number, priority)
#endif

#if (BLUETOOTH_SD == 110)
#define NRFX_IRQ_PENDING_SET(irq_number) \
    do { \
        if (ble_drv_stack_enabled() == 1) \
        { \
            sd_nvic_SetPendingIRQ(irq_number); \
        } else { \
            NVIC_SetPendingIRQ(irq_number); \
        } \
    } while (0)
#else
#define NRFX_IRQ_PENDING_SET(irq_number) sd_nvic_SetPendingIRQ(irq_number)
#endif

#if (BLUETOOTH_SD == 110)
#define NRFX_IRQ_PENDING_CLEAR(irq_number) \
    do { \
        if (ble_drv_stack_enabled() == 1) \
        { \
            sd_nvic_ClearPendingIRQ(irq_number); \
        } else { \
            NVIC_ClearPendingIRQ(irq_number)(irq_number); \
        } \
    } while (0)
#else
#define NRFX_IRQ_PENDING_CLEAR(irq_number) sd_nvic_ClearPendingIRQ(irq_number)
#endif

#define NRFX_CRITICAL_SECTION_ENTER() \
    { \
        uint8_t _is_nested_critical_region; \
        sd_nvic_critical_region_enter(&_is_nested_critical_region);

#define NRFX_CRITICAL_SECTION_EXIT() \
    sd_nvic_critical_region_exit(_is_nested_critical_region); \
}

#else // BLUETOOTH_SD

#define NRFX_IRQ_ENABLE(irq_number) NVIC_EnableIRQ(irq_number)
#define NRFX_IRQ_DISABLE(irq_number) NVIC_DisableIRQ(irq_number)
#define NRFX_IRQ_PRIORITY_SET(irq_number, priority) NVIC_SetPriority(irq_number, priority)
#define NRFX_IRQ_PENDING_SET(irq_number) NVIC_SetPendingIRQ(irq_number)
#define NRFX_IRQ_PENDING_CLEAR(irq_number) NVIC_ClearPendingIRQ(irq_number)

// Source:
// https://devzone.nordicsemi.com/f/nordic-q-a/8572/disable-interrupts-and-enable-interrupts-if-they-where-enabled/31347#31347
#define NRFX_CRITICAL_SECTION_ENTER() { int _old_primask = __get_PRIMASK(); __disable_irq();
#define NRFX_CRITICAL_SECTION_EXIT() __set_PRIMASK(_old_primask); }

#endif // !BLUETOOTH_SD

#define NRFX_IRQ_IS_ENABLED(irq_number) (0 != (NVIC->ISER[irq_number / 32] & (1UL << (irq_number % 32))))

#define NRFX_IRQ_IS_PENDING(irq_number) (0 != (NVIC->ISPR[irq_number / 32] & (1UL << (irq_number % 32))))

#endif // NRFX_GLUE_H
