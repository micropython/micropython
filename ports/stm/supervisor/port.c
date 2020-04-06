/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include <stdint.h>
#include "supervisor/port.h"
#include "boards/board.h"
#include "tick.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"

#if defined(STM32F4)
    #include "common-hal/pulseio/PWMOut.h"
    #include "common-hal/pulseio/PulseOut.h"
    #include "common-hal/pulseio/PulseIn.h"
#endif

#include "clocks.h"
#include "gpio.h"

//only enable the Reset Handler overwrite for the H7 for now
#if defined(STM32H7)

// Device memories must be accessed in order.
#define DEVICE 2
// Normal memory can have accesses reorder and prefetched.
#define NORMAL 0
// Prevents instruction access.
#define NO_EXECUTION 1
#define EXECUTION 0
// Shareable if the memory system manages coherency.
#define NOT_SHAREABLE 0
#define SHAREABLE 1
#define NOT_CACHEABLE 0
#define CACHEABLE 1
#define NOT_BUFFERABLE 0
#define BUFFERABLE 1
#define NO_SUBREGIONS 0

extern uint32_t _ld_stack_top;

extern uint32_t _ld_d1_ram_bss_start;
extern uint32_t _ld_d1_ram_bss_size;
extern uint32_t _ld_d1_ram_data_destination;
extern uint32_t _ld_d1_ram_data_size;
extern uint32_t _ld_d1_ram_data_flash_copy;
extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;

extern void main(void);
extern void SystemInit(void);

// This replaces the Reset_Handler in startup_*.S and SystemInit in system_*.c.
__attribute__((used, naked)) void Reset_Handler(void) {
    __disable_irq();
    __set_MSP((uint32_t) &_ld_stack_top);

    // TODO: Is any of this commented stuff actually required? 

    /* Disable I cache and D cache */ 
    // SCB_DisableICache();
    // SCB_DisableDCache(); // this causes an instant hardfault if used

    // #if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    //   SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
    // #endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

    // /* Disable Systick which might be enabled by bootrom */
    // if (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk)
    // {
    //     SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    // }

    /* Disable MPU */
    ARM_MPU_Disable();

    // Copy all of the itcm code to run from ITCM. Do this while the MPU is disabled because we write
    // protect it.
    for (uint32_t i = 0; i < ((size_t) &_ld_itcm_size) / 4; i++) {
        (&_ld_itcm_destination)[i] = (&_ld_itcm_flash_copy)[i];
    }

    // The first number in RBAR is the region number. When searching for a policy, the region with
    // the highest number wins. If none match, then the default policy set at enable applies.
    // TODO: what is the default policy? Where is that set? 

    // TODO: do I need to subdivide this up? 
    // Mark all the flash the same until instructed otherwise. 
    MPU->RBAR = ARM_MPU_RBAR(11, 0x08000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_2MB);

    // This the ITCM. Set it to read-only because we've loaded everything already and it's easy to
    // accidentally write the wrong value to 0x00000000 (aka NULL).
    MPU->RBAR = ARM_MPU_RBAR(12, 0x00000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_RO, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_128KB);

    // This the DTCM.
    MPU->RBAR = ARM_MPU_RBAR(14, 0x20000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_128KB);

    // This is AXI SRAM (D1).
    MPU->RBAR = ARM_MPU_RBAR(15, 0x24000000U);
    MPU->RASR = ARM_MPU_RASR(EXECUTION, ARM_MPU_AP_FULL, NORMAL, NOT_SHAREABLE, CACHEABLE, BUFFERABLE, NO_SUBREGIONS, ARM_MPU_REGION_SIZE_512KB);

    // TODO: what is the mask here doing? 
    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    //  We're done mucking with memory so enable I cache and D cache 
    // SCB_EnableDCache();
    // SCB_EnableICache();

    // Copy all of the data to run from DTCM.
    for (uint32_t i = 0; i < ((size_t) &_ld_dtcm_data_size) / 4; i++) {
        (&_ld_dtcm_data_destination)[i] = (&_ld_dtcm_data_flash_copy)[i];
    }

    // Clear DTCM bss.
    for (uint32_t i = 0; i < ((size_t) &_ld_dtcm_bss_size) / 4; i++) {
        (&_ld_dtcm_bss_start)[i] = 0;
    }

    // Copy all of the data to run from D1 RAM.
    for (uint32_t i = 0; i < ((size_t) &_ld_d1_ram_data_size) / 4; i++) {
        (&_ld_d1_ram_data_destination)[i] = (&_ld_d1_ram_data_flash_copy)[i];
    }

    // Clear D1 RAM bss.
    for (uint32_t i = 0; i < ((size_t) &_ld_d1_ram_bss_size) / 4; i++) {
        (&_ld_d1_ram_bss_start)[i] = 0;
    }

    SystemInit();
    __enable_irq();
    main();
}

#endif //end H7 specific code

safe_mode_t port_init(void) {
    HAL_Init();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    
    #if defined(STM32F4)
        __HAL_RCC_PWR_CLK_ENABLE();
    #endif

    stm32_peripherals_clocks_init();
    stm32_peripherals_gpio_init();

    tick_init();

    return NO_SAFE_MODE;
}

void reset_port(void) {
    reset_all_pins();
    i2c_reset();
    spi_reset();
    uart_reset();

    // TODO: it'd be nice if this was more automatic
    #if defined(STM32F4)

        pwmout_reset();
        pulseout_reset();
        pulsein_reset();
    #endif
}

void reset_to_bootloader(void) {

}

void reset_cpu(void) {
    NVIC_SystemReset();
}

extern uint32_t _ld_heap_start, _ld_heap_end, _ld_stack_top, _ld_stack_bottom;

uint32_t *port_heap_get_bottom(void) {
    return &_ld_heap_start;
}

uint32_t *port_heap_get_top(void) {
    return &_ld_heap_end;
}

uint32_t *port_stack_get_limit(void) {
    return &_ld_stack_bottom; 
}

uint32_t *port_stack_get_top(void) {
    return &_ld_stack_top;
}

// TODO: what even are these
extern uint32_t _ebss;
// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

__attribute__((used)) void MemManage_Handler(void)
{
    __ASM volatile ("bkpt");
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm("nop;");
    }
}

__attribute__((used)) void BusFault_Handler(void)
{
    __ASM volatile ("bkpt");
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm("nop;");
    }
}

__attribute__((used)) void UsageFault_Handler(void)
{
    __ASM volatile ("bkpt");
    reset_into_safe_mode(MEM_MANAGE);
    while (true) {
        asm("nop;");
    }
}

__attribute__((used)) void HardFault_Handler(void)
{
    __ASM volatile ("bkpt");
    reset_into_safe_mode(HARD_CRASH);
    while (true) {
        asm("nop;");
    }
}
