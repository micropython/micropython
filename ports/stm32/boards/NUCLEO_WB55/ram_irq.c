#include <string.h>
#include <py/mphal.h>

// This is the default MICROPY_BOARD_STARTUP on stm32
extern void powerctrl_check_enter_bootloader();

void ram_isr_fn_init() {

    // Copy IRQ vector table and other RAM functions to RAM and point VTOR there
    extern uint32_t _start_init_isr, _start_isr, _end_isr;
    memcpy(&_start_isr, &_start_init_isr, (&_end_isr - &_start_isr) * sizeof(uint32_t));
    SCB->VTOR = (uint32_t)&_start_isr;

    powerctrl_check_enter_bootloader();
}
