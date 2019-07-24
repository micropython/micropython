/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __FSL_DEVICE_REGISTERS_H__
#define __FSL_DEVICE_REGISTERS_H__

/*
 * Include the cpu specific register header files.
 *
 * The CPU macro should be declared in the project or makefile.
 */
#if (defined(CPU_MIMXRT1064CVJ5A) || defined(CPU_MIMXRT1064CVL5A) || defined(CPU_MIMXRT1064DVJ6A) || \
    defined(CPU_MIMXRT1064DVL6A))

#define MIMXRT1064_SERIES

/* CMSIS-style register definitions */
#include "MIMXRT1064.h"
/* CPU specific feature definitions */
#include "MIMXRT1064_features.h"

#else
    #error "No valid CPU defined!"
#endif

#endif /* __FSL_DEVICE_REGISTERS_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
