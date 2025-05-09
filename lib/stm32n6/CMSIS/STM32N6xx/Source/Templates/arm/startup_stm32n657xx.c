/******************************************************************************
 * @file     startup_stm32n657xx.c
 * @brief    CMSIS-Core Device Startup File for STM32N6xx Device
 ******************************************************************************/
/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stm32n6xx.h"

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void(*VECTOR_TABLE_Type)(void);

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;


extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
            void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler            (void) __attribute__ ((weak));
void MemManage_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));

void PVD_PVM_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DTS_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void LOCKUP_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void CACHE_ECC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void TCM_ECC_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void BKP_ECC_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_S_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TAMP_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void RIFSC_TAMPER_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void IAC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_S_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void IWDG_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void WWDG_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI5_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI6_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI7_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI8_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI10_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI11_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI12_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI13_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI14_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SAES_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void CRYP_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void PKA_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void HASH_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void RNG_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE1_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE2_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE3_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void MCE4_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void CSI_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void DCMIPP_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PAHB_ERR_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void NPU0_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void NPU1_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void NPU2_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void NPU3_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void CACHEAXI_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void LTDC_LO_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void LTDC_LO_ERR_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA2D_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void JPEG_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void VENC_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void GFXMMU_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GFXTIM_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void GPU2D_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void GPU2D_ER_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void ICACHE_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel0_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel1_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel2_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel3_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel4_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel5_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel6_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel7_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel8_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel9_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel10_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel11_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel12_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel13_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel14_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void HPDMA1_Channel15_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel0_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel1_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel2_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel3_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel4_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel5_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel6_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel7_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel8_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel9_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel10_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel11_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel12_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel13_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel14_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPDMA1_Channel15_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C4_EV_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C4_ER_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C1_EV_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C1_ER_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C2_EV_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void I3C2_ER_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM4_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM5_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM6_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM7_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_BRK_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_UP_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_TRG_COM_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM9_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM10_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM11_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM12_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM13_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM14_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM15_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM16_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM17_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM18_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM1_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM3_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM4_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIM5_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void ADF1_FLT0_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MDF1_FLT0_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MDF1_FLT1_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MDF1_FLT2_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MDF1_FLT3_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MDF1_FLT4_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MDF1_FLT5_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI1_A_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI1_B_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI2_A_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SAI2_B_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SPDIFRX1_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI4_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI5_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI6_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void USART6_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void UART7_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void UART8_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void UART9_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void USART10_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void LPUART1_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void XSPI1_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void XSPI2_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void XSPI3_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void FMC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void SDMMC1_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void SDMMC2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void UCPD1_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void USB1_OTG_HS_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void USB2_OTG_HS_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ETH1_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN1_IT0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN1_IT1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN2_IT0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN2_IT1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN3_IT0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN3_IT1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void FDCAN_CU_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void MDIOS_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));
void DCMI_PSSI_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void WAKEUP_PIN_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CTI_INT0_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void CTI_INT1_IRQHandler          (void) __attribute__ ((weak, alias("Default_Handler")));
void LTDC_UP_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void LTDC_UP_ERR_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));



/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[];
       const VECTOR_TABLE_Type __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */
  Reset_Handler,                            /*     Reset Handler */
  NMI_Handler,                              /* -14 NMI Handler */
  HardFault_Handler,                        /* -13 Hard Fault Handler */
  MemManage_Handler,                        /* -12 MPU Fault Handler */
  BusFault_Handler,                         /* -11 Bus Fault Handler */
  UsageFault_Handler,                       /* -10 Usage Fault Handler */
  SecureFault_Handler,                      /*  -9 Secure Fault Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  SVC_Handler,                              /*  -5 SVCall Handler */
  DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
  0,                                        /*     Reserved */
  PendSV_Handler,                           /*  -2 PendSV Handler */
  SysTick_Handler,                          /*  -1 SysTick Handler */

                                            /*     Interrupts */
  PVD_PVM_IRQHandler,                       /*     PVD/PVM through EXTI Line detection */
  0,                                        /*     Reserved */
  DTS_IRQHandler,                           /*     Digital Temperature Sensor */
  RCC_IRQHandler,                           /*     RCC global interrupts through EXTI Line detection */
  LOCKUP_IRQHandler,                        /*     Processor_SAFETY_interrupt due lost LOCKUP */
  CACHE_ECC_IRQHandler,                     /*     Error ECC cache */
  TCM_ECC_IRQHandler,                       /*     FError ECC TCM */
  BKP_ECC_IRQHandler,                       /*     Error ECC BKP */
  FPU_IRQHandler,                           /*     FPU */
  0,                                        /*     Reserved */
  RTC_S_IRQHandler,                         /*     RTC secure interrupts through the EXTI line */
  TAMP_IRQHandler,                          /*     TAMP secure and non-secure interrupts through the EXTI line */
  RIFSC_TAMPER_IRQHandler,                  /*     RIF Tamper interrupts */
  IAC_IRQHandler,                           /*     IAC interrupt */
  RCC_S_IRQHandler,                         /*     RCC secure global interrupts through EXTI Line detection */
  0,                                        /*     Reserved */
  RTC_IRQHandler,                           /*     RTC non-secure interrupts through the EXTI line */
  0,                                        /*     Reserved */
  IWDG_IRQHandler,                          /*     Internal Watchdog */
  WWDG_IRQHandler,                          /*     Window Watchdog */
  EXTI0_IRQHandler,                         /*     EXTI Line0 interrupt */
  EXTI1_IRQHandler,                         /*     EXTI Line1 interrupt */
  EXTI2_IRQHandler,                         /*     EXTI Line2 interrupt */
  EXTI3_IRQHandler,                         /*     EXTI Line3 interrupt */
  EXTI4_IRQHandler,                         /*     EXTI Line4 interrupt */
  EXTI5_IRQHandler,                         /*     EXTI Line5 interrupt */
  EXTI6_IRQHandler,                         /*     EXTI Line6 interrupt */
  EXTI7_IRQHandler,                         /*     EXTI Line7 interrupt */
  EXTI8_IRQHandler,                         /*     EXTI Line8 interrupt */
  EXTI9_IRQHandler,                         /*     EXTI Line9 iinterrupt */
  EXTI10_IRQHandler,                        /*     EXTI Line10 interrupt */
  EXTI11_IRQHandler,                        /*     EXTI Line11 interrupt */
  EXTI12_IRQHandler,                        /*     EXTI Line12 interrupt */
  EXTI13_IRQHandler,                        /*     EXTI Line13 interrupt */
  EXTI14_IRQHandler,                        /*     EXTI Line14 interrupt */
  EXTI15_IRQHandler,                        /*     EXTI Line15 interrupt */
  SAES_IRQHandler,                          /*     SAES */
  CRYP_IRQHandler,                          /*     CRYP */
  PKA_IRQHandler,                           /*     PKA */
  HASH_IRQHandler,                          /*     HASH */
  RNG_IRQHandler,                           /*     RNG */
  0,                                        /*     Reserved */      
  MCE1_IRQHandler,                          /*     MCE1 */
  MCE2_IRQHandler,                          /*     MCE2 */
  MCE3_IRQHandler,                          /*     MCE3 */
  MCE4_IRQHandler,                          /*     MCE4 */
  ADC1_2_IRQHandler,                        /*     MCE2 */
  CSI_IRQHandler,                           /*     MCE3 */
  DCMIPP_IRQHandler,                        /*     MCE4 */ 
  0,                                        /*     Reserved */  
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  PAHB_ERR_IRQHandler,                      /*     PAHB error interrupt */
  NPU0_IRQHandler,                          /*     NPU mst_ints[0] line interrupt */
  NPU1_IRQHandler,                          /*     NPU mst_ints[1] line interrupt */
  NPU2_IRQHandler,                          /*     NPU mst_ints[2] line interrupt*/
  NPU3_IRQHandler,                          /*     NPU mst_ints[3] line interrupt*/
  CACHEAXI_IRQHandler,                      /*     NPU CACHEAXI interrupt */
  LTDC_LO_IRQHandler,                       /*     LTDC low-layer interrupt */
  LTDC_LO_ERR_IRQHandler,                   /*     LTDC low-layer error interrupt */
  DMA2D_IRQHandler,                         /*     DMA2D */
  JPEG_IRQHandler,                          /*     JPEG*/
  VENC_IRQHandler,                          /*     VENC*/
  GFXMMU_IRQHandler,                        /*     GFXMMU */
  GFXTIM_IRQHandler,                        /*     GFXTIM */
  GPU2D_IRQHandler,                         /*     GPU2D */
  GPU2D_ER_IRQHandler,                      /*     GPU2D error */
  ICACHE_IRQHandler,                        /*     GPU2D cache */
  HPDMA1_Channel0_IRQHandler,               /*     HPDMA1 Channel 0 */
  HPDMA1_Channel1_IRQHandler,               /*     HPDMA1 Channel 1 */
  HPDMA1_Channel2_IRQHandler,               /*     HPDMA1 Channel 2 */
  HPDMA1_Channel3_IRQHandler,               /*     HPDMA1 Channel 3 */
  HPDMA1_Channel4_IRQHandler,               /*     HPDMA1 Channel 4 */
  HPDMA1_Channel5_IRQHandler,               /*     HPDMA1 Channel 5 */
  HPDMA1_Channel6_IRQHandler,               /*     HPDMA1 Channel 6 */
  HPDMA1_Channel7_IRQHandler,               /*     HPDMA1 Channel 7*/
  HPDMA1_Channel8_IRQHandler,               /*     HPDMA1 Channel 8 */
  HPDMA1_Channel9_IRQHandler,               /*     HPDMA1 Channel 9 */
  HPDMA1_Channel10_IRQHandler,              /*     HPDMA1 Channel 10 */
  HPDMA1_Channel11_IRQHandler,              /*     HPDMA1 Channel 11 */
  HPDMA1_Channel12_IRQHandler,              /*     HPDMA1 Channel 12 */
  HPDMA1_Channel13_IRQHandler,              /*     HPDMA1 Channel 13 */
  HPDMA1_Channel14_IRQHandler,              /*     HPDMA1 Channel 14 */
  HPDMA1_Channel15_IRQHandler,              /*     HPDMA1 Channel 15 */  
  GPDMA1_Channel0_IRQHandler,               /*     GPDMA1 Channel 0 */
  GPDMA1_Channel1_IRQHandler,               /*     GPDMA1 Channel 1 */
  GPDMA1_Channel2_IRQHandler,               /*     GPDMA1 Channel 2 */
  GPDMA1_Channel3_IRQHandler,               /*     GPDMA1 Channel 3 */
  GPDMA1_Channel4_IRQHandler,               /*     GPDMA1 Channel 4 */
  GPDMA1_Channel5_IRQHandler,               /*     GPDMA1 Channel 5 */
  GPDMA1_Channel6_IRQHandler,               /*     GPDMA1 Channel 6 */
  GPDMA1_Channel7_IRQHandler,               /*     GPDMA1 Channel 7*/
  GPDMA1_Channel8_IRQHandler,               /*     GPDMA1 Channel 8 */
  GPDMA1_Channel9_IRQHandler,               /*     GPDMA1 Channel 9 */
  GPDMA1_Channel10_IRQHandler,              /*     GPDMA1 Channel 10 */
  GPDMA1_Channel11_IRQHandler,              /*     GPDMA1 Channel 11 */
  GPDMA1_Channel12_IRQHandler,              /*     GPDMA1 Channel 12 */
  GPDMA1_Channel13_IRQHandler,              /*     GPDMA1 Channel 13 */
  GPDMA1_Channel14_IRQHandler,              /*     GPDMA1 Channel 14 */
  GPDMA1_Channel15_IRQHandler,              /*     GPDMA1 Channel 15 */                               
  I2C1_EV_IRQHandler,                       /*     I2C1 Event */
  I2C1_ER_IRQHandler,                       /*     I2C1 Error */
  I2C2_EV_IRQHandler,                       /*     I2C2 Event */
  I2C2_ER_IRQHandler,                       /*     I2C2 Error */
  I2C3_EV_IRQHandler,                       /*     I2C3 Event */
  I2C3_ER_IRQHandler,                       /*     I2C3 Error */
  I2C4_EV_IRQHandler,                       /*     I2C4 Event*/
  I2C4_ER_IRQHandler,                       /*     I2C4 Error */
  I3C1_EV_IRQHandler,                       /*     I3C1 Event */
  I3C1_ER_IRQHandler,                       /*     I3C1 Error */
  I3C2_EV_IRQHandler,                       /*     I3C2 Event */
  I3C2_ER_IRQHandler,                       /*     I3C2 Error */
  TIM1_BRK_IRQHandler,                      /*     TIM1 Break */
  TIM1_UP_IRQHandler,                       /*     TIM1 Update */
  TIM1_TRG_COM_IRQHandler,                  /*     TIM1 Trigger and Commutation */
  TIM1_CC_IRQHandler,                       /*     TIM1 Capture Compare*/
  TIM2_IRQHandler,                          /*     TIM2 */
  TIM3_IRQHandler,                          /*     TIM3 */
  TIM4_IRQHandler,                          /*     TIM4 */
  TIM5_IRQHandler,                          /*     TIM5 */
  TIM6_IRQHandler,                          /*     TIM6 */
  TIM7_IRQHandler,                          /*     TIM7 */
  TIM8_BRK_IRQHandler,                      /*     TIM8 Break */
  TIM8_UP_IRQHandler,                       /*     TIM8 Update */
  TIM8_TRG_COM_IRQHandler,                  /*     TIM8 Trigger and Commutation */
  TIM8_CC_IRQHandler,                       /*     TIM8 Capture Compare */
  TIM9_IRQHandler,                          /*     TIM9 */
  TIM10_IRQHandler,                         /*     TIM10 */
  TIM11_IRQHandler,                         /*     TIM11 */
  TIM12_IRQHandler,                         /*     TIM12 */
  TIM13_IRQHandler,                         /*     TIM13 */
  TIM14_IRQHandler,                         /*     TIM14 */
  TIM15_IRQHandler,                         /*     TIM15 */
  TIM16_IRQHandler,                         /*     TIM16 */
  TIM17_IRQHandler,                         /*     TIM17 */
  TIM18_IRQHandler,                         /*     TIM18 */
  LPTIM1_IRQHandler,                        /*     LP TIM1 */
  LPTIM2_IRQHandler,                        /*     LP TIM2 */
  LPTIM3_IRQHandler,                        /*     LP TIM3 */
  LPTIM4_IRQHandler,                        /*     LP TIM4 */
  LPTIM5_IRQHandler,                        /*     LP TIM5 */
  ADF1_FLT0_IRQHandler,                     /*     ADF1 Filter 0  interrupt */
  MDF1_FLT0_IRQHandler,                     /*     MDF1 Filter 0 global interrupt */
  MDF1_FLT1_IRQHandler,                     /*     MDF1 Filter 1 global interrupt */
  MDF1_FLT2_IRQHandler,                     /*     MDF1 Filter 2 global interrupt */
  MDF1_FLT3_IRQHandler,                     /*     MDF1 Filter 3 global interrupt */
  MDF1_FLT4_IRQHandler,                     /*     MDF1 Filter 4 global interrupt */
  MDF1_FLT5_IRQHandler,                     /*     MDF1 Filter 5 global interrupt */
  SAI1_A_IRQHandler,                        /*     Serial Audio Interface 1 block A interrupt */
  SAI1_B_IRQHandler,                        /*     Serial Audio Interface 1 block B interrupt */
  SAI2_A_IRQHandler,                        /*     Serial Audio Interface 2 block A interrupt */
  SAI2_B_IRQHandler,                        /*     Serial Audio Interface 2 block B interrupt */
  SPDIFRX1_IRQHandler,                      /*     SPDIFRX1 interrupt */
  SPI1_IRQHandler,                          /*     SPI1 */
  SPI2_IRQHandler,                          /*     SPI2 */
  SPI3_IRQHandler,                          /*     SPI3 */
  SPI4_IRQHandler,                          /*     SPI4 */
  SPI5_IRQHandler,                          /*     SPI5 */
  SPI6_IRQHandler,                          /*     SPI6 */
  USART1_IRQHandler,                        /*     USART1 */
  USART2_IRQHandler,                        /*     USART2 */
  USART3_IRQHandler,                        /*     USART3 */
  UART4_IRQHandler,                         /*     UART4 */
  UART5_IRQHandler,                         /*     UART5 */
  USART6_IRQHandler,                        /*     USART6 */
  UART7_IRQHandler,                         /*     UART7 */
  UART8_IRQHandler,                         /*     UART8 */
  UART9_IRQHandler,                         /*     UART9 */
  USART10_IRQHandler,                       /*     USART10 */
  LPUART1_IRQHandler,                       /*     LP UART1 */
  XSPI1_IRQHandler,                         /*     XSPI1 global interrupt */
  XSPI2_IRQHandler,                         /*     XSPI2 global interrupt */
  XSPI3_IRQHandler,                         /*     XSPI3 global interrupt */
  FMC_IRQHandler,                           /*     FMC */
  SDMMC1_IRQHandler,                        /*     SDMMC1 */
  SDMMC2_IRQHandler,                        /*     SDMMC2 */
  UCPD1_IRQHandler,                         /*     UCPD1 */
  USB1_OTG_HS_IRQHandler,                   /*     USB1 OTG HS */
  USB2_OTG_HS_IRQHandler,                   /*     USB2 OTG HS */
  ETH1_IRQHandler,                          /*     Ethernet */
  FDCAN1_IT0_IRQHandler,                    /*     FDCAN1 Interrupt 0 */
  FDCAN1_IT1_IRQHandler,                    /*     FDCAN1 Interrupt 1 */
  FDCAN2_IT0_IRQHandler,                    /*     FDCAN2 Interrupt 0 */
  FDCAN2_IT1_IRQHandler,                    /*     FDCAN2 Interrupt 1 */
  FDCAN3_IT0_IRQHandler,                    /*     FDCAN3 Interrupt 0 */
  FDCAN3_IT1_IRQHandler,                    /*     FDCAN3 Interrupt 1 */
  FDCAN_CU_IRQHandler,                      /*     FDCAN clock unit interrupt */
  MDIOS_IRQHandler,                         /*     MDIOS */
  DCMI_PSSI_IRQHandler,                     /*     DCMI_PSSI */
  WAKEUP_PIN_IRQHandler,                    /*     Wake-up pins interrupt */
  CTI_INT0_IRQHandler,                      /*     CTI INT0 interrupt */
  CTI_INT1_IRQHandler,                      /*     CTI INT1 interrupt */
  0,                                        /*     Reserved */
  LTDC_UP_IRQHandler,                       /*     LTDC up-layer interrupt */
  LTDC_UP_ERR_IRQHandler                    /*     LTDC up-layer error interrupt */                                     

};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
	__disable_irq();
  __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
  __set_MSP((uint32_t)(&__INITIAL_SP));
  __enable_irq();
		 
  SystemInit();                            /* CMSIS System Initialization */
  __PROGRAM_START();                       /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif

