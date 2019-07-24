/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#define SDK_MEM_MAGIC_NUMBER 12345U

typedef struct _mem_align_control_block
{
    uint16_t identifier; /*!< Identifier for the memory control block. */
    uint16_t offset;     /*!< offset from aligned address to real address */
} mem_align_cb_t;

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.common"
#endif

#ifndef __GIC_PRIO_BITS
#if defined(ENABLE_RAM_VECTOR_TABLE)
uint32_t InstallIRQHandler(IRQn_Type irq, uint32_t irqHandler)
{
/* Addresses for VECTOR_TABLE and VECTOR_RAM come from the linker file */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
    extern uint32_t Image$$VECTOR_ROM$$Base[];
    extern uint32_t Image$$VECTOR_RAM$$Base[];
    extern uint32_t Image$$RW_m_data$$Base[];

#define __VECTOR_TABLE Image$$VECTOR_ROM$$Base
#define __VECTOR_RAM Image$$VECTOR_RAM$$Base
#define __RAM_VECTOR_TABLE_SIZE (((uint32_t)Image$$RW_m_data$$Base - (uint32_t)Image$$VECTOR_RAM$$Base))
#elif defined(__ICCARM__)
    extern uint32_t __RAM_VECTOR_TABLE_SIZE[];
    extern uint32_t __VECTOR_TABLE[];
    extern uint32_t __VECTOR_RAM[];
#elif defined(__GNUC__)
    extern uint32_t __VECTOR_TABLE[];
    extern uint32_t __VECTOR_RAM[];
    extern uint32_t __RAM_VECTOR_TABLE_SIZE_BYTES[];
    uint32_t __RAM_VECTOR_TABLE_SIZE = (uint32_t)(__RAM_VECTOR_TABLE_SIZE_BYTES);
#endif /* defined(__CC_ARM) || defined(__ARMCC_VERSION) */
    uint32_t n;
    uint32_t ret;
    uint32_t irqMaskValue;

    irqMaskValue = DisableGlobalIRQ();
    if (SCB->VTOR != (uint32_t)__VECTOR_RAM)
    {
        /* Copy the vector table from ROM to RAM */
        for (n = 0; n < ((uint32_t)__RAM_VECTOR_TABLE_SIZE) / sizeof(uint32_t); n++)
        {
            __VECTOR_RAM[n] = __VECTOR_TABLE[n];
        }
        /* Point the VTOR to the position of vector table */
        SCB->VTOR = (uint32_t)__VECTOR_RAM;
    }

    ret = __VECTOR_RAM[irq + 16];
    /* make sure the __VECTOR_RAM is noncachable */
    __VECTOR_RAM[irq + 16] = irqHandler;

    EnableGlobalIRQ(irqMaskValue);

/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif

    return ret;
}
#endif /* ENABLE_RAM_VECTOR_TABLE. */
#endif /* __GIC_PRIO_BITS. */

#if (defined(FSL_FEATURE_SOC_SYSCON_COUNT) && (FSL_FEATURE_SOC_SYSCON_COUNT > 0))
#if !(defined(FSL_FEATURE_SYSCON_STARTER_DISCONTINUOUS) && FSL_FEATURE_SYSCON_STARTER_DISCONTINUOUS)

void EnableDeepSleepIRQ(IRQn_Type interrupt)
{
    uint32_t intNumber = (uint32_t)interrupt;

    uint32_t index = 0;

    while (intNumber >= 32u)
    {
        index++;
        intNumber -= 32u;
    }

    SYSCON->STARTERSET[index] = 1u << intNumber;
    EnableIRQ(interrupt); /* also enable interrupt at NVIC */
}

void DisableDeepSleepIRQ(IRQn_Type interrupt)
{
    uint32_t intNumber = (uint32_t)interrupt;

    DisableIRQ(interrupt); /* also disable interrupt at NVIC */
    uint32_t index = 0;

    while (intNumber >= 32u)
    {
        index++;
        intNumber -= 32u;
    }

    SYSCON->STARTERCLR[index] = 1u << intNumber;
}
#endif /* FSL_FEATURE_SYSCON_STARTER_DISCONTINUOUS */
#endif /* FSL_FEATURE_SOC_SYSCON_COUNT */

void *SDK_Malloc(size_t size, size_t alignbytes)
{
    mem_align_cb_t *p_cb = NULL;
    uint32_t alignedsize = SDK_SIZEALIGN(size, alignbytes) + alignbytes + sizeof(mem_align_cb_t);
    void *p_align_addr, *p_addr = malloc(alignedsize);

    if (!p_addr)
    {
        return NULL;
    }

    p_align_addr = (void *)SDK_SIZEALIGN((uint32_t)p_addr + sizeof(mem_align_cb_t), alignbytes);

    p_cb             = (mem_align_cb_t *)((uint32_t)p_align_addr - 4U);
    p_cb->identifier = SDK_MEM_MAGIC_NUMBER;
    p_cb->offset     = (uint32_t)p_align_addr - (uint32_t)p_addr;

    return (void *)p_align_addr;
}

void SDK_Free(void *ptr)
{
    mem_align_cb_t *p_cb = (mem_align_cb_t *)((uint32_t)ptr - 4U);

    if (p_cb->identifier != SDK_MEM_MAGIC_NUMBER)
    {
        return;
    }

    free((void *)((uint32_t)ptr - p_cb->offset));
}
