/*
 *  FIPS-46-3 compliant Triple-DES implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 *  DES, on which TDES is based, was originally designed by Horst Feistel
 *  at IBM in 1974, and was adopted as a standard by NIST (formerly NBS).
 *
 *  http://csrc.nist.gov/publications/fips/fips46-3/fips46-3.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_DES_C)

#include <string.h>
#include <stdio.h>
#include "des_alt.h"
#include "wm_include.h"
#include "wm_crypto_hard.h"

#if defined(MBEDTLS_DES_ALT)

void mbedtls_des_init(mbedtls_des_context *ctx)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = tls_mem_alloc(sizeof(psCipherContext_t));
        if (pctx)
        {
            memset(pctx, 0, sizeof(psCipherContext_t));
            *ctx = (mbedtls_des_context)pctx;
        }
    }
}

void mbedtls_des_free(mbedtls_des_context *ctx)
{
    if (ctx)
    {
        tls_mem_free(*ctx);
    }
}

void mbedtls_des3_init(mbedtls_des3_context *ctx)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = tls_mem_alloc(sizeof(psCipherContext_t));
        if (pctx)
        {
            memset(pctx, 0, sizeof(psCipherContext_t));
            *ctx = (mbedtls_des3_context)pctx;
        }
    }
}

void mbedtls_des3_free(mbedtls_des3_context *ctx)
{
    if (ctx)
    {
        tls_mem_free(*ctx);
    }
}

void mbedtls_des_key_set_parity(unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);
}

/*
 * Check the given key's parity, returns 1 on failure, 0 on SUCCESS
 */
int mbedtls_des_key_check_key_parity(const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);

    return -1;
}

int mbedtls_des_key_check_weak(const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);

    return -1;
}

#if !defined(MBEDTLS_DES_SETKEY_ALT)
void mbedtls_des_setkey(uint32_t SK[32], const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);
}
#endif /* !MBEDTLS_DES_SETKEY_ALT */

/*
 * DES key schedule (56-bit, encryption)
 */
int mbedtls_des_setkey_enc(mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->des3.key.ek[0], key, 8);
        pctx->des3.blocklen = CRYPTO_WAY_ENCRYPT;
    }
    return 0;
}

/*
 * DES key schedule (56-bit, decryption)
 */
int mbedtls_des_setkey_dec(mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->des3.key.ek[0], key, 8);
        pctx->des3.blocklen = CRYPTO_WAY_DECRYPT;
    }
    return 0;
}

/*
 * Triple-DES key schedule (112-bit, encryption)
 */
int mbedtls_des3_set2key_enc(mbedtls_des3_context *ctx,
                             const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->des3.key.ek[0], key, 16);
        pctx->des3.blocklen = CRYPTO_WAY_ENCRYPT;
    }
    return 0;
}

/*
 * Triple-DES key schedule (112-bit, decryption)
 */
int mbedtls_des3_set2key_dec(mbedtls_des3_context *ctx,
                             const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->des3.key.ek[0], key, 16);
        pctx->des3.blocklen = CRYPTO_WAY_DECRYPT;
    }
    return 0;
}

/*
 * Triple-DES key schedule (168-bit, encryption)
 */
int mbedtls_des3_set3key_enc(mbedtls_des3_context *ctx,
                             const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->des3.key.ek[0], key, 24);
        pctx->des3.blocklen = CRYPTO_WAY_ENCRYPT;
    }
    return 0;
}

/*
 * Triple-DES key schedule (168-bit, decryption)
 */
int mbedtls_des3_set3key_dec(mbedtls_des3_context *ctx,
                             const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->des3.key.ek[0], key, 24);
        pctx->des3.blocklen = CRYPTO_WAY_DECRYPT;
    }
    return 0;
}

/*
 * DES-ECB block encryption/decryption
 */
#if !defined(MBEDTLS_DES_CRYPT_ECB_ALT)
int mbedtls_des_crypt_ecb(mbedtls_des_context *ctx,
                          const unsigned char input[8],
                          unsigned char output[8])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->des3.key.ek[1][0] =  CRYPTO_MODE_ECB;

        tls_crypto_des_encrypt_decrypt(pctx, input, output, 8, pctx->des3.blocklen);
    }

    return 0;
}
#endif /* !MBEDTLS_DES_CRYPT_ECB_ALT */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * DES-CBC buffer encryption/decryption
 */
int mbedtls_des_crypt_cbc(mbedtls_des_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[8],
                          const unsigned char *input,
                          unsigned char *output)
{
    int i;
    psCipherContext_t *pctx;
    unsigned char temp[8];

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->des3.key.ek[1][0] =  CRYPTO_MODE_CBC;

        for (i = 0; i < 8; i++) {
    		pctx->des3.IV[i] = iv[i];
    	}

    	if (mode == MBEDTLS_DES_DECRYPT)
    	{
    		memcpy(temp, &input[length - 8], 8);
    	}

    	tls_crypto_des_encrypt_decrypt(pctx, input, output, length, 
    	mode == MBEDTLS_DES_ENCRYPT ? CRYPTO_WAY_ENCRYPT : CRYPTO_WAY_DECRYPT);

        if (mode == MBEDTLS_DES_DECRYPT)
		{
			memcpy(iv, temp, 8);
		}
		else if (mode == MBEDTLS_DES_ENCRYPT)
		{
			memcpy(iv, &output[length - 8], 8);
		}
    }

    return 0;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

/*
 * 3DES-ECB block encryption/decryption
 */
#if !defined(MBEDTLS_DES3_CRYPT_ECB_ALT)
int mbedtls_des3_crypt_ecb(mbedtls_des3_context *ctx,
                           const unsigned char input[8],
                           unsigned char output[8])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->des3.key.ek[1][0] =  CRYPTO_MODE_ECB;

        tls_crypto_3des_encrypt_decrypt(pctx, input, output, 8, pctx->des3.blocklen);
    }

    return 0;
}
#endif /* !MBEDTLS_DES3_CRYPT_ECB_ALT */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * 3DES-CBC buffer encryption/decryption
 */
int mbedtls_des3_crypt_cbc(mbedtls_des3_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[8],
                           const unsigned char *input,
                           unsigned char *output)
{
    int i;
    psCipherContext_t *pctx;
    unsigned char temp[8];

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->des3.key.ek[1][0] =  CRYPTO_MODE_CBC;

        for (i = 0; i < 8; i++) {
    		pctx->des3.IV[i] = iv[i];
    	}

    	if (mode == MBEDTLS_DES_DECRYPT)
    	{
    		memcpy(temp, &input[length - 8], 8);
    	}

    	tls_crypto_3des_encrypt_decrypt(pctx, input, output, length, 
    	mode == MBEDTLS_DES_ENCRYPT ? CRYPTO_WAY_ENCRYPT : CRYPTO_WAY_DECRYPT);

        if (mode == MBEDTLS_DES_DECRYPT)
		{
			memcpy(iv, temp, 8);
		}
		else if (mode == MBEDTLS_DES_ENCRYPT)
		{
			memcpy(iv, &output[length - 8], 8);
		}
    }

    return 0;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_DES_C */
