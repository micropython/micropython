/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Original template from ST Cube library.  See below for header.
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

/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "stm32_it.h"
#include "pendsv.h"
#include "irq.h"
#include "pybthread.h"
#include "gccollect.h"
#include "extint.h"
#include "timer.h"
#include "uart.h"
#include "storage.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "usb.h"

extern void __fatal_error(const char*);
extern PCD_HandleTypeDef pcd_fs_handle;
extern PCD_HandleTypeDef pcd_hs_handle;

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

// Set the following to 1 to get some more information on the Hard Fault
// More information about decoding the fault registers can be found here:
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0646a/Cihdjcfc.html

STATIC char *fmt_hex(uint32_t val, char *buf) {
    const char *hexDig = "0123456789abcdef";

    buf[0] = hexDig[(val >> 28) & 0x0f];
    buf[1] = hexDig[(val >> 24) & 0x0f];
    buf[2] = hexDig[(val >> 20) & 0x0f];
    buf[3] = hexDig[(val >> 16) & 0x0f];
    buf[4] = hexDig[(val >> 12) & 0x0f];
    buf[5] = hexDig[(val >>  8) & 0x0f];
    buf[6] = hexDig[(val >>  4) & 0x0f];
    buf[7] = hexDig[(val >>  0) & 0x0f];
    buf[8] = '\0';

    return buf;
}

STATIC void print_reg(const char *label, uint32_t val) {
    char hexStr[9];

    mp_hal_stdout_tx_str(label);
    mp_hal_stdout_tx_str(fmt_hex(val, hexStr));
    mp_hal_stdout_tx_str("\r\n");
}

STATIC void print_hex_hex(const char *label, uint32_t val1, uint32_t val2) {
    char hex_str[9];
    mp_hal_stdout_tx_str(label);
    mp_hal_stdout_tx_str(fmt_hex(val1, hex_str));
    mp_hal_stdout_tx_str("  ");
    mp_hal_stdout_tx_str(fmt_hex(val2, hex_str));
    mp_hal_stdout_tx_str("\r\n");
}

// The ARMv7M Architecture manual (section B.1.5.6) says that upon entry
// to an exception, that the registers will be in the following order on the
// // stack: R0, R1, R2, R3, R12, LR, PC, XPSR

typedef struct {
    uint32_t    r0, r1, r2, r3, r12, lr, pc, xpsr;
} ExceptionRegisters_t;

int pyb_hard_fault_debug = 0;

void HardFault_C_Handler(ExceptionRegisters_t *regs) {
    if (!pyb_hard_fault_debug) {
        NVIC_SystemReset();
    }

    #if MICROPY_HW_ENABLE_USB
    // We need to disable the USB so it doesn't try to write data out on
    // the VCP and then block indefinitely waiting for the buffer to drain.
    pyb_usb_flags = 0;
    #endif

    mp_hal_stdout_tx_str("HardFault\r\n");

    print_reg("R0    ", regs->r0);
    print_reg("R1    ", regs->r1);
    print_reg("R2    ", regs->r2);
    print_reg("R3    ", regs->r3);
    print_reg("R12   ", regs->r12);
    print_reg("SP    ", (uint32_t)regs);
    print_reg("LR    ", regs->lr);
    print_reg("PC    ", regs->pc);
    print_reg("XPSR  ", regs->xpsr);

    uint32_t cfsr = SCB->CFSR;

    print_reg("HFSR  ", SCB->HFSR);
    print_reg("CFSR  ", cfsr);
    if (cfsr & 0x80) {
        print_reg("MMFAR ", SCB->MMFAR);
    }
    if (cfsr & 0x8000) {
        print_reg("BFAR  ", SCB->BFAR);
    }

    if ((void*)&_ram_start <= (void*)regs && (void*)regs < (void*)&_ram_end) {
        mp_hal_stdout_tx_str("Stack:\r\n");
        uint32_t *stack_top = &_estack;
        if ((void*)regs < (void*)&_heap_end) {
            // stack not in static stack area so limit the amount we print
            stack_top = (uint32_t*)regs + 32;
        }
        for (uint32_t *sp = (uint32_t*)regs; sp < stack_top; ++sp) {
            print_hex_hex("  ", (uint32_t)sp, *sp);
        }
    }

    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
        __fatal_error("HardFault");
    }
}

// Naked functions have no compiler generated gunk, so are the best thing to
// use for asm functions.
__attribute__((naked))
void HardFault_Handler(void) {

    // From the ARMv7M Architecture Reference Manual, section B.1.5.6
    // on entry to the Exception, the LR register contains, amongst other
    // things, the value of CONTROL.SPSEL. This can be found in bit 3.
    //
    // If CONTROL.SPSEL is 0, then the exception was stacked up using the
    // main stack pointer (aka MSP). If CONTROL.SPSEL is 1, then the exception
    // was stacked up using the process stack pointer (aka PSP).

    __asm volatile(
    " tst lr, #4    \n"         // Test Bit 3 to see which stack pointer we should use.
    " ite eq        \n"         // Tell the assembler that the nest 2 instructions are if-then-else
    " mrseq r0, msp \n"         // Make R0 point to main stack pointer
    " mrsne r0, psp \n"         // Make R0 point to process stack pointer
    " b HardFault_C_Handler \n" // Off to C land
    );
}

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void) {
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
        __fatal_error("MemManage");
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
        __fatal_error("BusFault");
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
        __fatal_error("UsageFault");
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void) {
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void) {
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void) {
    pendsv_isr_handler();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
    // Instead of calling HAL_IncTick we do the increment here of the counter.
    // This is purely for efficiency, since SysTick is called 1000 times per
    // second at the highest interrupt priority.
    // Note: we don't need uwTick to be declared volatile here because this is
    // the only place where it can be modified, and the code is more efficient
    // without the volatile specifier.
    extern uint32_t uwTick;
    uwTick += 1;

    // Read the systick control regster. This has the side effect of clearing
    // the COUNTFLAG bit, which makes the logic in mp_hal_ticks_us
    // work properly.
    SysTick->CTRL;

    // Right now we have the storage and DMA controllers to process during
    // this interrupt and we use custom dispatch handlers.  If this needs to
    // be generalised in the future then a dispatch table can be used as
    // follows: ((void(*)(void))(systick_dispatch[uwTick & 0xf]))();

    if (STORAGE_IDLE_TICK(uwTick)) {
        NVIC->STIR = FLASH_IRQn;
    }

    if (DMA_IDLE_ENABLED() && DMA_IDLE_TICK(uwTick)) {
        dma_idle_handler(uwTick);
    }

    #if MICROPY_PY_THREAD
    if (pyb_thread_enabled) {
        if (pyb_thread_cur->timeslice == 0) {
            if (pyb_thread_cur->run_next != pyb_thread_cur) {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
            }
        } else {
            --pyb_thread_cur->timeslice;
        }
    }
    #endif
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
#if MICROPY_HW_USB_FS
void OTG_FS_IRQHandler(void) {
    IRQ_ENTER(OTG_FS_IRQn);
    HAL_PCD_IRQHandler(&pcd_fs_handle);
    IRQ_EXIT(OTG_FS_IRQn);
}
#endif
#if MICROPY_HW_USB_HS
void OTG_HS_IRQHandler(void) {
    IRQ_ENTER(OTG_HS_IRQn);
    HAL_PCD_IRQHandler(&pcd_hs_handle);
    IRQ_EXIT(OTG_HS_IRQn);
}
#endif

#if MICROPY_HW_USB_FS || MICROPY_HW_USB_HS
/**
  * @brief  This function handles USB OTG Common FS/HS Wakeup functions.
  * @param  *pcd_handle for FS or HS
  * @retval None
  */
STATIC void OTG_CMD_WKUP_Handler(PCD_HandleTypeDef *pcd_handle) {

  if (pcd_handle->Init.low_power_enable) {
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select
    PLL as system clock source (HSE and PLL are disabled in STOP mode) */

    __HAL_RCC_HSE_CONFIG(MICROPY_HW_CLK_HSE_STATE);

    /* Wait till HSE is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
    {}

    /* Enable the main PLL. */
    __HAL_RCC_PLL_ENABLE();

    /* Wait till PLL is ready */
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as SYSCLK */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);

    #if defined(STM32H7)
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL1)
    {}
    #else
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL)
    {}
    #endif

    /* ungate PHY clock */
     __HAL_PCD_UNGATE_PHYCLOCK(pcd_handle);
  }

}
#endif

#if MICROPY_HW_USB_FS
/**
  * @brief  This function handles USB OTG FS Wakeup IRQ Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_WKUP_IRQHandler(void) {
    IRQ_ENTER(OTG_FS_WKUP_IRQn);

  OTG_CMD_WKUP_Handler(&pcd_fs_handle);

  /* Clear EXTI pending Bit*/
  __HAL_USB_FS_EXTI_CLEAR_FLAG();

    IRQ_EXIT(OTG_FS_WKUP_IRQn);
}
#endif

#if MICROPY_HW_USB_HS
/**
  * @brief  This function handles USB OTG HS Wakeup IRQ Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_WKUP_IRQHandler(void) {
    IRQ_ENTER(OTG_HS_WKUP_IRQn);

  OTG_CMD_WKUP_Handler(&pcd_hs_handle);

  /* Clear EXTI pending Bit*/
  __HAL_USB_HS_EXTI_CLEAR_FLAG();

    IRQ_EXIT(OTG_HS_WKUP_IRQn);
}
#endif

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

// Handle a flash (erase/program) interrupt.
void FLASH_IRQHandler(void) {
    IRQ_ENTER(FLASH_IRQn);
    // This calls the real flash IRQ handler, if needed
    /*
    uint32_t flash_cr = FLASH->CR;
    if ((flash_cr & FLASH_IT_EOP) || (flash_cr & FLASH_IT_ERR)) {
        HAL_FLASH_IRQHandler();
    }
    */
    // This call the storage IRQ handler, to check if the flash cache needs flushing
    storage_irq_handler();
    IRQ_EXIT(FLASH_IRQn);
}

/**
  * @brief  These functions handle the EXTI interrupt requests.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void) {
    IRQ_ENTER(EXTI0_IRQn);
    Handle_EXTI_Irq(0);
    IRQ_EXIT(EXTI0_IRQn);
}

void EXTI1_IRQHandler(void) {
    IRQ_ENTER(EXTI1_IRQn);
    Handle_EXTI_Irq(1);
    IRQ_EXIT(EXTI1_IRQn);
}

void EXTI2_IRQHandler(void) {
    IRQ_ENTER(EXTI2_IRQn);
    Handle_EXTI_Irq(2);
    IRQ_EXIT(EXTI2_IRQn);
}

void EXTI3_IRQHandler(void) {
    IRQ_ENTER(EXTI3_IRQn);
    Handle_EXTI_Irq(3);
    IRQ_EXIT(EXTI3_IRQn);
}

void EXTI4_IRQHandler(void) {
    IRQ_ENTER(EXTI4_IRQn);
    Handle_EXTI_Irq(4);
    IRQ_EXIT(EXTI4_IRQn);
}

void EXTI9_5_IRQHandler(void) {
    IRQ_ENTER(EXTI9_5_IRQn);
    Handle_EXTI_Irq(5);
    Handle_EXTI_Irq(6);
    Handle_EXTI_Irq(7);
    Handle_EXTI_Irq(8);
    Handle_EXTI_Irq(9);
    IRQ_EXIT(EXTI9_5_IRQn);
}

void EXTI15_10_IRQHandler(void) {
    IRQ_ENTER(EXTI15_10_IRQn);
    Handle_EXTI_Irq(10);
    Handle_EXTI_Irq(11);
    Handle_EXTI_Irq(12);
    Handle_EXTI_Irq(13);
    Handle_EXTI_Irq(14);
    Handle_EXTI_Irq(15);
    IRQ_EXIT(EXTI15_10_IRQn);
}

void PVD_IRQHandler(void) {
    IRQ_ENTER(PVD_IRQn);
    Handle_EXTI_Irq(EXTI_PVD_OUTPUT);
    IRQ_EXIT(PVD_IRQn);
}

#if defined(STM32L4)
void PVD_PVM_IRQHandler(void) {
    IRQ_ENTER(PVD_PVM_IRQn);
    Handle_EXTI_Irq(EXTI_PVD_OUTPUT);
    IRQ_EXIT(PVD_PVM_IRQn);
}
#endif

void RTC_Alarm_IRQHandler(void) {
    IRQ_ENTER(RTC_Alarm_IRQn);
    Handle_EXTI_Irq(EXTI_RTC_ALARM);
    IRQ_EXIT(RTC_Alarm_IRQn);
}

#if defined(ETH)    // The 407 has ETH, the 405 doesn't
void ETH_WKUP_IRQHandler(void)  {
    IRQ_ENTER(ETH_WKUP_IRQn);
    Handle_EXTI_Irq(EXTI_ETH_WAKEUP);
    IRQ_EXIT(ETH_WKUP_IRQn);
}
#endif

void TAMP_STAMP_IRQHandler(void) {
    IRQ_ENTER(TAMP_STAMP_IRQn);
    Handle_EXTI_Irq(EXTI_RTC_TIMESTAMP);
    IRQ_EXIT(TAMP_STAMP_IRQn);
}

void RTC_WKUP_IRQHandler(void) {
    IRQ_ENTER(RTC_WKUP_IRQn);
    RTC->ISR &= ~(1 << 10); // clear wakeup interrupt flag
    Handle_EXTI_Irq(EXTI_RTC_WAKEUP); // clear EXTI flag and execute optional callback
    IRQ_EXIT(RTC_WKUP_IRQn);
}

void TIM1_BRK_TIM9_IRQHandler(void) {
    IRQ_ENTER(TIM1_BRK_TIM9_IRQn);
    timer_irq_handler(9);
    IRQ_EXIT(TIM1_BRK_TIM9_IRQn);
}

#if defined(STM32L4)
void TIM1_BRK_TIM15_IRQHandler(void) {
    IRQ_ENTER(TIM1_BRK_TIM15_IRQn);
    timer_irq_handler(15);
    IRQ_EXIT(TIM1_BRK_TIM15_IRQn);
}
#endif

void TIM1_UP_TIM10_IRQHandler(void) {
    IRQ_ENTER(TIM1_UP_TIM10_IRQn);
    timer_irq_handler(1);
    timer_irq_handler(10);
    IRQ_EXIT(TIM1_UP_TIM10_IRQn);
}

#if defined(STM32L4)
void TIM1_UP_TIM16_IRQHandler(void) {
    IRQ_ENTER(TIM1_UP_TIM16_IRQn);
    timer_irq_handler(1);
    timer_irq_handler(16);
    IRQ_EXIT(TIM1_UP_TIM16_IRQn);
}
#endif

void TIM1_TRG_COM_TIM11_IRQHandler(void) {
    IRQ_ENTER(TIM1_TRG_COM_TIM11_IRQn);
    timer_irq_handler(11);
    IRQ_EXIT(TIM1_TRG_COM_TIM11_IRQn);
}

#if defined(STM32L4)
void TIM1_TRG_COM_TIM17_IRQHandler(void) {
    IRQ_ENTER(TIM1_TRG_COM_TIM17_IRQn);
    timer_irq_handler(17);
    IRQ_EXIT(TIM1_TRG_COM_TIM17_IRQn);
}
#endif

void TIM1_CC_IRQHandler(void) {
    IRQ_ENTER(TIM1_CC_IRQn);
    timer_irq_handler(1);
    IRQ_EXIT(TIM1_CC_IRQn);
}

void TIM2_IRQHandler(void) {
    IRQ_ENTER(TIM2_IRQn);
    timer_irq_handler(2);
    IRQ_EXIT(TIM2_IRQn);
}

void TIM3_IRQHandler(void) {
    IRQ_ENTER(TIM3_IRQn);
    timer_irq_handler(3);
    IRQ_EXIT(TIM3_IRQn);
}

void TIM4_IRQHandler(void) {
    IRQ_ENTER(TIM4_IRQn);
    timer_irq_handler(4);
    IRQ_EXIT(TIM4_IRQn);
}

void TIM5_IRQHandler(void) {
    IRQ_ENTER(TIM5_IRQn);
    timer_irq_handler(5);
    HAL_TIM_IRQHandler(&TIM5_Handle);
    IRQ_EXIT(TIM5_IRQn);
}

#if defined(TIM6) // STM32F401 doesn't have TIM6
void TIM6_DAC_IRQHandler(void) {
    IRQ_ENTER(TIM6_DAC_IRQn);
    timer_irq_handler(6);
    IRQ_EXIT(TIM6_DAC_IRQn);
}
#endif

#if defined(TIM7) // STM32F401 doesn't have TIM7
void TIM7_IRQHandler(void) {
    IRQ_ENTER(TIM7_IRQn);
    timer_irq_handler(7);
    IRQ_EXIT(TIM7_IRQn);
}
#endif

#if defined(TIM8) // STM32F401 doesn't have TIM8
void TIM8_BRK_TIM12_IRQHandler(void) {
    IRQ_ENTER(TIM8_BRK_TIM12_IRQn);
    timer_irq_handler(12);
    IRQ_EXIT(TIM8_BRK_TIM12_IRQn);
}

void TIM8_UP_TIM13_IRQHandler(void) {
    IRQ_ENTER(TIM8_UP_TIM13_IRQn);
    timer_irq_handler(8);
    timer_irq_handler(13);
    IRQ_EXIT(TIM8_UP_TIM13_IRQn);
}

#if defined(STM32L4)
void TIM8_UP_IRQHandler(void) {
    IRQ_ENTER(TIM8_UP_IRQn);
    timer_irq_handler(8);
    IRQ_EXIT(TIM8_UP_IRQn);
}
#endif

void TIM8_CC_IRQHandler(void) {
    IRQ_ENTER(TIM8_CC_IRQn);
    timer_irq_handler(8);
    IRQ_EXIT(TIM8_CC_IRQn);
}

void TIM8_TRG_COM_TIM14_IRQHandler(void) {
    IRQ_ENTER(TIM8_TRG_COM_TIM14_IRQn);
    timer_irq_handler(14);
    IRQ_EXIT(TIM8_TRG_COM_TIM14_IRQn);
}
#endif

// UART/USART IRQ handlers
void USART1_IRQHandler(void) {
    IRQ_ENTER(USART1_IRQn);
    uart_irq_handler(1);
    IRQ_EXIT(USART1_IRQn);
}

void USART2_IRQHandler(void) {
    IRQ_ENTER(USART2_IRQn);
    uart_irq_handler(2);
    IRQ_EXIT(USART2_IRQn);
}

void USART3_IRQHandler(void) {
    IRQ_ENTER(USART3_IRQn);
    uart_irq_handler(3);
    IRQ_EXIT(USART3_IRQn);
}

void UART4_IRQHandler(void) {
    IRQ_ENTER(UART4_IRQn);
    uart_irq_handler(4);
    IRQ_EXIT(UART4_IRQn);
}

void UART5_IRQHandler(void) {
    IRQ_ENTER(UART5_IRQn);
    uart_irq_handler(5);
    IRQ_EXIT(UART5_IRQn);
}

void USART6_IRQHandler(void) {
    IRQ_ENTER(USART6_IRQn);
    uart_irq_handler(6);
    IRQ_EXIT(USART6_IRQn);
}

#if defined(MICROPY_HW_UART7_TX)
void UART7_IRQHandler(void) {
    IRQ_ENTER(UART7_IRQn);
    uart_irq_handler(7);
    IRQ_EXIT(UART7_IRQn);
}
#endif

#if defined(MICROPY_HW_UART8_TX)
void UART8_IRQHandler(void) {
    IRQ_ENTER(UART8_IRQn);
    uart_irq_handler(8);
    IRQ_EXIT(UART8_IRQn);
}
#endif

#if defined(MICROPY_HW_CAN1_TX)
void CAN1_RX0_IRQHandler(void) {
    IRQ_ENTER(CAN1_RX0_IRQn);
    can_rx_irq_handler(PYB_CAN_1, CAN_FIFO0);
    IRQ_EXIT(CAN1_RX0_IRQn);
}

void CAN1_RX1_IRQHandler(void) {
    IRQ_ENTER(CAN1_RX1_IRQn);
    can_rx_irq_handler(PYB_CAN_1, CAN_FIFO1);
    IRQ_EXIT(CAN1_RX1_IRQn);
}

void CAN1_SCE_IRQHandler(void) {
    IRQ_ENTER(CAN1_SCE_IRQn);
    can_sce_irq_handler(PYB_CAN_1);
    IRQ_EXIT(CAN1_SCE_IRQn);
}
#endif

#if defined(MICROPY_HW_CAN2_TX)
void CAN2_RX0_IRQHandler(void) {
    IRQ_ENTER(CAN2_RX0_IRQn);
    can_rx_irq_handler(PYB_CAN_2, CAN_FIFO0);
    IRQ_EXIT(CAN2_RX0_IRQn);
}

void CAN2_RX1_IRQHandler(void) {
    IRQ_ENTER(CAN2_RX1_IRQn);
    can_rx_irq_handler(PYB_CAN_2, CAN_FIFO1);
    IRQ_EXIT(CAN2_RX1_IRQn);
}

void CAN2_SCE_IRQHandler(void) {
    IRQ_ENTER(CAN2_SCE_IRQn);
    can_sce_irq_handler(PYB_CAN_2);
    IRQ_EXIT(CAN2_SCE_IRQn);
}
#endif

#if defined(MICROPY_HW_I2C1_SCL)
void I2C1_EV_IRQHandler(void) {
    IRQ_ENTER(I2C1_EV_IRQn);
    i2c_ev_irq_handler(1);
    IRQ_EXIT(I2C1_EV_IRQn);
}

void I2C1_ER_IRQHandler(void) {
    IRQ_ENTER(I2C1_ER_IRQn);
    i2c_er_irq_handler(1);
    IRQ_EXIT(I2C1_ER_IRQn);
}
#endif // defined(MICROPY_HW_I2C1_SCL)

#if defined(MICROPY_HW_I2C2_SCL)
void I2C2_EV_IRQHandler(void) {
    IRQ_ENTER(I2C2_EV_IRQn);
    i2c_ev_irq_handler(2);
    IRQ_EXIT(I2C2_EV_IRQn);
}

void I2C2_ER_IRQHandler(void) {
    IRQ_ENTER(I2C2_ER_IRQn);
    i2c_er_irq_handler(2);
    IRQ_EXIT(I2C2_ER_IRQn);
}
#endif // defined(MICROPY_HW_I2C2_SCL)

#if defined(MICROPY_HW_I2C3_SCL)
void I2C3_EV_IRQHandler(void) {
    IRQ_ENTER(I2C3_EV_IRQn);
    i2c_ev_irq_handler(3);
    IRQ_EXIT(I2C3_EV_IRQn);
}

void I2C3_ER_IRQHandler(void) {
    IRQ_ENTER(I2C3_ER_IRQn);
    i2c_er_irq_handler(3);
    IRQ_EXIT(I2C3_ER_IRQn);
}
#endif // defined(MICROPY_HW_I2C3_SCL)

#if defined(MICROPY_HW_I2C4_SCL)
void I2C4_EV_IRQHandler(void) {
    IRQ_ENTER(I2C4_EV_IRQn);
    i2c_ev_irq_handler(4);
    IRQ_EXIT(I2C4_EV_IRQn);
}

void I2C4_ER_IRQHandler(void) {
    IRQ_ENTER(I2C4_ER_IRQn);
    i2c_er_irq_handler(4);
    IRQ_EXIT(I2C4_ER_IRQn);
}
#endif // defined(MICROPY_HW_I2C4_SCL)
