/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcStreamingConfig.h
 *
 * Configuration parameters for the trace recorder library in streaming mode.
 * Read more at http://percepio.com/2016/10/05/rtos-tracing/
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

#ifndef TRC_STREAMING_CONFIG_H
#define TRC_STREAMING_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Configuration Macro: TRC_CFG_SYMBOL_TABLE_SLOTS
 *
 * The maximum number of symbols names that can be stored. This includes:
 * - Task names
 * - Named ISRs (vTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channels (vTraceStoreUserEventChannelName)
 *
 * If this value is too small, not all symbol names will be stored and the
 * trace display will be affected. In that case, there will be warnings
 * (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_CFG_SYMBOL_TABLE_SLOTS 40

/*******************************************************************************
 * Configuration Macro: TRC_CFG_SYMBOL_MAX_LENGTH
 *
 * The maximum length of symbol names, including:
 * - Task names
 * - Named ISRs (vTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channel names (vTraceStoreUserEventChannelName)
 *
 * If longer symbol names are used, they will be truncated by the recorder,
 * which will affect the trace display. In that case, there will be warnings
 * (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_CFG_SYMBOL_MAX_LENGTH 25

/*******************************************************************************
 * Configuration Macro: TRC_CFG_OBJECT_DATA_SLOTS
 *
 * The maximum number of object data entries (used for task priorities) that can
 * be stored at the same time. Must be sufficient for all tasks, otherwise there
 * will be warnings (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_CFG_OBJECT_DATA_SLOTS 40

/*******************************************************************************
 * Configuration Macro: TRC_CFG_CTRL_TASK_STACK_SIZE
 *
 * The stack size of the TzCtrl task, that receive commands.
 * We are aiming to remove this extra task in future versions.
 ******************************************************************************/
#define TRC_CFG_CTRL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

/*******************************************************************************
 * Configuration Macro: TRC_CFG_CTRL_TASK_PRIORITY
 *
 * The priority of the TzCtrl task, that receive commands from Tracealyzer.
 * Most stream ports also rely on the TzCtrl task to transmit the data from the
 * internal buffer to the stream interface (all except for the J-Link port).
 * For such ports, make sure the TzCtrl priority is high enough to ensure
 * reliable periodic execution and transfer of the data.
 ******************************************************************************/
#define TRC_CFG_CTRL_TASK_PRIORITY 1

/*******************************************************************************
 * Configuration Macro: TRC_CFG_CTRL_TASK_DELAY
 *
 * The delay between every loop of the TzCtrl task. A high delay will reduce the
 * CPU load, but may cause missed events if the TzCtrl task is performing the 
 * trace transfer.
 ******************************************************************************/
#define TRC_CFG_CTRL_TASK_DELAY ((10 * configTICK_RATE_HZ) / 1000)

/*******************************************************************************
 * Configuration Macro: TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT
 *
 * Specifies the number of pages used by the paged event buffer.
 * This may need to be increased if there are a lot of missed events.
 *
 * Note: not used by the J-Link RTT stream port (see SEGGER_RTT_Conf.h instead)
 ******************************************************************************/
#define TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT 2

/*******************************************************************************
 * Configuration Macro: TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE
 *
 * Specifies the size of each page in the paged event buffer. This can be tuned 
 * to match any internal low-level buffers used by the streaming interface, like
 * the Ethernet MTU (Maximum Transmission Unit).
 *
 * Note: not used by the J-Link RTT stream port (see SEGGER_RTT_Conf.h instead)
 ******************************************************************************/
#define TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE 2500

/*******************************************************************************
 * TRC_CFG_ISR_TAILCHAINING_THRESHOLD
 *
 * Macro which should be defined as an integer value.
 *
 * If tracing multiple ISRs, this setting allows for accurate display of the 
 * context-switching also in cases when the ISRs execute in direct sequence.
 * 
 * vTraceStoreISREnd normally assumes that the ISR returns to the previous
 * context, i.e., a task or a preempted ISR. But if another traced ISR 
 * executes in direct sequence, Tracealyzer may incorrectly display a minimal
 * fragment of the previous context in between the ISRs.
 *
 * By using TRC_CFG_ISR_TAILCHAINING_THRESHOLD you can avoid this. This is 
 * however a threshold value that must be measured for your specific setup.
 * See http://percepio.com/2014/03/21/isr_tailchaining_threshold/
 *
 * The default setting is 0, meaning "disabled" and that you may get an 
 * extra fragments of the previous context in between tail-chained ISRs.
 *
 * Note: This setting has separate definitions in trcSnapshotConfig.h and 
 * trcStreamingConfig.h, since it is affected by the recorder mode.
 ******************************************************************************/
#define TRC_CFG_ISR_TAILCHAINING_THRESHOLD 0

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_CONFIG_H */
