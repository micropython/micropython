;/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
;* File Name          : startup_stm32f411xe.s
;* Author             : MCD Application Team
;* Description        : STM32F411xExx devices vector table for EWARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR 
;*                        address.
;*                      - Configure the system clock
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M4 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;********************************************************************************
;* 
;* Redistribution and use in source and binary forms, with or without modification,
;* are permitted provided that the following conditions are met:
;*   1. Redistributions of source code must retain the above copyright notice,
;*      this list of conditions and the following disclaimer.
;*   2. Redistributions in binary form must reproduce the above copyright notice,
;*      this list of conditions and the following disclaimer in the documentation
;*      and/or other materials provided with the distribution.
;*   3. Neither the name of STMicroelectronics nor the names of its contributors
;*      may be used to endorse or promote products derived from this software
;*      without specific prior written permission.
;*
;* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*******************************************************************************
;
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler

        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler

         ; External Interrupts
        DCD     WWDG_IRQHandler                   ; Window WatchDog
        DCD     PVD_IRQHandler                    ; PVD through EXTI Line detection
        DCD     TAMP_STAMP_IRQHandler             ; Tamper and TimeStamps through the EXTI line
        DCD     RTC_WKUP_IRQHandler               ; RTC Wakeup through the EXTI line
        DCD     FLASH_IRQHandler                  ; FLASH
        DCD     RCC_IRQHandler                    ; RCC
        DCD     EXTI0_IRQHandler                  ; EXTI Line0
        DCD     EXTI1_IRQHandler                  ; EXTI Line1
        DCD     EXTI2_IRQHandler                  ; EXTI Line2
        DCD     EXTI3_IRQHandler                  ; EXTI Line3
        DCD     EXTI4_IRQHandler                  ; EXTI Line4
        DCD     DMA1_Stream0_IRQHandler           ; DMA1 Stream 0
        DCD     DMA1_Stream1_IRQHandler           ; DMA1 Stream 1
        DCD     DMA1_Stream2_IRQHandler           ; DMA1 Stream 2
        DCD     DMA1_Stream3_IRQHandler           ; DMA1 Stream 3
        DCD     DMA1_Stream4_IRQHandler           ; DMA1 Stream 4
        DCD     DMA1_Stream5_IRQHandler           ; DMA1 Stream 5
        DCD     DMA1_Stream6_IRQHandler           ; DMA1 Stream 6
        DCD     ADC_IRQHandler                    ; ADC1
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     EXTI9_5_IRQHandler                ; External Line[9:5]s
        DCD     TIM1_BRK_TIM9_IRQHandler          ; TIM1 Break and TIM9
        DCD     TIM1_UP_TIM10_IRQHandler          ; TIM1 Update and TIM10
        DCD     TIM1_TRG_COM_TIM11_IRQHandler     ; TIM1 Trigger and Commutation and TIM11
        DCD     TIM1_CC_IRQHandler                ; TIM1 Capture Compare
        DCD     TIM2_IRQHandler                   ; TIM2
        DCD     TIM3_IRQHandler                   ; TIM3
        DCD     TIM4_IRQHandler                   ; TIM4
        DCD     I2C1_EV_IRQHandler                ; I2C1 Event
        DCD     I2C1_ER_IRQHandler                ; I2C1 Error
        DCD     I2C2_EV_IRQHandler                ; I2C2 Event
        DCD     I2C2_ER_IRQHandler                ; I2C2 Error
        DCD     SPI1_IRQHandler                   ; SPI1
        DCD     SPI2_IRQHandler                   ; SPI2
        DCD     USART1_IRQHandler                 ; USART1
        DCD     USART2_IRQHandler                 ; USART2
        DCD     0                                 ; Reserved
        DCD     EXTI15_10_IRQHandler              ; External Line[15:10]s
        DCD     RTC_Alarm_IRQHandler              ; RTC Alarm (A and B) through EXTI Line
        DCD     OTG_FS_WKUP_IRQHandler            ; USB OTG FS Wakeup through EXTI line
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     DMA1_Stream7_IRQHandler           ; DMA1 Stream7
        DCD     0                                 ; Reserved
        DCD     SDIO_IRQHandler                   ; SDIO
        DCD     TIM5_IRQHandler                   ; TIM5
        DCD     SPI3_IRQHandler                   ; SPI3
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     DMA2_Stream0_IRQHandler           ; DMA2 Stream 0
        DCD     DMA2_Stream1_IRQHandler           ; DMA2 Stream 1
        DCD     DMA2_Stream2_IRQHandler           ; DMA2 Stream 2
        DCD     DMA2_Stream3_IRQHandler           ; DMA2 Stream 3
        DCD     DMA2_Stream4_IRQHandler           ; DMA2 Stream 4
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     OTG_FS_IRQHandler                 ; USB OTG FS
        DCD     DMA2_Stream5_IRQHandler           ; DMA2 Stream 5
        DCD     DMA2_Stream6_IRQHandler           ; DMA2 Stream 6
        DCD     DMA2_Stream7_IRQHandler           ; DMA2 Stream 7
        DCD     USART6_IRQHandler                 ; USART6
        DCD     I2C3_EV_IRQHandler                ; I2C3 event
        DCD     I2C3_ER_IRQHandler                ; I2C3 error
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     FPU_IRQHandler                    ; FPU
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     SPI4_IRQHandler                   ; SPI4
        DCD     SPI5_IRQHandler                   ; SPI5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WWDG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WWDG_IRQHandler  
        B WWDG_IRQHandler

        PUBWEAK PVD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PVD_IRQHandler  
        B PVD_IRQHandler

        PUBWEAK TAMP_STAMP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TAMP_STAMP_IRQHandler  
        B TAMP_STAMP_IRQHandler

        PUBWEAK RTC_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_WKUP_IRQHandler  
        B RTC_WKUP_IRQHandler

        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FLASH_IRQHandler  
        B FLASH_IRQHandler

        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RCC_IRQHandler  
        B RCC_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI0_IRQHandler  
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI1_IRQHandler  
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI2_IRQHandler  
        B EXTI2_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI3_IRQHandler
        B EXTI3_IRQHandler

        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI4_IRQHandler  
        B EXTI4_IRQHandler

        PUBWEAK DMA1_Stream0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream0_IRQHandler  
        B DMA1_Stream0_IRQHandler

        PUBWEAK DMA1_Stream1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream1_IRQHandler  
        B DMA1_Stream1_IRQHandler

        PUBWEAK DMA1_Stream2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream2_IRQHandler  
        B DMA1_Stream2_IRQHandler

        PUBWEAK DMA1_Stream3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream3_IRQHandler  
        B DMA1_Stream3_IRQHandler

        PUBWEAK DMA1_Stream4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream4_IRQHandler  
        B DMA1_Stream4_IRQHandler

        PUBWEAK DMA1_Stream5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream5_IRQHandler  
        B DMA1_Stream5_IRQHandler

        PUBWEAK DMA1_Stream6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream6_IRQHandler  
        B DMA1_Stream6_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_IRQHandler  
        B ADC_IRQHandler

        PUBWEAK EXTI9_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI9_5_IRQHandler  
        B EXTI9_5_IRQHandler

        PUBWEAK TIM1_BRK_TIM9_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_BRK_TIM9_IRQHandler  
        B TIM1_BRK_TIM9_IRQHandler

        PUBWEAK TIM1_UP_TIM10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_UP_TIM10_IRQHandler  
        B TIM1_UP_TIM10_IRQHandler

        PUBWEAK TIM1_TRG_COM_TIM11_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_TRG_COM_TIM11_IRQHandler  
        B TIM1_TRG_COM_TIM11_IRQHandler
        
        PUBWEAK TIM1_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM1_CC_IRQHandler  
        B TIM1_CC_IRQHandler

        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM2_IRQHandler  
        B TIM2_IRQHandler

        PUBWEAK TIM3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM3_IRQHandler  
        B TIM3_IRQHandler

        PUBWEAK TIM4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM4_IRQHandler  
        B TIM4_IRQHandler

        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_EV_IRQHandler  
        B I2C1_EV_IRQHandler

        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_ER_IRQHandler  
        B I2C1_ER_IRQHandler

        PUBWEAK I2C2_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_EV_IRQHandler  
        B I2C2_EV_IRQHandler

        PUBWEAK I2C2_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_ER_IRQHandler  
        B I2C2_ER_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI1_IRQHandler  
        B SPI1_IRQHandler

        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI2_IRQHandler  
        B SPI2_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_IRQHandler  
        B USART1_IRQHandler

        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART2_IRQHandler  
        B USART2_IRQHandler

        PUBWEAK EXTI15_10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EXTI15_10_IRQHandler  
        B EXTI15_10_IRQHandler

        PUBWEAK RTC_Alarm_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_Alarm_IRQHandler  
        B RTC_Alarm_IRQHandler

        PUBWEAK OTG_FS_WKUP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_FS_WKUP_IRQHandler  
        B OTG_FS_WKUP_IRQHandler

        PUBWEAK DMA1_Stream7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA1_Stream7_IRQHandler  
        B DMA1_Stream7_IRQHandler

        PUBWEAK SDIO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SDIO_IRQHandler  
        B SDIO_IRQHandler

        PUBWEAK TIM5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIM5_IRQHandler  
        B TIM5_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI3_IRQHandler  
        B SPI3_IRQHandler

        PUBWEAK DMA2_Stream0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream0_IRQHandler  
        B DMA2_Stream0_IRQHandler

        PUBWEAK DMA2_Stream1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream1_IRQHandler  
        B DMA2_Stream1_IRQHandler

        PUBWEAK DMA2_Stream2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream2_IRQHandler  
        B DMA2_Stream2_IRQHandler

        PUBWEAK DMA2_Stream3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream3_IRQHandler  
        B DMA2_Stream3_IRQHandler

        PUBWEAK DMA2_Stream4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream4_IRQHandler  
        B DMA2_Stream4_IRQHandler

        PUBWEAK OTG_FS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OTG_FS_IRQHandler  
        B OTG_FS_IRQHandler

        PUBWEAK DMA2_Stream5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream5_IRQHandler  
        B DMA2_Stream5_IRQHandler

        PUBWEAK DMA2_Stream6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream6_IRQHandler  
        B DMA2_Stream6_IRQHandler

        PUBWEAK DMA2_Stream7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA2_Stream7_IRQHandler  
        B DMA2_Stream7_IRQHandler

        PUBWEAK USART6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART6_IRQHandler  
        B USART6_IRQHandler

        PUBWEAK I2C3_EV_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C3_EV_IRQHandler  
        B I2C3_EV_IRQHandler

        PUBWEAK I2C3_ER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C3_ER_IRQHandler  
        B I2C3_ER_IRQHandler

        PUBWEAK FPU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FPU_IRQHandler  
        B FPU_IRQHandler

        PUBWEAK SPI4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI4_IRQHandler  
        B SPI4_IRQHandler
		
        PUBWEAK SPI5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI5_IRQHandler  
        B SPI5_IRQHandler
        
        END
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
