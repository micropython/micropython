/*
 *  RFC 1321 compliant MD5 implementation
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
 *  The MD5 algorithm was designed by Ron Rivest in 1991.
 *
 *  http://www.ietf.org/rfc/rfc1321.txt
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_MD5_C)

#include <string.h>
#include <stdio.h>
#include "md5_alt.h"
#include "wm_include.h"
#include "wm_crypto_hard.h"

#if defined(MBEDTLS_MD5_ALT)

void mbedtls_md5_init(mbedtls_md5_context *ctx)
{
    psDigestContext_t *pctx;

    if (ctx)
    {
        pctx = tls_mem_alloc(sizeof(psDigestContext_t));
        if (pctx)
        {
            memset(pctx, 0, sizeof(psDigestContext_t));
            *ctx = (mbedtls_md5_context)pctx;
        }
    }
}

void mbedtls_md5_free(mbedtls_md5_context *ctx)
{
    if (ctx)
    {
        tls_mem_free(*ctx);
    }
}

void mbedtls_md5_clone(mbedtls_md5_context *dst,
                       const mbedtls_md5_context *src)
{
    if (dst)
    {
        memcpy(dst, src, sizeof(psDigestContext_t));
    }

    return 0;
}

/*
 * MD5 context setup
 */
int mbedtls_md5_starts_ret(mbedtls_md5_context *ctx)
{
    psDigestContext_t *pctx;

    if (ctx)
    {
        pctx = (psDigestContext_t *)*ctx;
        tls_crypto_md5_init(pctx);
    }

    return 0;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_starts(mbedtls_md5_context *ctx)
{
    mbedtls_md5_starts_ret(ctx);
}
#endif

#if !defined(MBEDTLS_MD5_PROCESS_ALT)
int mbedtls_internal_md5_process(mbedtls_md5_context *ctx,
                                 const unsigned char data[64])
{
    return mbedtls_md5_update_ret(ctx, data, 64);
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_process(mbedtls_md5_context *ctx,
                         const unsigned char data[64])
{
    mbedtls_internal_md5_process(ctx, data);
}
#endif
#endif /* !MBEDTLS_MD5_PROCESS_ALT */

/*
 * MD5 process buffer
 */
int mbedtls_md5_update_ret(mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen)
{
    psDigestContext_t *pctx;

    if (ctx)
    {
        pctx = (psDigestContext_t *)*ctx;
        tls_crypto_md5_update(pctx, input, ilen);
    }

    return 0;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_update(mbedtls_md5_context *ctx,
                        const unsigned char *input,
                        size_t ilen)
{
    mbedtls_md5_update_ret(ctx, input, ilen);
}
#endif

/*
 * MD5 final digest
 */
int mbedtls_md5_finish_ret(mbedtls_md5_context *ctx, unsigned char output[16])
{
    psDigestContext_t *pctx;

    if (ctx)
    {
        pctx = (psDigestContext_t *)*ctx;
        tls_crypto_md5_final(pctx, output);
    }

    return 0;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_md5_finish(mbedtls_md5_context *ctx,
                        unsigned char output[16])
{
    mbedtls_md5_finish_ret(ctx, output);
}
#endif

#endif /* MBEDTLS_MD5_ALT */
#endif /* MBEDTLS_MD5_C */
