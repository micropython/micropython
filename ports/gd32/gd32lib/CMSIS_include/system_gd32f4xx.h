/*!
    \file  system_gd32f4xx.h
    \brief CMSIS Cortex-M4 Device Peripheral Access Layer Header File for
           GD32F4xx Device Series
*/

/* Copyright (c) 2012 ARM LIMITED
   Copyright (c) 2026, GigaDevice Semiconductor Inc.

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/* This file refers the CMSIS standard, some adjustments are made according to GigaDevice chips */

#ifndef SYSTEM_GD32F4XX_H
#define SYSTEM_GD32F4XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/* firmware version can be aquired by uncommenting the macro */
#define __FIRMWARE_VERSION_DEFINE

/* system clock frequency (core clock) */
extern uint32_t SystemCoreClock;

/* function declarations */
/* initialize the system and update the SystemCoreClock variable */
extern void SystemInit (void);
/* update the SystemCoreClock with current core clock retrieved from cpu registers */
extern void SystemCoreClockUpdate (void);
#ifdef __FIRMWARE_VERSION_DEFINE
/* get firmware version */
extern uint32_t gd32f4xx_firmware_version_get(void);
#endif /* __FIRMWARE_VERSION_DEFINE */

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_GD32F4XX_H */
