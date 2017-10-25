//*****************************************************************************
// startup_gcc.c
//
// Startup code for use with GCC.
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include <stdint.h>
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "fault_registers.h"

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t _estack;

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
#ifndef BOOTLOADER
__attribute__ ((section (".boot")))
#endif
void ResetISR(void);
#ifdef DEBUG
static void NmiSR(void) __attribute__( ( naked ) );
static void FaultISR( void ) __attribute__( ( naked ) );
void HardFault_HandlerC(uint32_t *pulFaultStackAddress);
static void BusFaultHandler(void) __attribute__( ( naked ) );
#endif
static void IntDefaultHandler(void) __attribute__( ( naked ) );

//*****************************************************************************
//
// External declaration for the freeRTOS handlers
//
//*****************************************************************************
#ifdef USE_FREERTOS
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
#endif

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".intvecs")))
void (* const g_pfnVectors[256])(void) =
{
    (void (*)(void))((uint32_t)&_estack),   // The initial stack pointer
    ResetISR,                               // The reset handler
#ifdef DEBUG
    NmiSR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
#else
    IntDefaultHandler,                      // The NMI handler
    IntDefaultHandler,                      // The hard fault handler
#endif
    IntDefaultHandler,                      // The MPU fault handler
#ifdef DEBUG
    BusFaultHandler,                        // The bus fault handler
#else
    IntDefaultHandler,                      // The bus fault handler
#endif
    IntDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
#ifdef USE_FREERTOS
    vPortSVCHandler,                        // SVCall handler
#else
    IntDefaultHandler,                      // SVCall handler
#endif
    IntDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
#ifdef USE_FREERTOS
    xPortPendSVHandler,                     // The PendSV handler
    xPortSysTickHandler,                    // The SysTick handler
#else
    IntDefaultHandler,                      // The PendSV handler
    IntDefaultHandler,                      // The SysTick handler
#endif
    IntDefaultHandler,                      // GPIO Port A
    IntDefaultHandler,                      // GPIO Port B
    IntDefaultHandler,                      // GPIO Port C
    IntDefaultHandler,                      // GPIO Port D
    0,                                      // Reserved
    IntDefaultHandler,                      // UART0 Rx and Tx
    IntDefaultHandler,                      // UART1 Rx and Tx
    0,                                      // Reserved
    IntDefaultHandler,                      // I2C0 Master and Slave
    0,0,0,0,0,                              // Reserved
    IntDefaultHandler,                      // ADC Channel 0
    IntDefaultHandler,                      // ADC Channel 1
    IntDefaultHandler,                      // ADC Channel 2
    IntDefaultHandler,                      // ADC Channel 3
    IntDefaultHandler,                      // Watchdog Timer
    IntDefaultHandler,                      // Timer 0 subtimer A
    IntDefaultHandler,                      // Timer 0 subtimer B
    IntDefaultHandler,                      // Timer 1 subtimer A
    IntDefaultHandler,                      // Timer 1 subtimer B
    IntDefaultHandler,                      // Timer 2 subtimer A
    IntDefaultHandler,                      // Timer 2 subtimer B
    0,0,0,0,                                // Reserved
    IntDefaultHandler,                      // Flash
    0,0,0,0,0,                              // Reserved
    IntDefaultHandler,                      // Timer 3 subtimer A
    IntDefaultHandler,                      // Timer 3 subtimer B
    0,0,0,0,0,0,0,0,0,                      // Reserved
    IntDefaultHandler,                      // uDMA Software Transfer
    IntDefaultHandler,                      // uDMA Error
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    IntDefaultHandler,                      // SHA
    0,0,                                    // Reserved
    IntDefaultHandler,                      // AES
    0,                                      // Reserved
    IntDefaultHandler,                      // DES
    0,0,0,0,0,                              // Reserved
    IntDefaultHandler,                      // SDHost
    0,                                      // Reserved
    IntDefaultHandler,                      // I2S
    0,                                      // Reserved
    IntDefaultHandler,                      // Camera
    0,0,0,0,0,0,0,                          // Reserved
    IntDefaultHandler,                      // NWP to APPS Interrupt
    IntDefaultHandler,                      // Power, Reset and Clock module
    0,0,                                    // Reserved
    IntDefaultHandler,                      // Shared SPI
    IntDefaultHandler,                      // Generic SPI
    IntDefaultHandler,                      // Link SPI
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0,0,0,0,0,0,0,0,0,                    // Reserved
    0,0                                     // Reserved
};



//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************

void ResetISR(void)
{
#if defined(DEBUG) && !defined(BOOTLOADER)
    {
        //
        // Fill the main stack with a known value so that
        // we can measure the main stack high water mark
        //
        __asm volatile
        (
                "ldr     r0, =_stack        \n"
                "ldr     r1, =_estack       \n"
                "mov     r2, #0x55555555    \n"
        ".thumb_func                        \n"
        "fill_loop:                         \n"
                "cmp     r0, r1             \n"
                "it      lt                 \n"
                "strlt   r2, [r0], #4       \n"
                "blt     fill_loop          \n"
        );
    }
#endif

    {
        // Get the initial stack pointer location from the vector table
        // and write this value to the msp register
        __asm volatile
        (
                "ldr r0, =_text             \n"
                "ldr r0, [r0]               \n"
                "msr msp, r0                \n"
        );
    }

    {
        //
        // Zero fill the bss segment.
        //
        __asm volatile
        (
                "ldr     r0, =_bss      \n"
                "ldr     r1, =_ebss     \n"
                "mov     r2, #0         \n"
        ".thumb_func                    \n"
        "zero_loop:                     \n"
                "cmp     r0, r1         \n"
                "it      lt             \n"
                "strlt   r2, [r0], #4   \n"
                "blt     zero_loop      \n"
        );
    }

    {
        //
        // Call the application's entry point.
        //
        main();
    }
}

#ifdef DEBUG
//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************

static void NmiSR(void)
{
    // Break into the debugger
    __asm volatile ("bkpt #0  \n");

    //
    // Enter an infinite loop.
    //
    for ( ; ; )
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a hard fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************

static void FaultISR(void)
{
        /*
         * Get the appropriate stack pointer, depending on our mode,
         * and use it as the parameter to the C handler. This function
         * will never return
         */

        __asm volatile
        (
                "movs   r0, #4  \n"
                "mov    r1, lr  \n"
                "tst    r0, r1  \n"
                "beq    _msp    \n"
                "mrs    r0, psp \n"
                "b      HardFault_HandlerC      \n"
        "_msp:  \n"
                "mrs    r0, msp \n"
                "b      HardFault_HandlerC      \n"
        ) ;
}

//***********************************************************************************
// HardFaultHandler_C:
// This is called from the FaultISR with a pointer the Fault stack
// as the parameter. We can then read the values from the stack and place them
// into local variables for ease of reading.
// We then read the various Fault Status and Address Registers to help decode
// cause of the fault.
// The function ends with a BKPT instruction to force control back into the debugger
//***********************************************************************************
void HardFault_HandlerC(uint32_t *pulFaultStackAddress)
{
    volatile uint32_t r0 ;
    volatile uint32_t r1 ;
    volatile uint32_t r2 ;
    volatile uint32_t r3 ;
    volatile uint32_t r12 ;
    volatile uint32_t lr ;
    volatile uint32_t pc ;
    volatile uint32_t psr ;
    volatile _CFSR_t _CFSR ;
    volatile _HFSR_t _HFSR ;
    volatile uint32_t _BFAR ;


    r0  = pulFaultStackAddress[0];
    r1  = pulFaultStackAddress[1];
    r2  = pulFaultStackAddress[2];
    r3  = pulFaultStackAddress[3];
    r12 = pulFaultStackAddress[4];
    lr  = pulFaultStackAddress[5];
    pc  = pulFaultStackAddress[6];
    psr = pulFaultStackAddress[7];

    // Configurable Fault Status Register
    // Consists of MMSR, BFSR and UFSR
    _CFSR = (*((volatile _CFSR_t *)(0xE000ED28)));
    // Hard Fault Status Register
    _HFSR = (*((volatile _HFSR_t *)(0xE000ED2C)));
    // Bus Fault Address Register
    _BFAR = (*((volatile uint32_t *)(0xE000ED38)));

    // Break into the debugger
    __asm volatile ("bkpt #0  \n");

    for ( ; ; )
    {
        // Keep the compiler happy
        (void)r0, (void)r1, (void)r2, (void)r3, (void)r12, (void)lr, (void)pc, (void)psr;
        (void)_CFSR, (void)_HFSR, (void)_BFAR;
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************

static void BusFaultHandler(void)
{
    // Break into the debugger
    __asm volatile ("bkpt #0  \n");

    //
    // Enter an infinite loop.
    //
    for ( ; ; )
    {
    }
}
#endif

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void IntDefaultHandler(void)
{
#ifdef DEBUG
    // Break into the debugger
    __asm volatile ("bkpt #0  \n");
#endif

    //
    // Enter an infinite loop.
    //
    for ( ; ; )
    {
    }
}

