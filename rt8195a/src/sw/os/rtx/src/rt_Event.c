/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_EVENT.C
 *      Purpose: Implements waits and wake-ups for event flags
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

#include "rt_TypeDef.h"
#include "RTX_Conf.h"
#include "rt_System.h"
#include "rt_Event.h"
#include "rt_List.h"
#include "rt_Task.h"
#include "rt_HAL_CM.h"


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_evt_wait -----------------------------------*/

OS_RESULT rt_evt_wait (U16 wait_flags, U16 timeout, BOOL and_wait) {
  /* Wait for one or more event flags with optional time-out.                */
  /* "wait_flags" identifies the flags to wait for.                          */
  /* "timeout" is the time-out limit in system ticks (0xffff if no time-out) */
  /* "and_wait" specifies the AND-ing of "wait_flags" as condition to be met */
  /* to complete the wait. (OR-ing if set to 0).                             */
  U32 block_state;

  if (and_wait) {
    /* Check for AND-connected events */
    if ((os_tsk.run->events & wait_flags) == wait_flags) {
      os_tsk.run->events &= ~wait_flags;
      return (OS_R_EVT);
    }
    block_state = WAIT_AND;
  }
  else {
    /* Check for OR-connected events */
    if (os_tsk.run->events & wait_flags) {
      os_tsk.run->waits = os_tsk.run->events & wait_flags;
      os_tsk.run->events &= ~wait_flags;
      return (OS_R_EVT);
    }
    block_state = WAIT_OR;
  }
  /* Task has to wait */
  os_tsk.run->waits = wait_flags;
  rt_block (timeout, (U8)block_state);
  return (OS_R_TMO);
}


/*--------------------------- rt_evt_set ------------------------------------*/

void rt_evt_set (U16 event_flags, OS_TID task_id) {
  /* Set one or more event flags of a selectable task. */
  P_TCB p_tcb;

  p_tcb = os_active_TCB[task_id-1];
  if (p_tcb == NULL) {
    return;
  }
  p_tcb->events |= event_flags;
  event_flags    = p_tcb->waits;
  /* If the task is not waiting for an event, it should not be put */
  /* to ready state. */
  if (p_tcb->state == WAIT_AND) {
    /* Check for AND-connected events */
    if ((p_tcb->events & event_flags) == event_flags) {
      goto wkup;
    }
  }
  if (p_tcb->state == WAIT_OR) {
    /* Check for OR-connected events */
    if (p_tcb->events & event_flags) {
      p_tcb->waits  &= p_tcb->events;
wkup: p_tcb->events &= ~event_flags;
      rt_rmv_dly (p_tcb);
      p_tcb->state   = READY;
#ifdef __CMSIS_RTOS
      rt_ret_val2(p_tcb, 0x08/*osEventSignal*/, p_tcb->waits);
#else
      rt_ret_val (p_tcb, OS_R_EVT);
#endif
      rt_dispatch (p_tcb);
    }
  }
}


/*--------------------------- rt_evt_clr ------------------------------------*/

void rt_evt_clr (U16 clear_flags, OS_TID task_id) {
  /* Clear one or more event flags (identified by "clear_flags") of a */
  /* selectable task (identified by "task"). */
  P_TCB task = os_active_TCB[task_id-1];

  if (task == NULL) {
    return;
  }
  task->events &= ~clear_flags;
}


/*--------------------------- isr_evt_set -----------------------------------*/

void isr_evt_set (U16 event_flags, OS_TID task_id) {
  /* Same function as "os_evt_set", but to be called by ISRs. */
  P_TCB p_tcb = os_active_TCB[task_id-1];

  if (p_tcb == NULL) {
    return;
  }
  rt_psq_enq (p_tcb, event_flags);
  rt_psh_req ();
}


/*--------------------------- rt_evt_get ------------------------------------*/

U16 rt_evt_get (void) {
  /* Get events of a running task after waiting for OR connected events. */
  return (os_tsk.run->waits);
}


/*--------------------------- rt_evt_psh ------------------------------------*/

void rt_evt_psh (P_TCB p_CB, U16 set_flags) {
  /* Check if task has to be waken up */
  U16 event_flags;

  p_CB->events |= set_flags;
  event_flags = p_CB->waits;
  if (p_CB->state == WAIT_AND) {
    /* Check for AND-connected events */
    if ((p_CB->events & event_flags) == event_flags) {
      goto rdy;
    }
  }
  if (p_CB->state == WAIT_OR) {
    /* Check for OR-connected events */
    if (p_CB->events & event_flags) {
      p_CB->waits  &= p_CB->events;
rdy:  p_CB->events &= ~event_flags;
      rt_rmv_dly (p_CB);
      p_CB->state   = READY;
#ifdef __CMSIS_RTOS
      rt_ret_val2(p_CB, 0x08/*osEventSignal*/, p_CB->waits);
#else
      rt_ret_val (p_CB, OS_R_EVT);
#endif
      rt_put_prio (&os_rdy, p_CB);
    }
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
