/*  test_sha256.c - TinyCrypt implementation of some SHA-256 tests */

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
  This module tests the following SHA256 routines:

  Scenarios tested include:
  - NIST SHA256 test vectors
*/

#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>
#include <test_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * NIST SHA256 test vector 1.
 */
unsigned int test_1(void)
{
        unsigned int result = TC_PASS;

        TC_PRINT("SHA256 test #1:\n");
        const uint8_t expected[32] = {
		0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde,
		0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
		0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
        };
        const char *m = "abc";
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, (const uint8_t *) m, strlen(m));
        (void)tc_sha256_final(digest, &s);
        result = check_result(1, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

/*
 * NIST SHA256 test vector 2.
 */
unsigned int test_2(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #2:\n");
        const uint8_t expected[32] = {
		0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xe5, 0xc0, 0x26, 0x93,
		0x0c, 0x3e, 0x60, 0x39, 0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
		0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
        };
        const char *m = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, (const uint8_t *) m, strlen(m));
        (void) tc_sha256_final(digest, &s);

        result = check_result(2, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_3(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #3:\n");
        const uint8_t expected[32] = {
		0x68, 0x32, 0x57, 0x20, 0xaa, 0xbd, 0x7c, 0x82, 0xf3, 0x0f, 0x55, 0x4b,
		0x31, 0x3d, 0x05, 0x70, 0xc9, 0x5a, 0xcc, 0xbb, 0x7d, 0xc4, 0xb5, 0xaa,
		0xe1, 0x12, 0x04, 0xc0, 0x8f, 0xfe, 0x73, 0x2b
        };
        const uint8_t m[1] = { 0xbd };
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(3, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_4(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #4:\n");
        const uint8_t expected[32] = {
		0x7a, 0xbc, 0x22, 0xc0, 0xae, 0x5a, 0xf2, 0x6c, 0xe9, 0x3d, 0xbb, 0x94,
		0x43, 0x3a, 0x0e, 0x0b, 0x2e, 0x11, 0x9d, 0x01, 0x4f, 0x8e, 0x7f, 0x65,
		0xbd, 0x56, 0xc6, 0x1c, 0xcc, 0xcd, 0x95, 0x04
        };
        const uint8_t m[4] = { 0xc9, 0x8c, 0x8e, 0x55 };
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(4, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_5(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #5:\n");

        const uint8_t expected[32] = {
		0x02, 0x77, 0x94, 0x66, 0xcd, 0xec, 0x16, 0x38, 0x11, 0xd0, 0x78, 0x81,
		0x5c, 0x63, 0x3f, 0x21, 0x90, 0x14, 0x13, 0x08, 0x14, 0x49, 0x00, 0x2f,
		0x24, 0xaa, 0x3e, 0x80, 0xf0, 0xb8, 0x8e, 0xf7
        };
        uint8_t m[55];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x00, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(5, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_6(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #6:\n");
        const uint8_t expected[32] = {
		0xd4, 0x81, 0x7a, 0xa5, 0x49, 0x76, 0x28, 0xe7, 0xc7, 0x7e, 0x6b, 0x60,
		0x61, 0x07, 0x04, 0x2b, 0xbb, 0xa3, 0x13, 0x08, 0x88, 0xc5, 0xf4, 0x7a,
		0x37, 0x5e, 0x61, 0x79, 0xbe, 0x78, 0x9f, 0xbb
        };
        uint8_t m[56];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x00, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(6, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_7(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #7:\n");
        const uint8_t expected[32] = {
		0x65, 0xa1, 0x6c, 0xb7, 0x86, 0x13, 0x35, 0xd5, 0xac, 0xe3, 0xc6, 0x07,
		0x18, 0xb5, 0x05, 0x2e, 0x44, 0x66, 0x07, 0x26, 0xda, 0x4c, 0xd1, 0x3b,
		0xb7, 0x45, 0x38, 0x1b, 0x23, 0x5a, 0x17, 0x85
        };
        uint8_t m[57];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x00, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(7, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_8(void)
{
        unsigned int result = TC_PASS;

        TC_PRINT("SHA256 test #8:\n");
        const uint8_t expected[32] = {
		0xf5, 0xa5, 0xfd, 0x42, 0xd1, 0x6a, 0x20, 0x30, 0x27, 0x98, 0xef, 0x6e,
		0xd3, 0x09, 0x97, 0x9b, 0x43, 0x00, 0x3d, 0x23, 0x20, 0xd9, 0xf0, 0xe8,
		0xea, 0x98, 0x31, 0xa9, 0x27, 0x59, 0xfb, 0x4b
        };
        uint8_t m[64];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x00, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(8, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_9(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #9:\n");
        const uint8_t expected[32] = {
		0x54, 0x1b, 0x3e, 0x9d, 0xaa, 0x09, 0xb2, 0x0b, 0xf8, 0x5f, 0xa2, 0x73,
		0xe5, 0xcb, 0xd3, 0xe8, 0x01, 0x85, 0xaa, 0x4e, 0xc2, 0x98, 0xe7, 0x65,
		0xdb, 0x87, 0x74, 0x2b, 0x70, 0x13, 0x8a, 0x53
        };
        uint8_t m[1000];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x00, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(9, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_10(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #10:\n");
        const uint8_t expected[32] = {
		0xc2, 0xe6, 0x86, 0x82, 0x34, 0x89, 0xce, 0xd2, 0x01, 0x7f, 0x60, 0x59,
		0xb8, 0xb2, 0x39, 0x31, 0x8b, 0x63, 0x64, 0xf6, 0xdc, 0xd8, 0x35, 0xd0,
		0xa5, 0x19, 0x10, 0x5a, 0x1e, 0xad, 0xd6, 0xe4
        };
        uint8_t m[1000];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x41, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(10, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_11(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #11:\n");
        const uint8_t expected[32] = {
		0xf4, 0xd6, 0x2d, 0xde, 0xc0, 0xf3, 0xdd, 0x90, 0xea, 0x13, 0x80, 0xfa,
		0x16, 0xa5, 0xff, 0x8d, 0xc4, 0xc5, 0x4b, 0x21, 0x74, 0x06, 0x50, 0xf2,
		0x4a, 0xfc, 0x41, 0x20, 0x90, 0x35, 0x52, 0xb0
        };
        uint8_t m[1005];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;

        (void)memset(m, 0x55, sizeof(m));

        (void)tc_sha256_init(&s);
        tc_sha256_update(&s, m, sizeof(m));
        (void)tc_sha256_final(digest, &s);

        result = check_result(11, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_12(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #12:\n");

        const uint8_t expected[32] = {
		0xd2, 0x97, 0x51, 0xf2, 0x64, 0x9b, 0x32, 0xff, 0x57, 0x2b, 0x5e, 0x0a,
		0x9f, 0x54, 0x1e, 0xa6, 0x60, 0xa5, 0x0f, 0x94, 0xff, 0x0b, 0xee, 0xdf,
		0xb0, 0xb6, 0x92, 0xb9, 0x24, 0xcc, 0x80, 0x25
        };
        uint8_t m[1000];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;
        unsigned int i;

        (void)memset(m, 0x00, sizeof(m));

        (void)tc_sha256_init(&s);
        for (i = 0; i < 1000; ++i) {
                tc_sha256_update(&s, m, sizeof(m));
        }
        (void)tc_sha256_final(digest, &s);

        result = check_result(12, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_13(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #13:\n");
        const uint8_t expected[32] = {
		0x15, 0xa1, 0x86, 0x8c, 0x12, 0xcc, 0x53, 0x95, 0x1e, 0x18, 0x23, 0x44,
		0x27, 0x74, 0x47, 0xcd, 0x09, 0x79, 0x53, 0x6b, 0xad, 0xcc, 0x51, 0x2a,
		0xd2, 0x4c, 0x67, 0xe9, 0xb2, 0xd4, 0xf3, 0xdd
        };
        uint8_t m[32768];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;
        unsigned int i;

        (void)memset(m, 0x5a, sizeof(m));

        (void)tc_sha256_init(&s);
        for (i = 0; i < 16384; ++i) {
                tc_sha256_update(&s, m, sizeof(m));
        }
        (void)tc_sha256_final(digest, &s);

        result = check_result(13, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

unsigned int test_14(void)
{
        unsigned int result = TC_PASS;
        TC_PRINT("SHA256 test #14:\n");
        const uint8_t expected[32] = {
		0x46, 0x1c, 0x19, 0xa9, 0x3b, 0xd4, 0x34, 0x4f, 0x92, 0x15, 0xf5, 0xec,
		0x64, 0x35, 0x70, 0x90, 0x34, 0x2b, 0xc6, 0x6b, 0x15, 0xa1, 0x48, 0x31,
		0x7d, 0x27, 0x6e, 0x31, 0xcb, 0xc2, 0x0b, 0x53
        };
        uint8_t m[32768];
        uint8_t digest[32];
        struct tc_sha256_state_struct s;
        unsigned int i;

        (void)memset(m, 0x00, sizeof(m));

        (void) tc_sha256_init(&s);
        for (i = 0; i < 33280; ++i) {
                tc_sha256_update(&s, m, sizeof(m));
        }
        (void) tc_sha256_final(digest, &s);

        result = check_result(14, expected, sizeof(expected),
			      digest, sizeof(digest));
        TC_END_RESULT(result);
        return result;
}

/*
 * Main task to test AES
 */

int main(void)
{
        unsigned int result = TC_PASS;
        TC_START("Performing SHA256 tests (NIST tests vectors):");

        result = test_1();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #1 failed.\n");
                goto exitTest;
        }
        result = test_2();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #2 failed.\n");
                goto exitTest;
        }
        result = test_3();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #3 failed.\n");
                goto exitTest;
        }
        result = test_4();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #4 failed.\n");
                goto exitTest;
        }
        result = test_5();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #5 failed.\n");
                goto exitTest;
        }
        result = test_6();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #6 failed.\n");
                goto exitTest;
        }
        result = test_7();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #7 failed.\n");
                goto exitTest;
        }
        result = test_8();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #8 failed.\n");
                goto exitTest;
        }
        result = test_9();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #9 failed.\n");
                goto exitTest;
        }
        result = test_10();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #10 failed.\n");
                goto exitTest;
        }
        result = test_11();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #11 failed.\n");
                goto exitTest;
        }
        result = test_12();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #12 failed.\n");
                goto exitTest;
        }
  	/* memory and computation intensive test cases: */
        result = test_13();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #13 failed.\n");
                goto exitTest;
        }
        result = test_14();
        if (result == TC_FAIL) {
		/* terminate test */
                TC_ERROR("SHA256 test #14 failed.\n");
                goto exitTest;
        }

        TC_PRINT("All SHA256 tests succeeded!\n");

exitTest:
        TC_END_RESULT(result);
        TC_END_REPORT(result);
}

