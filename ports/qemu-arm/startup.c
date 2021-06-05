#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "uart.h"

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

__attribute__((naked)) void Reset_Handler(void) {
    // set stack pointer
    __asm volatile ("ldr r0, =_estack");
    __asm volatile ("mov sp, r0");
    // copy .data section from flash to RAM
    for (uint32_t *src = &_sidata, *dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }
    // zero out .bss section
    for (uint32_t *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }
    // jump to board initialisation
    void _start(void);
    _start();
}

void Default_Handler(void) {
    for (;;) {
    }
}

#if defined(__ARM_ARCH_ISA_ARM)

// ARM architecture with standard ARM ISA.

__attribute__((naked, section(".isr_vector"))) void isr_vector(void) {
    __asm volatile (
        "b Reset_Handler\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        "nop\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        );
}

#elif defined(__ARM_ARCH_ISA_THUMB)

// ARM architecture with Thumb-only ISA.

const uint32_t isr_vector[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)&Reset_Handler,
    (uint32_t)&Default_Handler, // NMI_Handler
    (uint32_t)&Default_Handler, // HardFault_Handler
    (uint32_t)&Default_Handler, // MemManage_Handler
    (uint32_t)&Default_Handler, // BusFault_Handler
    (uint32_t)&Default_Handler, // UsageFault_Handler
    0,
    0,
    0,
    0,
    (uint32_t)&Default_Handler, // SVC_Handler
    (uint32_t)&Default_Handler, // DebugMon_Handler
    0,
    (uint32_t)&Default_Handler, // PendSV_Handler
    (uint32_t)&Default_Handler, // SysTick_Handler
};

#endif

void _start(void) {
    // Enable the UART
    uart_init();

    // Now that we have a basic system up and running we can call main
    extern int main();
    main(0, 0);

    // Finished
    exit(0);
}

__attribute__((naked)) void exit(int status) {
    // Force qemu to exit using ARM Semihosting
    __asm volatile (
        "mov r1, r0\n"
        "cmp r1, #0\n"
        "bne .notclean\n"
        "ldr r1, =0x20026\n" // ADP_Stopped_ApplicationExit, a clean exit
        ".notclean:\n"
        "movs r0, #0x18\n" // SYS_EXIT
        #if defined(__ARM_ARCH_ISA_ARM)
        "svc 0x00123456\n"
        #elif defined(__ARM_ARCH_ISA_THUMB)
        "bkpt 0xab\n"
        #endif
        );
    for (;;) {
    }
}

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    exit(1);
}
#endif

// The following are needed for tinytest

#include <stdio.h>

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
    return 0;
}

struct _reent *_impure_ptr;
