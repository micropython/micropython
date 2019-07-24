/*  test_hmac_prng.c - TinyCrypt implementation of some HMAC-PRNG tests */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
  DESCRIPTION
  This module tests the following PRNG routines:

  Scenarios tested include:
  - HMAC-PRNG init
  - HMAC-PRNG reseed
  - HMAC-PRNG generate)
*/

#include <tinycrypt/hmac_prng.h>
#include <tinycrypt/constants.h>
#include <test_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TC_DEBUG_MODE 0

#ifdef TC_DEBUG_MODE
void show(const char *label, const uint8_t *s, size_t len)
{
	unsigned int i;
	printf ("%s = ", label);
	for (i = 0; i < (unsigned int) len; ++i) {
		printf ("%02x", s[i]);
	}
	printf ("\n");
}

void printBinaryFile(const uint8_t *s, unsigned int slen)
{
	FILE *write_ptr;
	write_ptr = fopen("pseudo-random-data.bin","wb");
	fwrite(s, slen, 1, write_ptr);
}
#endif

/*
 * Main task to test AES
 */
int main(void)
{
        uint8_t seed[128];
        struct tc_hmac_prng_struct h;
        unsigned int size = (1 << 19);
        uint8_t random[size];
        unsigned int i;
        unsigned int result = TC_PASS;

        TC_START("Performing HMAC-PRNG tests:");
        TC_PRINT("HMAC-PRNG test#1 (init, reseed, generate):\n");

        /* Fake seed (replace by a a truly random seed): */
        for (i = 0; i < (unsigned int) sizeof(seed); ++i) {
                seed[i] = i;
        }

        /* Fake personalization and additional_input (replace by appropriate
	     * values): *
	     * e.g.: hostname+timestamp */
        uint8_t *personalization = (uint8_t *) "HOSTNAME";
        uint8_t *additional_input = (uint8_t *) "additional input";

        TC_PRINT("HMAC-PRNG test#1 (init):\n");
        if (tc_hmac_prng_init(&h, personalization,
			      sizeof(personalization)) == 0) {
                TC_ERROR("HMAC-PRNG initialization failed.\n");
                result = TC_FAIL;
                goto exitTest;
        }
        TC_END_RESULT(result);

        TC_PRINT("HMAC-PRNG test#1 (reseed):\n");
        if (tc_hmac_prng_reseed(&h, seed, sizeof(seed), additional_input,
                                sizeof(additional_input)) == 0) {
                TC_ERROR("HMAC-PRNG reseed failed.\n");
                result = TC_FAIL;
                goto exitTest;
        }

        TC_END_RESULT(result);

        TC_PRINT("HMAC-PRNG test#1 (generate):\n");
        if (tc_hmac_prng_generate(random, size, &h) < 1) {
                TC_ERROR("HMAC-PRNG generate failed.\n");
                result = TC_FAIL;
                goto exitTest;
        }
        TC_END_RESULT(result);

#ifdef TC_DEBUG_MODE
	printBinaryFile(random, size);
	show ("Pseudo-random data", random, size);
#endif

        TC_PRINT("All HMAC tests succeeded!\n");

 exitTest:
        TC_END_RESULT(result);
        TC_END_REPORT(result);
}
