/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021-2023 Renesas Electronics Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef RA_RA_UTILS_H_
#define RA_RA_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

__attribute__((always_inline)) static inline void ra_enable_irq(__attribute__((unused)) uint32_t state) {
    __asm__ volatile ("cpsie f"
        :
        :
        : "memory");
}

__attribute__((always_inline)) static inline uint32_t ra_disable_irq(void) {
    uint32_t state;
    __asm__ volatile ("cpsid f"
        : "=r" (state)
        :
        : "memory");
    return state;
}

void ra_mstpcra_stop(uint32_t mod_mask);
void ra_mstpcra_start(uint32_t mod_mask);
void ra_mstpcrb_stop(uint32_t mod_mask);
void ra_mstpcrb_start(uint32_t mod_mask);
void ra_mstpcrc_stop(uint32_t mod_mask);
void ra_mstpcrc_start(uint32_t mod_mask);
void ra_mstpcrd_stop(uint32_t mod_mask);
void ra_mstpcrd_start(uint32_t mod_mask);
void ra_mstpcre_stop(uint32_t mod_mask);
void ra_mstpcre_start(uint32_t mod_mask);

/* mask bit definition for Module Stop Control Register B */
#ifndef R_MSTP_MSTPCRB_MSTPB7_Msk
#define R_MSTP_MSTPCRB_MSTPB7_Msk    (0x1UL << 7)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB8_Msk
#define R_MSTP_MSTPCRB_MSTPB8_Msk    (0x1UL << 8)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB9_Msk
#define R_MSTP_MSTPCRB_MSTPB9_Msk    (0x1UL << 9)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB18_Msk
#define R_MSTP_MSTPCRB_MSTPB18_Msk    (0x1UL << 18)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB19_Msk
#define R_MSTP_MSTPCRB_MSTPB19_Msk    (0x1UL << 19)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB22_Msk
#define R_MSTP_MSTPCRB_MSTPB22_Msk    (0x1UL << 22)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB23_Msk
#define R_MSTP_MSTPCRB_MSTPB23_Msk    (0x1UL << 23)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB24_Msk
#define R_MSTP_MSTPCRB_MSTPB24_Msk    (0x1UL << 24)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB25_Msk
#define R_MSTP_MSTPCRB_MSTPB25_Msk    (0x1UL << 25)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB26_Msk
#define R_MSTP_MSTPCRB_MSTPB26_Msk    (0x1UL << 26)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB27_Msk
#define R_MSTP_MSTPCRB_MSTPB27_Msk    (0x1UL << 27)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB28_Msk
#define R_MSTP_MSTPCRB_MSTPB28_Msk    (0x1UL << 28)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB29_Msk
#define R_MSTP_MSTPCRB_MSTPB29_Msk    (0x1UL << 29)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB30_Msk
#define R_MSTP_MSTPCRB_MSTPB30_Msk    (0x1UL << 30)
#endif
#ifndef R_MSTP_MSTPCRB_MSTPB31_Msk
#define R_MSTP_MSTPCRB_MSTPB31_Msk    (0x1UL << 31)
#endif

/* mask bit definition for Module Stop Control Register D */
#ifndef R_MSTP_MSTPCRD_MSTPD2_Msk
#define R_MSTP_MSTPCRD_MSTPD2_Msk    (0x1UL << 2)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD3_Msk
#define R_MSTP_MSTPCRD_MSTPD3_Msk    (0x1UL << 3)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD5_Msk
#define R_MSTP_MSTPCRD_MSTPD5_Msk    (0x1UL << 5)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD6_Msk
#define R_MSTP_MSTPCRD_MSTPD6_Msk    (0x1UL << 6)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD15_Msk
#define R_MSTP_MSTPCRD_MSTPD15_Msk    (0x1UL << 15)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD16_Msk
#define R_MSTP_MSTPCRD_MSTPD16_Msk    (0x1UL << 16)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD20_Msk
#define R_MSTP_MSTPCRD_MSTPD20_Msk    (0x1UL << 20)
#endif
#ifndef R_MSTP_MSTPCRD_MSTPD22_Msk
#define R_MSTP_MSTPCRD_MSTPD22_Msk    (0x1UL << 22)
#endif

#endif /* RA_RA_UTILS_H_ */
