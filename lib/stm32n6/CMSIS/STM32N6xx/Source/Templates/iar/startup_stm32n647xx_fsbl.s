;********************************************************************************
;* File Name          : startup_stm32n647xx_fsbl.s
;* Author             : MCD Application Team
;* Description        : STM32N647xx Devices vector after boot ROM execution
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR
;*                        address.
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the Cortex-M55 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;********************************************************************************
;*
;* Copyright (c) 2023 STMicroelectronics.
;* All rights reserved.
;*
;* This software is licensed under terms that can be found in the LICENSE file
;* in the root directory of this software component.
;* If no LICENSE file comes with this software, it is provided AS-IS.
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
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA
__vector_table

        DCD     sfe(CSTACK)                ; Initial Stack pointer
        DCD     Reset_Handler              ; Reset Handler

        DCD     NMI_Handler                ; NMI Handler
        DCD     HardFault_Handler          ; Hard Fault Handler
        DCD     MemManage_Handler          ; MPU Fault Handler
        DCD     BusFault_Handler           ; Bus Fault Handler
        DCD     UsageFault_Handler         ; Usage Fault Handler
        DCD     SecureFault_Handler        ; Secure Fault Handler
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     SVC_Handler                ; SVCall Handler
        DCD     DebugMon_Handler           ; Debug Monitor Handler
        DCD     0                          ; Reserved
        DCD     PendSV_Handler             ; PendSV Handler
        DCD     SysTick_Handler            ; SysTick Handler

        ; External Interrupts
        DCD     PVD_PVM_IRQHandler                ; PVD/PVM through EXTI Line detection
        DCD     0                                 ; Reserved
        DCD     DTS_IRQHandler                    ; Digital Temperature Sensor
        DCD     RCC_IRQHandler                    ; RCC global interrupts through EXTI Line detection
        DCD     LOCKUP_IRQHandler                 ; Processor_SAFETY_interrupt due lost LOCKUP
        DCD     CACHE_ECC_IRQHandler              ; Error ECC cache
        DCD     TCM_ECC_IRQHandler                ; Error ECC TCM
        DCD     BKP_ECC_IRQHandler                ; Error ECC BKP
        DCD     FPU_IRQHandler                    ; FPU
        DCD     0                                 ; Reserved
        DCD     RTC_S_IRQHandler                  ; RTC secure interrupts through the EXTI line
        DCD     TAMP_IRQHandler                   ; TAMP secure and non-secure interrupts through the EXTI line
        DCD     RIFSC_TAMPER_IRQHandler           ; RIF Tamper interrupts
        DCD     IAC_IRQHandler                    ; IAC interrupt
        DCD     RCC_S_IRQHandler                  ; RCC secure global interrupts through EXTI Line detection
        DCD     0                                 ; Reserved
        DCD     RTC_IRQHandler                    ; RTC non-secure interrupts through the EXTI line
        DCD     0                                 ; Reserved
        DCD     IWDG_IRQHandler                   ; Internal Watchdog
        DCD     WWDG_IRQHandler                   ; Window Watchdog
        DCD     EXTI0_IRQHandler                  ; EXTI Line0
        DCD     EXTI1_IRQHandler                  ; EXTI Line1
        DCD     EXTI2_IRQHandler                  ; EXTI Line2
        DCD     EXTI3_IRQHandler                  ; EXTI Line3
        DCD     EXTI4_IRQHandler                  ; EXTI Line4
        DCD     EXTI5_IRQHandler                  ; EXTI Line5
        DCD     EXTI6_IRQHandler                  ; EXTI Line6
        DCD     EXTI7_IRQHandler                  ; EXTI Line7
        DCD     EXTI8_IRQHandler                  ; EXTI Line8
        DCD     EXTI9_IRQHandler                  ; EXTI Line9
        DCD     EXTI10_IRQHandler                 ; EXTI Line10
        DCD     EXTI11_IRQHandler                 ; EXTI Line11
        DCD     EXTI12_IRQHandler                 ; EXTI Line12
        DCD     EXTI13_IRQHandler                 ; EXTI Line13
        DCD     EXTI14_IRQHandler                 ; EXTI Line14
        DCD     EXTI15_IRQHandler                 ; EXTI Line15
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     PKA_IRQHandler                    ; PKA
        DCD     HASH_IRQHandler                   ; HASH
        DCD     RNG_IRQHandler                    ; RNG
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     ADC1_2_IRQHandler                 ; ADC1 & ADC2
        DCD     CSI_IRQHandler                    ; CSI
        DCD     DCMIPP_IRQHandler                 ; DCMIPP
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     0                                 ; Reserved
        DCD     PAHB_ERR_IRQHandler               ; PAHB error interrupt
        DCD     NPU0_IRQHandler                   ; NPU mst_ints[1] line interrupt
        DCD     NPU1_IRQHandler                   ; NPU mst_ints[1] line interrupt
        DCD     NPU2_IRQHandler                   ; NPU mst_ints[2] line interrupt
        DCD     NPU3_IRQHandler                   ; NPU mst_ints[3] line interrupt
        DCD     CACHEAXI_IRQHandler               ; NPU CACHEAXI interrupt
        DCD     LTDC_LO_IRQHandler                ; LTDC low-layer interrupt
        DCD     LTDC_LO_ERR_IRQHandler            ; LTDC low-layer error interrupt
        DCD     DMA2D_IRQHandler                  ; DMA2D
        DCD     JPEG_IRQHandler                   ; JPEG
        DCD     VENC_IRQHandler                   ; VENC
        DCD     GFXMMU_IRQHandler                 ; GFXMMU
        DCD     GFXTIM_IRQHandler                 ; GFXTIM
        DCD     GPU2D_IRQHandler                  ; GPU2D
        DCD     GPU2D_ER_IRQHandler               ; GPU2D error
        DCD     ICACHE_IRQHandler                 ; GPU2D cache
        DCD     HPDMA1_Channel0_IRQHandler        ; HPDMA1 Channel 0
        DCD     HPDMA1_Channel1_IRQHandler        ; HPDMA1 Channel 1
        DCD     HPDMA1_Channel2_IRQHandler        ; HPDMA1 Channel 2
        DCD     HPDMA1_Channel3_IRQHandler        ; HPDMA1 Channel 3
        DCD     HPDMA1_Channel4_IRQHandler        ; HPDMA1 Channel 4
        DCD     HPDMA1_Channel5_IRQHandler        ; HPDMA1 Channel 5
        DCD     HPDMA1_Channel6_IRQHandler        ; HPDMA1 Channel 6
        DCD     HPDMA1_Channel7_IRQHandler        ; HPDMA1 Channel 7
        DCD     HPDMA1_Channel8_IRQHandler        ; HPDMA1 Channel 8
        DCD     HPDMA1_Channel9_IRQHandler        ; HPDMA1 Channel 9
        DCD     HPDMA1_Channel10_IRQHandler       ; HPDMA1 Channel 10
        DCD     HPDMA1_Channel11_IRQHandler       ; HPDMA1 Channel 11
        DCD     HPDMA1_Channel12_IRQHandler       ; HPDMA1 Channel 12
        DCD     HPDMA1_Channel13_IRQHandler       ; HPDMA1 Channel 13
        DCD     HPDMA1_Channel14_IRQHandler       ; HPDMA1 Channel 14
        DCD     HPDMA1_Channel15_IRQHandler       ; HPDMA1 Channel 15
        DCD     GPDMA1_Channel0_IRQHandler        ; GPDMA1 Channel 0
        DCD     GPDMA1_Channel1_IRQHandler        ; GPDMA1 Channel 1
        DCD     GPDMA1_Channel2_IRQHandler        ; GPDMA1 Channel 2
        DCD     GPDMA1_Channel3_IRQHandler        ; GPDMA1 Channel 3
        DCD     GPDMA1_Channel4_IRQHandler        ; GPDMA1 Channel 4
        DCD     GPDMA1_Channel5_IRQHandler        ; GPDMA1 Channel 5
        DCD     GPDMA1_Channel6_IRQHandler        ; GPDMA1 Channel 6
        DCD     GPDMA1_Channel7_IRQHandler        ; GPDMA1 Channel 7
        DCD     GPDMA1_Channel8_IRQHandler        ; GPDMA1 Channel 8
        DCD     GPDMA1_Channel9_IRQHandler        ; GPDMA1 Channel 9
        DCD     GPDMA1_Channel10_IRQHandler       ; GPDMA1 Channel 10
        DCD     GPDMA1_Channel11_IRQHandler       ; GPDMA1 Channel 11
        DCD     GPDMA1_Channel12_IRQHandler       ; GPDMA1 Channel 12
        DCD     GPDMA1_Channel13_IRQHandler       ; GPDMA1 Channel 13
        DCD     GPDMA1_Channel14_IRQHandler       ; GPDMA1 Channel 14
        DCD     GPDMA1_Channel15_IRQHandler       ; GPDMA1 Channel 15
        DCD     I2C1_EV_IRQHandler                ; I2C1 Event
        DCD     I2C1_ER_IRQHandler                ; I2C1 Error
        DCD     I2C2_EV_IRQHandler                ; I2C2 Event
        DCD     I2C2_ER_IRQHandler                ; I2C2 Error
        DCD     I2C3_EV_IRQHandler                ; I2C3 Event
        DCD     I2C3_ER_IRQHandler                ; I2C3 Error
        DCD     I2C4_EV_IRQHandler                ; I2C4 Event
        DCD     I2C4_ER_IRQHandler                ; I2C4 Error
        DCD     I3C1_EV_IRQHandler                ; I3C1 Event
        DCD     I3C1_ER_IRQHandler                ; I3C1 Error
        DCD     I3C2_EV_IRQHandler                ; I3C2 Event
        DCD     I3C2_ER_IRQHandler                ; I3C2 Error
        DCD     TIM1_BRK_IRQHandler               ; TIM1 Break
        DCD     TIM1_UP_IRQHandler                ; TIM1 Update
        DCD     TIM1_TRG_COM_IRQHandler           ; TIM1 Trigger and Commutation
        DCD     TIM1_CC_IRQHandler                ; TIM1 Capture Compare
        DCD     TIM2_IRQHandler                   ; TIM2
        DCD     TIM3_IRQHandler                   ; TIM3
        DCD     TIM4_IRQHandler                   ; TIM4
        DCD     TIM5_IRQHandler                   ; TIM5
        DCD     TIM6_IRQHandler                   ; TIM6
        DCD     TIM7_IRQHandler                   ; TIM7
        DCD     TIM8_BRK_IRQHandler               ; TIM8 Break
        DCD     TIM8_UP_IRQHandler                ; TIM8 Update
        DCD     TIM8_TRG_COM_IRQHandler           ; TIM8 Trigger and Commutation
        DCD     TIM8_CC_IRQHandler                ; TIM8 Capture Compare
        DCD     TIM9_IRQHandler                   ; TIM9
        DCD     TIM10_IRQHandler                  ; TIM10
        DCD     TIM11_IRQHandler                  ; TIM11
        DCD     TIM12_IRQHandler                  ; TIM12
        DCD     TIM13_IRQHandler                  ; TIM13
        DCD     TIM14_IRQHandler                  ; TIM14
        DCD     TIM15_IRQHandler                  ; TIM15
        DCD     TIM16_IRQHandler                  ; TIM16
        DCD     TIM17_IRQHandler                  ; TIM17
        DCD     TIM18_IRQHandler                  ; TIM18
        DCD     LPTIM1_IRQHandler                 ; LP TIM1
        DCD     LPTIM2_IRQHandler                 ; LP TIM2
        DCD     LPTIM3_IRQHandler                 ; LP TIM3
        DCD     LPTIM4_IRQHandler                 ; LP TIM4
        DCD     LPTIM5_IRQHandler                 ; LP TIM5
        DCD     ADF1_FLT0_IRQHandler              ; ADF1 Filter 0
        DCD     MDF1_FLT0_IRQHandler              ; MDF1 Filter 0
        DCD     MDF1_FLT1_IRQHandler              ; MDF1 Filter 1
        DCD     MDF1_FLT2_IRQHandler              ; MDF1 Filter 2
        DCD     MDF1_FLT3_IRQHandler              ; MDF1 Filter 3
        DCD     MDF1_FLT4_IRQHandler              ; MDF1 Filter 4
        DCD     MDF1_FLT5_IRQHandler              ; MDF1 Filter 5
        DCD     SAI1_A_IRQHandler                 ; Serial Audio Interface 1 block A interrupt
        DCD     SAI1_B_IRQHandler                 ; Serial Audio Interface 1 block B interrupt
        DCD     SAI2_A_IRQHandler                 ; Serial Audio Interface 2 block A interrupt
        DCD     SAI2_B_IRQHandler                 ; Serial Audio Interface 2 block B interrupt
        DCD     SPDIFRX1_IRQHandler               ; SPDIFRX1
        DCD     SPI1_IRQHandler                   ; SPI1
        DCD     SPI2_IRQHandler                   ; SPI2
        DCD     SPI3_IRQHandler                   ; SPI3
        DCD     SPI4_IRQHandler                   ; SPI4
        DCD     SPI5_IRQHandler                   ; SPI5
        DCD     SPI6_IRQHandler                   ; SPI6
        DCD     USART1_IRQHandler                 ; USART1
        DCD     USART2_IRQHandler                 ; USART2
        DCD     USART3_IRQHandler                 ; USART3
        DCD     UART4_IRQHandler                  ; UART4
        DCD     UART5_IRQHandler                  ; UART5
        DCD     USART6_IRQHandler                 ; USART6
        DCD     UART7_IRQHandler                  ; UART7
        DCD     UART8_IRQHandler                  ; UART8
        DCD     UART9_IRQHandler                  ; UART9
        DCD     USART10_IRQHandler                ; USART10
        DCD     LPUART1_IRQHandler                ; LP UART1
        DCD     XSPI1_IRQHandler                  ; XSPI1 global interrupt
        DCD     XSPI2_IRQHandler                  ; XSPI2 global interrupt
        DCD     XSPI3_IRQHandler                  ; XSPI3 global interrupt
        DCD     FMC_IRQHandler                    ; FMC
        DCD     SDMMC1_IRQHandler                 ; SDMMC1
        DCD     SDMMC2_IRQHandler                 ; SDMMC2
        DCD     UCPD1_IRQHandler                  ; UCPD1
        DCD     USB1_OTG_HS_IRQHandler            ; USB1 OTG HS
        DCD     USB2_OTG_HS_IRQHandler            ; USB2 OTG HS
        DCD     ETH1_IRQHandler                   ; Ethernet
        DCD     FDCAN1_IT0_IRQHandler             ; FDCAN1 Interrupt 0
        DCD     FDCAN1_IT1_IRQHandler             ; FDCAN1 Interrupt 1
        DCD     FDCAN2_IT0_IRQHandler             ; FDCAN2 Interrupt 0
        DCD     FDCAN2_IT1_IRQHandler             ; FDCAN2 Interrupt 1
        DCD     FDCAN3_IT0_IRQHandler             ; FDCAN3 Interrupt 0
        DCD     FDCAN3_IT1_IRQHandler             ; FDCAN3 Interrupt 1
        DCD     FDCAN_CU_IRQHandler               ; FDCAN clock unit interrupt
        DCD     MDIOS_IRQHandler                  ; MDIOS
        DCD     DCMI_PSSI_IRQHandler              ; DCMI_PSSI
        DCD     WAKEUP_PIN_IRQHandler             ; Wake-up pins interrupt
        DCD     CTI_INT0_IRQHandler               ; CTI INT0 interrupt
        DCD     CTI_INT1_IRQHandler               ; CTI INT1 interrupt
        DCD     0                                 ; Reserved
        DCD     LTDC_UP_IRQHandler                ; LTDC up-layer interrupt
        DCD     LTDC_UP_ERR_IRQHandler            ; LTDC up-layer error interrupt
__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler

        LDR     LR, =0xFFFFFFFF
        LDR     R0, =sfb(CSTACK)
        MSR     MSPLIM, R0               ; Set Stack Pointer Limit
        LDR     SP, =sfe(CSTACK)         ; Set Stack Pointer
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SecureFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SecureFault_Handler
        B SecureFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK PVD_PVM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PVD_PVM_IRQHandler
        B PVD_PVM_IRQHandler

        PUBWEAK DTS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DTS_IRQHandler
        B DTS_IRQHandler

        PUBWEAK RCC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCC_IRQHandler
        B RCC_IRQHandler

        PUBWEAK LOCKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LOCKUP_IRQHandler
        B LOCKUP_IRQHandler

        PUBWEAK CACHE_ECC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CACHE_ECC_IRQHandler
        B CACHE_ECC_IRQHandler

        PUBWEAK TCM_ECC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TCM_ECC_IRQHandler
        B TCM_ECC_IRQHandler

        PUBWEAK BKP_ECC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
BKP_ECC_IRQHandler
        B BKP_ECC_IRQHandler

        PUBWEAK FPU_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FPU_IRQHandler
        B FPU_IRQHandler

        PUBWEAK RTC_S_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_S_IRQHandler
        B RTC_S_IRQHandler

        PUBWEAK TAMP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TAMP_IRQHandler
        B TAMP_IRQHandler

        PUBWEAK RIFSC_TAMPER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RIFSC_TAMPER_IRQHandler
        B RIFSC_TAMPER_IRQHandler

        PUBWEAK IAC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
IAC_IRQHandler
        B IAC_IRQHandler

        PUBWEAK RCC_S_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCC_S_IRQHandler
        B RCC_S_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK IWDG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
IWDG_IRQHandler
        B IWDG_IRQHandler

        PUBWEAK WWDG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
WWDG_IRQHandler
        B WWDG_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI0_IRQHandler
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI1_IRQHandler
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI2_IRQHandler
        B EXTI2_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI3_IRQHandler
        B EXTI3_IRQHandler

        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI4_IRQHandler
        B EXTI4_IRQHandler

        PUBWEAK EXTI5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI5_IRQHandler
        B EXTI5_IRQHandler

        PUBWEAK EXTI6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI6_IRQHandler
        B EXTI6_IRQHandler

        PUBWEAK EXTI7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI7_IRQHandler
        B EXTI7_IRQHandler

        PUBWEAK EXTI8_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI8_IRQHandler
        B EXTI8_IRQHandler

        PUBWEAK EXTI9_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI9_IRQHandler
        B EXTI9_IRQHandler

        PUBWEAK EXTI10_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI10_IRQHandler
        B EXTI10_IRQHandler

        PUBWEAK EXTI11_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI11_IRQHandler
        B EXTI11_IRQHandler

        PUBWEAK EXTI12_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI12_IRQHandler
        B EXTI12_IRQHandler

        PUBWEAK EXTI13_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI13_IRQHandler
        B EXTI13_IRQHandler

        PUBWEAK EXTI14_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI14_IRQHandler
        B EXTI14_IRQHandler

        PUBWEAK EXTI15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI15_IRQHandler
        B EXTI15_IRQHandler

        PUBWEAK PKA_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PKA_IRQHandler
        B PKA_IRQHandler

        PUBWEAK HASH_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HASH_IRQHandler
        B HASH_IRQHandler

        PUBWEAK RNG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RNG_IRQHandler
        B RNG_IRQHandler

        PUBWEAK ADC1_2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADC1_2_IRQHandler
        B ADC1_2_IRQHandler

        PUBWEAK CSI_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CSI_IRQHandler
        B CSI_IRQHandler

        PUBWEAK DCMIPP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DCMIPP_IRQHandler
        B DCMIPP_IRQHandler

        PUBWEAK PAHB_ERR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
PAHB_ERR_IRQHandler
        B PAHB_ERR_IRQHandler

        PUBWEAK NPU0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
NPU0_IRQHandler
        B NPU0_IRQHandler

        PUBWEAK NPU1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
NPU1_IRQHandler
        B NPU1_IRQHandler

        PUBWEAK NPU2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
NPU2_IRQHandler
        B NPU2_IRQHandler

        PUBWEAK NPU3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
NPU3_IRQHandler
        B NPU3_IRQHandler

        PUBWEAK CACHEAXI_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CACHEAXI_IRQHandler
        B CACHEAXI_IRQHandler

        PUBWEAK LTDC_LO_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LTDC_LO_IRQHandler
        B LTDC_LO_IRQHandler

        PUBWEAK LTDC_LO_ERR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LTDC_LO_ERR_IRQHandler
        B LTDC_LO_ERR_IRQHandler

        PUBWEAK DMA2D_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA2D_IRQHandler
        B DMA2D_IRQHandler

        PUBWEAK JPEG_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
JPEG_IRQHandler
        B JPEG_IRQHandler

        PUBWEAK VENC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
VENC_IRQHandler
        B VENC_IRQHandler

        PUBWEAK GFXMMU_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GFXMMU_IRQHandler
        B GFXMMU_IRQHandler

        PUBWEAK GFXTIM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GFXTIM_IRQHandler
        B GFXTIM_IRQHandler

        PUBWEAK GPU2D_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPU2D_IRQHandler
        B GPU2D_IRQHandler

        PUBWEAK GPU2D_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPU2D_ER_IRQHandler
        B GPU2D_ER_IRQHandler

        PUBWEAK ICACHE_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ICACHE_IRQHandler
        B ICACHE_IRQHandler

        PUBWEAK HPDMA1_Channel0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel0_IRQHandler
        B HPDMA1_Channel0_IRQHandler

        PUBWEAK HPDMA1_Channel1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel1_IRQHandler
        B HPDMA1_Channel1_IRQHandler

        PUBWEAK HPDMA1_Channel2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel2_IRQHandler
        B HPDMA1_Channel2_IRQHandler

        PUBWEAK HPDMA1_Channel3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel3_IRQHandler
        B HPDMA1_Channel3_IRQHandler

        PUBWEAK HPDMA1_Channel4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel4_IRQHandler
        B HPDMA1_Channel4_IRQHandler

        PUBWEAK HPDMA1_Channel5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel5_IRQHandler
        B HPDMA1_Channel5_IRQHandler

        PUBWEAK HPDMA1_Channel6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel6_IRQHandler
        B HPDMA1_Channel6_IRQHandler

        PUBWEAK HPDMA1_Channel7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel7_IRQHandler
        B HPDMA1_Channel7_IRQHandler

        PUBWEAK HPDMA1_Channel8_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel8_IRQHandler
        B HPDMA1_Channel8_IRQHandler

        PUBWEAK HPDMA1_Channel9_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel9_IRQHandler
        B HPDMA1_Channel9_IRQHandler

        PUBWEAK HPDMA1_Channel10_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel10_IRQHandler
        B HPDMA1_Channel10_IRQHandler

        PUBWEAK HPDMA1_Channel11_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel11_IRQHandler
        B HPDMA1_Channel11_IRQHandler

        PUBWEAK HPDMA1_Channel12_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel12_IRQHandler
        B HPDMA1_Channel12_IRQHandler

        PUBWEAK HPDMA1_Channel13_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel13_IRQHandler
        B HPDMA1_Channel13_IRQHandler

        PUBWEAK HPDMA1_Channel14_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel14_IRQHandler
        B HPDMA1_Channel14_IRQHandler

        PUBWEAK HPDMA1_Channel15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
HPDMA1_Channel15_IRQHandler
        B HPDMA1_Channel15_IRQHandler

        PUBWEAK GPDMA1_Channel0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel0_IRQHandler
        B GPDMA1_Channel0_IRQHandler

        PUBWEAK GPDMA1_Channel1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel1_IRQHandler
        B GPDMA1_Channel1_IRQHandler

        PUBWEAK GPDMA1_Channel2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel2_IRQHandler
        B GPDMA1_Channel2_IRQHandler

        PUBWEAK GPDMA1_Channel3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel3_IRQHandler
        B GPDMA1_Channel3_IRQHandler

        PUBWEAK GPDMA1_Channel4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel4_IRQHandler
        B GPDMA1_Channel4_IRQHandler

        PUBWEAK GPDMA1_Channel5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel5_IRQHandler
        B GPDMA1_Channel5_IRQHandler

        PUBWEAK GPDMA1_Channel6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel6_IRQHandler
        B GPDMA1_Channel6_IRQHandler

        PUBWEAK GPDMA1_Channel7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel7_IRQHandler
        B GPDMA1_Channel7_IRQHandler

        PUBWEAK GPDMA1_Channel8_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel8_IRQHandler
        B GPDMA1_Channel8_IRQHandler

        PUBWEAK GPDMA1_Channel9_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel9_IRQHandler
        B GPDMA1_Channel9_IRQHandler

        PUBWEAK GPDMA1_Channel10_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel10_IRQHandler
        B GPDMA1_Channel10_IRQHandler

        PUBWEAK GPDMA1_Channel11_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel11_IRQHandler
        B GPDMA1_Channel11_IRQHandler

        PUBWEAK GPDMA1_Channel12_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel12_IRQHandler
        B GPDMA1_Channel12_IRQHandler

        PUBWEAK GPDMA1_Channel13_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel13_IRQHandler
        B GPDMA1_Channel13_IRQHandler

        PUBWEAK GPDMA1_Channel14_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel14_IRQHandler
        B GPDMA1_Channel14_IRQHandler

        PUBWEAK GPDMA1_Channel15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
GPDMA1_Channel15_IRQHandler
        B GPDMA1_Channel15_IRQHandler

        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_EV_IRQHandler
        B I2C1_EV_IRQHandler

        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_ER_IRQHandler
        B I2C1_ER_IRQHandler

        PUBWEAK I2C2_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C2_EV_IRQHandler
        B I2C2_EV_IRQHandler

        PUBWEAK I2C2_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C2_ER_IRQHandler
        B I2C2_ER_IRQHandler

        PUBWEAK I2C3_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C3_EV_IRQHandler
        B I2C3_EV_IRQHandler

        PUBWEAK I2C3_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C3_ER_IRQHandler
        B I2C3_ER_IRQHandler

        PUBWEAK I2C4_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C4_EV_IRQHandler
        B I2C4_EV_IRQHandler

        PUBWEAK I2C4_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C4_ER_IRQHandler
        B I2C4_ER_IRQHandler

        PUBWEAK I3C1_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I3C1_EV_IRQHandler
        B I3C1_EV_IRQHandler

        PUBWEAK I3C1_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I3C1_ER_IRQHandler
        B I3C1_ER_IRQHandler

        PUBWEAK I3C2_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I3C2_EV_IRQHandler
        B I3C2_EV_IRQHandler

        PUBWEAK I3C2_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I3C2_ER_IRQHandler
        B I3C2_ER_IRQHandler

        PUBWEAK TIM1_BRK_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM1_BRK_IRQHandler
        B TIM1_BRK_IRQHandler

        PUBWEAK TIM1_UP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM1_UP_IRQHandler
        B TIM1_UP_IRQHandler

        PUBWEAK TIM1_TRG_COM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM1_TRG_COM_IRQHandler
        B TIM1_TRG_COM_IRQHandler

        PUBWEAK TIM1_CC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM1_CC_IRQHandler
        B TIM1_CC_IRQHandler

        PUBWEAK TIM2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM2_IRQHandler
        B TIM2_IRQHandler

        PUBWEAK TIM3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM3_IRQHandler
        B TIM3_IRQHandler

        PUBWEAK TIM4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM4_IRQHandler
        B TIM4_IRQHandler

        PUBWEAK TIM5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM5_IRQHandler
        B TIM5_IRQHandler

        PUBWEAK TIM6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM6_IRQHandler
        B TIM6_IRQHandler

        PUBWEAK TIM7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM7_IRQHandler
        B TIM7_IRQHandler

        PUBWEAK TIM8_BRK_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM8_BRK_IRQHandler
        B TIM8_BRK_IRQHandler

        PUBWEAK TIM8_UP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM8_UP_IRQHandler
        B TIM8_UP_IRQHandler

        PUBWEAK TIM8_TRG_COM_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM8_TRG_COM_IRQHandler
        B TIM8_TRG_COM_IRQHandler

        PUBWEAK TIM8_CC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM8_CC_IRQHandler
        B TIM8_CC_IRQHandler

        PUBWEAK TIM9_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM9_IRQHandler
        B TIM9_IRQHandler

        PUBWEAK TIM10_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM10_IRQHandler
        B TIM10_IRQHandler

        PUBWEAK TIM11_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM11_IRQHandler
        B TIM11_IRQHandler

        PUBWEAK TIM12_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM12_IRQHandler
        B TIM12_IRQHandler

        PUBWEAK TIM13_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM13_IRQHandler
        B TIM13_IRQHandler

        PUBWEAK TIM14_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM14_IRQHandler
        B TIM14_IRQHandler

        PUBWEAK TIM15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM15_IRQHandler
        B TIM15_IRQHandler

        PUBWEAK TIM16_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM16_IRQHandler
        B TIM16_IRQHandler

        PUBWEAK TIM17_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM17_IRQHandler
        B TIM17_IRQHandler

        PUBWEAK TIM18_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIM18_IRQHandler
        B TIM18_IRQHandler

        PUBWEAK LPTIM1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPTIM1_IRQHandler
        B LPTIM1_IRQHandler

        PUBWEAK LPTIM2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPTIM2_IRQHandler
        B LPTIM2_IRQHandler

        PUBWEAK LPTIM3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPTIM3_IRQHandler
        B LPTIM3_IRQHandler

        PUBWEAK LPTIM4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPTIM4_IRQHandler
        B LPTIM4_IRQHandler

        PUBWEAK LPTIM5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPTIM5_IRQHandler
        B LPTIM5_IRQHandler

        PUBWEAK ADF1_FLT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADF1_FLT0_IRQHandler
        B ADF1_FLT0_IRQHandler

        PUBWEAK MDF1_FLT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDF1_FLT0_IRQHandler
        B MDF1_FLT0_IRQHandler

        PUBWEAK MDF1_FLT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDF1_FLT1_IRQHandler
        B MDF1_FLT1_IRQHandler

        PUBWEAK MDF1_FLT2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDF1_FLT2_IRQHandler
        B MDF1_FLT2_IRQHandler

        PUBWEAK MDF1_FLT3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDF1_FLT3_IRQHandler
        B MDF1_FLT3_IRQHandler

        PUBWEAK MDF1_FLT4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDF1_FLT4_IRQHandler
        B MDF1_FLT4_IRQHandler

        PUBWEAK MDF1_FLT5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDF1_FLT5_IRQHandler
        B MDF1_FLT5_IRQHandler

        PUBWEAK SAI1_A_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SAI1_A_IRQHandler
        B SAI1_A_IRQHandler

        PUBWEAK SAI1_B_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SAI1_B_IRQHandler
        B SAI1_A_IRQHandler

        PUBWEAK SAI2_A_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SAI2_A_IRQHandler
        B SAI2_A_IRQHandler

        PUBWEAK SAI2_B_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SAI2_B_IRQHandler
        B SAI2_B_IRQHandler

        PUBWEAK SPDIFRX1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPDIFRX1_IRQHandler
        B SPDIFRX1_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI1_IRQHandler
        B SPI1_IRQHandler

        PUBWEAK SPI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI2_IRQHandler
        B SPI2_IRQHandler

        PUBWEAK SPI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI3_IRQHandler
        B SPI3_IRQHandler

        PUBWEAK SPI4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI4_IRQHandler
        B SPI4_IRQHandler

        PUBWEAK SPI5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI5_IRQHandler
        B SPI5_IRQHandler

        PUBWEAK SPI6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI6_IRQHandler
        B SPI6_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART1_IRQHandler
        B USART1_IRQHandler

        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART2_IRQHandler
        B USART2_IRQHandler

        PUBWEAK USART3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART3_IRQHandler
        B USART3_IRQHandler

        PUBWEAK UART4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART4_IRQHandler
        B UART4_IRQHandler

        PUBWEAK UART5_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART5_IRQHandler
        B UART5_IRQHandler

        PUBWEAK USART6_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART6_IRQHandler
        B USART6_IRQHandler

        PUBWEAK UART7_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART7_IRQHandler
        B UART7_IRQHandler

        PUBWEAK UART8_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART8_IRQHandler
        B UART8_IRQHandler

        PUBWEAK UART9_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UART9_IRQHandler
        B UART9_IRQHandler

        PUBWEAK USART10_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART10_IRQHandler
        B USART10_IRQHandler

        PUBWEAK LPUART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LPUART1_IRQHandler
        B LPUART1_IRQHandler

        PUBWEAK XSPI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
XSPI1_IRQHandler
        B XSPI1_IRQHandler

        PUBWEAK XSPI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
XSPI2_IRQHandler
        B XSPI2_IRQHandler

        PUBWEAK XSPI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
XSPI3_IRQHandler
        B XSPI3_IRQHandler

        PUBWEAK FMC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FMC_IRQHandler
        B FMC_IRQHandler

        PUBWEAK SDMMC1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SDMMC1_IRQHandler
        B SDMMC1_IRQHandler

        PUBWEAK SDMMC2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SDMMC2_IRQHandler
        B SDMMC2_IRQHandler

        PUBWEAK UCPD1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
UCPD1_IRQHandler
        B UCPD1_IRQHandler

        PUBWEAK USB1_OTG_HS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USB1_OTG_HS_IRQHandler
        B USB1_OTG_HS_IRQHandler

        PUBWEAK USB2_OTG_HS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USB2_OTG_HS_IRQHandler
        B USB2_OTG_HS_IRQHandler

        PUBWEAK ETH1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ETH1_IRQHandler
        B ETH1_IRQHandler

        PUBWEAK FDCAN1_IT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN1_IT0_IRQHandler
        B FDCAN1_IT0_IRQHandler

        PUBWEAK FDCAN1_IT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN1_IT1_IRQHandler
        B FDCAN1_IT1_IRQHandler

        PUBWEAK FDCAN2_IT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN2_IT0_IRQHandler
        B FDCAN2_IT0_IRQHandler

        PUBWEAK FDCAN2_IT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN2_IT1_IRQHandler
        B FDCAN2_IT1_IRQHandler

        PUBWEAK FDCAN3_IT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN3_IT0_IRQHandler
        B FDCAN3_IT0_IRQHandler

        PUBWEAK FDCAN3_IT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN3_IT1_IRQHandler
        B FDCAN3_IT1_IRQHandler

        PUBWEAK FDCAN_CU_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FDCAN_CU_IRQHandler
        B FDCAN_CU_IRQHandler

        PUBWEAK MDIOS_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
MDIOS_IRQHandler
        B MDIOS_IRQHandler

        PUBWEAK DCMI_PSSI_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DCMI_PSSI_IRQHandler
        B DCMI_PSSI_IRQHandler

        PUBWEAK WAKEUP_PIN_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
WAKEUP_PIN_IRQHandler
        B WAKEUP_PIN_IRQHandler

        PUBWEAK CTI_INT0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CTI_INT0_IRQHandler
        B CTI_INT0_IRQHandler

        PUBWEAK CTI_INT1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CTI_INT1_IRQHandler
        B CTI_INT1_IRQHandler

        PUBWEAK LTDC_UP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LTDC_UP_IRQHandler
        B LTDC_UP_IRQHandler

        PUBWEAK LTDC_UP_ERR_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
LTDC_UP_ERR_IRQHandler
        B LTDC_UP_ERR_IRQHandler

        END
