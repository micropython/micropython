#include "chip.h"

#define weak_alias(name, aliasname) \
    extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));

void SysTick_Handler(void) {
}

void Default_Handler(void) {
    while (1);
}

weak_alias(Default_Handler, NMI_Handler);
weak_alias(Default_Handler, HardFault_Handler);
weak_alias(Default_Handler, MemManage_Handler);
weak_alias(Default_Handler, BusFault_Handler);
weak_alias(Default_Handler, UsageFault_Handler);
weak_alias(Default_Handler, SVC_Handler);
weak_alias(Default_Handler, DebugMon_Handler);
weak_alias(Default_Handler, PendSV_Handler);
weak_alias(Default_Handler, IntDefaultHandler);

weak_alias(Default_Handler, DAC_IRQHandler);
#if defined (__USE_LPCOPEN)
weak_alias(Default_Handler, M0APP_IRQHandler);
#else
weak_alias(Default_Handler, M0CORE_IRQHandler);
#endif
weak_alias(Default_Handler, DMA_IRQHandler);
weak_alias(Default_Handler, FLASH_EEPROM_IRQHandler);
weak_alias(Default_Handler, ETH_IRQHandler);
weak_alias(Default_Handler, SDIO_IRQHandler);
weak_alias(Default_Handler, LCD_IRQHandler);
weak_alias(Default_Handler, USB0_IRQHandler);
weak_alias(Default_Handler, USB1_IRQHandler);
weak_alias(Default_Handler, SCT_IRQHandler);
weak_alias(Default_Handler, RIT_IRQHandler);
weak_alias(Default_Handler, TIMER0_IRQHandler);
weak_alias(Default_Handler, TIMER1_IRQHandler);
weak_alias(Default_Handler, TIMER2_IRQHandler);
weak_alias(Default_Handler, TIMER3_IRQHandler);
weak_alias(Default_Handler, MCPWM_IRQHandler);
weak_alias(Default_Handler, ADC0_IRQHandler);
weak_alias(Default_Handler, I2C0_IRQHandler);
weak_alias(Default_Handler, SPI_IRQHandler);
weak_alias(Default_Handler, I2C1_IRQHandler);
weak_alias(Default_Handler, ADC1_IRQHandler);
weak_alias(Default_Handler, SSP0_IRQHandler);
weak_alias(Default_Handler, SSP1_IRQHandler);
weak_alias(Default_Handler, UART0_IRQHandler);
weak_alias(Default_Handler, UART1_IRQHandler);
weak_alias(Default_Handler, UART2_IRQHandler);
weak_alias(Default_Handler, UART3_IRQHandler);
weak_alias(Default_Handler, I2S0_IRQHandler);
weak_alias(Default_Handler, I2S1_IRQHandler);
weak_alias(Default_Handler, SPIFI_IRQHandler);
weak_alias(Default_Handler, SGPIO_IRQHandler);
weak_alias(Default_Handler, GPIO0_IRQHandler);
weak_alias(Default_Handler, GPIO1_IRQHandler);
weak_alias(Default_Handler, GPIO2_IRQHandler);
weak_alias(Default_Handler, GPIO3_IRQHandler);
weak_alias(Default_Handler, GPIO4_IRQHandler);
weak_alias(Default_Handler, GPIO5_IRQHandler);
weak_alias(Default_Handler, GPIO6_IRQHandler);
weak_alias(Default_Handler, GPIO7_IRQHandler);
weak_alias(Default_Handler, GINT0_IRQHandler);
weak_alias(Default_Handler, GINT1_IRQHandler);
weak_alias(Default_Handler, EVRT_IRQHandler);
weak_alias(Default_Handler, CAN1_IRQHandler);
#if defined (__USE_LPCOPEN)
weak_alias(Default_Handler, ADCHS_IRQHandler);
#else
weak_alias(Default_Handler, VADC_IRQHandler);
#endif
weak_alias(Default_Handler, ATIMER_IRQHandler);
weak_alias(Default_Handler, RTC_IRQHandler);
weak_alias(Default_Handler, WDT_IRQHandler);
weak_alias(Default_Handler, M0SUB_IRQHandler);
weak_alias(Default_Handler, CAN0_IRQHandler);
weak_alias(Default_Handler, QEI_IRQHandler);

extern unsigned int _estack, _sidata, _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void) {

    __asm volatile ("cpsid i");
    Chip_RGU_TriggerResetAll();
    NVIC_ClearAll();
    __asm volatile ("cpsie i");

    // copy .data section from flash to RAM
    for (unsigned int *src = &_sidata, *dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }

    // zero out .bss section
    for (unsigned int *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }

    int main(void);
    main();
}

extern void (* const g_pfnVectors[])(void);
__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void*)&_estack,                // The initial stack pointer
    Reset_Handler,                  // The reset handler
    NMI_Handler,                    // The NMI handler
    HardFault_Handler,              // The hard fault handler
    MemManage_Handler,              // The MPU fault handler
    BusFault_Handler,               // The bus fault handler
    UsageFault_Handler,             // The usage fault handler
    0, //__valid_user_code_checksum,     // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    SVC_Handler,                    // SVCall handler
    DebugMon_Handler,               // Debug monitor handler
    0,                              // Reserved
    PendSV_Handler,                 // The PendSV handler
    SysTick_Handler,                // The SysTick handler
    DAC_IRQHandler,           // 16
#if defined (__USE_LPCOPEN)
    M0APP_IRQHandler,        // 17 CortexM4/M0 (LPC43XX ONLY)
#else
    M0CORE_IRQHandler,        // 17
#endif
    DMA_IRQHandler,           // 18
    0,           // 19
    FLASH_EEPROM_IRQHandler,   // 20 ORed flash Bank A, flash Bank B, EEPROM interrupts
    ETH_IRQHandler,           // 21
    SDIO_IRQHandler,          // 22
    LCD_IRQHandler,           // 23
    USB0_IRQHandler,          // 24
    USB1_IRQHandler,          // 25
    SCT_IRQHandler,           // 26
    RIT_IRQHandler,           // 27
    TIMER0_IRQHandler,        // 28
    TIMER1_IRQHandler,        // 29
    TIMER2_IRQHandler,        // 30
    TIMER3_IRQHandler,        // 31
    MCPWM_IRQHandler,         // 32
    ADC0_IRQHandler,          // 33
    I2C0_IRQHandler,          // 34
    I2C1_IRQHandler,          // 35
    SPI_IRQHandler,           // 36
    ADC1_IRQHandler,          // 37
    SSP0_IRQHandler,          // 38
    SSP1_IRQHandler,          // 39
    UART0_IRQHandler,         // 40
    UART1_IRQHandler,         // 41
    UART2_IRQHandler,         // 42
    UART3_IRQHandler,         // 43
    I2S0_IRQHandler,          // 44
    I2S1_IRQHandler,          // 45
    SPIFI_IRQHandler,         // 46
    SGPIO_IRQHandler,         // 47
    GPIO0_IRQHandler,         // 48
    GPIO1_IRQHandler,         // 49
    GPIO2_IRQHandler,         // 50
    GPIO3_IRQHandler,         // 51
    GPIO4_IRQHandler,         // 52
    GPIO5_IRQHandler,         // 53
    GPIO6_IRQHandler,         // 54
    GPIO7_IRQHandler,         // 55
    GINT0_IRQHandler,         // 56
    GINT1_IRQHandler,         // 57
    EVRT_IRQHandler,          // 58
    CAN1_IRQHandler,          // 59
    0,                        // 60
#if defined (__USE_LPCOPEN)
    ADCHS_IRQHandler,         // 61 ADCHS combined interrupt
#else
    VADC_IRQHandler,          // 61
#endif
    ATIMER_IRQHandler,        // 62
    RTC_IRQHandler,           // 63
    0,                        // 64
    WDT_IRQHandler,           // 65
    M0SUB_IRQHandler,         // 66
    CAN0_IRQHandler,          // 67
    QEI_IRQHandler,           // 68
};
