/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_TASK.C
 *      Purpose: Task functions and system start up.
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
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_MemBox.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

/* Running and next task info. */
struct OS_TSK os_tsk;

/* Task Control Blocks of idle demon */
struct OS_TCB os_idle_TCB;


/*----------------------------------------------------------------------------
 *      Local Functions
 *---------------------------------------------------------------------------*/

OS_TID rt_get_TID (void) {
  U32 tid;

  for (tid = 1; tid <= os_maxtaskrun; tid++) {
    if (os_active_TCB[tid-1] == NULL) {
      return ((OS_TID)tid);
    }
  }
  return (0);
}

#if defined (__CC_ARM) && !defined (__MICROLIB)
/*--------------------------- __user_perthread_libspace ---------------------*/
extern void  *__libspace_start;

void *__user_perthread_libspace (void) {
  /* Provide a separate libspace for each task. */
  if (os_tsk.run == NULL) {
    /* RTX not running yet. */
    return (&__libspace_start);
  }
  return (void *)(os_tsk.run->std_libspace);
}
#endif

/*--------------------------- rt_init_context -------------------------------*/

void rt_init_context (P_TCB p_TCB, U8 priority, FUNCP task_body) {
  /* Initialize general part of the Task Control Block. */
  p_TCB->cb_type = TCB;
  p_TCB->state   = READY;
  p_TCB->prio    = priority;
  p_TCB->p_lnk   = NULL;
  p_TCB->p_rlnk  = NULL;
  p_TCB->p_dlnk  = NULL;
  p_TCB->p_blnk  = NULL;
  p_TCB->delta_time    = 0;
  p_TCB->interval_time = 0;
  p_TCB->events  = 0;
  p_TCB->waits   = 0;
  p_TCB->stack_frame = 0;

  rt_init_stack (p_TCB, task_body);
}


/*--------------------------- rt_switch_req ---------------------------------*/

void rt_switch_req (P_TCB p_new) {
  /* Switch to next task (identified by "p_new"). */
  os_tsk.new_tsk   = p_new;
  p_new->state = RUNNING;
  DBG_TASK_SWITCH(p_new->task_id);
}


/*--------------------------- rt_dispatch -----------------------------------*/

void rt_dispatch (P_TCB next_TCB) {
  /* Dispatch next task if any identified or dispatch highest ready task    */
  /* "next_TCB" identifies a task to run or has value NULL (=no next task)  */
  if (next_TCB == NULL) {
    /* Running task was blocked: continue with highest ready task */
    next_TCB = rt_get_first (&os_rdy);
    rt_switch_req (next_TCB);
  }
  else {
    /* Check which task continues */
    if (next_TCB->prio > os_tsk.run->prio) {
      /* preempt running task */
      rt_put_rdy_first (os_tsk.run);
      os_tsk.run->state = READY;
      rt_switch_req (next_TCB);
    }
    else {
      /* put next task into ready list, no task switch takes place */
      next_TCB->state = READY;
      rt_put_prio (&os_rdy, next_TCB);
    }
  }
}


/*--------------------------- rt_block --------------------------------------*/

void rt_block (U16 timeout, U8 block_state) {
  /* Block running task and choose next ready task.                         */
  /* "timeout" sets a time-out value or is 0xffff (=no time-out).           */
  /* "block_state" defines the appropriate task state */
  P_TCB next_TCB;

  if (timeout) {
    if (timeout < 0xffff) {
      rt_put_dly (os_tsk.run, timeout);
    }
    os_tsk.run->state = block_state;
    next_TCB = rt_get_first (&os_rdy);
    rt_switch_req (next_TCB);
  }
}


/*--------------------------- rt_tsk_pass -----------------------------------*/

void rt_tsk_pass (void) {
  /* Allow tasks of same priority level to run cooperatively.*/
  P_TCB p_new;

  p_new = rt_get_same_rdy_prio();
  if (p_new != NULL) {
    rt_put_prio ((P_XCB)&os_rdy, os_tsk.run);
    os_tsk.run->state = READY;
    rt_switch_req (p_new);
  }
}


/*--------------------------- rt_tsk_self -----------------------------------*/

OS_TID rt_tsk_self (void) {
  /* Return own task identifier value. */
  if (os_tsk.run == NULL) {
    return (0);
  }
  return (os_tsk.run->task_id);
}


/*--------------------------- rt_tsk_prio -----------------------------------*/

OS_RESULT rt_tsk_prio (OS_TID task_id, U8 new_prio) {
  /* Change execution priority of a task to "new_prio". */
  P_TCB p_task;

  if (task_id == 0) {
    /* Change execution priority of calling task. */
    os_tsk.run->prio = new_prio;
run:if (rt_rdy_prio() > new_prio) {
      rt_put_prio (&os_rdy, os_tsk.run);
      os_tsk.run->state   = READY;
      rt_dispatch (NULL);
    }
    return (OS_R_OK);
  }

  /* Find the task in the "os_active_TCB" array. */
  if (task_id > os_maxtaskrun || os_active_TCB[task_id-1] == NULL) {
    /* Task with "task_id" not found or not started. */
    return (OS_R_NOK);
  }
  p_task = os_active_TCB[task_id-1];
  p_task->prio = new_prio;
  if (p_task == os_tsk.run) {
    goto run;
  }
  rt_resort_prio (p_task);
  if (p_task->state == READY) {
    /* Task enqueued in a ready list. */
    p_task = rt_get_first (&os_rdy);
    rt_dispatch (p_task);
  }
  return (OS_R_OK);
}

/*--------------------------- rt_tsk_delete ---------------------------------*/

OS_RESULT rt_tsk_delete (OS_TID task_id) {
  /* Terminate the task identified with "task_id". */
  P_TCB task_context;

  if (task_id == 0 || task_id == os_tsk.run->task_id) {
    /* Terminate itself. */
    os_tsk.run->state     = INACTIVE;
    os_tsk.run->tsk_stack = rt_get_PSP ();
    rt_stk_check ();
    os_active_TCB[os_tsk.run->task_id-1] = NULL;

    os_tsk.run->stack = NULL;
    DBG_TASK_NOTIFY(os_tsk.run, __FALSE);
    os_tsk.run = NULL;
    rt_dispatch (NULL);
    /* The program should never come to this point. */
  }
  else {
    /* Find the task in the "os_active_TCB" array. */
    if (task_id > os_maxtaskrun || os_active_TCB[task_id-1] == NULL) {
      /* Task with "task_id" not found or not started. */
      return (OS_R_NOK);
    }
    task_context = os_active_TCB[task_id-1];
    rt_rmv_list (task_context);
    rt_rmv_dly (task_context);
    os_active_TCB[task_id-1] = NULL;

    task_context->stack = NULL;
    DBG_TASK_NOTIFY(task_context, __FALSE);
  }
  return (OS_R_OK);
}


/*--------------------------- rt_sys_init -----------------------------------*/

#ifdef __CMSIS_RTOS
void rt_sys_init (void) {
#else
void rt_sys_init (FUNCP first_task, U32 prio_stksz, void *stk) {
#endif
  /* Initialize system and start up task declared with "first_task". */
  U32 i;

  DBG_INIT();

  /* Initialize dynamic memory and task TCB pointers to NULL. */
  for (i = 0; i < os_maxtaskrun; i++) {
    os_active_TCB[i] = NULL;
  }

  /* Set up TCB of idle demon */
  os_idle_TCB.task_id = 255;
  os_idle_TCB.priv_stack = idle_task_stack_size;
  os_idle_TCB.stack = idle_task_stack;
  rt_init_context (&os_idle_TCB, 0, os_idle_demon);

  /* Set up ready list: initially empty */
  os_rdy.cb_type = HCB;
  os_rdy.p_lnk   = NULL;
  /* Set up delay list: initially empty */
  os_dly.cb_type = HCB;
  os_dly.p_dlnk  = NULL;
  os_dly.p_blnk  = NULL;
  os_dly.delta_time = 0;

  /* Fix SP and systemvariables to assume idle task is running  */
  /* Transform main program into idle task by assuming idle TCB */
#ifndef __CMSIS_RTOS
  rt_set_PSP (os_idle_TCB.tsk_stack+32);
#endif
  os_tsk.run = &os_idle_TCB;
  os_tsk.run->state = RUNNING;

  /* Initialize ps queue */
  os_psq->first = 0;
  os_psq->last  = 0;
  os_psq->size  = os_fifo_size;

  rt_init_robin ();

  /* Intitialize SVC and PendSV */
  rt_svc_init ();

#ifndef __CMSIS_RTOS
  /* Intitialize and start system clock timer */
  os_tick_irqn = os_tick_init ();
  if (os_tick_irqn >= 0) {
    OS_X_INIT(os_tick_irqn);
  }

  /* Start up first user task before entering the endless loop */
  rt_tsk_create (first_task, prio_stksz, stk, NULL);
#endif
}


/*--------------------------- rt_sys_start ----------------------------------*/

#ifdef __CMSIS_RTOS
void rt_sys_start (void) {
  /* Start system */

  /* Intitialize and start system clock timer */
  os_tick_irqn = os_tick_init ();
  if (os_tick_irqn >= 0) {
    OS_X_INIT(os_tick_irqn);
  }
}
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
