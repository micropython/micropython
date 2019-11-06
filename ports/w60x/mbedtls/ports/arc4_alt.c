/*
 *  An implementation of the ARCFOUR algorithm
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
 *  The ARCFOUR algorithm was publicly disclosed on 94/09.
 *
 *  http://groups.google.com/group/sci.crypt/msg/10a300c9d21afca0
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ARC4_C)

#include <string.h>
#include <stdio.h>
#include "arc4_alt.h"
#include "wm_include.h"
#include "wm_crypto_hard.h"

#if defined(MBEDTLS_ARC4_ALT)

void mbedtls_arc4_init(mbedtls_arc4_context *ctx)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = tls_mem_alloc(sizeof(psCipherContext_t));
        if (pctx)
        {
            memset(pctx, 0, sizeof(psCipherContext_t));
            *ctx = (mbedtls_arc4_context)pctx;
        }
    }
}

void mbedtls_arc4_free(mbedtls_arc4_context *ctx)
{
    if (ctx)
    {
        tls_mem_free(*ctx);
    }
}

/*
 * ARC4 key schedule
 */
void mbedtls_arc4_setup(mbedtls_arc4_context *ctx, const unsigned char *key,
                        unsigned int keylen)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        tls_crypto_rc4_init(pctx, key, keylen);
    }
}

/*
 * ARC4 cipher function
 */
int mbedtls_arc4_crypt(mbedtls_arc4_context *ctx, size_t length, const unsigned char *input,
                       unsigned char *output)
{
    psCipherContext_t *pctx;

    if (ctx)
    {
        pctx = (psCipherContext_t *)*ctx;
        tls_crypto_rc4(pctx, input, output, length);
    }

    return 0;
}

#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_ARC4_C */
