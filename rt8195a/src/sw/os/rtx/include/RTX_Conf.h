/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.H
 *      Purpose: Exported functions of RTX_Config.c
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2012 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/


/* Error Codes */
#define OS_ERR_STK_OVF          1
#define OS_ERR_FIFO_OVF         2
#define OS_ERR_MBX_OVF          3

/* Definitions */
#define BOX_ALIGN_8                   0x80000000
#define _declare_box(pool,size,cnt)   U32 pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt)  U64 pool[(((size)+7)/8)*(cnt) + 2]
#define _init_box8(pool,size,bsize)   _init_box (pool,size,(bsize) | BOX_ALIGN_8)

/* Variables */
extern U32 idle_task_stack[];
extern U32 os_fifo[];
extern void *os_active_TCB[];

/* Constants */
extern U16 const os_maxtaskrun;
extern U32 const os_trv;
extern U8  const os_flags;
extern U32 const os_rrobin;
extern U32 const os_clockrate;
extern U32 const os_timernum;
extern U16 const idle_task_stack_size;

extern U8  const os_fifo_size;

/* Functions */
extern void os_idle_demon   (void);
extern int  os_tick_init    (void);
extern void os_tick_irqack  (void);
extern void os_tmr_call     (U16  info);
extern void os_error        (U32 err_code);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
