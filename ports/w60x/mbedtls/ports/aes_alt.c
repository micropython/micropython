/*
 *  FIPS-197 compliant AES implementation
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
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C)

#include <string.h>
#include "aes_alt.h"
#include "wm_include.h"
#include "wm_crypto_hard.h"

#if defined(MBEDTLS_AES_ALT)

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = tls_mem_alloc(sizeof(psCipherContext_t));
        if (pctx)
        {
            memset(pctx, 0, sizeof(psCipherContext_t));
            *ctx = (mbedtls_aes_context)pctx;
        }
    }
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx)
    {
        tls_mem_free(*ctx);
    }
}

/*
 * AES key schedule (encryption)
 */
#if !defined(MBEDTLS_AES_SETKEY_ENC_ALT)
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->aes.key.eK, key, 16);
    }
    return 0;
}
#endif /* !MBEDTLS_AES_SETKEY_ENC_ALT */

/*
 * AES key schedule (decryption)
 */
#if !defined(MBEDTLS_AES_SETKEY_DEC_ALT)
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        memcpy(pctx->aes.key.eK, key, 16);
    }
    return 0;
}
#endif /* !MBEDTLS_AES_SETKEY_DEC_ALT */

/*
 * AES-ECB block encryption
 */
#if !defined(MBEDTLS_AES_ENCRYPT_ALT)
int mbedtls_internal_aes_encrypt(mbedtls_aes_context *ctx,
                                 const unsigned char input[16],
                                 unsigned char output[16])
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, input, output);
}
#endif /* !MBEDTLS_AES_ENCRYPT_ALT */

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_aes_encrypt(mbedtls_aes_context *ctx,
                         const unsigned char input[16],
                         unsigned char output[16])
{
    mbedtls_internal_aes_encrypt(ctx, input, output);
}
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

/*
 * AES-ECB block decryption
 */
#if !defined(MBEDTLS_AES_DECRYPT_ALT)
int mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx,
                                 const unsigned char input[16],
                                 unsigned char output[16])
{
    return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_DECRYPT, input, output);
}
#endif /* !MBEDTLS_AES_DECRYPT_ALT */

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_aes_decrypt(mbedtls_aes_context *ctx,
                         const unsigned char input[16],
                         unsigned char output[16])
{
    mbedtls_internal_aes_decrypt(ctx, input, output);
}
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16])
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->aes.key.Nr = CRYPTO_MODE_ECB;

        tls_crypto_aes_encrypt_decrypt(pctx, input, output, 16, 
        mode == MBEDTLS_AES_ENCRYPT ? CRYPTO_WAY_ENCRYPT : CRYPTO_WAY_DECRYPT);
    }

    return 0;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    int i;
    psCipherContext_t *pctx;
    unsigned char temp[16];

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->aes.key.Nr = CRYPTO_MODE_CBC;

        for (i = 0; i < 16; i++) {
		    pctx->aes.IV[i] = iv[i];
	    }

	    if (mode == MBEDTLS_AES_DECRYPT)
    	{
    		memcpy(temp, &input[length - 16], 16);
    	}

	    tls_crypto_aes_encrypt_decrypt(pctx, input, output, length, 
        mode == MBEDTLS_AES_ENCRYPT ? CRYPTO_WAY_ENCRYPT : CRYPTO_WAY_DECRYPT);

        if (mode == MBEDTLS_AES_DECRYPT)
		{
			memcpy(iv, temp, 16);
		}
		else if (mode == MBEDTLS_AES_ENCRYPT)
		{
			memcpy(iv, &output[length - 16], 16);
		}
    }

    return 0;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);
    return -1;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output)
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);
    return -1;
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    printf("%s:%d don't support\r\n", __func__, __LINE__);
    return -1;
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    int i;
    psCipherContext_t *pctx;
    unsigned char temp[16];
    size_t nc_off_s;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        pctx->aes.key.Nr = CRYPTO_MODE_CTR;

        if (*nc_off == 0)
        {
            for (i = 0; i < 16; i++) {
                pctx->aes.IV[i] = nonce_counter[i];
            }

            memcpy(temp, &input[length - 16], 16);
        }
        nc_off_s = *nc_off;
        tls_crypto_aes_encrypt_decrypt(pctx, input, output, length, CRYPTO_WAY_DECRYPT);
        *nc_off = nc_off_s;
        if (stream_block)
        {
            memcpy(stream_block, temp, 16);
        }
    }

    return 0;
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_AES_C */
