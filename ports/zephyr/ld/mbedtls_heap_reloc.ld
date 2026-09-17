/*
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define MBEDTLS_HEAP_NODE DT_CHOSEN(zephyr_mbedtls_heap)

SECTION_PROLOGUE(.mbedtls_heap,(NOLOAD),)
{
	. = ALIGN (CONFIG_DCACHE_LINE_SIZE);
	KEEP(*(.mbedtls_heap))
	KEEP(*(.mbedtls_heap*))
} GROUP_LINK_IN(LINKER_DT_NODE_REGION_NAME(DT_PROP(MBEDTLS_HEAP_NODE, memory_region)))
