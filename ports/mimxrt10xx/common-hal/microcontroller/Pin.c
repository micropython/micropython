/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/gc.h"

STATIC bool claimed_pins[IOMUXC_SW_PAD_CTL_PAD_COUNT];
STATIC bool never_reset_pins[IOMUXC_SW_PAD_CTL_PAD_COUNT];

// Default is that no pins are forbidden to reset.
MP_WEAK const mcu_pin_obj_t *mimxrt10xx_reset_forbidden_pins[] = {
    NULL,
};

STATIC bool _reset_forbidden(const mcu_pin_obj_t *pin) {
    const mcu_pin_obj_t **forbidden_pin = &mimxrt10xx_reset_forbidden_pins[0];
    while (*forbidden_pin) {
        if (pin == *forbidden_pin) {
            return true;
        }
        forbidden_pin++;
    }
    return false;
}

// There are two numbering systems used here:
// IOMUXC index, used for iterating through pins and accessing reset information,
// and GPIO port and number, used to store claimed and reset tagging. The two number
// systems are not related and one cannot determine the other without a pin object
void reset_all_pins(void) {
    for (uint8_t i = 0; i < IOMUXC_SW_PAD_CTL_PAD_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }
    for (uint8_t i = 0; i < IOMUXC_SW_PAD_CTL_PAD_COUNT; i++) {
        mcu_pin_obj_t *pin = mcu_pin_globals.map.table[i].value;
        if (never_reset_pins[pin->mux_idx]) {
            continue;
        }
        common_hal_reset_pin(pin);
    }
}

MP_WEAK bool mimxrt10xx_board_reset_pin_number(const mcu_pin_obj_t *pin) {
    return false;
}

// Since i.MX pins need extra register and reset information to reset properly,
// resetting pins by number alone has been removed.
void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }

    if (_reset_forbidden(pin)) {
        return;
    }

    // Give the board a chance to reset the pin in a particular way, or not reset it at all.
    if (mimxrt10xx_board_reset_pin_number(pin)) {
        return;
    }

    disable_pin_change_interrupt(pin);
    never_reset_pins[pin->mux_idx] = false;
    claimed_pins[pin->mux_idx] = false;
    *(uint32_t *)pin->mux_reg = pin->mux_reset;
    *(uint32_t *)pin->cfg_reg = pin->pad_reset;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    never_reset_pins[pin->mux_idx] = true;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return !claimed_pins[pin->mux_idx];
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->mux_idx; // returns IOMUXC to align with pin table
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claimed_pins[pin->mux_idx] = true;
}

void claim_pin(const mcu_pin_obj_t *pin) {
    common_hal_mcu_pin_claim(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    common_hal_reset_pin((mcu_pin_obj_t *)(mcu_pin_globals.map.table[pin_no].value));
}

/* Array of GPIO peripheral base address. */
static GPIO_Type *const s_gpioBases[] = GPIO_BASE_PTRS;
static uint32_t GPIO_GetInstance(GPIO_Type *base) {
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0U; instance < ARRAY_SIZE(s_gpioBases); instance++)
    {
        if (s_gpioBases[instance] == base) {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_gpioBases));

    return instance;
}

/* to find IRQ based on GPIO */
static const IRQn_Type low_irqs[] = GPIO_COMBINED_LOW_IRQS;
static const IRQn_Type high_irqs[] = GPIO_COMBINED_HIGH_IRQS;

typedef struct {
    gpio_change_interrupt_t *func;
    void *data;
} pin_change_interrupt_data;

volatile static pin_change_interrupt_data pcid[MP_ARRAY_SIZE(s_gpioBases)][32];

// The 'data' pointers may be to gc objects, they must be kept alive.
void pin_gc_collect() {
    gc_collect_root((void **)&pcid, sizeof(pcid) / sizeof(void *));
}

void enable_pin_change_interrupt(const mcu_pin_obj_t *pin, gpio_change_interrupt_t func, void *data) {
    int instance = GPIO_GetInstance(pin->gpio);
    volatile pin_change_interrupt_data *pci = &pcid[instance][pin->number];
    common_hal_mcu_disable_interrupts();
    pci->data = data;
    pci->func = func;
    IRQn_Type irq = pin->number < 16 ? low_irqs[instance] : high_irqs[instance];
    if (irq != NotAvail_IRQn) {
        EnableIRQ(irq);
    }
    pin->gpio->IMR |= (1 << pin->number);
    common_hal_mcu_enable_interrupts();
}

void disable_pin_change_interrupt(const mcu_pin_obj_t *pin) {
    volatile pin_change_interrupt_data *pci = &pcid[GPIO_GetInstance(pin->gpio)][pin->number];
    common_hal_mcu_disable_interrupts();
    pin->gpio->IMR &= ~(1 << pin->number);
    pci->data = NULL;
    pci->func = NULL;
    pin->gpio->ISR = (1 << pin->number); // acknowledge any pending interrupt
    common_hal_mcu_enable_interrupts();
}

static void pin_change_interrupt_common(uint32_t isr, volatile pin_change_interrupt_data *pcr) {
    for (uint32_t i = 0; i < 32; i++) {
        if (isr & (1 << i)) {
            pin_change_interrupt_data cb = pcr[i];
            if (cb.func) {
                cb.func(cb.data);
            }
        }
    }
}

#define GPIO_INTERRUPT_HANDLER(name, ptr, instance, offset) \
    void name(void); \
    __attribute__((used)) void name(void) { \
        uint32_t isr = ptr->ISR; \
        ptr->ISR = isr; \
        pin_change_interrupt_common(isr, pcid[instance]); \
    }

#if defined(GPIO1)
GPIO_INTERRUPT_HANDLER(GPIO1_Combined_0_15_IRQHandler, GPIO1, 1, 0);
GPIO_INTERRUPT_HANDLER(GPIO1_Combined_16_31_IRQHandler, GPIO1, 1, 16);
#endif
#if defined(GPIO2)
GPIO_INTERRUPT_HANDLER(GPIO2_Combined_0_15_IRQHandler, GPIO2, 2, 0);
GPIO_INTERRUPT_HANDLER(GPIO2_Combined_16_31_IRQHandler, GPIO2, 2, 16);
#endif
#if defined(GPIO3)
GPIO_INTERRUPT_HANDLER(GPIO3_Combined_0_15_IRQHandler, GPIO3, 3, 0);
GPIO_INTERRUPT_HANDLER(GPIO3_Combined_16_31_IRQHandler, GPIO3, 3, 16);
#endif
#if defined(GPIO4)
GPIO_INTERRUPT_HANDLER(GPIO4_Combined_0_15_IRQHandler, GPIO4, 4, 0);
GPIO_INTERRUPT_HANDLER(GPIO4_Combined_16_31_IRQHandler, GPIO4, 4, 16);
#endif
#if defined(GPIO5)
GPIO_INTERRUPT_HANDLER(GPIO5_Combined_0_15_IRQHandler, GPIO5, 5, 0);
GPIO_INTERRUPT_HANDLER(GPIO5_Combined_16_31_IRQHandler, GPIO5, 5, 16);
#endif
#if defined(GPIO6)
GPIO_INTERRUPT_HANDLER(GPIO6_Combined_0_15_IRQHandler, GPIO6, 6, 0);
GPIO_INTERRUPT_HANDLER(GPIO6_Combined_16_31_IRQHandler, GPIO6, 6, 16);
#endif
