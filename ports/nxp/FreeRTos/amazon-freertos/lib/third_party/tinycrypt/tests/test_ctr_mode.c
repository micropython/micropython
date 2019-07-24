/* test_ctr_mode.c - TinyCrypt implementation of some AES-CTR tests */

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
  This module tests the following AES-CTR Mode routines:

  Scenarios tested include:
  - AES128 CTR mode encryption SP 800-38a tests
*/

#include <tinycrypt/ctr_mode.h>
#include <tinycrypt/aes.h>
#include <tinycrypt/constants.h>
#include <test_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * NIST SP 800-38a CTR Test for encryption and decryption.
 */
unsigned int test_1_and_2(void)
{
        const uint8_t key[16] = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
		0x09, 0xcf, 0x4f, 0x3c
        };
        uint8_t ctr[16] = {
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
		0xfc, 0xfd, 0xfe, 0xff
        };
        const uint8_t plaintext[64] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11,
		0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, 0x30, 0xc8, 0x1c, 0x46,
		0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b,
		0xe6, 0x6c, 0x37, 0x10
        };
        const uint8_t ciphertext[80] = {
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
		0xfc, 0xfd, 0xfe, 0xff, 0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
		0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce, 0x98, 0x06, 0xf6, 0x6b,
		0x79, 0x70, 0xfd, 0xff, 0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
		0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e, 0x5b, 0x4f, 0x09, 0x02,
		0x0d, 0xb0, 0x3e, 0xab, 0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
		0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee
        };
        struct tc_aes_key_sched_struct sched;
        uint8_t out[80];
        uint8_t decrypted[64];
        unsigned int result = TC_PASS;

        TC_PRINT("CTR test #1 (encryption SP 800-38a tests):\n");
        (void)tc_aes128_set_encrypt_key(&sched, key);

        (void)memcpy(out, ctr, sizeof(ctr));
        if (tc_ctr_mode(&out[TC_AES_BLOCK_SIZE], sizeof(plaintext), plaintext,
			sizeof(plaintext), ctr, &sched) == 0) {
                TC_ERROR("CTR test #1 (encryption SP 800-38a tests) failed in %s.\n", __func__);
                result = TC_FAIL;
                goto exitTest1;
        }

        result = check_result(1, ciphertext, sizeof(out), out, sizeof(out));
        TC_END_RESULT(result);

        TC_PRINT("CTR test #2 (decryption SP 800-38a tests):\n");
        (void) memcpy(ctr, out, sizeof(ctr));
        if (tc_ctr_mode(decrypted, sizeof(decrypted), &out[TC_AES_BLOCK_SIZE],
                        sizeof(decrypted), ctr, &sched) == 0) {
                TC_ERROR("CTR test #2 (decryption SP 800-38a tests) failed in %s.\n", __func__);
                result = TC_FAIL;
                goto exitTest1;
        }

        result = check_result(2, plaintext, sizeof(plaintext),
			      decrypted, sizeof(plaintext));

 exitTest1:
        TC_END_RESULT(result);
        return result;
}

/*
 * Main task to test AES
 */

int main(void)
{
        unsigned int result = TC_PASS;

        TC_START("Performing AES128-CTR mode tests:");

        TC_PRINT("Performing CTR tests:\n");
        result = test_1_and_2();
        if (result == TC_FAIL) { /* terminate test */
                TC_ERROR("CBC test #1 failed.\n");
                goto exitTest;
        }

        TC_PRINT("All CTR tests succeeded!\n");

 exitTest:
        TC_END_RESULT(result);
        TC_END_REPORT(result);
}
