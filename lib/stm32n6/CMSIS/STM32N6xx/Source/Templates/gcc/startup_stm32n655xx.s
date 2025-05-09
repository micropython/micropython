/**
 ******************************************************************************
 * @file      startup_stm32n655xx.s
 * @author    GPM Application Team
 * @brief     STM32N655XX device vector table for GCC toolchain.
 *            This module performs:
 *                - Set the initial SP
 *                - Set the initial PC == Reset_Handler,
 *                - Set the vector table entries with the exceptions ISR address
 *                - Branches to main in the C library (which eventually
 *                  calls main()).
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

.syntax unified
.arch armv8.1-m.main
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_sstack
  msr   MSPLIM, r0
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */
/* Call the clock system initialization function.*/
  bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
  bl main

LoopForever:
  b LoopForever

  .size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
  .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

/******************************************************************************
*
* The STM32N655XX vector table.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word	MemManage_Handler
  .word	BusFault_Handler
  .word	UsageFault_Handler
  .word	SecureFault_Handler
  .word	0
  .word	0
  .word	0
  .word	SVC_Handler
  .word	DebugMon_Handler
  .word	0
  .word	PendSV_Handler
  .word	SysTick_Handler
  .word	PVD_PVM_IRQHandler         			/* PVDOUT through the EXTI line                                  */
  .word	0                          			/* Reserved                                                      */
  .word	DTS_IRQHandler             			/* Thermal sensor interruption                                   */
  .word	RCC_IRQHandler             			/* RCC global interrupt                                          */
  .word	LOCKUP_IRQHandler          			/* LOCKUP - no overstack in Cortex-M55                           */
  .word	CACHE_ECC_IRQHandler       			/* Cache ECC error                                               */
  .word	TCM_ECC_IRQHandler         			/* TCM ECC error                                                 */
  .word	BCK_ECC_IRQHandler         			/* Backup RAM interrupts (SEC and DED)                           */
  .word	FPU_IRQHandler             			/* FPU safety flag                                               */
  .word	0                          			/* Reserved                                                      */
  .word	RTC_S_IRQHandler           			/* RTC secure interrupt                                          */
  .word	TAMP_IRQHandler            			/* TAMP secure and non-secure synchronous interrupt line         */
  .word	RIFSC_TAMPER_IRQHandler    			/* RIF can generate an interrupt when a laser attack is detected */
  .word	IAC_IRQHandler             			/* IAC global interrupt                                          */
  .word	RCC_S_IRQHandler           			/* RCC global secure interrupt                                   */
  .word	0                          			/* Reserved                                                      */
  .word	RTC_IRQHandler             			/* RTC interrupt                                                 */
  .word	0                          			/* Reserved                                                      */
  .word	IWDG_IRQHandler         			/* Independent watchdog interrupt                                */
  .word	WWDG_IRQHandler         			/* Window watchdog interrupt                                     */
  .word	EXTI0_IRQHandler           			/* EXTI Line 0 interrupt through the EXTI line                   */
  .word	EXTI1_IRQHandler           			/* EXTI Line 1 interrupt through the EXTI line                   */
  .word	EXTI2_IRQHandler           			/* EXTI Line 2 interrupt through the EXTI line                   */
  .word	EXTI3_IRQHandler           			/* EXTI Line 3 interrupt through the EXTI line                   */
  .word	EXTI4_IRQHandler           			/* EXTI Line 4 interrupt through the EXTI line                   */
  .word	EXTI5_IRQHandler           			/* EXTI Line 5 interrupt through the EXTI line                   */
  .word	EXTI6_IRQHandler           			/* EXTI Line 6 interrupt through the EXTI line                   */
  .word	EXTI7_IRQHandler           			/* EXTI Line 7 interrupt through the EXTI line                   */
  .word	EXTI8_IRQHandler           			/* EXTI Line 8 interrupt through the EXTI line                   */
  .word	EXTI9_IRQHandler           			/* EXTI Line 9 interrupt                                         */
  .word	EXTI10_IRQHandler          			/* EXTI Line 10 interrupt                                        */
  .word	EXTI11_IRQHandler          			/* EXTI Line 11 interrupt                                        */
  .word	EXTI12_IRQHandler          			/* EXTI Line 12 interrupt                                        */
  .word	EXTI13_IRQHandler          			/* EXTI Line 13 interrupt                                        */
  .word	EXTI14_IRQHandler          			/* EXTI Line 14 interrupt                                        */
  .word	EXTI15_IRQHandler          			/* EXTI Line 15 interrupt                                        */
  .word	SAES_IRQHandler            			/* SAES global interrupt                                         */
  .word	CRYP_IRQHandler            			/* CRYP global interrupt                                         */
  .word	PKA_IRQHandler             			/* PKA global interrupt                                          */
  .word	HASH_IRQHandler            			/* HASH global interrupt                                         */
  .word	RNG_IRQHandler             			/* RNG global interrupt                                          */
  .word	0                          			/* Reserved                                                      */
  .word	MCE1_IRQHandler            			/* MCE1 global interrupt                                         */
  .word	MCE2_IRQHandler            			/* MCE2 global interrupt                                         */
  .word	MCE3_IRQHandler            			/* MCE3 global interrupt                                         */
  .word	MCE4_IRQHandler            			/* MCE4 global interrupt                                         */
  .word	ADC1_2_IRQHandler           		/* ADC1/ADC2 global interrupt                                    */
  .word	CSI_IRQHandler         			    /* CSI global interrupt                                          */
  .word	DCMIPP_IRQHandler          			/* DCMIPP global interrupt                                       */
  .word	0                          			/* Reserved                                                      */
  .word	0                          			/* Reserved                                                      */
  .word	0                          			/* Reserved                                                      */
  .word	PAHB_ERR_IRQHandler        			/* Write posting errors on Cortex-M55 PAHB interface             */
  .word 0                                   /* Reserved                                                      */
  .word 0                                   /* Reserved                                                      */
  .word 0                                   /* Reserved                                                      */
  .word 0                                   /* Reserved                                                      */
  .word 0                                   /* Reserved                                                      */
  .word	LTDC_LO_IRQHandler         			/* LCD low-layer global interrupt                                */
  .word	LTDC_LO_ERR_IRQHandler     			/* LCD low-layer error interrupt                                 */
  .word	DMA2D_IRQHandler           			/* DMA2D global interrupt                                        */
  .word	JPEG_IRQHandler            			/* JPEG global interrupt                                         */
  .word	VENC_IRQHandler            			/* VENC global interrupt                                         */
  .word	GFXMMU_IRQHandler          			/* GFXMMU global interrupt                                       */
  .word	GFXTIM_IRQHandler          			/* GFXTIM global interrupt                                       */
  .word	GPU2D_IRQHandler           			/* GPU2D global interrupt                                        */
  .word	GPU2D_ER_IRQHandler     			/* GPU2D global interrupt                                        */
  .word	ICACHE_IRQHandler       			/* GPU cache interrupt                                           */
  .word	HPDMA1_Channel0_IRQHandler  		/* HPDMA1 Channel 0 interrupt                                    */
  .word	HPDMA1_Channel1_IRQHandler  		/* HPDMA1 Channel 1 interrupt                                    */
  .word	HPDMA1_Channel2_IRQHandler  		/* HPDMA1 Channel 2 interrupt                                    */
  .word	HPDMA1_Channel3_IRQHandler  		/* HPDMA1 Channel 3 interrupt                                    */
  .word	HPDMA1_Channel4_IRQHandler  		/* HPDMA1 Channel 4 interrupt                                    */
  .word	HPDMA1_Channel5_IRQHandler  		/* HPDMA1 Channel 5 interrupt                                    */
  .word	HPDMA1_Channel6_IRQHandler  		/* HPDMA1 Channel 6 interrupt                                    */
  .word	HPDMA1_Channel7_IRQHandler  		/* HPDMA1 Channel 7 interrupt                                    */
  .word	HPDMA1_Channel8_IRQHandler  		/* HPDMA1 Channel 8 interrupt                                    */
  .word	HPDMA1_Channel9_IRQHandler  		/* HPDMA1 Channel 9 interrupt                                    */
  .word	HPDMA1_Channel10_IRQHandler 		/* HPDMA1 Channel 10 interrupt                                   */
  .word	HPDMA1_Channel11_IRQHandler 		/* HPDMA1 Channel 11 interrupt                                   */
  .word	HPDMA1_Channel12_IRQHandler 		/* HPDMA1 Channel 12 interrupt                                   */
  .word	HPDMA1_Channel13_IRQHandler 		/* HPDMA1 Channel 13 interrupt                                   */
  .word	HPDMA1_Channel14_IRQHandler 		/* HPDMA1 Channel 14 interrupt                                   */
  .word	HPDMA1_Channel15_IRQHandler 		/* HPDMA1 Channel 15 interrupt                                   */
  .word	GPDMA1_Channel0_IRQHandler  		/* GPDMA1 channel 0 interrupt                                    */
  .word	GPDMA1_Channel1_IRQHandler  		/* GPDMA1 channel 1 interrupt                                    */
  .word	GPDMA1_Channel2_IRQHandler  		/* GPDMA1 channel 2 interrupt                                    */
  .word	GPDMA1_Channel3_IRQHandler  		/* GPDMA1 channel 3 interrupt                                    */
  .word	GPDMA1_Channel4_IRQHandler  		/* GPDMA1 channel 4 interrupt                                    */
  .word	GPDMA1_Channel5_IRQHandler  		/* GPDMA1 channel 5 interrupt                                    */
  .word	GPDMA1_Channel6_IRQHandler  		/* GPDMA1 channel 6 interrupt                                    */
  .word	GPDMA1_Channel7_IRQHandler  		/* GPDMA1 channel 7 interrupt                                    */
  .word	GPDMA1_Channel8_IRQHandler  		/* GPDMA1 channel 8 interrupt                                    */
  .word	GPDMA1_Channel9_IRQHandler  		/* GPDMA1 channel 9 interrupt                                    */
  .word	GPDMA1_Channel10_IRQHandler 		/* GPDMA1 channel 10 interrupt                                   */
  .word	GPDMA1_Channel11_IRQHandler 		/* GPDMA1 channel 11 interrupt                                   */
  .word	GPDMA1_Channel12_IRQHandler 		/* GPDMA1 channel 12 interrupt                                   */
  .word	GPDMA1_Channel13_IRQHandler 		/* GPDMA1 channel 13 interrupt                                   */
  .word	GPDMA1_Channel14_IRQHandler 		/* GPDMA1 channel 14 interrupt                                   */
  .word	GPDMA1_Channel15_IRQHandler 		/* GPDMA1 channel 15 interrupt                                   */
  .word	I2C1_EV_IRQHandler         			/* I2C1 event interrupt                                          */
  .word	I2C1_ER_IRQHandler         			/* I2C1 error interrupt                                          */
  .word	I2C2_EV_IRQHandler         			/* I2C2 event interrupt                                          */
  .word	I2C2_ER_IRQHandler         			/* I2C2 error interrupt                                          */
  .word	I2C3_EV_IRQHandler         			/* I2C3 event interrupt                                          */
  .word	I2C3_ER_IRQHandler         			/* I2C3 error interrupt                                          */
  .word	I2C4_EV_IRQHandler         			/* I2C4 event interrupt                                          */
  .word	I2C4_ER_IRQHandler         			/* I2C4 error interrupt                                          */
  .word	I3C1_EV_IRQHandler         			/* I3C1 event interrupt                                          */
  .word	I3C1_ER_IRQHandler         			/* I3C1 error interrupt                                          */
  .word	I3C2_EV_IRQHandler         			/* I3C2 event interrupt                                          */
  .word	I3C2_ER_IRQHandler         			/* I3C2 error interrupt                                          */
  .word	TIM1_BRK_IRQHandler        			/* TIM1 Break interrupt                                          */
  .word	TIM1_UP_IRQHandler         			/* TIM1 Update interrupt                                         */
  .word	TIM1_TRG_CCU_IRQHandler    			/* TIM1 Trigger and Commutation interrupts                       */
  .word	TIM1_CC_IRQHandler         			/* TIM1 Capture Compare interrupt                                */
  .word	TIM2_IRQHandler            			/* TIM2 global interrupt                                         */
  .word	TIM3_IRQHandler            			/* TIM3 global interrupt                                         */
  .word	TIM4_IRQHandler            			/* TIM4 global interrupt                                         */
  .word	TIM5_IRQHandler            			/* TIM5 global interrupt                                         */
  .word	TIM6_IRQHandler            			/* TIM6 Global interrupt                                         */
  .word	TIM7_IRQHandler            			/* TIM7 Global interrupt                                         */
  .word	TIM8_BRK_IRQHandler        			/* TIM8 Break interrupt                                          */
  .word	TIM8_UP_IRQHandler         			/* TIM8 Update interrupt                                         */
  .word	TIM8_TRG_CCU_IRQHandler    			/* TIM8 Trigger and Commutation interrupts                       */
  .word	TIM8_CC_IRQHandler         			/* TIM8 Capture Compare interrupt                                */
  .word	TIM9_IRQHandler            			/* TIM9 Global interrupt                                         */
  .word	TIM10_IRQHandler           			/* TIM10 Global interrupt                                        */
  .word	TIM11_IRQHandler           			/* TIM11 Global interrupt                                        */
  .word	TIM12_IRQHandler           			/* TIM12 Global interrupt                                        */
  .word	TIM13_IRQHandler           			/* TIM13 Global interrupt                                        */
  .word	TIM14_IRQHandler           			/* TIM14 Global interrupt                                        */
  .word	TIM15_IRQHandler           			/* TIM15 global interrupt                                        */
  .word	TIM16_IRQHandler           			/* TIM16 global interrupt                                        */
  .word	TIM17_IRQHandler           			/* TIM17 global interrupt                                        */
  .word	TIM18_IRQHandler           			/* TIM18 Global interrupt                                        */
  .word	LPTIM1_IRQHandler          			/* LPTIM1 global interrupt                                       */
  .word	LPTIM2_IRQHandler          			/* LPTIM2 global interrupt                                       */
  .word	LPTIM3_IRQHandler          			/* LPTIM3 global interrupt                                       */
  .word	LPTIM4_IRQHandler          			/* LPTIM4 global interrupt                                       */
  .word	LPTIM5_IRQHandler          			/* LPTIM5 global interrupt                                       */
  .word	ADF1_FLT0_IRQHandler       			/* ADF1 filter 0 global interrupt                                */
  .word	MDF1_FLT0_IRQHandler       			/* MDF global Interrupt for Filter0                              */
  .word	MDF1_FLT1_IRQHandler       			/* MDF global Interrupt for Filter1                              */
  .word	MDF1_FLT2_IRQHandler       			/* MDF global Interrupt for Filter2                              */
  .word	MDF1_FLT3_IRQHandler       			/* MDF global Interrupt for Filter3                              */
  .word	MDF1_FLT4_IRQHandler       			/* MDF global Interrupt for Filter4                              */
  .word	MDF1_FLT5_IRQHandler       			/* MDF global Interrupt for Filter5                              */
  .word	SAI1_A_IRQHandler   			    /* SAI1 global interrupt A                                       */
  .word	SAI1_B_IRQHandler   		  	    /* SAI1 global interrupt B                                       */
  .word	SAI2_A_IRQHandler   			    /* SAI2 global interrupt A                                       */
  .word	SAI2_B_IRQHandler   			    /* SAI2 global interrupt B                                       */
  .word	SPDIFRX_IRQHandler         			/* SPDIFRX global interrupt                                      */
  .word	SPI1_IRQHandler            			/* SPI1 global interrupt A                                       */
  .word	SPI2_IRQHandler            			/* SPI2 global interrupt A                                       */
  .word	SPI3_IRQHandler            			/* SPI3 global interrupt A                                       */
  .word	SPI4_IRQHandler            			/* SPI4 global interrupt A                                       */
  .word	SPI5_IRQHandler            			/* SPI5 global interrupt A                                       */
  .word	SPI6_IRQHandler            			/* SPI6 global interrupt A                                       */
  .word	USART1_IRQHandler          			/* USART1 Global interrupt                                       */
  .word	USART2_IRQHandler          			/* USART2 Global interrupt                                       */
  .word	USART3_IRQHandler          			/* USART3 Global interrupt                                       */
  .word	UART4_IRQHandler           			/* UART4 Global interrupt                                        */
  .word	UART5_IRQHandler           			/* UART5 Global interrupt                                        */
  .word	USART6_IRQHandler          			/* USART6 Global interrupt                                       */
  .word	UART7_IRQHandler           			/* UART7 Global interrupt                                        */
  .word	UART8_IRQHandler           			/* UART8 Global interrupt                                        */
  .word	UART9_IRQHandler           			/* UART9 Global interrupt                                        */
  .word	USART10_IRQHandler         			/* USART10 Global interrupt                                      */
  .word	LPUART1_IRQHandler         			/* LPUART1 global interrupt                                      */
  .word	XSPI1_IRQHandler           			/* XSPI1 global interrupt                                        */
  .word	XSPI2_IRQHandler           			/* XSPI2 global interrupt                                        */
  .word	XSPI3_IRQHandler           			/* XSPI3 global interrupt                                        */
  .word	FMC_IRQHandler             			/* FMC global interrupt                                          */
  .word	SDMMC1_IRQHandler          			/* SDMMC1 global interrupt                                       */
  .word	SDMMC2_IRQHandler          			/* SDMMC2 global interrupt                                       */
  .word	UCPD1_IRQHandler            		/* UCPD global interrupt                                         */
  .word	USB1_OTG_HS_IRQHandler              /* USB OTG1 HS global interrupt                                  */
  .word	USB2_OTG_HS_IRQHandler         	    /* USB OTG2 HS global interrupt                                  */
  .word	ETH1_IRQHandler            			/* Ethernet global interrupt                                     */
  .word	FDCAN1_IT0_IRQHandler         	    /* FDCAN1 interrupt 0                                            */
  .word	FDCAN1_IT1_IRQHandler         	    /* FDCAN1 interrupt 1                                            */
  .word	FDCAN2_IT0_IRQHandler        		/* FDCAN2 interrupt 0                                            */
  .word	FDCAN2_IT1_IRQHandler        		/* FDCAN2 interrupt 1                                            */
  .word	FDCAN3_IT0_IRQHandler        		/* FDCAN3 interrupt 0                                            */
  .word	FDCAN3_IT1_IRQHandler        		/* FDCAN3 interrupt 1                                            */
  .word	FDCAN_CU_IRQHandler        			/* Clock calibration unit interrupt line(FDCAN1 only)            */
  .word	MDIOS_IRQHandler           			/* MDIOS global Interrupt                                        */
  .word	DCMI_PSSI_IRQHandler       			/* DCMI/PSSI global interrupt                                    */
  .word	WAKEUP_PIN_IRQHandler      			/* Wake-up pin interrupts                                        */
  .word	CTI_INT0_IRQHandler            	    /* Debug monitor (Cortex-M55 related)                            */
  .word	CTI_INT1_IRQHandler            	    /* Debug monitor (Cortex-M55 related)                            */
  .word	0                          			/* Reserved                                                      */
  .word	LTDC_UP_IRQHandler         			/* LCD up-layer global interrupt                                 */
  .word	LTDC_UP_ERR_IRQHandler     			/* LCD up-layer error interrupt                                  */

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

	.weak	NMI_Handler
	.thumb_set NMI_Handler,Default_Handler

	.weak	HardFault_Handler
	.thumb_set HardFault_Handler,Default_Handler

	.weak	MemManage_Handler
	.thumb_set MemManage_Handler,Default_Handler

	.weak	BusFault_Handler
	.thumb_set BusFault_Handler,Default_Handler

	.weak	UsageFault_Handler
	.thumb_set UsageFault_Handler,Default_Handler

	.weak	SecureFault_Handler
	.thumb_set SecureFault_Handler,Default_Handler

	.weak	SVC_Handler
	.thumb_set SVC_Handler,Default_Handler

	.weak	DebugMon_Handler
	.thumb_set DebugMon_Handler,Default_Handler

	.weak	PendSV_Handler
	.thumb_set PendSV_Handler,Default_Handler

	.weak	SysTick_Handler
	.thumb_set SysTick_Handler,Default_Handler

	.weak	PVD_PVM_IRQHandler
	.thumb_set PVD_PVM_IRQHandler,Default_Handler

	.weak	DTS_IRQHandler
	.thumb_set DTS_IRQHandler,Default_Handler

	.weak	RCC_IRQHandler
	.thumb_set RCC_IRQHandler,Default_Handler

	.weak	LOCKUP_IRQHandler
	.thumb_set LOCKUP_IRQHandler,Default_Handler

	.weak	CACHE_ECC_IRQHandler
	.thumb_set CACHE_ECC_IRQHandler,Default_Handler

	.weak	TCM_ECC_IRQHandler
	.thumb_set TCM_ECC_IRQHandler,Default_Handler

	.weak	BCK_ECC_IRQHandler
	.thumb_set BCK_ECC_IRQHandler,Default_Handler

	.weak	FPU_IRQHandler
	.thumb_set FPU_IRQHandler,Default_Handler

	.weak	RTC_S_IRQHandler
	.thumb_set RTC_S_IRQHandler,Default_Handler

	.weak	TAMP_IRQHandler
	.thumb_set TAMP_IRQHandler,Default_Handler

	.weak	RIFSC_TAMPER_IRQHandler
	.thumb_set RIFSC_TAMPER_IRQHandler,Default_Handler

	.weak	IAC_IRQHandler
	.thumb_set IAC_IRQHandler,Default_Handler

	.weak	RCC_S_IRQHandler
	.thumb_set RCC_S_IRQHandler,Default_Handler

	.weak	RTC_IRQHandler
	.thumb_set RTC_IRQHandler,Default_Handler

	.weak	IWDF_IRQHandler
	.thumb_set IWDG_IRQHandler,Default_Handler

	.weak	WWDG_IRQHandler
	.thumb_set WWDG_IRQHandler,Default_Handler

	.weak	EXTI0_IRQHandler
	.thumb_set EXTI0_IRQHandler,Default_Handler

	.weak	EXTI1_IRQHandler
	.thumb_set EXTI1_IRQHandler,Default_Handler

	.weak	EXTI2_IRQHandler
	.thumb_set EXTI2_IRQHandler,Default_Handler

	.weak	EXTI3_IRQHandler
	.thumb_set EXTI3_IRQHandler,Default_Handler

	.weak	EXTI4_IRQHandler
	.thumb_set EXTI4_IRQHandler,Default_Handler

	.weak	EXTI5_IRQHandler
	.thumb_set EXTI5_IRQHandler,Default_Handler

	.weak	EXTI6_IRQHandler
	.thumb_set EXTI6_IRQHandler,Default_Handler

	.weak	EXTI7_IRQHandler
	.thumb_set EXTI7_IRQHandler,Default_Handler

	.weak	EXTI8_IRQHandler
	.thumb_set EXTI8_IRQHandler,Default_Handler

	.weak	EXTI9_IRQHandler
	.thumb_set EXTI9_IRQHandler,Default_Handler

	.weak	EXTI10_IRQHandler
	.thumb_set EXTI10_IRQHandler,Default_Handler

	.weak	EXTI11_IRQHandler
	.thumb_set EXTI11_IRQHandler,Default_Handler

	.weak	EXTI12_IRQHandler
	.thumb_set EXTI12_IRQHandler,Default_Handler

	.weak	EXTI13_IRQHandler
	.thumb_set EXTI13_IRQHandler,Default_Handler

	.weak	EXTI14_IRQHandler
	.thumb_set EXTI14_IRQHandler,Default_Handler

	.weak	EXTI15_IRQHandler
	.thumb_set EXTI15_IRQHandler,Default_Handler

	.weak	SAES_IRQHandler
	.thumb_set SAES_IRQHandler,Default_Handler

	.weak	CRYP_IRQHandler
	.thumb_set CRYP_IRQHandler,Default_Handler

	.weak	PKA_IRQHandler
	.thumb_set PKA_IRQHandler,Default_Handler

	.weak	HASH_IRQHandler
	.thumb_set HASH_IRQHandler,Default_Handler

	.weak	RNG_IRQHandler
	.thumb_set RNG_IRQHandler,Default_Handler

	.weak	MCE1_IRQHandler
	.thumb_set MCE1_IRQHandler,Default_Handler

	.weak	MCE2_IRQHandler
	.thumb_set MCE2_IRQHandler,Default_Handler

	.weak	MCE3_IRQHandler
	.thumb_set MCE3_IRQHandler,Default_Handler

	.weak	MCE4_IRQHandler
	.thumb_set MCE4_IRQHandler,Default_Handler

	.weak	ADC1_2_IRQHandler
	.thumb_set ADC1_2_IRQHandler,Default_Handler

	.weak	CSI_IRQHandler
	.thumb_set CSI_IRQHandler,Default_Handler

	.weak	DCMIPP_IRQHandler
	.thumb_set DCMIPP_IRQHandler,Default_Handler

	.weak	PAHB_ERR_IRQHandler
	.thumb_set PAHB_ERR_IRQHandler,Default_Handler

	.weak	LTDC_LO_IRQHandler
	.thumb_set LTDC_LO_IRQHandler,Default_Handler

	.weak	LTDC_LO_ERR_IRQHandler
	.thumb_set LTDC_LO_ERR_IRQHandler,Default_Handler

	.weak	DMA2D_IRQHandler
	.thumb_set DMA2D_IRQHandler,Default_Handler

	.weak	JPEG_IRQHandler
	.thumb_set JPEG_IRQHandler,Default_Handler

	.weak	VENC_IRQHandler
	.thumb_set VENC_IRQHandler,Default_Handler

	.weak	GFXMMU_IRQHandler
	.thumb_set GFXMMU_IRQHandler,Default_Handler

	.weak	GFXTIM_IRQHandler
	.thumb_set GFXTIM_IRQHandler,Default_Handler

	.weak	GPU2D_IRQHandler
	.thumb_set GPU2D_IRQHandler,Default_Handler

	.weak	GPU2D_ER_IRQHandler
	.thumb_set GPU2D_ER_IRQHandler,Default_Handler

	.weak	ICACHE_IRQHandler
	.thumb_set ICACHE_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel0_IRQHandler
	.thumb_set HPDMA1_Channel0_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel1_IRQHandler
	.thumb_set HPDMA1_Channel1_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel2_IRQHandler
	.thumb_set HPDMA1_Channel2_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel3_IRQHandler
	.thumb_set HPDMA1_Channel3_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel4_IRQHandler
	.thumb_set HPDMA1_Channel4_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel5_IRQHandler
	.thumb_set HPDMA1_Channel5_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel6_IRQHandler
	.thumb_set HPDMA1_Channel6_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel7_IRQHandler
	.thumb_set HPDMA1_Channel7_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel8_IRQHandler
	.thumb_set HPDMA1_Channel8_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel9_IRQHandler
	.thumb_set HPDMA1_Channel9_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel10_IRQHandler
	.thumb_set HPDMA1_Channel10_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel11_IRQHandler
	.thumb_set HPDMA1_Channel11_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel12_IRQHandler
	.thumb_set HPDMA1_Channel12_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel13_IRQHandler
	.thumb_set HPDMA1_Channel13_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel14_IRQHandler
	.thumb_set HPDMA1_Channel14_IRQHandler,Default_Handler

	.weak	HPDMA1_Channel15_IRQHandler
	.thumb_set HPDMA1_Channel15_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel0_IRQHandler
	.thumb_set GPDMA1_Channel0_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel1_IRQHandler
	.thumb_set GPDMA1_Channel1_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel2_IRQHandler
	.thumb_set GPDMA1_Channel2_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel3_IRQHandler
	.thumb_set GPDMA1_Channel3_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel4_IRQHandler
	.thumb_set GPDMA1_Channel4_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel5_IRQHandler
	.thumb_set GPDMA1_Channel5_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel6_IRQHandler
	.thumb_set GPDMA1_Channel6_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel7_IRQHandler
	.thumb_set GPDMA1_Channel7_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel8_IRQHandler
	.thumb_set GPDMA1_Channel8_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel9_IRQHandler
	.thumb_set GPDMA1_Channel9_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel10_IRQHandler
	.thumb_set GPDMA1_Channel10_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel11_IRQHandler
	.thumb_set GPDMA1_Channel11_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel12_IRQHandler
	.thumb_set GPDMA1_Channel12_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel13_IRQHandler
	.thumb_set GPDMA1_Channel13_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel14_IRQHandler
	.thumb_set GPDMA1_Channel14_IRQHandler,Default_Handler

	.weak	GPDMA1_Channel15_IRQHandler
	.thumb_set GPDMA1_Channel15_IRQHandler,Default_Handler

	.weak	I2C1_EV_IRQHandler
	.thumb_set I2C1_EV_IRQHandler,Default_Handler

	.weak	I2C1_ER_IRQHandler
	.thumb_set I2C1_ER_IRQHandler,Default_Handler

	.weak	I2C2_EV_IRQHandler
	.thumb_set I2C2_EV_IRQHandler,Default_Handler

	.weak	I2C2_ER_IRQHandler
	.thumb_set I2C2_ER_IRQHandler,Default_Handler

	.weak	I2C3_EV_IRQHandler
	.thumb_set I2C3_EV_IRQHandler,Default_Handler

	.weak	I2C3_ER_IRQHandler
	.thumb_set I2C3_ER_IRQHandler,Default_Handler

	.weak	I2C4_EV_IRQHandler
	.thumb_set I2C4_EV_IRQHandler,Default_Handler

	.weak	I2C4_ER_IRQHandler
	.thumb_set I2C4_ER_IRQHandler,Default_Handler

	.weak	I3C1_EV_IRQHandler
	.thumb_set I3C1_EV_IRQHandler,Default_Handler

	.weak	I3C1_ER_IRQHandler
	.thumb_set I3C1_ER_IRQHandler,Default_Handler

	.weak	I3C2_EV_IRQHandler
	.thumb_set I3C2_EV_IRQHandler,Default_Handler

	.weak	I3C2_ER_IRQHandler
	.thumb_set I3C2_ER_IRQHandler,Default_Handler

	.weak	TIM1_BRK_IRQHandler
	.thumb_set TIM1_BRK_IRQHandler,Default_Handler

	.weak	TIM1_UP_IRQHandler
	.thumb_set TIM1_UP_IRQHandler,Default_Handler

	.weak	TIM1_TRG_CCU_IRQHandler
	.thumb_set TIM1_TRG_CCU_IRQHandler,Default_Handler

	.weak	TIM1_CC_IRQHandler
	.thumb_set TIM1_CC_IRQHandler,Default_Handler

	.weak	TIM2_IRQHandler
	.thumb_set TIM2_IRQHandler,Default_Handler

	.weak	TIM3_IRQHandler
	.thumb_set TIM3_IRQHandler,Default_Handler

	.weak	TIM4_IRQHandler
	.thumb_set TIM4_IRQHandler,Default_Handler

	.weak	TIM5_IRQHandler
	.thumb_set TIM5_IRQHandler,Default_Handler

	.weak	TIM6_IRQHandler
	.thumb_set TIM6_IRQHandler,Default_Handler

	.weak	TIM7_IRQHandler
	.thumb_set TIM7_IRQHandler,Default_Handler

	.weak	TIM8_BRK_IRQHandler
	.thumb_set TIM8_BRK_IRQHandler,Default_Handler

	.weak	TIM8_UP_IRQHandler
	.thumb_set TIM8_UP_IRQHandler,Default_Handler

	.weak	TIM8_TRG_CCU_IRQHandler
	.thumb_set TIM8_TRG_CCU_IRQHandler,Default_Handler

	.weak	TIM8_CC_IRQHandler
	.thumb_set TIM8_CC_IRQHandler,Default_Handler

	.weak	TIM9_IRQHandler
	.thumb_set TIM9_IRQHandler,Default_Handler

	.weak	TIM10_IRQHandler
	.thumb_set TIM10_IRQHandler,Default_Handler

	.weak	TIM11_IRQHandler
	.thumb_set TIM11_IRQHandler,Default_Handler

	.weak	TIM12_IRQHandler
	.thumb_set TIM12_IRQHandler,Default_Handler

	.weak	TIM13_IRQHandler
	.thumb_set TIM13_IRQHandler,Default_Handler

	.weak	TIM14_IRQHandler
	.thumb_set TIM14_IRQHandler,Default_Handler

	.weak	TIM15_IRQHandler
	.thumb_set TIM15_IRQHandler,Default_Handler

	.weak	TIM16_IRQHandler
	.thumb_set TIM16_IRQHandler,Default_Handler

	.weak	TIM17_IRQHandler
	.thumb_set TIM17_IRQHandler,Default_Handler

	.weak	TIM18_IRQHandler
	.thumb_set TIM18_IRQHandler,Default_Handler

	.weak	LPTIM1_IRQHandler
	.thumb_set LPTIM1_IRQHandler,Default_Handler

	.weak	LPTIM2_IRQHandler
	.thumb_set LPTIM2_IRQHandler,Default_Handler

	.weak	LPTIM3_IRQHandler
	.thumb_set LPTIM3_IRQHandler,Default_Handler

	.weak	LPTIM4_IRQHandler
	.thumb_set LPTIM4_IRQHandler,Default_Handler

	.weak	LPTIM5_IRQHandler
	.thumb_set LPTIM5_IRQHandler,Default_Handler

	.weak	ADF1_FLT0_IRQHandler
	.thumb_set ADF1_FLT0_IRQHandler,Default_Handler

	.weak	MDF1_FLT0_IRQHandler
	.thumb_set MDF1_FLT0_IRQHandler,Default_Handler

	.weak	MDF1_FLT1_IRQHandler
	.thumb_set MDF1_FLT1_IRQHandler,Default_Handler

	.weak	MDF1_FLT2_IRQHandler
	.thumb_set MDF1_FLT2_IRQHandler,Default_Handler

	.weak	MDF1_FLT3_IRQHandler
	.thumb_set MDF1_FLT3_IRQHandler,Default_Handler

	.weak	MDF1_FLT4_IRQHandler
	.thumb_set MDF1_FLT4_IRQHandler,Default_Handler

	.weak	MDF1_FLT5_IRQHandler
	.thumb_set MDF1_FLT5_IRQHandler,Default_Handler

	.weak	SAI1_A_IRQHandler
	.thumb_set SAI1_A_IRQHandler,Default_Handler

	.weak	SAI1_B_IRQHandler
	.thumb_set SAI1_B_IRQHandler,Default_Handler

	.weak	SAI2_A_IRQHandler
	.thumb_set SAI2_A_IRQHandler,Default_Handler

	.weak	SAI2_B_IRQHandler
	.thumb_set SAI2_B_IRQHandler,Default_Handler

	.weak	SPDIFRX_IRQHandler
	.thumb_set SPDIFRX_IRQHandler,Default_Handler

	.weak	SPI1_IRQHandler
	.thumb_set SPI1_IRQHandler,Default_Handler

	.weak	SPI2_IRQHandler
	.thumb_set SPI2_IRQHandler,Default_Handler

	.weak	SPI3_IRQHandler
	.thumb_set SPI3_IRQHandler,Default_Handler

	.weak	SPI4_IRQHandler
	.thumb_set SPI4_IRQHandler,Default_Handler

	.weak	SPI5_IRQHandler
	.thumb_set SPI5_IRQHandler,Default_Handler

	.weak	SPI6_IRQHandler
	.thumb_set SPI6_IRQHandler,Default_Handler

	.weak	USART1_IRQHandler
	.thumb_set USART1_IRQHandler,Default_Handler

	.weak	USART2_IRQHandler
	.thumb_set USART2_IRQHandler,Default_Handler

	.weak	USART3_IRQHandler
	.thumb_set USART3_IRQHandler,Default_Handler

	.weak	UART4_IRQHandler
	.thumb_set UART4_IRQHandler,Default_Handler

	.weak	UART5_IRQHandler
	.thumb_set UART5_IRQHandler,Default_Handler

	.weak	USART6_IRQHandler
	.thumb_set USART6_IRQHandler,Default_Handler

	.weak	UART7_IRQHandler
	.thumb_set UART7_IRQHandler,Default_Handler

	.weak	UART8_IRQHandler
	.thumb_set UART8_IRQHandler,Default_Handler

	.weak	UART9_IRQHandler
	.thumb_set UART9_IRQHandler,Default_Handler

	.weak	USART10_IRQHandler
	.thumb_set USART10_IRQHandler,Default_Handler

	.weak	LPUART1_IRQHandler
	.thumb_set LPUART1_IRQHandler,Default_Handler

	.weak	XSPI1_IRQHandler
	.thumb_set XSPI1_IRQHandler,Default_Handler

	.weak	XSPI2_IRQHandler
	.thumb_set XSPI2_IRQHandler,Default_Handler

	.weak	XSPI3_IRQHandler
	.thumb_set XSPI3_IRQHandler,Default_Handler

	.weak	FMC_IRQHandler
	.thumb_set FMC_IRQHandler,Default_Handler

	.weak	SDMMC1_IRQHandler
	.thumb_set SDMMC1_IRQHandler,Default_Handler

	.weak	SDMMC2_IRQHandler
	.thumb_set SDMMC2_IRQHandler,Default_Handler

	.weak	UCPD1_IRQHandler
	.thumb_set UCPD1_IRQHandler,Default_Handler

	.weak	USB1_OTG_HS_IRQHandler
	.thumb_set USB1_OTG_HS_IRQHandler,Default_Handler

	.weak	USB2_OTG_HS_IRQHandler
	.thumb_set USB2_OTG_HS_IRQHandler,Default_Handler

	.weak	ETH1_IRQHandler
	.thumb_set ETH1_IRQHandler,Default_Handler

	.weak	FDCAN1_IT0_IRQHandler
	.thumb_set FDCAN1_IT0_IRQHandler,Default_Handler

	.weak	FDCAN1_IT1_IRQHandler
	.thumb_set FDCAN1_IT1_IRQHandler,Default_Handler

	.weak	FDCAN2_IT0_IRQHandler
	.thumb_set FDCAN2_IT0_IRQHandler,Default_Handler

	.weak	FDCAN2_IT1_IRQHandler
	.thumb_set FDCAN2_IT1_IRQHandler,Default_Handler

	.weak	FDCAN3_IT0_IRQHandler
	.thumb_set FDCAN3_IT0_IRQHandler,Default_Handler

	.weak	FDCAN3_IT1_IRQHandler
	.thumb_set FDCAN3_IT1_IRQHandler,Default_Handler

	.weak	FDCAN_CU_IRQHandler
	.thumb_set FDCAN_CU_IRQHandler,Default_Handler

	.weak	MDIOS_IRQHandler
	.thumb_set MDIOS_IRQHandler,Default_Handler

	.weak	DCMI_PSSI_IRQHandler
	.thumb_set DCMI_PSSI_IRQHandler,Default_Handler

	.weak	WAKEUP_PIN_IRQHandler
	.thumb_set WAKEUP_PIN_IRQHandler,Default_Handler

	.weak	CTI_INT0_IRQHandler
	.thumb_set CTI_INT0_IRQHandler,Default_Handler

	.weak	CTI_INT1_IRQHandler
	.thumb_set CTI_INT1_IRQHandler,Default_Handler

	.weak	LTDC_UP_IRQHandler
	.thumb_set LTDC_UP_IRQHandler,Default_Handler

	.weak	LTDC_UP_ERR_IRQHandler
	.thumb_set LTDC_UP_ERR_IRQHandler,Default_Handler

	.weak	SystemInit

/************************ (C) COPYRIGHT STMicroelectonics *****END OF FILE****/
