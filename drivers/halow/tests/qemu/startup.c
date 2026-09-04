/*
 * Minimal bare-metal startup for the QEMU mps3-an547 (Cortex-M55) harness.
 *
 * Deliberately self-contained rather than reusing ports/qemu/mcu/arm/startup.c,
 * which pulls in MicroPython. All this needs to do is land in main() on a core
 * with the FPU enabled, and give the test a way to print and to exit QEMU.
 */

#include <stdint.h>

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

int main(void);

// --- ARM semihosting ---------------------------------------------------------
// Output and exit without needing a UART model. QEMU is run with -semihosting.
#define SYS_WRITE0  (0x04)
#define SYS_EXIT    (0x18)
#define ADP_STOPPED_APPLICATION_EXIT (0x20026)
#define ADP_STOPPED_RUN_TIME_ERROR   (0x20023)

static inline int semihost(int op, void *arg) {
    register int r0 __asm__ ("r0") = op;
    register void *r1 __asm__ ("r1") = arg;
    __asm__ volatile ("bkpt 0xAB" : "+r" (r0) : "r" (r1) : "memory");
    return r0;
}

void qemu_puts(const char *s) {
    semihost(SYS_WRITE0, (void *)s);
}

void qemu_exit(int code) {
    // On AArch32, SYS_EXIT takes the reason code DIRECTLY in r1. Passing a
    // pointer to a {reason, subcode} block is the AArch64 / SYS_EXIT_EXTENDED
    // convention; do that here and QEMU reads the pointer value as an unknown
    // reason and exits non-zero -- so a fully passing run still failed the
    // build, which is worse than a test that just fails honestly.
    semihost(SYS_EXIT, (void *)(uintptr_t)(code == 0
        ? ADP_STOPPED_APPLICATION_EXIT
        : ADP_STOPPED_RUN_TIME_ERROR));
    for (;;) {
    }
}

// --- fault handlers ----------------------------------------------------------
// A context-switch bug shows up as a HardFault. Naming it beats QEMU spinning
// silently, which is what an unhandled fault otherwise looks like.
static void fault(const char *name) {
    qemu_puts("\nFAULT: ");
    qemu_puts(name);
    qemu_puts("\n");
    qemu_exit(1);
}

void HardFault_Handler(void) {
    fault("HardFault");
}
void MemManage_Handler(void) {
    fault("MemManage");
}
void BusFault_Handler(void) {
    fault("BusFault");
}
void UsageFault_Handler(void) {
    fault("UsageFault");
}
static void Default_Handler(void) {
    fault("unexpected exception");
}

__attribute__((naked)) void Reset_Handler(void) {
    __asm volatile (
        "ldr r0, =_estack   \n"
        "mov sp, r0         \n"
        "bl  startup_main   \n"
        );
}

void startup_main(void) {
    for (uint32_t *src = &_sidata, *dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }
    for (uint32_t *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }
    // Enable CP10/CP11 (the FPU). The scheduler's context switch saves d8-d15
    // when built with hard float, so without this the first vpush faults.
    #define SCB_CPACR (*(volatile uint32_t *)0xE000ED88)
    SCB_CPACR |= (0xF << 20);
    __asm volatile ("dsb");
    __asm volatile ("isb");

    int rc = main();
    qemu_exit(rc);
}

// --- vector table ------------------------------------------------------------
__attribute__((section(".isr_vector"), used))
void(*const isr_vector[])(void) = {
    (void (*)(void)) & _estack,
    Reset_Handler,
    Default_Handler,      // NMI
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
};
