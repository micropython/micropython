/* ----------------------------------------------------------------------
* Copyright (C) 2010-2013 ARM Limited. All rights reserved.
*
* $Date:        17. January 2013
* $Revision:    V1.4.1
*
* Project:      CMSIS DSP Library
* Title:        arm_const_structs.h
*
* Description:  This file has constant structs that are initialized for
*               user convenience.  For example, some can be given as
*               arguments to the arm_cfft_f32() function.
*
* Target Processor: Cortex-M4/Cortex-M3
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* -------------------------------------------------------------------- */

#ifndef _ARM_CONST_STRUCTS_H
#define _ARM_CONST_STRUCTS_H

#include "arm_math.h"
#include "arm_common_tables.h"

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len16 = {
      16, twiddleCoef_16, armBitRevIndexTable16, ARMBITREVINDEXTABLE__16_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len32 = {
      32, twiddleCoef_32, armBitRevIndexTable32, ARMBITREVINDEXTABLE__32_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len64 = {
      64, twiddleCoef_64, armBitRevIndexTable64, ARMBITREVINDEXTABLE__64_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len128 = {
      128, twiddleCoef_128, armBitRevIndexTable128, ARMBITREVINDEXTABLE_128_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len256 = {
      256, twiddleCoef_256, armBitRevIndexTable256, ARMBITREVINDEXTABLE_256_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len512 = {
      512, twiddleCoef_512, armBitRevIndexTable512, ARMBITREVINDEXTABLE_512_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len1024 = {
      1024, twiddleCoef_1024, armBitRevIndexTable1024, ARMBITREVINDEXTABLE1024_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len2048 = {
      2048, twiddleCoef_2048, armBitRevIndexTable2048, ARMBITREVINDEXTABLE2048_TABLE_LENGTH
   };

   const arm_cfft_instance_f32 arm_cfft_sR_f32_len4096 = {
      4096, twiddleCoef_4096, armBitRevIndexTable4096, ARMBITREVINDEXTABLE4096_TABLE_LENGTH
   };

#endif
