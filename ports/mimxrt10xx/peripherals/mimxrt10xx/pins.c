#include "peripherals/mimxrt10xx/pins.h"

typedef struct {
    gpio_change_interrupt_t *func;
    void *data;
} pin_change_interrupt_data;

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

static volatile pin_change_interrupt_data pcid[MP_ARRAY_SIZE(s_gpioBases)][32];

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
