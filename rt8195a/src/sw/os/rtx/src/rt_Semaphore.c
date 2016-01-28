/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_SEMAPHORE.C
 *      Purpose: Implements binary and counting semaphores
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
#include "rt_Semaphore.h"
#include "rt_HAL_CM.h"


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_sem_init -----------------------------------*/

void rt_sem_init (OS_ID semaphore, U16 token_count) {
  /* Initialize a semaphore */
  P_SCB p_SCB = semaphore;

  p_SCB->cb_type = SCB;
  p_SCB->p_lnk  = NULL;
  p_SCB->tokens = token_count;
}


/*--------------------------- rt_sem_delete ---------------------------------*/

#ifdef __CMSIS_RTOS
OS_RESULT rt_sem_delete (OS_ID semaphore) {
  /* Delete semaphore */
  P_SCB p_SCB = semaphore;
  P_TCB p_TCB;

  while (p_SCB->p_lnk != NULL) {
    /* A task is waiting for token */
    p_TCB = rt_get_first ((P_XCB)p_SCB);
    rt_ret_val(p_TCB, 0);
    rt_rmv_dly(p_TCB);
    p_TCB->state = READY;
    rt_put_prio (&os_rdy, p_TCB);
  }

  if (os_rdy.p_lnk && (os_rdy.p_lnk->prio > os_tsk.run->prio)) {
    /* preempt running task */
    rt_put_prio (&os_rdy, os_tsk.run);
    os_tsk.run->state = READY;
    rt_dispatch (NULL);
  }

  p_SCB->cb_type = 0;

  return (OS_R_OK);
}
#endif


/*--------------------------- rt_sem_send -----------------------------------*/

OS_RESULT rt_sem_send (OS_ID semaphore) {
  /* Return a token to semaphore */
  P_SCB p_SCB = semaphore;
  P_TCB p_TCB;

  if (p_SCB->p_lnk != NULL) {
    /* A task is waiting for token */
    p_TCB = rt_get_first ((P_XCB)p_SCB);
#ifdef __CMSIS_RTOS
    rt_ret_val(p_TCB, 1);
#else
    rt_ret_val(p_TCB, OS_R_SEM);
#endif
    rt_rmv_dly (p_TCB);
    rt_dispatch (p_TCB);
  }
  else {
    /* Store token. */
    p_SCB->tokens++;
  }
  return (OS_R_OK);
}


/*--------------------------- rt_sem_wait -----------------------------------*/

OS_RESULT rt_sem_wait (OS_ID semaphore, U16 timeout) {
  /* Obtain a token; possibly wait for it */
  P_SCB p_SCB = semaphore;

  if (p_SCB->tokens) {
    p_SCB->tokens--;
    return (OS_R_OK);
  }
  /* No token available: wait for one */
  if (timeout == 0) {
    return (OS_R_TMO);
  }
  if (p_SCB->p_lnk != NULL) {
    rt_put_prio ((P_XCB)p_SCB, os_tsk.run);
  }
  else {
    p_SCB->p_lnk = os_tsk.run;
    os_tsk.run->p_lnk = NULL;
    os_tsk.run->p_rlnk = (P_TCB)p_SCB;
  }
  rt_block(timeout, WAIT_SEM);
  return (OS_R_TMO);
}


/*--------------------------- isr_sem_send ----------------------------------*/

void isr_sem_send (OS_ID semaphore) {
  /* Same function as "os_sem"send", but to be called by ISRs */
  P_SCB p_SCB = semaphore;

  rt_psq_enq (p_SCB, 0);
  rt_psh_req ();
}


/*--------------------------- rt_sem_psh ------------------------------------*/

void rt_sem_psh (P_SCB p_CB) {
  /* Check if task has to be waken up */
  P_TCB p_TCB;

  if (p_CB->p_lnk != NULL) {
    /* A task is waiting for token */
    p_TCB = rt_get_first ((P_XCB)p_CB);
    rt_rmv_dly (p_TCB);
    p_TCB->state   = READY;
#ifdef __CMSIS_RTOS
    rt_ret_val(p_TCB, 1);
#else
    rt_ret_val(p_TCB, OS_R_SEM);
#endif
    rt_put_prio (&os_rdy, p_TCB);
  }
  else {
    /* Store token */
    p_CB->tokens++;
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

