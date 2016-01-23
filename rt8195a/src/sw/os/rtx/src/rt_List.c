/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_LIST.C
 *      Purpose: Functions for the management of different lists
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
#include "rt_List.h"
#include "rt_Task.h"
#include "rt_Time.h"
#include "rt_HAL_CM.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

/* List head of chained ready tasks */
struct OS_XCB  os_rdy;
/* List head of chained delay tasks */
struct OS_XCB  os_dly;


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_put_prio -----------------------------------*/

void rt_put_prio (P_XCB p_CB, P_TCB p_task) {
  /* Put task identified with "p_task" into list ordered by priority.       */
  /* "p_CB" points to head of list; list has always an element at end with  */
  /* a priority less than "p_task->prio".                                   */
  P_TCB p_CB2;
  U32 prio;
  BOOL sem_mbx = __FALSE;

  if (p_CB->cb_type == SCB || p_CB->cb_type == MCB || p_CB->cb_type == MUCB) {
    sem_mbx = __TRUE;
  }
  prio = p_task->prio;
  p_CB2 = p_CB->p_lnk;
  /* Search for an entry in the list */
  while (p_CB2 != NULL && prio <= p_CB2->prio) {
    p_CB = (P_XCB)p_CB2;
    p_CB2 = p_CB2->p_lnk;
  }
  /* Entry found, insert the task into the list */
  p_task->p_lnk = p_CB2;
  p_CB->p_lnk = p_task;
  if (sem_mbx) {
    if (p_CB2 != NULL) {
      p_CB2->p_rlnk = p_task;
    }
    p_task->p_rlnk = (P_TCB)p_CB;
  }
  else {
    p_task->p_rlnk = NULL;
  }
}


/*--------------------------- rt_get_first ----------------------------------*/

P_TCB rt_get_first (P_XCB p_CB) {
  /* Get task at head of list: it is the task with highest priority. */
  /* "p_CB" points to head of list. */
  P_TCB p_first;

  p_first = p_CB->p_lnk;
  p_CB->p_lnk = p_first->p_lnk;
  if (p_CB->cb_type == SCB || p_CB->cb_type == MCB || p_CB->cb_type == MUCB) {
    if (p_first->p_lnk != NULL) {
      p_first->p_lnk->p_rlnk = (P_TCB)p_CB;
      p_first->p_lnk = NULL;
    }
    p_first->p_rlnk = NULL;
  }
  else {
    p_first->p_lnk = NULL;
  }
  return (p_first);
}


/*--------------------------- rt_put_rdy_first ------------------------------*/

void rt_put_rdy_first (P_TCB p_task) {
  /* Put task identified with "p_task" at the head of the ready list. The   */
  /* task must have at least a priority equal to highest priority in list.  */
  p_task->p_lnk = os_rdy.p_lnk;
  p_task->p_rlnk = NULL;
  os_rdy.p_lnk = p_task;
}


/*--------------------------- rt_get_same_rdy_prio --------------------------*/

P_TCB rt_get_same_rdy_prio (void) {
  /* Remove a task of same priority from ready list if any exists. Other-   */
  /* wise return NULL.                                                      */
  P_TCB p_first;

  p_first = os_rdy.p_lnk;
  if (p_first->prio == os_tsk.run->prio) {
    os_rdy.p_lnk = os_rdy.p_lnk->p_lnk;
    return (p_first);
  }
  return (NULL);
}


/*--------------------------- rt_resort_prio --------------------------------*/

void rt_resort_prio (P_TCB p_task) {
  /* Re-sort ordered lists after the priority of 'p_task' has changed.      */
  P_TCB p_CB;

  if (p_task->p_rlnk == NULL) {
    if (p_task->state == READY) {
      /* Task is chained into READY list. */
      p_CB = (P_TCB)&os_rdy;
      goto res;
    }
  }
  else {
    p_CB = p_task->p_rlnk;
    while (p_CB->cb_type == TCB) {
      /* Find a header of this task chain list. */
      p_CB = p_CB->p_rlnk;
    }
res:rt_rmv_list (p_task);
    rt_put_prio ((P_XCB)p_CB, p_task);
  }
}


/*--------------------------- rt_put_dly ------------------------------------*/

void rt_put_dly (P_TCB p_task, U16 delay) {
  /* Put a task identified with "p_task" into chained delay wait list using */
  /* a delay value of "delay".                                              */
  P_TCB p;
  U32 delta,idelay = delay;

  p = (P_TCB)&os_dly;
  if (p->p_dlnk == NULL) {
    /* Delay list empty */
    delta = 0;
    goto last;
  }
  delta = os_dly.delta_time;
  while (delta < idelay) {
    if (p->p_dlnk == NULL) {
      /* End of list found */
last: p_task->p_dlnk = NULL;
      p->p_dlnk = p_task;
      p_task->p_blnk = p;
      p->delta_time = (U16)(idelay - delta);
      p_task->delta_time = 0;
      return;
    }
    p = p->p_dlnk;
    delta += p->delta_time;
  }
  /* Right place found */
  p_task->p_dlnk = p->p_dlnk;
  p->p_dlnk = p_task;
  p_task->p_blnk = p;
  if (p_task->p_dlnk != NULL) {
    p_task->p_dlnk->p_blnk = p_task;
  }
  p_task->delta_time = (U16)(delta - idelay);
  p->delta_time -= p_task->delta_time;
}


/*--------------------------- rt_dec_dly ------------------------------------*/

void rt_dec_dly (void) {
  /* Decrement delta time of list head: remove tasks having a value of zero.*/
  P_TCB p_rdy;

  if (os_dly.p_dlnk == NULL) {
    return;
  }
  os_dly.delta_time--;
  while ((os_dly.delta_time == 0) && (os_dly.p_dlnk != NULL)) {
    p_rdy = os_dly.p_dlnk;
    if (p_rdy->p_rlnk != NULL) {
      /* Task is really enqueued, remove task from semaphore/mailbox */
      /* timeout waiting list. */
      p_rdy->p_rlnk->p_lnk = p_rdy->p_lnk;
      if (p_rdy->p_lnk != NULL) {
        p_rdy->p_lnk->p_rlnk = p_rdy->p_rlnk;
        p_rdy->p_lnk = NULL;
      }
      p_rdy->p_rlnk = NULL;
    }
    rt_put_prio (&os_rdy, p_rdy);
    os_dly.delta_time = p_rdy->delta_time;
    if (p_rdy->state == WAIT_ITV) {
      /* Calculate the next time for interval wait. */
      p_rdy->delta_time = p_rdy->interval_time + (U16)os_time;
    }
    p_rdy->state   = READY;
    os_dly.p_dlnk = p_rdy->p_dlnk;
    if (p_rdy->p_dlnk != NULL) {
      p_rdy->p_dlnk->p_blnk =  (P_TCB)&os_dly;
      p_rdy->p_dlnk = NULL;
    }
    p_rdy->p_blnk = NULL;
  }
}


/*--------------------------- rt_rmv_list -----------------------------------*/

void rt_rmv_list (P_TCB p_task) {
  /* Remove task identified with "p_task" from ready, semaphore or mailbox  */
  /* waiting list if enqueued.                                              */
  P_TCB p_b;

  if (p_task->p_rlnk != NULL) {
    /* A task is enqueued in semaphore / mailbox waiting list. */
    p_task->p_rlnk->p_lnk = p_task->p_lnk;
    if (p_task->p_lnk != NULL) {
      p_task->p_lnk->p_rlnk = p_task->p_rlnk;
    }
    return;
  }

  p_b = (P_TCB)&os_rdy;
  while (p_b != NULL) {
    /* Search the ready list for task "p_task" */
    if (p_b->p_lnk == p_task) {
      p_b->p_lnk = p_task->p_lnk;
      return;
    }
    p_b = p_b->p_lnk;
  }
}


/*--------------------------- rt_rmv_dly ------------------------------------*/

void rt_rmv_dly (P_TCB p_task) {
  /* Remove task identified with "p_task" from delay list if enqueued.      */
  P_TCB p_b;

  p_b = p_task->p_blnk;
  if (p_b != NULL) {
    /* Task is really enqueued */
    p_b->p_dlnk = p_task->p_dlnk;
    if (p_task->p_dlnk != NULL) {
      /* 'p_task' is in the middle of list */
      p_b->delta_time += p_task->delta_time;
      p_task->p_dlnk->p_blnk = p_b;
      p_task->p_dlnk = NULL;
    }
    else {
      /* 'p_task' is at the end of list */
      p_b->delta_time = 0;
    }
    p_task->p_blnk = NULL;
  }
}


/*--------------------------- rt_psq_enq ------------------------------------*/

void rt_psq_enq (OS_ID entry, U32 arg) {
  /* Insert post service request "entry" into ps-queue. */
  U32 idx;

  idx = rt_inc_qi (os_psq->size, &os_psq->count, &os_psq->first);
  if (idx < os_psq->size) {
    os_psq->q[idx].id  = entry;
    os_psq->q[idx].arg = arg;
  }
  else {
    os_error (OS_ERR_FIFO_OVF);
  }
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

