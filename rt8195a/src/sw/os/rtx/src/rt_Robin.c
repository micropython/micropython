/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_ROBIN.C
 *      Purpose: Round Robin Task switching
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
#include "rt_List.h"
#include "rt_Task.h"
#include "rt_Time.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

struct OS_ROBIN os_robin;


/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- rt_init_robin ---------------------------------*/

__weak void rt_init_robin (void) {
  /* Initialize Round Robin variables. */
  os_robin.task = NULL;
  os_robin.tout = (U16)os_rrobin;
}

/*--------------------------- rt_chk_robin ----------------------------------*/

__weak void rt_chk_robin (void) {
  /* Check if Round Robin timeout expired and switch to the next ready task.*/
  P_TCB p_new;

  if (os_robin.task != os_rdy.p_lnk) {
    /* New task was suspended, reset Round Robin timeout. */
    os_robin.task = os_rdy.p_lnk;
    os_robin.time = (U16)os_time + os_robin.tout - 1;
  }
  if (os_robin.time == (U16)os_time) {
    /* Round Robin timeout has expired, swap Robin tasks. */
    os_robin.task = NULL;
    p_new = rt_get_first (&os_rdy);
    rt_put_prio ((P_XCB)&os_rdy, p_new);
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

