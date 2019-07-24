/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcPortDefines.h
 *
 * Some common defines for the trace recorder.
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

#ifndef TRC_PORTDEFINES_H
#define TRC_PORTDEFINES_H

#define TRC_FREE_RUNNING_32BIT_INCR 1
#define TRC_FREE_RUNNING_32BIT_DECR 2
#define TRC_OS_TIMER_INCR 3
#define TRC_OS_TIMER_DECR 4
#define TRC_CUSTOM_TIMER_INCR 5
#define TRC_CUSTOM_TIMER_DECR 6

/* Start options for vTraceEnable. */
#define TRC_INIT 0
#define TRC_START 1
#define TRC_START_AWAIT_HOST 2

/* Command codes for TzCtrl task */
#define CMD_SET_ACTIVE      1 /* Start (param1 = 1) or Stop (param1 = 0) */

/* The final command code, used to validate commands. */
#define CMD_LAST_COMMAND 1

#define TRC_RECORDER_MODE_SNAPSHOT		0
#define TRC_RECORDER_MODE_STREAMING		1

#define TRC_RECORDER_BUFFER_ALLOCATION_STATIC   (0x00)
#define TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC  (0x01)
#define TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM   (0x02)


/******************************************************************************
 * Supported ports
 *
 * TRC_HARDWARE_PORT_HWIndependent
 * A hardware independent fallback option for event timestamping. Provides low
 * resolution timestamps based on the OS tick.
 * This may be used on the Win32 port, but may also be used on embedded hardware
 * platforms. All time durations will be truncated to the OS tick frequency,
 * typically 1 KHz. This means that a task or ISR that executes in less than
 * 1 ms get an execution time of zero.
 *
 * TRC_HARDWARE_PORT_APPLICATION_DEFINED
 * Allows for defining the port macros in other source code files.
 *
 * TRC_HARDWARE_PORT_Win32
 * "Accurate" timestamping based on the Windows performance counter for Win32
 * builds. Note that this gives the host machine time, not the kernel time.
 *
 * Hardware specific ports
 * To get accurate timestamping, a hardware timer is necessary. Below are the
 * available ports. Some of these are "unofficial", meaning that
 * they have not yet been verified by Percepio but have been contributed by
 * external developers. They should work, otherwise let us know by emailing
 * support@percepio.com. Some work on any OS platform, while other are specific
 * to a certain operating system.
 *****************************************************************************/

/****** Port Name ************************************* Code ** Official ** OS Platform *********/
#define TRC_HARDWARE_PORT_APPLICATION_DEFINED			98	/*	-			-					*/
#define TRC_HARDWARE_PORT_NOT_SET						99	/*	-			-					*/
#define TRC_HARDWARE_PORT_HWIndependent					0	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_Win32							1	/*	Yes			FreeRTOS on Win32	*/
#define TRC_HARDWARE_PORT_Atmel_AT91SAM7				2	/*	No			Any					*/
#define TRC_HARDWARE_PORT_Atmel_UC3A0					3	/*	No			Any					*/
#define TRC_HARDWARE_PORT_ARM_Cortex_M					4	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_Renesas_RX600					6	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_MICROCHIP_PIC24_PIC32			7	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_TMS570_RM48	8	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_MSP430		9	/*	No			Any					*/
#define TRC_HARDWARE_PORT_XILINX_PPC405					11	/*	No			FreeRTOS			*/
#define TRC_HARDWARE_PORT_XILINX_PPC440					12	/*	No			FreeRTOS			*/
#define TRC_HARDWARE_PORT_XILINX_MICROBLAZE				13	/*	No			Any					*/
#define TRC_HARDWARE_PORT_NXP_LPC210X					14	/*	No			Any					*/
#define TRC_HARDWARE_PORT_ARM_CORTEX_A9					15	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_POWERPC_Z4                    16  /*  No          FreeRTOS            */

#endif /*TRC_PORTDEFINES_H*/
