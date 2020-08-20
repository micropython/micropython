/****************************************************************************
 * tools/cxd56/clefia.h
 *
 * Copyright (C) 2007, 2008 Sony Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef _TOOLS_CXD56_CLEFIA_H_
#define _TOOLS_CXD56_CLEFIA_H_

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct cipher
  {
    int mode;
    int dir;
    uint8_t rk[8 * 26 + 16];
    uint8_t vector[16];
    int round;
    uint8_t k1[16];
    uint8_t k2[16];
  };

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

struct cipher *cipher_init(uint8_t * key, uint8_t * iv);
void cipher_deinit(struct cipher *c);
int cipher_calc_cmac(struct cipher *c, void *data, int size, void *cmac);
void bytexor(unsigned char *dst, const unsigned char *a,
             const unsigned char *b, int bytelen);
int clefiakeyset(unsigned char *rk, const unsigned char *skey);
void clefiaencrypt(unsigned char *ct, const unsigned char *pt,
                   const unsigned char *rk, const int r);

#endif
