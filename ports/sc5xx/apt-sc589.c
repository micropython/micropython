/* Copyright (c) 2015-2016, Analog Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*!
* @file      abstract_page_table.c
*
* @brief     Memory-management initialization API implementation
*
* @details
*            This is the table which defines the MMU attributes of memory.
*            This table should be kept in sync with your ARM and SHARC linker
*            description files.
*/

#include <runtime/mmu/adi_mmu.h>

/* See adi_mmu.h for definitions of the ADI_MMU_* memory class macros. */

/*#ifndef ADI_UNCACHED_SHARC_MEMORY
#define SHARC_L2 ADI_MMU_RO_CACHED
#define SHARC_L3 ADI_MMU_RO_CACHED
#else
#define SHARC_L2 ADI_MMU_RO_UNCACHED
#define SHARC_L3 ADI_MMU_RO_UNCACHED
#endif*/
#define SHARC_L2 ADI_MMU_RW_UNCACHED
#define SHARC_L3 ADI_MMU_RW_UNCACHED

/* ADI_CHANGES is defined for the default page table objects to ensure the
 * default symbols are weak.  Users should not define this macro when building
 * their application specific page table. */
#ifdef ADI_CHANGES
const adi_mmu_AbstractPageEntry _adi_mmu_absPageTable[] __attribute__ ((weak)) =
#else
const adi_mmu_AbstractPageEntry _adi_mmu_absPageTable[] =
#endif
{
    /* Boot ROM0 */
    { 0x00000000u, 0x00007FFFu, ADI_MMU_RO_CACHED           }, /* 32KB Boot ROM0 */

    /* L2 Cache Controller MMR space */
    { 0x10000000u, 0x10000FFFu, ADI_MMU_RW_DEVICE           }, /* 4KB L2CC MMRs */

    /* L2 SRAM */
    { 0x20080000u, 0x20080FFFu, ADI_MMU_RW_STRONGLY_ORDERED }, /* 4KB ICC */
    { 0x20081000u, 0x20081FFFu, ADI_MMU_RW_UNCACHED         }, /* 4KB MCAPI ARM */
    { 0x20082000u, 0x20083FFFu, ADI_MMU_RO_UNCACHED         }, /* 8KB MCAPI SHARC */
    { 0x20084000u, 0x20087FFFu, ADI_MMU_RW_UNCACHED         }, /* 16KB ARM uncached L2 */
#if defined(__ADSPSC582__)
    { 0x20088000u, 0x200A3FFFu, ADI_MMU_WB_CACHED           }, /* 112KB ARM cached L2 */
    { 0x200A4000u, 0x200BFFFFu, SHARC_L2                    }, /* 104KB SHARC0, & 8KB boot code working area */
#else
    { 0x20088000u, 0x2009FFFFu, ADI_MMU_WB_CACHED           }, /* 96KB ARM cached L2 */
    { 0x200A0000u, 0x200BFFFFu, SHARC_L2                    }, /* 120KB SHARC1, SHARC0, & 8KB boot code working area */
#endif

    /* Boot ROM1 */
    { 0x20100000u, 0x20107FFFu, ADI_MMU_RO_CACHED           }, /* 32KB Boot ROM1 */

    /* L2 ROM1 */
    { 0x20180000u, 0x201BFFFFu, ADI_MMU_RO_CACHED           }, /* 256KB L2 ROM1 */

    /* Boot ROM2 */
    { 0x20200000u, 0x20207FFFu, ADI_MMU_RO_CACHED           }, /* 32KB Boot ROM2 */

    /* L2 ROM2 */
    { 0x20280000u, 0x202BFFFFu, ADI_MMU_RO_CACHED           }, /* 256KB L2 ROM2 */

    /* otp space */
    { 0x24000000u, 0x240FFFFFu, ADI_MMU_RW_UNCACHED           }, /* 1MB OTP Space */

#if defined(__ADSPSC583__)
    /* L1 memory of SHARC0 in MP space via Slave1 port */
    { 0x28240000u, 0x2825FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B0 */

    { 0x282C0000u, 0x282DFFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B1 */

    { 0x28300000u, 0x2830FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC0 L1B2 */

    { 0x28380000u, 0x2838FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC0 L1B3 */

    /* L1 memory of SHARC0 in MP space via Slave2 port */
    { 0x28640000u, 0x2865FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B0 */

    { 0x286C0000u, 0x286DFFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B1 */

    { 0x28700000u, 0x2870FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC0 L1B2 */

    { 0x28780000u, 0x2878FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC0 L1B3 */

    /* L1 memory of SHARC1 in MP space via Slave1 port */
    { 0x28A40000u, 0x28A5FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B0 */

    { 0x28AC0000u, 0x28ADFFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B1 */

    { 0x28B00000u, 0x28B0FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC1 L1B2 */

    { 0x28B80000u, 0x28B8FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC1 L1B3 */

    /* L1 memory of SHARC1 in MP space via Slave2 port */
    { 0x28E40000u, 0x28E5FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B0 */

    { 0x28EC0000u, 0x28EDFFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B1 */

    { 0x28F00000u, 0x28F0FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC1 L1B2 */

    { 0x28F80000u, 0x28F8FFFFu, ADI_MMU_RW_UNCACHED         }, /*  64KB SHARC1 L1B3 */
#else
    /* L1 memory of SHARC0 in MP space via Slave1 port */
    { 0x28240000u, 0x2826FFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC0 L1B0 */

    { 0x282C0000u, 0x282EFFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC0 L1B1 */

    { 0x28300000u, 0x2831FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B2 */

    { 0x28380000u, 0x2839FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B3 */

    /* L1 memory of SHARC0 in MP space via Slave2 port */
    { 0x28640000u, 0x2866FFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC0 L1B0 */

    { 0x286C0000u, 0x286EFFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC0 L1B1 */

    { 0x28700000u, 0x2871FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B2 */

    { 0x28780000u, 0x2879FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC0 L1B3 */

    /* L1 memory of SHARC1 in MP space via Slave1 port */
    { 0x28A40000u, 0x28A6FFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC1 L1B0 */

    { 0x28AC0000u, 0x28AEFFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC1 L1B1 */

    { 0x28B00000u, 0x28B1FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B2 */

    { 0x28B80000u, 0x28B9FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B3 */

    /* L1 memory of SHARC1 in MP space via Slave2 port */
    { 0x28E40000u, 0x28E6FFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC1 L1B0 */

    { 0x28EC0000u, 0x28EEFFFFu, ADI_MMU_RW_UNCACHED         }, /* 192KB SHARC1 L1B1 */

    { 0x28F00000u, 0x28F1FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B2 */

    { 0x28F80000u, 0x28F9FFFFu, ADI_MMU_RW_UNCACHED         }, /* 128KB SHARC1 L1B3 */
#endif

    /* System MMR space */
    { 0x30000000u, 0x3FFFFFFFu, ADI_MMU_RW_DEVICE           }, /* 256MB System MMRs */

    /* Asynchronous memory */
    { 0x40000000u, 0x43FFFFFFu, ADI_MMU_RW_UNCACHED         }, /* 64MB Async memory bank 0 */
    { 0x44000000u, 0x47FFFFFFu, ADI_MMU_RW_UNCACHED         }, /* 64MB Async memory bank 1 */
    { 0x48000000u, 0x4BFFFFFFu, ADI_MMU_RW_UNCACHED         }, /* 64MB Async memory bank 2 */
    { 0x4C000000u, 0x4FFFFFFFu, ADI_MMU_RW_UNCACHED         }, /* 64MB Async memory bank 3 */

    /* PCI Express address space */
    { 0x50000000u, 0x5FFFFFFFu, ADI_MMU_RW_DEVICE           }, /* 256MB PCIe */

    /* SPI Flash memory-mapped address space */
    { 0x60000000u, 0x60FFFFFFu, ADI_MMU_RO_DEVICE           }, /* 16MB SPI flash */

#if defined(__ADSPSC582__) /* 1 SHARC core, 1 Dynamic Memory Controller */
    /* Dynamic Memory Controller 0 (DMC0) 256MB SDRAM */
    { 0x80000000u, 0x805FFFFFu, ADI_MMU_RW_UNCACHED         }, /*   6MB DDR-A : SHARC0 NW code   */
    { 0x80600000u, 0x809FFFFFu, SHARC_L3                    }, /*   4MB DDR-A : SHARC0 data      */
    { 0x80A00000u, 0x80FFFFFFu, ADI_MMU_RW_UNCACHED         }, /*   6MB DDR-A : SHARC0 VISA code */
    { 0x81000000u, 0x87FFFFFFu, SHARC_L3                    }, /* 112MB DDR-A : SHARC0 data      */
    { 0x88000000u, 0x8FFFFFFFu, ADI_MMU_WB_CACHED           }, /* 128MB DDR-A : ARM    code&data */
#elif defined(__ADSPSC583__) || defined(__ADSPSC584__) /* 2 SHARC cores, 1 Dynamic Memory Controller */
    /* Dynamic Memory Controller 0 (DMC0) 256MB SDRAM */
    { 0x80000000u, 0x802FFFFFu, ADI_MMU_RW_UNCACHED         }, /*   3MB DDR-A : SHARC0 NW code   */
    { 0x80300000u, 0x805FFFFFu, ADI_MMU_RW_UNCACHED         }, /*   3MB DDR-A : SHARC1 NW code   */
    { 0x80600000u, 0x809FFFFFu, SHARC_L3                    }, /*   4MB DDR-A : SHARC0 data      */
    { 0x80A00000u, 0x80CFFFFFu, ADI_MMU_RW_UNCACHED         }, /*   3MB DDR-A : SHARC0 VISA code */
    { 0x80D00000u, 0x80FFFFFFu, ADI_MMU_RW_UNCACHED         }, /*   3MB DDR-A : SHARC1 VISA code */
    { 0x81000000u, 0x84DFFFFFu, SHARC_L3                    }, /*  62MB DDR-A : SHARC0 data2     */
    { 0x84E00000u, 0x88FFFFFFu, SHARC_L3                    }, /*  66MB DDR-A : SHARC1 data2     */
    { 0x89000000u, 0x8FFFFFFFu, ADI_MMU_WB_CACHED           }, /* 112MB DDR-A : ARM    code&data */
#else                         /* 2 SHARC cores, 2 Dynamic Memory Controllers */
    /* Dynamic Memory Controller 0 (DMC0) 256MB SDRAM */
    { 0x80000000u, 0x805FFFFFu, ADI_MMU_RW_UNCACHED         }, /*   6MB DDR-A : SHARC0 NW code   */
    { 0x80600000u, 0x809FFFFFu, SHARC_L3                    }, /*   4MB DDR-A : SHARC0 data      */
    { 0x80A00000u, 0x80FFFFFFu, ADI_MMU_RW_UNCACHED         }, /*   6MB DDR-A : SHARC0 VISA code */
    { 0x81000000u, 0x88FFFFFFu, SHARC_L3                    }, /* 128MB DDR-A : SHARC0 data2     */
    { 0x89000000u, 0x8FFFFFFFu, SHARC_L3                    }, /* 112MB DDR-A : SHARC1 data2     */

    /* Dynamic Memory Controller 1 (DMC1) 256MB SDRAM */
    { 0xC0000000u, 0xC05FFFFFu, ADI_MMU_RW_UNCACHED         }, /*   6MB DDR-B : SHARC1 NW code   */
    { 0xC0600000u, 0xC09FFFFFu, SHARC_L3                    }, /*   4MB DDR-B : SHARC1 data      */
    { 0xC0A00000u, 0xC0FFFFFFu, ADI_MMU_RW_UNCACHED         }, /*   6MB DDR-B : SHARC1 VISA code */
    { 0xC1000000u, 0xCFFFFFFFu, ADI_MMU_WB_CACHED           }, /* 240MB DDR-B : ARM    code&data */
#endif
};

/* ADI_CHANGES is defined for the default page table objects to ensure the
 * default symbols are weak.  Users should not define this macro when building
 * their application specific page table. */
#ifdef ADI_CHANGES
const uint32_t _adi_mmu_absPageTableSize __attribute__ ((weak)) = sizeof(_adi_mmu_absPageTable) / sizeof(_adi_mmu_absPageTable[0]);
#else
const uint32_t _adi_mmu_absPageTableSize = sizeof(_adi_mmu_absPageTable) / sizeof(_adi_mmu_absPageTable[0]);
#endif
