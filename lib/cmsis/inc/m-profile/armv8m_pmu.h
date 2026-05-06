/*
 * Copyright (c) 2020 Arm Limited. All rights reserved.
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

/*
 * CMSIS-Core(M) PMU API for Armv8.1-M PMU
 */

#ifndef ARM_PMU_ARMV8_H
#define ARM_PMU_ARMV8_H

#if   defined ( __ICCARM__ )
  #pragma system_include         /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header    /* treat file as system include file */
#endif

/**
 * \brief PMU Events
 * \note  See the Armv8.1-M Architecture Reference Manual for full details on these PMU events.
 * */

#define ARM_PMU_SW_INCR                              0x0000             /*!< Software update to the PMU_SWINC register, architecturally executed and condition code check pass */
#define ARM_PMU_L1I_CACHE_REFILL                     0x0001             /*!< L1 I-Cache refill */
#define ARM_PMU_L1D_CACHE_REFILL                     0x0003             /*!< L1 D-Cache refill */
#define ARM_PMU_L1D_CACHE                            0x0004             /*!< L1 D-Cache access */
#define ARM_PMU_LD_RETIRED                           0x0006             /*!< Memory-reading instruction architecturally executed and condition code check pass */
#define ARM_PMU_ST_RETIRED                           0x0007             /*!< Memory-writing instruction architecturally executed and condition code check pass */
#define ARM_PMU_INST_RETIRED                         0x0008             /*!< Instruction architecturally executed */
#define ARM_PMU_EXC_TAKEN                            0x0009             /*!< Exception entry */
#define ARM_PMU_EXC_RETURN                           0x000A             /*!< Exception return instruction architecturally executed and the condition code check pass */
#define ARM_PMU_PC_WRITE_RETIRED                     0x000C             /*!< Software change to the Program Counter (PC). Instruction is architecturally executed and condition code check pass */
#define ARM_PMU_BR_IMMED_RETIRED                     0x000D             /*!< Immediate branch architecturally executed */
#define ARM_PMU_BR_RETURN_RETIRED                    0x000E             /*!< Function return instruction architecturally executed and the condition code check pass */
#define ARM_PMU_UNALIGNED_LDST_RETIRED               0x000F             /*!< Unaligned memory memory-reading or memory-writing instruction architecturally executed and condition code check pass */
#define ARM_PMU_BR_MIS_PRED                          0x0010             /*!< Mispredicted or not predicted branch speculatively executed */
#define ARM_PMU_CPU_CYCLES                           0x0011             /*!< Cycle */
#define ARM_PMU_BR_PRED                              0x0012             /*!< Predictable branch speculatively executed */
#define ARM_PMU_MEM_ACCESS                           0x0013             /*!< Data memory access */
#define ARM_PMU_L1I_CACHE                            0x0014             /*!< Level 1 instruction cache access */
#define ARM_PMU_L1D_CACHE_WB                         0x0015             /*!< Level 1 data cache write-back */
#define ARM_PMU_L2D_CACHE                            0x0016             /*!< Level 2 data cache access */
#define ARM_PMU_L2D_CACHE_REFILL                     0x0017             /*!< Level 2 data cache refill */
#define ARM_PMU_L2D_CACHE_WB                         0x0018             /*!< Level 2 data cache write-back */
#define ARM_PMU_BUS_ACCESS                           0x0019             /*!< Bus access */
#define ARM_PMU_MEMORY_ERROR                         0x001A             /*!< Local memory error */
#define ARM_PMU_INST_SPEC                            0x001B             /*!< Instruction speculatively executed */
#define ARM_PMU_BUS_CYCLES                           0x001D             /*!< Bus cycles */
#define ARM_PMU_CHAIN                                0x001E             /*!< For an odd numbered counter, increment when an overflow occurs on the preceding even-numbered counter on the same PE */
#define ARM_PMU_L1D_CACHE_ALLOCATE                   0x001F             /*!< Level 1 data cache allocation without refill */
#define ARM_PMU_L2D_CACHE_ALLOCATE                   0x0020             /*!< Level 2 data cache allocation without refill */
#define ARM_PMU_BR_RETIRED                           0x0021             /*!< Branch instruction architecturally executed */
#define ARM_PMU_BR_MIS_PRED_RETIRED                  0x0022             /*!< Mispredicted branch instruction architecturally executed */
#define ARM_PMU_STALL_FRONTEND                       0x0023             /*!< No operation issued because of the frontend */
#define ARM_PMU_STALL_BACKEND                        0x0024             /*!< No operation issued because of the backend */
#define ARM_PMU_L2I_CACHE                            0x0027             /*!< Level 2 instruction cache access */
#define ARM_PMU_L2I_CACHE_REFILL                     0x0028             /*!< Level 2 instruction cache refill */
#define ARM_PMU_L3D_CACHE_ALLOCATE                   0x0029             /*!< Level 3 data cache allocation without refill */
#define ARM_PMU_L3D_CACHE_REFILL                     0x002A             /*!< Level 3 data cache refill */
#define ARM_PMU_L3D_CACHE                            0x002B             /*!< Level 3 data cache access */
#define ARM_PMU_L3D_CACHE_WB                         0x002C             /*!< Level 3 data cache write-back */
#define ARM_PMU_LL_CACHE_RD                          0x0036             /*!< Last level data cache read */
#define ARM_PMU_LL_CACHE_MISS_RD                     0x0037             /*!< Last level data cache read miss */
#define ARM_PMU_L1D_CACHE_MISS_RD                    0x0039             /*!< Level 1 data cache read miss */
#define ARM_PMU_OP_COMPLETE                          0x003A             /*!< Operation retired */
#define ARM_PMU_OP_SPEC                              0x003B             /*!< Operation speculatively executed */
#define ARM_PMU_STALL                                0x003C             /*!< Stall cycle for instruction or operation not sent for execution */
#define ARM_PMU_STALL_OP_BACKEND                     0x003D             /*!< Stall cycle for instruction or operation not sent for execution due to pipeline backend */
#define ARM_PMU_STALL_OP_FRONTEND                    0x003E             /*!< Stall cycle for instruction or operation not sent for execution due to pipeline frontend */
#define ARM_PMU_STALL_OP                             0x003F             /*!< Instruction or operation slots not occupied each cycle */
#define ARM_PMU_L1D_CACHE_RD                         0x0040             /*!< Level 1 data cache read */
#define ARM_PMU_LE_RETIRED                           0x0100             /*!< Loop end instruction executed */
#define ARM_PMU_LE_SPEC                              0x0101             /*!< Loop end instruction speculatively executed */
#define ARM_PMU_BF_RETIRED                           0x0104             /*!< Branch future instruction architecturally executed and condition code check pass */
#define ARM_PMU_BF_SPEC                              0x0105             /*!< Branch future instruction speculatively executed and condition code check pass */
#define ARM_PMU_LE_CANCEL                            0x0108             /*!< Loop end instruction not taken */
#define ARM_PMU_BF_CANCEL                            0x0109             /*!< Branch future instruction not taken */
#define ARM_PMU_SE_CALL_S                            0x0114             /*!< Call to secure function, resulting in Security state change */
#define ARM_PMU_SE_CALL_NS                           0x0115             /*!< Call to non-secure function, resulting in Security state change */
#define ARM_PMU_DWT_CMPMATCH0                        0x0118             /*!< DWT comparator 0 match */
#define ARM_PMU_DWT_CMPMATCH1                        0x0119             /*!< DWT comparator 1 match */
#define ARM_PMU_DWT_CMPMATCH2                        0x011A             /*!< DWT comparator 2 match */
#define ARM_PMU_DWT_CMPMATCH3                        0x011B             /*!< DWT comparator 3 match */
#define ARM_PMU_MVE_INST_RETIRED                     0x0200             /*!< MVE instruction architecturally executed */
#define ARM_PMU_MVE_INST_SPEC                        0x0201             /*!< MVE instruction speculatively executed */
#define ARM_PMU_MVE_FP_RETIRED                       0x0204             /*!< MVE floating-point instruction architecturally executed */
#define ARM_PMU_MVE_FP_SPEC                          0x0205             /*!< MVE floating-point instruction speculatively executed */
#define ARM_PMU_MVE_FP_HP_RETIRED                    0x0208             /*!< MVE half-precision floating-point instruction architecturally executed */
#define ARM_PMU_MVE_FP_HP_SPEC                       0x0209             /*!< MVE half-precision floating-point instruction speculatively executed */
#define ARM_PMU_MVE_FP_SP_RETIRED                    0x020C             /*!< MVE single-precision floating-point instruction architecturally executed */
#define ARM_PMU_MVE_FP_SP_SPEC                       0x020D             /*!< MVE single-precision floating-point instruction speculatively executed */
#define ARM_PMU_MVE_FP_MAC_RETIRED                   0x0214             /*!< MVE floating-point multiply or multiply-accumulate instruction architecturally executed */
#define ARM_PMU_MVE_FP_MAC_SPEC                      0x0215             /*!< MVE floating-point multiply or multiply-accumulate instruction speculatively executed */
#define ARM_PMU_MVE_INT_RETIRED                      0x0224             /*!< MVE integer instruction architecturally executed */
#define ARM_PMU_MVE_INT_SPEC                         0x0225             /*!< MVE integer instruction speculatively executed */
#define ARM_PMU_MVE_INT_MAC_RETIRED                  0x0228             /*!< MVE multiply or multiply-accumulate instruction architecturally executed */
#define ARM_PMU_MVE_INT_MAC_SPEC                     0x0229             /*!< MVE multiply or multiply-accumulate instruction speculatively executed */
#define ARM_PMU_MVE_LDST_RETIRED                     0x0238             /*!< MVE load or store instruction architecturally executed */
#define ARM_PMU_MVE_LDST_SPEC                        0x0239             /*!< MVE load or store instruction speculatively executed */
#define ARM_PMU_MVE_LD_RETIRED                       0x023C             /*!< MVE load instruction architecturally executed */
#define ARM_PMU_MVE_LD_SPEC                          0x023D             /*!< MVE load instruction speculatively executed */
#define ARM_PMU_MVE_ST_RETIRED                       0x0240             /*!< MVE store instruction architecturally executed */
#define ARM_PMU_MVE_ST_SPEC                          0x0241             /*!< MVE store instruction speculatively executed */
#define ARM_PMU_MVE_LDST_CONTIG_RETIRED              0x0244             /*!< MVE contiguous load or store instruction architecturally executed */
#define ARM_PMU_MVE_LDST_CONTIG_SPEC                 0x0245             /*!< MVE contiguous load or store instruction speculatively executed */
#define ARM_PMU_MVE_LD_CONTIG_RETIRED                0x0248             /*!< MVE contiguous load instruction architecturally executed */
#define ARM_PMU_MVE_LD_CONTIG_SPEC                   0x0249             /*!< MVE contiguous load instruction speculatively executed */
#define ARM_PMU_MVE_ST_CONTIG_RETIRED                0x024C             /*!< MVE contiguous store instruction architecturally executed */
#define ARM_PMU_MVE_ST_CONTIG_SPEC                   0x024D             /*!< MVE contiguous store instruction speculatively executed */
#define ARM_PMU_MVE_LDST_NONCONTIG_RETIRED           0x0250             /*!< MVE non-contiguous load or store instruction architecturally executed */
#define ARM_PMU_MVE_LDST_NONCONTIG_SPEC              0x0251             /*!< MVE non-contiguous load or store instruction speculatively executed */
#define ARM_PMU_MVE_LD_NONCONTIG_RETIRED             0x0254             /*!< MVE non-contiguous load instruction architecturally executed */
#define ARM_PMU_MVE_LD_NONCONTIG_SPEC                0x0255             /*!< MVE non-contiguous load instruction speculatively executed */
#define ARM_PMU_MVE_ST_NONCONTIG_RETIRED             0x0258             /*!< MVE non-contiguous store instruction architecturally executed */
#define ARM_PMU_MVE_ST_NONCONTIG_SPEC                0x0259             /*!< MVE non-contiguous store instruction speculatively executed */
#define ARM_PMU_MVE_LDST_MULTI_RETIRED               0x025C             /*!< MVE memory instruction targeting multiple registers architecturally executed */
#define ARM_PMU_MVE_LDST_MULTI_SPEC                  0x025D             /*!< MVE memory instruction targeting multiple registers speculatively executed */
#define ARM_PMU_MVE_LD_MULTI_RETIRED                 0x0260             /*!< MVE memory load instruction targeting multiple registers architecturally executed */
#define ARM_PMU_MVE_LD_MULTI_SPEC                    0x0261             /*!< MVE memory load instruction targeting multiple registers speculatively executed */
#define ARM_PMU_MVE_ST_MULTI_RETIRED                 0x0261             /*!< MVE memory store instruction targeting multiple registers architecturally executed */
#define ARM_PMU_MVE_ST_MULTI_SPEC                    0x0265             /*!< MVE memory store instruction targeting multiple registers speculatively executed */
#define ARM_PMU_MVE_LDST_UNALIGNED_RETIRED           0x028C             /*!< MVE unaligned memory load or store instruction architecturally executed */
#define ARM_PMU_MVE_LDST_UNALIGNED_SPEC              0x028D             /*!< MVE unaligned memory load or store instruction speculatively executed */
#define ARM_PMU_MVE_LD_UNALIGNED_RETIRED             0x0290             /*!< MVE unaligned load instruction architecturally executed */
#define ARM_PMU_MVE_LD_UNALIGNED_SPEC                0x0291             /*!< MVE unaligned load instruction speculatively executed */
#define ARM_PMU_MVE_ST_UNALIGNED_RETIRED             0x0294             /*!< MVE unaligned store instruction architecturally executed */
#define ARM_PMU_MVE_ST_UNALIGNED_SPEC                0x0295             /*!< MVE unaligned store instruction speculatively executed */
#define ARM_PMU_MVE_LDST_UNALIGNED_NONCONTIG_RETIRED 0x0298             /*!< MVE unaligned noncontiguous load or store instruction architecturally executed */
#define ARM_PMU_MVE_LDST_UNALIGNED_NONCONTIG_SPEC    0x0299             /*!< MVE unaligned noncontiguous load or store instruction speculatively executed */
#define ARM_PMU_MVE_VREDUCE_RETIRED                  0x02A0             /*!< MVE vector reduction instruction architecturally executed */
#define ARM_PMU_MVE_VREDUCE_SPEC                     0x02A1             /*!< MVE vector reduction instruction speculatively executed */
#define ARM_PMU_MVE_VREDUCE_FP_RETIRED               0x02A4             /*!< MVE floating-point vector reduction instruction architecturally executed */
#define ARM_PMU_MVE_VREDUCE_FP_SPEC                  0x02A5             /*!< MVE floating-point vector reduction instruction speculatively executed */
#define ARM_PMU_MVE_VREDUCE_INT_RETIRED              0x02A8             /*!< MVE integer vector reduction instruction architecturally executed */
#define ARM_PMU_MVE_VREDUCE_INT_SPEC                 0x02A9             /*!< MVE integer vector reduction instruction speculatively executed */
#define ARM_PMU_MVE_PRED                             0x02B8             /*!< Cycles where one or more predicated beats architecturally executed */
#define ARM_PMU_MVE_STALL                            0x02CC             /*!< Stall cycles caused by an MVE instruction */
#define ARM_PMU_MVE_STALL_RESOURCE                   0x02CD             /*!< Stall cycles caused by an MVE instruction because of resource conflicts */
#define ARM_PMU_MVE_STALL_RESOURCE_MEM               0x02CE             /*!< Stall cycles caused by an MVE instruction because of memory resource conflicts */
#define ARM_PMU_MVE_STALL_RESOURCE_FP                0x02CF             /*!< Stall cycles caused by an MVE instruction because of floating-point resource conflicts */
#define ARM_PMU_MVE_STALL_RESOURCE_INT               0x02D0             /*!< Stall cycles caused by an MVE instruction because of integer resource conflicts */
#define ARM_PMU_MVE_STALL_BREAK                      0x02D3             /*!< Stall cycles caused by an MVE chain break */
#define ARM_PMU_MVE_STALL_DEPENDENCY                 0x02D4             /*!< Stall cycles caused by MVE register dependency */
#define ARM_PMU_ITCM_ACCESS                          0x4007             /*!< Instruction TCM access */
#define ARM_PMU_DTCM_ACCESS                          0x4008             /*!< Data TCM access */
#define ARM_PMU_TRCEXTOUT0                           0x4010             /*!< ETM external output 0 */
#define ARM_PMU_TRCEXTOUT1                           0x4011             /*!< ETM external output 1 */
#define ARM_PMU_TRCEXTOUT2                           0x4012             /*!< ETM external output 2 */
#define ARM_PMU_TRCEXTOUT3                           0x4013             /*!< ETM external output 3 */
#define ARM_PMU_CTI_TRIGOUT4                         0x4018             /*!< Cross-trigger Interface output trigger 4 */
#define ARM_PMU_CTI_TRIGOUT5                         0x4019             /*!< Cross-trigger Interface output trigger 5 */
#define ARM_PMU_CTI_TRIGOUT6                         0x401A             /*!< Cross-trigger Interface output trigger 6 */
#define ARM_PMU_CTI_TRIGOUT7                         0x401B             /*!< Cross-trigger Interface output trigger 7 */

/** \brief PMU Functions */

__STATIC_INLINE void ARM_PMU_Enable(void);
__STATIC_INLINE void ARM_PMU_Disable(void);

__STATIC_INLINE void ARM_PMU_Set_EVTYPER(uint32_t num, uint32_t type);

__STATIC_INLINE void ARM_PMU_CYCCNT_Reset(void);
__STATIC_INLINE void ARM_PMU_EVCNTR_ALL_Reset(void);

__STATIC_INLINE void ARM_PMU_CNTR_Enable(uint32_t mask);
__STATIC_INLINE void ARM_PMU_CNTR_Disable(uint32_t mask);

__STATIC_INLINE uint32_t ARM_PMU_Get_CCNTR(void);
__STATIC_INLINE uint32_t ARM_PMU_Get_EVCNTR(uint32_t num);

__STATIC_INLINE uint32_t ARM_PMU_Get_CNTR_OVS(void);
__STATIC_INLINE void ARM_PMU_Set_CNTR_OVS(uint32_t mask);

__STATIC_INLINE void ARM_PMU_Set_CNTR_IRQ_Enable(uint32_t mask);
__STATIC_INLINE void ARM_PMU_Set_CNTR_IRQ_Disable(uint32_t mask);

__STATIC_INLINE void ARM_PMU_CNTR_Increment(uint32_t mask);

/**
  \brief   Enable the PMU
*/
__STATIC_INLINE void ARM_PMU_Enable(void)
{
  PMU->CTRL |= PMU_CTRL_ENABLE_Msk;
}

/**
  \brief   Disable the PMU
*/
__STATIC_INLINE void ARM_PMU_Disable(void)
{
  PMU->CTRL &= ~PMU_CTRL_ENABLE_Msk;
}

/**
  \brief   Set event to count for PMU eventer counter
  \param [in]    num     Event counter (0-30) to configure
  \param [in]    type    Event to count
*/
__STATIC_INLINE void ARM_PMU_Set_EVTYPER(uint32_t num, uint32_t type)
{
  PMU->EVTYPER[num] = type;
}

/**
  \brief  Reset cycle counter
*/
__STATIC_INLINE void ARM_PMU_CYCCNT_Reset(void)
{
  PMU->CTRL |= PMU_CTRL_CYCCNT_RESET_Msk;
}

/**
  \brief  Reset all event counters
*/
__STATIC_INLINE void ARM_PMU_EVCNTR_ALL_Reset(void)
{
  PMU->CTRL |= PMU_CTRL_EVENTCNT_RESET_Msk;
}

/**
  \brief  Enable counters
  \param [in]     mask    Counters to enable
  \note   Enables one or more of the following:
          - event counters (0-30)
          - cycle counter
*/
__STATIC_INLINE void ARM_PMU_CNTR_Enable(uint32_t mask)
{
  PMU->CNTENSET = mask;
}

/**
  \brief  Disable counters
  \param [in]     mask    Counters to enable
  \note   Disables one or more of the following:
          - event counters (0-30)
          - cycle counter
*/
__STATIC_INLINE void ARM_PMU_CNTR_Disable(uint32_t mask)
{
  PMU->CNTENCLR = mask;
}

/**
  \brief  Read cycle counter
  \return                 Cycle count
*/
__STATIC_INLINE uint32_t ARM_PMU_Get_CCNTR(void)
{
  return PMU->CCNTR;
}

/**
  \brief   Read event counter
  \param [in]     num     Event counter (0-30) to read
  \return                 Event count
*/
__STATIC_INLINE uint32_t ARM_PMU_Get_EVCNTR(uint32_t num)
{
  return PMU_EVCNTR_CNT_Msk & PMU->EVCNTR[num];
}

/**
  \brief   Read counter overflow status
  \return  Counter overflow status bits for the following:
          - event counters (0-30)
          - cycle counter
*/
__STATIC_INLINE uint32_t ARM_PMU_Get_CNTR_OVS(void)
{
  return PMU->OVSSET;
}

/**
  \brief   Clear counter overflow status
  \param [in]     mask    Counter overflow status bits to clear
  \note    Clears overflow status bits for one or more of the following:
           - event counters (0-30)
           - cycle counter
*/
__STATIC_INLINE void ARM_PMU_Set_CNTR_OVS(uint32_t mask)
{
  PMU->OVSCLR = mask;
}

/**
  \brief   Enable counter overflow interrupt request
  \param [in]     mask    Counter overflow interrupt request bits to set
  \note    Sets overflow interrupt request bits for one or more of the following:
           - event counters (0-30)
           - cycle counter
*/
__STATIC_INLINE void ARM_PMU_Set_CNTR_IRQ_Enable(uint32_t mask)
{
  PMU->INTENSET = mask;
}

/**
  \brief   Disable counter overflow interrupt request
  \param [in]     mask    Counter overflow interrupt request bits to clear
  \note    Clears overflow interrupt request bits for one or more of the following:
           - event counters (0-30)
           - cycle counter
*/
__STATIC_INLINE void ARM_PMU_Set_CNTR_IRQ_Disable(uint32_t mask)
{
  PMU->INTENCLR = mask;
}

/**
  \brief   Software increment event counter
  \param [in]     mask    Counters to increment
  \note    Software increment bits for one or more event counters (0-30)
*/
__STATIC_INLINE void ARM_PMU_CNTR_Increment(uint32_t mask)
{
  PMU->SWINC = mask;
}

#endif
