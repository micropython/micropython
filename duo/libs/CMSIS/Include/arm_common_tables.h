/* ---------------------------------------------------------------------- 
* Copyright (C) 2010 ARM Limited. All rights reserved. 
* 
* $Date:        11. November 2010  
* $Revision: 	V1.0.2  
* 
* Project: 	    CMSIS DSP Library 
* Title:	    arm_common_tables.h 
* 
* Description:	This file has extern declaration for common tables like Bitreverse, reciprocal etc which are used across different functions 
* 
* Target Processor: Cortex-M4/Cortex-M3
*  
* Version 1.0.2 2010/11/11 
*    Documentation updated.  
* 
* Version 1.0.1 2010/10/05  
*    Production release and review comments incorporated. 
* 
* Version 1.0.0 2010/09/20  
*    Production release and review comments incorporated. 
* -------------------------------------------------------------------- */

#ifndef _ARM_COMMON_TABLES_H
#define _ARM_COMMON_TABLES_H

#include "arm_math.h"

extern const uint16_t armBitRevTable[1024];
extern const q15_t armRecipTableQ15[64];
extern const q31_t armRecipTableQ31[64];
extern const q31_t realCoefAQ31[1024];
extern const q31_t realCoefBQ31[1024];
extern const float32_t twiddleCoef[6144];
extern const q31_t twiddleCoefQ31[6144];
extern const q15_t twiddleCoefQ15[6144];

#endif /*  ARM_COMMON_TABLES_H */
