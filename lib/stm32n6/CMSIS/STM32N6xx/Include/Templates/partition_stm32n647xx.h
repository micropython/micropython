/**
  ******************************************************************************
  * @file    partition_stm32n647xx.h
  * @author  MCD Application Team
  * @brief   CMSIS STM32N647xx Device Initial Setup for Secure / Non-Secure Zones
  *          for ARMCM55 based on CMSIS CORE V5.3.1 partition_ARMCM33.h Template.
  *
  *          This file contains:
  *           - Initialize Security Attribution Unit (SAU) CTRL register
  *           - Setup behavior of Sleep and Exception Handling
  *           - Setup behavior of Floating Point Unit
  *           - Setup Interrupt Target
  *
  ******************************************************************************/
/*
 * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
 * Portions Copyright (c) 2023 STMicroelectronics, all rights reserved
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PARTITION_STM32N647XX_H
#define PARTITION_STM32N647XX_H

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/

/*
// <e>Initialize Security Attribution Unit (SAU) CTRL register
*/
#define SAU_INIT_CTRL          0

/*
//   <q> Enable SAU
//   <i> Value for SAU->CTRL register bit ENABLE
*/
#define SAU_INIT_CTRL_ENABLE   0

/*
//   <o> When SAU is disabled
//     <0=> All Memory is Secure
//     <1=> All Memory is Non-Secure
//   <i> Value for SAU->CTRL register bit ALLNS
//   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
*/
#define SAU_INIT_CTRL_ALLNS  0

/*
// </e>
*/

/*
// <h>Initialize Security Attribution Unit (SAU) Address Regions
// <i>SAU configuration specifies regions to be one of:
// <i> - Secure and Non-Secure Callable
// <i> - Non-Secure
// <i>Note: All memory regions not configured by SAU are Secure
*/
#define SAU_REGIONS_MAX   8                 /* Max. number of SAU regions */

/*
//   <e>Initialize SAU Region 0
//   <i> Setup SAU Region 0 memory attributes
*/
#define SAU_INIT_REGION0    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START0     0x00000000      /* start address of SAU region 0 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END0       0x00000000      /* end address of SAU region 0 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC0       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 1
//   <i> Setup SAU Region 1 memory attributes
*/
#define SAU_INIT_REGION1    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START1     0x00000000      /* start address of SAU region 1 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END1       0x00000000      /* end address of SAU region 1 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC1       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 2
//   <i> Setup SAU Region 2 memory attributes
*/
#define SAU_INIT_REGION2    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START2     0x00000000      /* start address of SAU region 2 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END2       0x00000000      /* end address of SAU region 2 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC2       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 3
//   <i> Setup SAU Region 3 memory attributes
*/
#define SAU_INIT_REGION3    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START3     0x00000000      /* start address of SAU region 3 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END3       0x00000000      /* end address of SAU region 3 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC3       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 4
//   <i> Setup SAU Region 4 memory attributes
*/
#define SAU_INIT_REGION4    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START4     0x00000000      /* start address of SAU region 4 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END4       0x00000000      /* end address of SAU region 4 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC4       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 5
//   <i> Setup SAU Region 5 memory attributes
*/
#define SAU_INIT_REGION5    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START5     0x00000000      /* start address of SAU region 5 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END5       0x00000000      /* end address of SAU region 5 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC5       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 6
//   <i> Setup SAU Region 6 memory attributes
*/
#define SAU_INIT_REGION6    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START6     0x00000000      /* start address of SAU region 6 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END6       0x00000000      /* end address of SAU region 6 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC6       0
/*
//   </e>
*/

/*
//   <e>Initialize SAU Region 7
//   <i> Setup SAU Region 7 memory attributes
*/
#define SAU_INIT_REGION7    0

/*
//     <o>Start Address <0-0xFFFFFFE0>
*/
#define SAU_INIT_START7     0x00000000      /* start address of SAU region 7 */

/*
//     <o>End Address <0x1F-0xFFFFFFFF>
*/
#define SAU_INIT_END7       0x00000000      /* end address of SAU region 7 */

/*
//     <o>Region is
//         <0=>Non-Secure
//         <1=>Secure, Non-Secure Callable
*/
#define SAU_INIT_NSC7       0
/*
//   </e>
*/

/*
// </h>
*/

/*
// <e>Setup behaviour of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT  0

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Value for SCB->CSR register bit DEEPSLEEPS
*/
#define SCB_CSR_DEEPSLEEPS_VAL  0

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for SCB->AIRCR register bit SYSRESETREQS
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  0

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x04-0x07
//   <i> Value for SCB->AIRCR register bit PRIS
*/
#define SCB_AIRCR_PRIS_VAL      0

/*
//   <o>BusFault, HardFault, and NMI target
//     <0=> Secure state
//     <1=> Non-Secure state
//   <i> Value for SCB->AIRCR register bit BFHFNMINS
*/
#define SCB_AIRCR_BFHFNMINS_VAL 0

/*
// </e>
*/

/*
// <e>Setup behaviour of Floating Point Unit
*/
#define TZ_FPU_NS_USAGE 1

/*
// <o>Floating Point Unit usage
//     <0=> Secure state only
//     <3=> Secure and Non-Secure state
//   <i> Value for SCB->NSACR register bits CP10, CP11
*/
#define SCB_NSACR_CP10_11_VAL       3

/*
// <o>Treat floating-point registers as Secure
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit TS
*/
#define FPU_FPCCR_TS_VAL            0

/*
// <o>Clear on return (CLRONRET) accessibility
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for FPU->FPCCR register bit CLRONRETS
*/
#define FPU_FPCCR_CLRONRETS_VAL     0

/*
// <o>Clear floating-point caller saved registers on exception return
//     <0=> Disabled
//     <1=> Enabled
//   <i> Value for FPU->FPCCR register bit CLRONRET
*/
#define FPU_FPCCR_CLRONRET_VAL      1

/*
// </e>
*/

/*
// <h>Setup Interrupt Target
*/

/*
//   <e>Initialize ITNS 0 (Interrupts 0..31)
*/
#define NVIC_INIT_ITNS0    1

/*
// Interrupts 0..31
//   <o.0>  PVD_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.1>  Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.2>  DTS_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.3>  RCC_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.4>  LOCKUP_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.5>  CACHE_ECC_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.6>  TCM_ECC_IRQn        <0=> Secure state <1=> Non-Secure state
//   <o.7>  BKP_ECC_IRQn        <0=> Secure state <1=> Non-Secure state
//   <o.8>  FPU_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.9>  Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.10> RTC_S_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.11> TAMP_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.12> RIFSC_TAMPER_IRQn   <0=> Secure state <1=> Non-Secure state
//   <o.13> IAC_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.14> RCC_S_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.15> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.16> RTC_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.17> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.18> IWDG_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.19> WWDG_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.20> EXTI0_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.21> EXTI1_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.22> EXTI2_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.23> EXTI3_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.24> EXTI4_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.25> EXTI5_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.26> EXTI6_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.27> EXTI7_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.28> EXTI8_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.29> EXTI9_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.30> EXTI10_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.31> EXTI11_IRQn         <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS0_VAL     0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 1 (Interrupts 32..63)
*/
#define NVIC_INIT_ITNS1    1

/*
// Interrupts 32..63
//   <o.0>  EXTI12_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.1>  EXTI13_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.2>  EXTI14_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.3>  EXTI15_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.4>  Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.5>  Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.6>  PKA_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.7>  HASH_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.8>  RNG_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.9>  Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.10> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.11> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.12> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.13> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.14> ADC1_2_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.15> CSI_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.16> DCMIPP_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.17> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.18> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.19> Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.20> PAHB_ERR_IRQn       <0=> Secure state <1=> Non-Secure state
//   <o.21> NPU0_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.22> NPU1_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.23> NPU2_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.24> NPU3_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.25> CACHEAXI_IRQn       <0=> Secure state <1=> Non-Secure state
//   <o.26> LTDC_LO_IRQn        <0=> Secure state <1=> Non-Secure state
//   <o.27> LTDC_LO_ERR_IRQn    <0=> Secure state <1=> Non-Secure state
//   <o.28> DMA2D_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.29> JPEG_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.30> VENC_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.31> GFXMMU_IRQn         <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS1_VAL     0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 2 (Interrupts 64..95)
*/
#define NVIC_INIT_ITNS2    1

/*
// Interrupts 64..95
//   <o.0>  GFXTIM_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.1>  GPU2D_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.2>  GPU2D_ER_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.3>  ICACHE_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.4>  HPDMA1_Channel0_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.5>  HPDMA1_Channel1_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.6>  HPDMA1_Channel2_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.7>  HPDMA1_Channel3_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.8>  HPDMA1_Channel4_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.9>  HPDMA1_Channel5_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.10> HPDMA1_Channel6_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.11> HPDMA1_Channel7_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.12> HPDMA1_Channel8_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.13> HPDMA1_Channel9_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.14> HPDMA1_Channel10_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.15> HPDMA1_Channel11_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.16> HPDMA1_Channel12_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.17> HPDMA1_Channel13_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.18> HPDMA1_Channel14_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.19> HPDMA1_Channel15_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.20> GPDMA1_Channel0_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.21> GPDMA1_Channel1_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.22> GPDMA1_Channel2_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.23> GPDMA1_Channel3_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.24> GPDMA1_Channel4_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.25> GPDMA1_Channel5_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.26> GPDMA1_Channel6_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.27> GPDMA1_Channel7_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.28> GPDMA1_Channel8_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.29> GPDMA1_Channel9_IRQn  <0=> Secure state <1=> Non-Secure state
//   <o.30> GPDMA1_Channel10_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.31> GPDMA1_Channel11_IRQn <0=> Secure state <1=> Non-Secure state
*/
#define NVIC_INIT_ITNS2_VAL     0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 3 (Interrupts 96..127)
*/
#define NVIC_INIT_ITNS3    1

/*
// Interrupts 96..127
//   <o.0>  GPDMA1_Channel12_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.1>  GPDMA1_Channel13_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.2>  GPDMA1_Channel14_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.3>  GPDMA1_Channel15_IRQn <0=> Secure state <1=> Non-Secure state
//   <o.4>  I2C1_EV_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.5>  I2C1_ER_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.6>  I2C2_EV_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.7>  I2C2_ER_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.8>  I2C3_EV_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.9>  I2C3_ER_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.10> I2C4_EV_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.11> I2C4_ER_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.12> I3C1_EV_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.13> I3C1_ER_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.14> I3C2_EV_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.15> I3C2_ER_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.16> TIM1_BRK_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.17> TIM1_UP_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.18> TIM1_TRG_COM_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.19> TIM1_CC_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.20> TIM2_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.21> TIM3_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.22> TIM4_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.23> TIM5_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.24> TIM6_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.25> TIM7_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.26> TIM8_BRK_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.27> TIM8_UP_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.28> TIM8_TRG_COM_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.29> TIM8_CC_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.30> TIM9_IRQn             <0=> Secure state <1=> Non-Secure state
//   <o.31> TIM10_IRQn            <0=> Secure state <1=> Non-Secure state

*/
#define NVIC_INIT_ITNS3_VAL     0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 4 (Interrupts 128..159)
*/
#define NVIC_INIT_ITNS4    1

/*
// Interrupts 128..159
//   <o.0>  TIM11_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.1>  TIM12_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.2>  TIM13_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.3>  TIM14_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.4>  TIM15_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.5>  TIM16_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.6>  TIM17_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.7>  TIM18_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.8>  LPTIM1_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.9>  LPTIM2_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.10> LPTIM3_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.11> LPTIM4_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.12> LPTIM5_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.13> ADF1_FLT0_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.14> MDF1_FLT0_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.15> MDF1_FLT1_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.16> MDF1_FLT2_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.17> MDF1_FLT3_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.18> MDF1_FLT4_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.19> MDF1_FLT5_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.20> SAI1_A_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.21> SAI1_B_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.22> SAI2_A_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.23> SAI2_B_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.24> SPDIFRX1_IRQn       <0=> Secure state <1=> Non-Secure state
//   <o.25> SPI1_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.26> SPI2_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.27> SPI3_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.28> SPI4_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.29> SPI5_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.30> SPI6_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.31> USART1_IRQn         <0=> Secure state <1=> Non-Secure state

*/
#define NVIC_INIT_ITNS4_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 5 (Interrupts 160..191)
*/
#define NVIC_INIT_ITNS5    1

/*
// Interrupts 160..191
//   <o.0>  USART2_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.1>  USART3_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.2>  UART4_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.3>  UART5_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.4>  USART6_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.5>  UART7_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.6>  UART8_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.7>  UART9_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.8>  USART10_IRQn        <0=> Secure state <1=> Non-Secure state
//   <o.9>  LPUART1_IRQn        <0=> Secure state <1=> Non-Secure state
//   <o.10> XSPI1_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.11> XSPI2_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.12> XSPI3_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.13> FMC_IRQn            <0=> Secure state <1=> Non-Secure state
//   <o.14> SDMMC1_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.15> SDMMC2_IRQn         <0=> Secure state <1=> Non-Secure state
//   <o.16> UCPD1_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.17> USB1_OTG_HS_IRQn    <0=> Secure state <1=> Non-Secure state
//   <o.18> USB2_OTG_HS_IRQn    <0=> Secure state <1=> Non-Secure state
//   <o.19> ETH1_IRQn           <0=> Secure state <1=> Non-Secure state
//   <o.20> FDCAN1_IT0_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.21> FDCAN1_IT1_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.22> FDCAN2_IT0_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.23> FDCAN2_IT1_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.24> FDCAN3_IT0_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.25> FDCAN3_IT1_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.26> FDCAN_CU_IRQn       <0=> Secure state <1=> Non-Secure state
//   <o.27> MDIOS_IRQn          <0=> Secure state <1=> Non-Secure state
//   <o.28> DCMI_PSSI_IRQn      <0=> Secure state <1=> Non-Secure state
//   <o.29> WAKEUP_PIN_IRQn     <0=> Secure state <1=> Non-Secure state
//   <o.30> CTI_INT0_IRQn       <0=> Secure state <1=> Non-Secure state
//   <o.31> CTI_INT1_IRQn       <0=> Secure state <1=> Non-Secure state

*/
#define NVIC_INIT_ITNS5_VAL      0x00000000

/*
//   </e>
*/

/*
//   <e>Initialize ITNS 6 (Interrupts 192..223)
*/
#define NVIC_INIT_ITNS6    1

/*
// Interrupts 192..223
//   <o.0>  Reserved            <0=> Secure state <1=> Non-Secure state
//   <o.1>  LTDC_UP_IRQn        <0=> Secure state <1=> Non-Secure state
//   <o.2>  LTDC_UP_ERR_IRQn    <0=> Secure state <1=> Non-Secure state

*/
#define NVIC_INIT_ITNS6_VAL      0x00000000

/*
//   </e>
*/

/*
// </h>
*/



/*
    max 8 SAU regions.
    SAU regions are defined in partition.h
 */

#define SAU_INIT_REGION(n) \
    SAU->RNR  =  (n                                     & SAU_RNR_REGION_Msk); \
    SAU->RBAR =  (SAU_INIT_START##n                     & SAU_RBAR_BADDR_Msk); \
    SAU->RLAR =  (SAU_INIT_END##n                       & SAU_RLAR_LADDR_Msk) | \
                ((SAU_INIT_NSC##n << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | 1U

/**
  \brief   Setup a SAU Region
  \details Writes the region information contained in SAU_Region to the
           registers SAU_RNR, SAU_RBAR, and SAU_RLAR
 */
__STATIC_INLINE void TZ_SAU_Setup (void)
{

#if defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U)

  #if defined (SAU_INIT_REGION0) && (SAU_INIT_REGION0 == 1U)
    SAU_INIT_REGION(0);
  #endif

  #if defined (SAU_INIT_REGION1) && (SAU_INIT_REGION1 == 1U)
    SAU_INIT_REGION(1);
  #endif

  #if defined (SAU_INIT_REGION2) && (SAU_INIT_REGION2 == 1U)
    SAU_INIT_REGION(2);
  #endif

  #if defined (SAU_INIT_REGION3) && (SAU_INIT_REGION3 == 1U)
    SAU_INIT_REGION(3);
  #endif

  #if defined (SAU_INIT_REGION4) && (SAU_INIT_REGION4 == 1U)
    SAU_INIT_REGION(4);
  #endif

  #if defined (SAU_INIT_REGION5) && (SAU_INIT_REGION5 == 1U)
    SAU_INIT_REGION(5);
  #endif

  #if defined (SAU_INIT_REGION6) && (SAU_INIT_REGION6 == 1U)
    SAU_INIT_REGION(6);
  #endif

  #if defined (SAU_INIT_REGION7) && (SAU_INIT_REGION7 == 1U)
    SAU_INIT_REGION(7);
  #endif

  /* repeat this for all possible SAU regions */

#endif /* defined (__SAUREGION_PRESENT) && (__SAUREGION_PRESENT == 1U) */


  #if defined (SAU_INIT_CTRL) && (SAU_INIT_CTRL == 1U)
    SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
                ((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;
  #endif

  #if defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U)
    SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk    )) |
                   ((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

    SCB->AIRCR = (SCB->AIRCR & ~(SCB_AIRCR_VECTKEY_Msk   | SCB_AIRCR_SYSRESETREQS_Msk |
                                 SCB_AIRCR_BFHFNMINS_Msk | SCB_AIRCR_PRIS_Msk)        )                     |
                   ((0x05FAU                    << SCB_AIRCR_VECTKEY_Pos)      & SCB_AIRCR_VECTKEY_Msk)      |
                   ((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) |
                   ((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk)         |
                   ((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk);
  #endif /* defined (SCB_CSR_AIRCR_INIT) && (SCB_CSR_AIRCR_INIT == 1U) */

  #if defined (__FPU_USED) && (__FPU_USED == 1U) && \
      defined (TZ_FPU_NS_USAGE) && (TZ_FPU_NS_USAGE == 1U)

    SCB->NSACR = (SCB->NSACR & ~(SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk)) |
                   ((SCB_NSACR_CP10_11_VAL << SCB_NSACR_CP10_Pos) & (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk));

    FPU->FPCCR = (FPU->FPCCR & ~(FPU_FPCCR_TS_Msk | FPU_FPCCR_CLRONRETS_Msk | FPU_FPCCR_CLRONRET_Msk)) |
                   ((FPU_FPCCR_TS_VAL        << FPU_FPCCR_TS_Pos       ) & FPU_FPCCR_TS_Msk       ) |
                   ((FPU_FPCCR_CLRONRETS_VAL << FPU_FPCCR_CLRONRETS_Pos) & FPU_FPCCR_CLRONRETS_Msk) |
                   ((FPU_FPCCR_CLRONRET_VAL  << FPU_FPCCR_CLRONRET_Pos ) & FPU_FPCCR_CLRONRET_Msk );
  #endif

  #if defined (NVIC_INIT_ITNS0) && (NVIC_INIT_ITNS0 == 1U)
    NVIC->ITNS[0] = NVIC_INIT_ITNS0_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS1) && (NVIC_INIT_ITNS1 == 1U)
    NVIC->ITNS[1] = NVIC_INIT_ITNS1_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS2) && (NVIC_INIT_ITNS2 == 1U)
    NVIC->ITNS[2] = NVIC_INIT_ITNS2_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS3) && (NVIC_INIT_ITNS3 == 1U)
    NVIC->ITNS[3] = NVIC_INIT_ITNS3_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS4) && (NVIC_INIT_ITNS4 == 1U)
    NVIC->ITNS[4] = NVIC_INIT_ITNS4_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS5) && (NVIC_INIT_ITNS5 == 1U)
    NVIC->ITNS[5] = NVIC_INIT_ITNS5_VAL;
  #endif

  #if defined (NVIC_INIT_ITNS6) && (NVIC_INIT_ITNS6 == 1U)
    NVIC->ITNS[6] = NVIC_INIT_ITNS6_VAL;
  #endif    

}

#endif  /* PARTITION_STM32N647XX_H */
