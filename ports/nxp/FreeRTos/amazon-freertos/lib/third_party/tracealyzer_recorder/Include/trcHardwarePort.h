/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcHardwarePort.h
 *
 * The hardware abstraction layer for the trace recorder.
 *
 * Terms of Use
 * This file is part of the trace recorder library (RECORDER), which is the 
 * intellectual property of Percepio AB (PERCEPIO) and provided under a
 * license as follows.
 * The RECORDER may be used free of charge for the purpose of recording data
 * intended for analysis in PERCEPIO products. It may not be used or modified
 * for other purposes without explicit permission from PERCEPIO.
 * You may distribute the RECORDER in its original source code form, assuming
 * this text (terms of use, disclaimer, copyright notice) is unchanged. You are
 * allowed to distribute the RECORDER with minor modifications intended for
 * configuration or porting of the RECORDER, e.g., to allow using it on a 
 * specific processor, processor family or with a specific communication
 * interface. Any such modifications should be documented directly below
 * this comment block.
 *
 * Disclaimer
 * The RECORDER is being delivered to you AS IS and PERCEPIO makes no warranty
 * as to its use or performance. PERCEPIO does not and cannot warrant the 
 * performance or results you may obtain by using the RECORDER or documentation.
 * PERCEPIO make no warranties, express or implied, as to noninfringement of
 * third party rights, merchantability, or fitness for any particular purpose.
 * In no event will PERCEPIO, its technology partners, or distributors be liable
 * to you for any consequential, incidental or special damages, including any
 * lost profits or lost savings, even if a representative of PERCEPIO has been
 * advised of the possibility of such damages, or for any claim by any third
 * party. Some jurisdictions do not allow the exclusion or limitation of
 * incidental, consequential or special damages, or the exclusion of implied
 * warranties or limitations on how long an implied warranty may last, so the
 * above limitations may not apply to you.
 *
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2017.
 * www.percepio.com
 ******************************************************************************/

#ifndef TRC_HARDWARE_PORT_H
#define TRC_HARDWARE_PORT_H

#include "trcPortDefines.h"


#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_NOT_SET)
	#error "TRC_CFG_HARDWARE_PORT not selected - see trcConfig.h"
#endif

/*******************************************************************************
 * TRC_IRQ_PRIORITY_ORDER
 *
 * Macro which should be defined as an integer of 0 or 1.
 *
 * This should be 0 if lower IRQ priority values implies higher priority
 * levels, such as on ARM Cortex M. If the opposite scheme is used, i.e.,
 * if higher IRQ priority values means higher priority, this should be 1.
 *
 * This setting is not critical. It is used only to sort and colorize the
 * interrupts in priority order, in case you record interrupts using
 * the vTraceStoreISRBegin and vTraceStoreISREnd routines.
 *
 ******************************************************************************
 *
 * HWTC Macros
 *
 * These macros provides a hardware isolation layer representing the
 * hardware timer/counter used for the event timestamping.
 *
 * TRC_HWTC_COUNT: How to read the current value of the timer/counter.
 *
 * TRC_HWTC_TYPE: Tells the type of timer/counter used for TRC_HWTC_COUNT:
 *
 * - TRC_FREE_RUNNING_32BIT_INCR:
 *   Free-running 32-bit timer/counter, counting upwards from 0.
 *
 * - TRC_FREE_RUNNING_32BIT_DECR
 *   Free-running 32-bit timer/counter, counting downwards from 0xFFFFFFFF.
 *
 * - TRC_OS_TIMER_INCR
 *	 Periodic timer that drives the OS tick interrupt, counting upwards
 *   from 0 until (TRC_HWTC_PERIOD-1).
 *
 * - TRC_OS_TIMER_DECR
 *	 Periodic timer that drives the OS tick interrupt, counting downwards
 *   from TRC_HWTC_PERIOD-1 until 0.
 *
 * - TRC_CUSTOM_TIMER_INCR
 *   A custom timer or counter independent of the OS tick, counting
 *   downwards from TRC_HWTC_PERIOD-1 until 0. (Currently only supported
 *   in streaming mode).
 *
 * - TRC_CUSTOM_TIMER_DECR
 *   A custom timer independent of the OS tick, counting downwards
 *   from TRC_HWTC_PERIOD-1 until 0. (Currently only supported
 *   in streaming mode).
 *
 * TRC_HWTC_PERIOD: The number of HWTC_COUNT ticks until the timer wraps
 * around. If using TRC_FREE_RUNNING_32BIT_INCR/DECR, this should be 0. 
 *
 * TRC_HWTC_FREQ_HZ: The clock rate of the TRC_HWTC_COUNT counter in Hz. If using 
 * TRC_OS_TIMER_INCR/DECR, this is should be TRC_HWTC_PERIOD * TRACE_TICK_RATE_HZ.
 * If using a free-running timer, this is often TRACE_CPU_CLOCK_HZ (if running at
 * the core clock rate). If using TRC_CUSTOM_TIMER_INCR/DECR, this should match
 * the clock rate of your custom timer (i.e., TRC_HWTC_COUNT). If the default value
 * of TRC_HWTC_FREQ_HZ is incorrect for your setup, you can override it by calling
 * vTraceSetFrequency before calling vTraceEnable.
 *
 * TRC_HWTC_DIVISOR (used in snapshot mode only):
 * In snapshot mode, the timestamp resolution is TRC_HWTC_FREQ_HZ/TRC_HWTC_DIVISOR.
 * If the timer frequency is very high (hundreds of MHz), we recommend increasing
 * the TRC_HWTC_DIVISOR prescaler, to reduce the bandwidth needed to store
 * timestamps. This since extra "XTS" events are inserted if the time since the
 * previous event exceeds a certain limit (255 or 65535 depending on event type).
 * It is advised to keep the time between most events below 65535 native ticks
 * (after division by TRC_HWTC_DIVISOR) to avoid frequent XTS events.
 ******************************************************************************/

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_NOT_SET)
	#error "TRC_CFG_HARDWARE_PORT not selected - see trcConfig.h"
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win32)
	/* This can be used as a template for any free-running 32-bit counter */
	#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
	#define TRC_HWTC_COUNT (ulGetRunTimeCounterValue())
	#define TRC_HWTC_PERIOD 0
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ 100000
	
	#define TRC_IRQ_PRIORITY_ORDER 1

	#define TRC_PORT_SPECIFIC_INIT()

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_HWIndependent)
	/* Timestamping by OS tick only (typically 1 ms resolution) */
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT 0
	#define TRC_HWTC_PERIOD 1
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ TRACE_TICK_RATE_HZ

	/* Set the meaning of IRQ priorities in ISR tracing - see above */
	#define TRC_IRQ_PRIORITY_ORDER NOT_SET

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M)
	
	#ifndef __CORTEX_M
	#error "Can't find the CMSIS API. Please include your processor's header file in trcConfig.h" 	
	#endif

	/**************************************************************************
	* For Cortex-M3, M4 and M7, the DWT cycle counter is used for timestamping.
	* For Cortex-M0 and M0+, the SysTick timer is used since DWT is not
	* available. Systick timestamping can also be forced on Cortex-M3, M4 and
	* M7 by defining the preprocessor directive TRC_CFG_ARM_CM_USE_SYSTICK,
	* either directly below or in trcConfig.h.
	*
	* #define TRC_CFG_ARM_CM_USE_SYSTICK
    **************************************************************************/

	#if ((__CORTEX_M >= 0x03) && (! defined TRC_CFG_ARM_CM_USE_SYSTICK))
		
		void prvTraceInitCortexM(void);

		#define TRC_REG_DEMCR (*(volatile uint32_t*)0xE000EDFC)
		#define TRC_REG_DWT_CTRL (*(volatile uint32_t*)0xE0001000)
		#define TRC_REG_DWT_CYCCNT (*(volatile uint32_t*)0xE0001004)
		#define TRC_REG_DWT_EXCCNT (*(volatile uint32_t*)0xE000100C)

		#define TRC_REG_ITM_LOCKACCESS (*(volatile uint32_t*)0xE0001FB0)		
		#define TRC_ITM_LOCKACCESS_UNLOCK (0xC5ACCE55)
		
		/* Bit mask for TRCENA bit in DEMCR - Global enable for DWT and ITM */
		#define TRC_DEMCR_TRCENA (1 << 24)

		/* Bit mask for NOPRFCNT bit in DWT_CTRL. If 1, DWT_EXCCNT is not supported */
		#define TRC_DWT_CTRL_NOPRFCNT (1 << 24)

		/* Bit mask for NOCYCCNT bit in DWT_CTRL. If 1, DWT_CYCCNT is not supported */
		#define TRC_DWT_CTRL_NOCYCCNT (1 << 25)

		/* Bit mask for EXCEVTENA_ bit in DWT_CTRL. Set to 1 to enable DWT_EXCCNT */
		#define TRC_DWT_CTRL_EXCEVTENA (1 << 18)

		/* Bit mask for EXCEVTENA_ bit in DWT_CTRL. Set to 1 to enable DWT_CYCCNT */
		#define TRC_DWT_CTRL_CYCCNTENA (1)

		#define TRC_PORT_SPECIFIC_INIT() prvTraceInitCortexM()

		#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
		#define TRC_HWTC_COUNT TRC_REG_DWT_CYCCNT
		#define TRC_HWTC_PERIOD 0
		#define TRC_HWTC_DIVISOR 4
		#define TRC_HWTC_FREQ_HZ TRACE_CPU_CLOCK_HZ
		#define TRC_IRQ_PRIORITY_ORDER 0
	
	#else
			
		#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
		#define TRC_HWTC_COUNT (*((volatile uint32_t*)0xE000E018))
		#define TRC_HWTC_PERIOD ((*((volatile uint32_t*)0xE000E014)) + 1)
		#define TRC_HWTC_DIVISOR 4
		#define TRC_HWTC_FREQ_HZ TRACE_CPU_CLOCK_HZ
		#define TRC_IRQ_PRIORITY_ORDER 0
	
	#endif

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Renesas_RX600)

	#include "iodefine.h"

	#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)	
		
		#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
		#define TRC_HWTC_COUNT (CMT0.CMCNT)
		
	#elif (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
		
		/* Decreasing counters better for Tickless Idle? */
		#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
		#define TRC_HWTC_COUNT (CMT0.CMCOR - CMT0.CMCNT)
	
	#endif
	
	#define TRC_HWTC_PERIOD (CMT0.CMCOR + 1)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1 
	
#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_MICROCHIP_PIC24_PIC32)

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT (TMR1)
	#define TRC_HWTC_PERIOD (PR1 + 1)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_TMS570_RM48)

	#define TRC_RTIFRC0 *((uint32_t *)0xFFFFFC10)
	#define TRC_RTICOMP0 *((uint32_t *)0xFFFFFC50)
	#define TRC_RTIUDCP0 *((uint32_t *)0xFFFFFC54)
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT (TRC_RTIFRC0 - (TRC_RTICOMP0 - TRC_RTIUDCP0))
	#define TRC_HWTC_PERIOD (TRC_RTIUDCP0)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Atmel_AT91SAM7)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT ((uint32_t)(AT91C_BASE_PITC->PITC_PIIR & 0xFFFFF))
	#define TRC_HWTC_PERIOD ((uint32_t)(AT91C_BASE_PITC->PITC_PIMR + 1))
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Atmel_UC3A0)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO*/
	
	/* For Atmel AVR32 (AT32UC3A) */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT ((uint32_t)sysreg_read(AVR32_COUNT))
	#define TRC_HWTC_PERIOD ((uint32_t)(sysreg_read(AVR32_COMPARE) + 1))
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_NXP_LPC210X)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */
	
	/* Tested with LPC2106, but should work with most LPC21XX chips. */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT *((uint32_t *)0xE0004008 )
	#define TRC_HWTC_PERIOD *((uint32_t *)0xE0004018 )
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_MSP430)

	/* UNOFFICIAL PORT - NOT YET VERIFIED */
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT (TA0R)
	#define TRC_HWTC_PERIOD (((uint16_t)TACCR0)+1)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_PPC405)

	/* UNOFFICIAL PORT - NOT YET VERIFIED */
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT mfspr(0x3db)
	#define TRC_HWTC_PERIOD (TRACE_CPU_CLOCK_HZ / TRACE_TICK_RATE_HZ)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_PPC440)

	/* UNOFFICIAL PORT */

	/* This should work with most PowerPC chips */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT mfspr(0x016)
	#define TRC_HWTC_PERIOD (TRACE_CPU_CLOCK_HZ / TRACE_TICK_RATE_HZ)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_MICROBLAZE)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */

	/* This should work with most Microblaze configurations.
	 * It uses the AXI Timer 0 - the tick interrupt source.
	 * If an AXI Timer 0 peripheral is available on your hardware platform, no modifications are required.
	 */
	#include "xtmrctr_l.h"
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT XTmrCtr_GetTimerCounterReg( XPAR_TMRCTR_0_BASEADDR, 0 )
 	#define TRC_HWTC_PERIOD (XTmrCtr_mGetLoadReg( XPAR_TMRCTR_0_BASEADDR, 0) + 1)
	#define TRC_HWTC_DIVISOR 16
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_CORTEX_A9)
	/* INPUT YOUR PERIPHERAL BASE ADDRESS HERE */
	#define TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS	0xSOMETHING
	
	#define TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET	0x0600
	#define TRC_CA9_MPCORE_PRIVCTR_PERIOD_REG	(*(volatile uint32_t*)(TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS + TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET + 0x00))
	#define TRC_CA9_MPCORE_PRIVCTR_COUNTER_REG	(*(volatile uint32_t*)(TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS + TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET + 0x04))
	#define TRC_CA9_MPCORE_PRIVCTR_CONTROL_REG	(*(volatile uint32_t*)(TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS + TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET + 0x08))
	
	#define TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_MASK    0x0000FF00
	#define TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_SHIFT   8
	#define TRC_CA9_MPCORE_PRIVCTR_PRESCALER        (((TRC_CA9_MPCORE_PRIVCTR_CONTROL_REG & TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_MASK) >> TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_SHIFT) + 1)

    #define TRC_HWTC_TYPE                           TRC_OS_TIMER_DECR
    #define TRC_HWTC_COUNT                          TRC_CA9_MPCORE_PRIVCTR_COUNTER_REG
    #define TRC_HWTC_PERIOD                         (TRC_CA9_MPCORE_PRIVCTR_PERIOD_REG + 1)

    /****************************************************************************************
	NOTE: The private timer ticks with a very high frequency (half the core-clock usually), 
	depending on the prescaler used. If a low prescaler is used, the number of HW ticks between
	the trace events gets large, and thereby inefficient to store (sometimes extra events are
	needed). To improve efficiency, you may use the TRC_HWTC_DIVISOR as an additional prescaler.
    *****************************************************************************************/	
	#define TRC_HWTC_DIVISOR 1
	
	#define TRC_HWTC_FREQ_HZ (TRACE_TICK_RATE_HZ * TRC_HWTC_PERIOD)
    #define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_POWERPC_Z4)

    /* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */

    #define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
    //#define HWTC_COUNT_DIRECTION DIRECTION_DECREMENTING
    #define TRC_HWTC_COUNT PIT.TIMER[configTICK_PIT_CHANNEL].CVAL.R // must be the PIT channel used for the systick
    #define TRC_HWTC_PERIOD ((configPIT_CLOCK_HZ / configTICK_RATE_HZ) - 1U) // TODO FIXME or maybe not -1? what's the right "period" value?
    #define TRC_HWTC_FREQ_HZ configPIT_CLOCK_HZ
    #define TRC_HWTC_DIVISOR 1
    #define TRC_IRQ_PRIORITY_ORDER 1 // higher IRQ priority values are more significant

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_APPLICATION_DEFINED)

	#if !( defined (TRC_HWTC_TYPE) && defined (TRC_HWTC_COUNT) && defined (TRC_HWTC_PERIOD) && defined (TRC_HWTC_FREQ_HZ) && defined (TRC_IRQ_PRIORITY_ORDER) )
		#error "The hardware port is not completely defined!"
	#endif

#elif (TRC_CFG_HARDWARE_PORT != TRC_HARDWARE_PORT_NOT_SET)

	#error "TRC_CFG_HARDWARE_PORT had unsupported value!"
	#define TRC_CFG_HARDWARE_PORT TRC_HARDWARE_PORT_NOT_SET

#endif

#ifndef TRC_HWTC_DIVISOR
	#define TRC_HWTC_DIVISOR 1
#endif

#ifndef TRC_PORT_SPECIFIC_INIT
	#define TRC_PORT_SPECIFIC_INIT() 
#endif

/* If Win32 port */
#ifdef WIN32

	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0600

	/* Standard includes. */
	#include <stdio.h>
	#include <windows.h>
	#include <direct.h>

    /***************************************************************************
    * The Win32 port by default saves the trace to file and then kills the
    * program when the recorder is stopped, to facilitate quick, simple tests
    * of the recorder.
    ***************************************************************************/
	#define WIN32_PORT_SAVE_WHEN_STOPPED 1
	#define WIN32_PORT_EXIT_WHEN_STOPPED 1

#endif

#if (TRC_CFG_HARDWARE_PORT != TRC_HARDWARE_PORT_NOT_SET)
	
	#ifndef TRC_HWTC_TYPE
	#error "TRC_HWTC_TYPE is not set!"
	#endif

	#ifndef TRC_HWTC_COUNT
	#error "TRC_HWTC_COUNT is not set!"
	#endif

	#ifndef TRC_HWTC_PERIOD
	#error "TRC_HWTC_PERIOD is not set!"
	#endif

	#ifndef TRC_HWTC_DIVISOR
	#error "TRC_HWTC_DIVISOR is not set!"
	#endif

	#ifndef TRC_IRQ_PRIORITY_ORDER
	#error "TRC_IRQ_PRIORITY_ORDER is not set!"
	#elif (TRC_IRQ_PRIORITY_ORDER != 0) && (TRC_IRQ_PRIORITY_ORDER != 1)
	#error "TRC_IRQ_PRIORITY_ORDER has bad value!"
	#endif

	#if (TRC_HWTC_DIVISOR < 1)
	#error "TRC_HWTC_DIVISOR must be a non-zero positive value!"
	#endif
	
	#ifndef TRC_HWTC_FREQ_HZ 
	#error "TRC_HWTC_FREQ_HZ not defined!"
	#endif
	
#endif

#endif /*TRC_SNAPSHOT_HARDWARE_PORT_H*/
