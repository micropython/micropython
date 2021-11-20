/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#ifdef MICROPY_SSL_MBEDTLS

#include "py/runtime.h"
#include "py/gc.h"
#include "fsl_trng.h"
#include "mbedtls_config.h"

#define DEBUG (0)

#if DEBUG
static size_t count_links(uint32_t *nb) {
    void **p = MP_STATE_PORT(mbedtls_memory);
    size_t n = 0;
    *nb = 0;
    while (p != NULL) {
        ++n;
        *nb += gc_nbytes(p);
        p = (void **)p[1];
    }
    return n;
}
#endif

void *m_calloc_mbedtls(size_t nmemb, size_t size) {
    void **ptr = m_malloc0(nmemb * size + 2 * sizeof(uintptr_t));
    #if DEBUG
    uint32_t nb;
    size_t n = count_links(&nb);
    printf("mbed_alloc(%u, %u) -> (%u;%u) %p\n", nmemb, size, n, (uint)nb, ptr);
    #endif
    if (MP_STATE_PORT(mbedtls_memory) != NULL) {
        MP_STATE_PORT(mbedtls_memory)[0] = ptr;
    }
    ptr[0] = NULL;
    ptr[1] = MP_STATE_PORT(mbedtls_memory);
    MP_STATE_PORT(mbedtls_memory) = ptr;
    return &ptr[2];
}

void m_free_mbedtls(void *ptr_in) {
    void **ptr = &((void **)ptr_in)[-2];
    #if DEBUG
    uint32_t nb;
    size_t n = count_links(&nb);
    printf("mbed_free(%p, [%p, %p], nbytes=%u, links=%u;%u)\n", ptr, ptr[0], ptr[1], gc_nbytes(ptr), n, (uint)nb);
    #endif
    if (ptr[1] != NULL) {
        ((void **)ptr[1])[0] = ptr[0];
    }
    if (ptr[0] != NULL) {
        ((void **)ptr[0])[1] = ptr[1];
    } else {
        MP_STATE_PORT(mbedtls_memory) = ptr[1];
    }
    m_free(ptr);
}

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen) {

    // assumes that TRNG_Init was called during startup
    *olen = len;
    TRNG_GetRandomData(TRNG, output, len);

    return 0;
}

#endif
