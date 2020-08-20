/****************************************************************************
 * tools/cxd56/clefia.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "clefia.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define clefiamul4(_x) (clefiamul2(clefiamul2((_x))))
#define clefiamul6(_x) (clefiamul2((_x)) ^ clefiamul4((_x)))
#define clefiamul8(_x) (clefiamul2(clefiamul4((_x))))
#define clefiamula(_x) (clefiamul2((_x)) ^ clefiamul8((_x)))

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* S0 (8-bit S-box based on four 4-bit S-boxes) */

static const unsigned char clefia_s0[256] =
{
  0x57u, 0x49u, 0xd1u, 0xc6u, 0x2fu, 0x33u, 0x74u, 0xfbu,
  0x95u, 0x6du, 0x82u, 0xeau, 0x0eu, 0xb0u, 0xa8u, 0x1cu,
  0x28u, 0xd0u, 0x4bu, 0x92u, 0x5cu, 0xeeu, 0x85u, 0xb1u,
  0xc4u, 0x0au, 0x76u, 0x3du, 0x63u, 0xf9u, 0x17u, 0xafu,
  0xbfu, 0xa1u, 0x19u, 0x65u, 0xf7u, 0x7au, 0x32u, 0x20u,
  0x06u, 0xceu, 0xe4u, 0x83u, 0x9du, 0x5bu, 0x4cu, 0xd8u,
  0x42u, 0x5du, 0x2eu, 0xe8u, 0xd4u, 0x9bu, 0x0fu, 0x13u,
  0x3cu, 0x89u, 0x67u, 0xc0u, 0x71u, 0xaau, 0xb6u, 0xf5u,
  0xa4u, 0xbeu, 0xfdu, 0x8cu, 0x12u, 0x00u, 0x97u, 0xdau,
  0x78u, 0xe1u, 0xcfu, 0x6bu, 0x39u, 0x43u, 0x55u, 0x26u,
  0x30u, 0x98u, 0xccu, 0xddu, 0xebu, 0x54u, 0xb3u, 0x8fu,
  0x4eu, 0x16u, 0xfau, 0x22u, 0xa5u, 0x77u, 0x09u, 0x61u,
  0xd6u, 0x2au, 0x53u, 0x37u, 0x45u, 0xc1u, 0x6cu, 0xaeu,
  0xefu, 0x70u, 0x08u, 0x99u, 0x8bu, 0x1du, 0xf2u, 0xb4u,
  0xe9u, 0xc7u, 0x9fu, 0x4au, 0x31u, 0x25u, 0xfeu, 0x7cu,
  0xd3u, 0xa2u, 0xbdu, 0x56u, 0x14u, 0x88u, 0x60u, 0x0bu,
  0xcdu, 0xe2u, 0x34u, 0x50u, 0x9eu, 0xdcu, 0x11u, 0x05u,
  0x2bu, 0xb7u, 0xa9u, 0x48u, 0xffu, 0x66u, 0x8au, 0x73u,
  0x03u, 0x75u, 0x86u, 0xf1u, 0x6au, 0xa7u, 0x40u, 0xc2u,
  0xb9u, 0x2cu, 0xdbu, 0x1fu, 0x58u, 0x94u, 0x3eu, 0xedu,
  0xfcu, 0x1bu, 0xa0u, 0x04u, 0xb8u, 0x8du, 0xe6u, 0x59u,
  0x62u, 0x93u, 0x35u, 0x7eu, 0xcau, 0x21u, 0xdfu, 0x47u,
  0x15u, 0xf3u, 0xbau, 0x7fu, 0xa6u, 0x69u, 0xc8u, 0x4du,
  0x87u, 0x3bu, 0x9cu, 0x01u, 0xe0u, 0xdeu, 0x24u, 0x52u,
  0x7bu, 0x0cu, 0x68u, 0x1eu, 0x80u, 0xb2u, 0x5au, 0xe7u,
  0xadu, 0xd5u, 0x23u, 0xf4u, 0x46u, 0x3fu, 0x91u, 0xc9u,
  0x6eu, 0x84u, 0x72u, 0xbbu, 0x0du, 0x18u, 0xd9u, 0x96u,
  0xf0u, 0x5fu, 0x41u, 0xacu, 0x27u, 0xc5u, 0xe3u, 0x3au,
  0x81u, 0x6fu, 0x07u, 0xa3u, 0x79u, 0xf6u, 0x2du, 0x38u,
  0x1au, 0x44u, 0x5eu, 0xb5u, 0xd2u, 0xecu, 0xcbu, 0x90u,
  0x9au, 0x36u, 0xe5u, 0x29u, 0xc3u, 0x4fu, 0xabu, 0x64u,
  0x51u, 0xf8u, 0x10u, 0xd7u, 0xbcu, 0x02u, 0x7du, 0x8eu
};

/* S1 (8-bit S-box based on inverse function) */

static const unsigned char clefia_s1[256] =
{
  0x6cu, 0xdau, 0xc3u, 0xe9u, 0x4eu, 0x9du, 0x0au, 0x3du,
  0xb8u, 0x36u, 0xb4u, 0x38u, 0x13u, 0x34u, 0x0cu, 0xd9u,
  0xbfu, 0x74u, 0x94u, 0x8fu, 0xb7u, 0x9cu, 0xe5u, 0xdcu,
  0x9eu, 0x07u, 0x49u, 0x4fu, 0x98u, 0x2cu, 0xb0u, 0x93u,
  0x12u, 0xebu, 0xcdu, 0xb3u, 0x92u, 0xe7u, 0x41u, 0x60u,
  0xe3u, 0x21u, 0x27u, 0x3bu, 0xe6u, 0x19u, 0xd2u, 0x0eu,
  0x91u, 0x11u, 0xc7u, 0x3fu, 0x2au, 0x8eu, 0xa1u, 0xbcu,
  0x2bu, 0xc8u, 0xc5u, 0x0fu, 0x5bu, 0xf3u, 0x87u, 0x8bu,
  0xfbu, 0xf5u, 0xdeu, 0x20u, 0xc6u, 0xa7u, 0x84u, 0xceu,
  0xd8u, 0x65u, 0x51u, 0xc9u, 0xa4u, 0xefu, 0x43u, 0x53u,
  0x25u, 0x5du, 0x9bu, 0x31u, 0xe8u, 0x3eu, 0x0du, 0xd7u,
  0x80u, 0xffu, 0x69u, 0x8au, 0xbau, 0x0bu, 0x73u, 0x5cu,
  0x6eu, 0x54u, 0x15u, 0x62u, 0xf6u, 0x35u, 0x30u, 0x52u,
  0xa3u, 0x16u, 0xd3u, 0x28u, 0x32u, 0xfau, 0xaau, 0x5eu,
  0xcfu, 0xeau, 0xedu, 0x78u, 0x33u, 0x58u, 0x09u, 0x7bu,
  0x63u, 0xc0u, 0xc1u, 0x46u, 0x1eu, 0xdfu, 0xa9u, 0x99u,
  0x55u, 0x04u, 0xc4u, 0x86u, 0x39u, 0x77u, 0x82u, 0xecu,
  0x40u, 0x18u, 0x90u, 0x97u, 0x59u, 0xddu, 0x83u, 0x1fu,
  0x9au, 0x37u, 0x06u, 0x24u, 0x64u, 0x7cu, 0xa5u, 0x56u,
  0x48u, 0x08u, 0x85u, 0xd0u, 0x61u, 0x26u, 0xcau, 0x6fu,
  0x7eu, 0x6au, 0xb6u, 0x71u, 0xa0u, 0x70u, 0x05u, 0xd1u,
  0x45u, 0x8cu, 0x23u, 0x1cu, 0xf0u, 0xeeu, 0x89u, 0xadu,
  0x7au, 0x4bu, 0xc2u, 0x2fu, 0xdbu, 0x5au, 0x4du, 0x76u,
  0x67u, 0x17u, 0x2du, 0xf4u, 0xcbu, 0xb1u, 0x4au, 0xa8u,
  0xb5u, 0x22u, 0x47u, 0x3au, 0xd5u, 0x10u, 0x4cu, 0x72u,
  0xccu, 0x00u, 0xf9u, 0xe0u, 0xfdu, 0xe2u, 0xfeu, 0xaeu,
  0xf8u, 0x5fu, 0xabu, 0xf1u, 0x1bu, 0x42u, 0x81u, 0xd6u,
  0xbeu, 0x44u, 0x29u, 0xa6u, 0x57u, 0xb9u, 0xafu, 0xf2u,
  0xd4u, 0x75u, 0x66u, 0xbbu, 0x68u, 0x9fu, 0x50u, 0x02u,
  0x01u, 0x3cu, 0x7fu, 0x8du, 0x1au, 0x88u, 0xbdu, 0xacu,
  0xf7u, 0xe4u, 0x79u, 0x96u, 0xa2u, 0xfcu, 0x6du, 0xb2u,
  0x6bu, 0x03u, 0xe1u, 0x2eu, 0x7du, 0x14u, 0x95u, 0x1du
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void bytecpy(unsigned char *dst, const unsigned char *src, int bytelen)
{
  while (bytelen-- > 0)
    {
      *dst++ = *src++;
    }
}

static unsigned char clefiamul2(unsigned char x)
{
  /* multiplication over GF(2^8) (p(x) = '11d') */

  if (x & 0x80u)
    {
      x ^= 0x0eu;
    }

  return ((x << 1) | (x >> 7));
}

static void clefiaf0xor(unsigned char *dst, const unsigned char *src,
                        const unsigned char *rk)
{
  unsigned char x[4];
  unsigned char y[4];
  unsigned char z[4];

  /* F0 */

  /* Key addition */

  bytexor(x, src, rk, 4);

  /* Substitution layer */

  z[0] = clefia_s0[x[0]];
  z[1] = clefia_s1[x[1]];
  z[2] = clefia_s0[x[2]];
  z[3] = clefia_s1[x[3]];

  /* Diffusion layer (M0) */

  y[0] = z[0] ^ clefiamul2(z[1]) ^ clefiamul4(z[2]) ^ clefiamul6(z[3]);
  y[1] = clefiamul2(z[0]) ^ z[1] ^ clefiamul6(z[2]) ^ clefiamul4(z[3]);
  y[2] = clefiamul4(z[0]) ^ clefiamul6(z[1]) ^ z[2] ^ clefiamul2(z[3]);
  y[3] = clefiamul6(z[0]) ^ clefiamul4(z[1]) ^ clefiamul2(z[2]) ^ z[3];

  /* Xoring after F0 */

  bytecpy(dst + 0, src + 0, 4);
  bytexor(dst + 4, src + 4, y, 4);
}

static void clefiaf1xor(unsigned char *dst, const unsigned char *src,
                        const unsigned char *rk)
{
  unsigned char x[4];
  unsigned char y[4];
  unsigned char z[4];

  /* F1 */

  /* Key addition */

  bytexor(x, src, rk, 4);

  /* Substitution layer */

  z[0] = clefia_s1[x[0]];
  z[1] = clefia_s0[x[1]];
  z[2] = clefia_s1[x[2]];
  z[3] = clefia_s0[x[3]];

  /* Diffusion layer (M1) */

  y[0] = z[0] ^ clefiamul8(z[1]) ^ clefiamul2(z[2]) ^ clefiamula(z[3]);
  y[1] = clefiamul8(z[0]) ^ z[1] ^ clefiamula(z[2]) ^ clefiamul2(z[3]);
  y[2] = clefiamul2(z[0]) ^ clefiamula(z[1]) ^ z[2] ^ clefiamul8(z[3]);
  y[3] = clefiamula(z[0]) ^ clefiamul2(z[1]) ^ clefiamul8(z[2]) ^ z[3];

  /* Xoring after F1 */

  bytecpy(dst + 0, src + 0, 4);
  bytexor(dst + 4, src + 4, y, 4);
}

static void clefiagfn4(unsigned char *y, const unsigned char *x,
                       const unsigned char *rk, int r)
{
  unsigned char fin[16];
  unsigned char fout[16];

  bytecpy(fin, x, 16);
  while (r-- > 0)
    {
      clefiaf0xor(fout + 0, fin + 0, rk + 0);
      clefiaf1xor(fout + 8, fin + 8, rk + 4);
      rk += 8;
      if (r)
        {
          /* swapping for encryption */

          bytecpy(fin + 0, fout + 4, 12);
          bytecpy(fin + 12, fout + 0, 4);
        }
    }

  bytecpy(y, fout, 16);
}

#if 0 /* Not used */
static void clefiagfn8(unsigned char *y, const unsigned char *x,
                       const unsigned char *rk, int r)
{
  unsigned char fin[32];
  unsigned char fout[32];

  bytecpy(fin, x, 32);
  while (r-- > 0)
    {
      clefiaf0xor(fout + 0, fin + 0, rk + 0);
      clefiaf1xor(fout + 8, fin + 8, rk + 4);
      clefiaf0xor(fout + 16, fin + 16, rk + 8);
      clefiaf1xor(fout + 24, fin + 24, rk + 12);
      rk += 16;
      if (r)
        {
          /* swapping for encryption */

          bytecpy(fin + 0, fout + 4, 28);
          bytecpy(fin + 28, fout + 0, 4);
        }
    }

  bytecpy(y, fout, 32);
}
#endif

#if 0 /* Not used */
static void clefiagfn4inv(unsigned char *y, const unsigned char *x,
                          const unsigned char *rk, int r)
{
  unsigned char fin[16];
  unsigned char fout[16];

  rk += (r - 1) * 8;
  bytecpy(fin, x, 16);
  while (r-- > 0)
    {
      clefiaf0xor(fout + 0, fin + 0, rk + 0);
      clefiaf1xor(fout + 8, fin + 8, rk + 4);
      rk -= 8;
      if (r)
        {
          /* swapping for decryption */

          bytecpy(fin + 0, fout + 12, 4);
          bytecpy(fin + 4, fout + 0, 12);
        }
    }

  bytecpy(y, fout, 16);
}
#endif

static void clefiadoubleswap(unsigned char *lk)
{
  unsigned char t[16];

  t[0] = (lk[0] << 7) | (lk[1] >> 1);
  t[1] = (lk[1] << 7) | (lk[2] >> 1);
  t[2] = (lk[2] << 7) | (lk[3] >> 1);
  t[3] = (lk[3] << 7) | (lk[4] >> 1);
  t[4] = (lk[4] << 7) | (lk[5] >> 1);
  t[5] = (lk[5] << 7) | (lk[6] >> 1);
  t[6] = (lk[6] << 7) | (lk[7] >> 1);
  t[7] = (lk[7] << 7) | (lk[15] & 0x7fu);

  t[8] = (lk[8] >> 7) | (lk[0] & 0xfeu);
  t[9] = (lk[9] >> 7) | (lk[8] << 1);
  t[10] = (lk[10] >> 7) | (lk[9] << 1);
  t[11] = (lk[11] >> 7) | (lk[10] << 1);
  t[12] = (lk[12] >> 7) | (lk[11] << 1);
  t[13] = (lk[13] >> 7) | (lk[12] << 1);
  t[14] = (lk[14] >> 7) | (lk[13] << 1);
  t[15] = (lk[15] >> 7) | (lk[14] << 1);

  bytecpy(lk, t, 16);
}

static void clefiaconset(unsigned char *con, const unsigned char *iv, int lk)
{
  unsigned char t[2];
  unsigned char tmp;

  bytecpy(t, iv, 2);
  while (lk-- > 0)
    {
      con[0] = t[0] ^ 0xb7u;    /* P_16 = 0xb7e1 (natural logarithm) */
      con[1] = t[1] ^ 0xe1u;
      con[2] = ~((t[0] << 1) | (t[1] >> 7));
      con[3] = ~((t[1] << 1) | (t[0] >> 7));
      con[4] = ~t[0] ^ 0x24u;   /* Q_16 = 0x243f (circle ratio) */
      con[5] = ~t[1] ^ 0x3fu;
      con[6] = t[1];
      con[7] = t[0];
      con += 8;

      /* updating T */

      if (t[1] & 0x01u)
        {
          t[0] ^= 0xa8u;
          t[1] ^= 0x30u;
        }

      tmp = t[0] << 7;
      t[0] = (t[0] >> 1) | (t[1] << 7);
      t[1] = (t[1] >> 1) | tmp;
    }
}

static void left_shift_one(uint8_t * in, uint8_t * out)
{
  int i;
  int overflow;

  overflow = 0;
  for (i = 15; i >= 0; i--)
    {
      out[i] = in[i] << 1;
      out[i] |= overflow;
      overflow = (in[i] >> 7) & 1;
    }
}

static void gen_subkey(struct cipher *c)
{
  uint8_t L[16];

  memset(L, 0, 16);
  clefiaencrypt(L, L, c->rk, c->round);

  left_shift_one(L, c->k1);
  if (L[0] & 0x80)
    {
      c->k1[15] = c->k1[15] ^ 0x87;
    }

  left_shift_one(c->k1, c->k2);
  if (c->k1[0] & 0x80)
    {
      c->k2[15] = c->k2[15] ^ 0x87;
    }

  memset(L, 0, 16);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

struct cipher *cipher_init(uint8_t * key, uint8_t * iv)
{
  struct cipher *c;

  c = (struct cipher *)malloc(sizeof(*c));
  if (!c)
    {
      return NULL;
    }

  c->round = clefiakeyset(c->rk, key);

  gen_subkey(c);
  memset(c->vector, 0, 16);

  return c;
}

void cipher_deinit(struct cipher *c)
{
  memset(c, 0, sizeof(*c));
  free(c);
}

int cipher_calc_cmac(struct cipher *c, void *data, int size, void *cmac)
{
  uint8_t m[16];
  uint8_t *p;

  if (size & 0xf)
    {
      return -1;
    }

  p = (uint8_t *) data;
  while (size)
    {
      bytexor(m, c->vector, p, 16);
      clefiaencrypt(c->vector, m, c->rk, c->round);
      size -= 16;
      p += 16;
    }

  bytexor(cmac, m, c->k1, 16);
  clefiaencrypt(cmac, cmac, c->rk, c->round);
  memset(m, 0, 16);

  return 0;
}

void bytexor(unsigned char *dst, const unsigned char *a,
             const unsigned char *b, int bytelen)
{
  while (bytelen-- > 0)
    {
      *dst++ = *a++ ^ *b++;
    }
}

int clefiakeyset(unsigned char *rk, const unsigned char *skey)
{
  const unsigned char iv[2] =
  {
    0x42u, 0x8au  /* cubic root of 2 */
  };

  unsigned char lk[16];
  unsigned char con128[4 * 60];
  int i;

  /* generating CONi^(128) (0 <= i < 60, lk = 30) */

  clefiaconset(con128, iv, 30);

  /* GFN_{4,12} (generating L from K) */

  clefiagfn4(lk, skey, con128, 12);

  bytecpy(rk, skey, 8);    /* initial whitening key (WK0, WK1) */
  rk += 8;
  for (i = 0; i < 9; i++)
    {
      /* round key (RKi (0 <= i < 36)) */

      bytexor(rk, lk, con128 + i * 16 + (4 * 24), 16);
      if (i % 2)
        {
          bytexor(rk, rk, skey, 16);    /* Xoring K */
        }

      clefiadoubleswap(lk);     /* Updating L (DoubleSwap function) */
      rk += 16;
    }

  bytecpy(rk, skey + 8, 8);     /* final whitening key (WK2, WK3) */

  return 18;
}

void clefiaencrypt(unsigned char *ct, const unsigned char *pt,
                   const unsigned char *rk, const int r)
{
  unsigned char rin[16];
  unsigned char  rout[16];

  bytecpy(rin, pt, 16);

  bytexor(rin + 4, rin + 4, rk + 0, 4); /* initial key whitening */
  bytexor(rin + 12, rin + 12, rk + 4, 4);
  rk += 8;

  clefiagfn4(rout, rin, rk, r); /* GFN_{4,r} */

  bytecpy(ct, rout, 16);
  bytexor(ct + 4, ct + 4, rk + r * 8 + 0, 4);   /* final key whitening */
  bytexor(ct + 12, ct + 12, rk + r * 8 + 4, 4);
}
