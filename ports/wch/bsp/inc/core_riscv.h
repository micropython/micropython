/********************************** (C) COPYRIGHT  *******************************
* File Name          : core_riscv.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : RISC-V Core Peripheral Access Layer Header File for CH32V30x
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CORE_RISCV_H__
#define __CORE_RISCV_H__

/* IO definitions */
#ifdef __cplusplus
  #define     __I     volatile                /* defines 'read only' permissions */
#else
  #define     __I     volatile const          /* defines 'read only' permissions */
#endif
#define     __O     volatile                  /* defines 'write only' permissions */
#define     __IO    volatile                  /* defines 'read / write' permissions */

/* Standard Peripheral Library old types (maintained for legacy purpose) */
typedef __I uint64_t vuc64;  /* Read Only */
typedef __I uint32_t vuc32;  /* Read Only */
typedef __I uint16_t vuc16;  /* Read Only */
typedef __I uint8_t vuc8;   /* Read Only */

typedef const uint64_t uc64;  /* Read Only */
typedef const uint32_t uc32;  /* Read Only */
typedef const uint16_t uc16;  /* Read Only */
typedef const uint8_t uc8;   /* Read Only */

typedef __I int64_t vsc64;  /* Read Only */
typedef __I int32_t vsc32;  /* Read Only */
typedef __I int16_t vsc16;  /* Read Only */
typedef __I int8_t vsc8;   /* Read Only */

typedef const int64_t sc64;  /* Read Only */
typedef const int32_t sc32;  /* Read Only */
typedef const int16_t sc16;  /* Read Only */
typedef const int8_t sc8;   /* Read Only */

typedef __IO uint64_t  vu64;
typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef __IO int64_t  vs64;
typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

#define   RV_STATIC_INLINE  static  inline

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct{
  __I  uint32_t ISR[8];
  __I  uint32_t IPR[8];
  __IO uint32_t ITHRESDR;
  __IO uint32_t RESERVED;
  __IO uint32_t CFGR;
  __I  uint32_t GISR;
  uint8_t VTFIDR[4];
  uint8_t RESERVED0[12];
  __IO uint32_t VTFADDR[4];
  uint8_t RESERVED1[0x90];
  __O  uint32_t IENR[8];
  uint8_t RESERVED2[0x60];
  __O  uint32_t IRER[8];
  uint8_t RESERVED3[0x60];
  __O  uint32_t IPSR[8];
  uint8_t RESERVED4[0x60];
  __O  uint32_t IPRR[8];
  uint8_t RESERVED5[0x60];
  __IO uint32_t IACTR[8];
  uint8_t RESERVED6[0xE0];
  __IO uint8_t IPRIOR[256];
  uint8_t RESERVED7[0x810];
  __IO uint32_t SCTLR;
}PFIC_Type;

/* memory mapped structure for SysTick */
typedef struct
{
    __IO u32 CTLR;
    __IO u32 SR;
    __IO u64 CNT;
    __IO u64 CMP;
}SysTick_Type;


#define PFIC            ((PFIC_Type *) 0xE000E000 )
#define NVIC            PFIC
#define NVIC_KEY1       ((uint32_t)0xFA050000)
#define	NVIC_KEY2				((uint32_t)0xBCAF0000)
#define	NVIC_KEY3				((uint32_t)0xBEEF0000)

#define SysTick         ((SysTick_Type *) 0xE000F000)

/*********************************************************************
 * @fn      __enable_irq
 *
 * @brief   Enable Global Interrupt
 *
 * @return  none
 */
RV_STATIC_INLINE void __enable_irq()
{
  __asm volatile ("csrw 0x800, %0" : : "r" (0x6088) );
}

/*********************************************************************
 * @fn      __disable_irq
 *
 * @brief   Disable Global Interrupt
 *
 * @return  none
 */
RV_STATIC_INLINE void __disable_irq()
{
  __asm volatile ("csrw 0x800, %0" : : "r" (0x6000) );
}

/*********************************************************************
 * @fn      __NOP
 *
 * @brief   nop
 *
 * @return  none
 */
RV_STATIC_INLINE void __NOP()
{
  __asm volatile ("nop");
}

/*********************************************************************
 * @fn      NVIC_EnableIRQ
 *
 * @brief   Enable Interrupt
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  none
 */
RV_STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
{
  NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*********************************************************************
 * @fn      NVIC_DisableIRQ
 *
 * @brief   Disable Interrupt
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  none
 */
RV_STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn)
{
  NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*********************************************************************
 * @fn      NVIC_GetStatusIRQ
 *
 * @brief   Get Interrupt Enable State
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  1 - Interrupt Enable
 *          0 - Interrupt Disable
 */
RV_STATIC_INLINE uint32_t NVIC_GetStatusIRQ(IRQn_Type IRQn)
{
  return((uint32_t) ((NVIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

/*********************************************************************
 * @fn      NVIC_GetPendingIRQ
 *
 * @brief   Get Interrupt Pending State
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  1 - Interrupt Pending Enable
 *          0 - Interrupt Pending Disable
 */
RV_STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  return((uint32_t) ((NVIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

/*********************************************************************
 * @fn      NVIC_SetPendingIRQ
 *
 * @brief   Set Interrupt Pending
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  None
 */
RV_STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  NVIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*********************************************************************
 * @fn      NVIC_ClearPendingIRQ
 *
 * @brief   Clear Interrupt Pending
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  None
 */
RV_STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  NVIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

/*********************************************************************
 * @fn      NVIC_GetActive
 *
 * @brief   Get Interrupt Active State
 *
 * @param   IRQn: Interrupt Numbers
 *
 * @return  1 - Interrupt Active
 *          0 - Interrupt No Active
 */
RV_STATIC_INLINE uint32_t NVIC_GetActive(IRQn_Type IRQn)
{
  return((uint32_t)((NVIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

/*********************************************************************
 * @fn      NVIC_SetPriority
 *
 * @brief   Set Interrupt Priority
 *
 * @param   IRQn - Interrupt Numbers
 *          priority -
 *              bit7 - pre-emption priority
 *              bit6~bit4 - subpriority
 * @return  None
 */
RV_STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
  NVIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

/*********************************************************************
 * @fn      __WFI
 *
 * @brief   Wait for Interrupt
 *
 * @return  None
 */
__attribute__( ( always_inline ) ) RV_STATIC_INLINE void __WFI(void)
{
  NVIC->SCTLR &= ~(1<<3);	// wfi
  asm volatile ("wfi");
}

/*********************************************************************
 * @fn      __WFE
 *
 * @brief   Wait for Events
 *
 * @return  None
 */
__attribute__( ( always_inline ) ) RV_STATIC_INLINE void __WFE(void)
{
  uint32_t t;

  t = NVIC->SCTLR;
  NVIC->SCTLR |= (1<<3)|(1<<5);		// (wfi->wfe)+(__sev)
  NVIC->SCTLR = (NVIC->SCTLR & ~(1<<5)) | ( t & (1<<5));
  asm volatile ("wfi");
  asm volatile ("wfi");
}

/*********************************************************************
 * @fn      SetVTFIRQ
 *
 * @brief   Set VTF Interrupt
 *
 * @param   add - VTF interrupt service function base address.
 *          IRQn -Interrupt Numbers
 *          num - VTF Interrupt Numbers
 *          NewState - DISABLE or ENABLE
 * @return  None
 */
RV_STATIC_INLINE void SetVTFIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num, FunctionalState NewState){
  if(num > 3)  return ;

  if (NewState != DISABLE)
  {
      NVIC->VTFIDR[num] = IRQn;
      NVIC->VTFADDR[num] = ((addr&0xFFFFFFFE)|0x1);
  }
  else{
      NVIC->VTFIDR[num] = IRQn;
      NVIC->VTFADDR[num] = ((addr&0xFFFFFFFE)&(~0x1));
  }
}

/*********************************************************************
 * @fn      NVIC_SystemReset
 *
 * @brief   Initiate a system reset request
 *
 * @return  None
 */
RV_STATIC_INLINE void NVIC_SystemReset(void)
{
  NVIC->CFGR = NVIC_KEY3|(1<<7);
}


/* Core_Exported_Functions */  
extern uint32_t __get_FFLAGS(void);
extern void __set_FFLAGS(uint32_t value);
extern uint32_t __get_FRM(void);
extern void __set_FRM(uint32_t value);
extern uint32_t __get_FCSR(void);
extern void __set_FCSR(uint32_t value);
extern uint32_t __get_MSTATUS(void);
extern void __set_MSTATUS(uint32_t value);
extern uint32_t __get_MISA(void);
extern void __set_MISA(uint32_t value);
extern uint32_t __get_MIE(void);
extern void __set_MIE(uint32_t value);
extern uint32_t __get_MTVEC(void);
extern void __set_MTVEC(uint32_t value);
extern uint32_t __get_MSCRATCH(void);
extern void __set_MSCRATCH(uint32_t value);
extern uint32_t __get_MEPC(void);
extern void __set_MEPC(uint32_t value);
extern uint32_t __get_MCAUSE(void);
extern void __set_MCAUSE(uint32_t value);
extern uint32_t __get_MTVAL(void);
extern void __set_MTVAL(uint32_t value);
extern uint32_t __get_MIP(void);
extern void __set_MIP(uint32_t value);
extern uint32_t __get_MCYCLE(void);
extern void __set_MCYCLE(uint32_t value);
extern uint32_t __get_MCYCLEH(void);
extern void __set_MCYCLEH(uint32_t value);
extern uint32_t __get_MINSTRET(void);
extern void __set_MINSTRET(uint32_t value);
extern uint32_t __get_MINSTRETH(void);
extern void __set_MINSTRETH(uint32_t value);
extern uint32_t __get_MVENDORID(void);
extern uint32_t __get_MARCHID(void);
extern uint32_t __get_MIMPID(void);
extern uint32_t __get_MHARTID(void);
extern uint32_t __get_SP(void);


#endif





