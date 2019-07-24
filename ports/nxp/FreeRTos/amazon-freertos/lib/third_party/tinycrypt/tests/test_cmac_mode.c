/* test_cmac_mode.c - TinyCrypt AES-CMAC tests (including SP 800-38B tests) */

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
 *  DESCRIPTION
 * This module tests the following AES-CMAC test (including SP 800-38B):
 *
 *  Scenarios tested include:
 *  - CMAC test #1 (GF(2^128) double))
 *  - CMAC test #2 null msg (SP 800-38B test vector #1)
 *  - CMAC test #3 1 block msg (SP 800-38B test vector #2)
 *  - CMAC test #4 320 bit msg (SP 800-38B test vector #3)
 *  - CMAC test #5 512 bit msg (SP 800-38B test vector #4)
 */

#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/aes.h>
#include <test_utils.h>

#include <stdio.h>
#include <string.h>

#define BUF_LEN 16

static void show(const char *label, const uint8_t *s, size_t slen)
{
	unsigned int i;

	TC_PRINT("%s\t", label);
	for (i = 0; i < slen; ++i) {
		TC_PRINT("%02x", s[i]);
	}
	TC_PRINT("\n");
}

extern void gf_double(uint8_t *out, uint8_t *in);

static int verify_gf_2_128_double(uint8_t *K1, uint8_t *K2,
				  struct tc_cmac_struct s)
{
	int result = TC_PASS;

	TC_PRINT("Performing CMAC test #1 (GF(2^128) double):\n");

	uint8_t zero[BUF_LEN];
	uint8_t L[BUF_LEN];
	const uint8_t l[BUF_LEN] = {
		0x7d, 0xf7, 0x6b, 0x0c, 0x1a, 0xb8, 0x99, 0xb3,
		0x3e, 0x42, 0xf0, 0x47, 0xb9, 0x1b, 0x54, 0x6f
	};
	const uint8_t k1[BUF_LEN] = {
		0xfb, 0xee, 0xd6, 0x18, 0x35, 0x71, 0x33, 0x66,
		0x7c, 0x85, 0xe0, 0x8f, 0x72, 0x36, 0xa8, 0xde
	};
	const uint8_t k2[BUF_LEN] = {
		0xf7, 0xdd, 0xac, 0x30, 0x6a, 0xe2, 0x66, 0xcc,
		0xf9, 0x0b, 0xc1, 0x1e, 0xe4, 0x6d, 0x51, 0x3b
	};

	(void) memset(zero, '\0', sizeof(zero));
	tc_aes_encrypt(L, zero, s.sched);
	if (memcmp(L, l, BUF_LEN) != 0) {
		TC_ERROR("%s: AES encryption failed\n", __func__);
		show("expected L =", l, sizeof(l));
		show("computed L =", L, sizeof(L));
		return TC_FAIL;
	}

	gf_double(K1, L);
	if (memcmp(K1, k1, BUF_LEN) != 0) {
		TC_ERROR("%s: gf_2_128_double failed when msb = 0\n", __func__);
		show("expected K1 =", k1, sizeof(k1));
		show("computed K1 =", K1, sizeof(k1));
		return TC_FAIL;
	}

	gf_double(K2, K1);
	if (memcmp(K2, k2, BUF_LEN) != 0) {
		TC_ERROR("%s: gf_2_128_double failed when msb = 1\n", __func__);
		show("expected K2 =", k2, sizeof(k2));
		show("computed K2 =", K2, sizeof(k2));
		return TC_FAIL;
	}

	TC_END_RESULT(result);
	return result;
}

static int verify_cmac_null_msg(TCCmacState_t s)
{
	int result = TC_PASS;

	TC_PRINT("Performing CMAC test #2 (SP 800-38B test vector #1):\n");

	const uint8_t tag[BUF_LEN] = {
		0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28,
		0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46
	};
	uint8_t Tag[BUF_LEN];

	(void) tc_cmac_init(s);
	(void) tc_cmac_update(s, (const uint8_t *) 0, 0);
	(void) tc_cmac_final(Tag, s);

	if (memcmp(Tag, tag, BUF_LEN) != 0) {
		TC_ERROR("%s: aes_cmac failed with null msg = 1\n", __func__);
		show("expected Tag =", tag, sizeof(tag));
		show("computed Tag =", Tag, sizeof(Tag));
		return TC_FAIL;
	}

	TC_END_RESULT(result);
	return result;
}

static int verify_cmac_1_block_msg(TCCmacState_t s)
{
	int result = TC_PASS;

	TC_PRINT("Performing CMAC test #3 (SP 800-38B test vector #2):\n");

	const uint8_t msg[BUF_LEN] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
	};
	const uint8_t tag[BUF_LEN] = {
		0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
		0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c
	};
	uint8_t Tag[BUF_LEN];

	(void) tc_cmac_init(s);
	(void) tc_cmac_update(s, msg, sizeof(msg));
	(void) tc_cmac_final(Tag, s);

	if (memcmp(Tag, tag, BUF_LEN) != 0) {
		TC_ERROR("%s: aes_cmac failed with 1 block msg\n", __func__);
		show("aes_cmac failed with 1 block msg =", msg, sizeof(msg));
		show("expected Tag =", tag, sizeof(tag));
		show("computed Tag =", Tag, sizeof(Tag));
		return TC_FAIL;
	}

	TC_END_RESULT(result);
	return result;
}

static int verify_cmac_320_bit_msg(TCCmacState_t s)
{
	int result = TC_PASS;

	TC_PRINT("Performing CMAC test #4 (SP 800-38B test vector #3):\n");

	const uint8_t msg[40] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11
	};
	const uint8_t tag[BUF_LEN] = {
		0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a, 0xe6, 0x30,
		0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27
	};
	uint8_t Tag[BUF_LEN];

	(void) tc_cmac_init(s);
	(void) tc_cmac_update(s, msg, sizeof(msg));
	(void) tc_cmac_final(Tag, s);

	if (memcmp(Tag, tag, BUF_LEN) != 0) {
		TC_ERROR("%s: aes_cmac failed with 320 bit msg\n", __func__);
		show("aes_cmac failed with 320 bit msg =", msg, sizeof(msg));
		show("expected Tag =", tag, sizeof(tag));
		show("computed Tag =", Tag, sizeof(Tag));
		return TC_FAIL;
	}

	TC_END_RESULT(result);
	return result;
}

static int verify_cmac_512_bit_msg(TCCmacState_t s)
{
	int result = TC_PASS;

	TC_PRINT("Performing CMAC test #5 (SP 800-38B test vector #4)\n");

	const uint8_t msg[64] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
		0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
		0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
	};
	const uint8_t tag[BUF_LEN] = {
		0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92,
		0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe
	};
	uint8_t Tag[BUF_LEN];

	(void)tc_cmac_init(s);
	(void)tc_cmac_update(s, msg, sizeof(msg));
	(void)tc_cmac_final(Tag, s);

	if (memcmp(Tag, tag, BUF_LEN) != 0) {
		TC_ERROR("%s: aes_cmac failed with 512 bit msg\n", __func__);
		show("aes_cmac failed with 512 bit msg =", msg, sizeof(msg));
		show("expected Tag =", tag, sizeof(tag));
		show("computed Tag =", Tag, sizeof(Tag));
		return TC_FAIL;
	}

	TC_END_RESULT(result);
	return result;
}

/*
 * Main task to test CMAC
 * effects:    returns 1 if all tests pass
 * exceptions: returns a negative value if some test fails
 */
int main(void)
{

	int result = TC_PASS;

	struct tc_cmac_struct state;
	struct tc_aes_key_sched_struct sched;

	const uint8_t key[BUF_LEN] = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	};
	uint8_t K1[BUF_LEN], K2[BUF_LEN];

	TC_START("Performing CMAC tests:");

	(void) tc_cmac_setup(&state, key, &sched);
	result = verify_gf_2_128_double(K1, K2, state);
	if (result == TC_FAIL) {
		/* terminate test */
		TC_ERROR("CMAC test #1 (128 double) failed.\n");
		goto exitTest;
	}
	(void) tc_cmac_setup(&state, key, &sched);
	result = verify_cmac_null_msg(&state);
	if (result == TC_FAIL) {
		/* terminate test */
		TC_ERROR("CMAC test #2 (null msg) failed.\n");
		goto exitTest;
	}
	(void) tc_cmac_setup(&state, key, &sched);
	result = verify_cmac_1_block_msg(&state);
	if (result == TC_FAIL) {
		/* terminate test */
		TC_ERROR("CMAC test #3 (1 block msg)failed.\n");
		goto exitTest;
	}
	(void) tc_cmac_setup(&state, key, &sched);
	result = verify_cmac_320_bit_msg(&state);
	if (result == TC_FAIL) { 
		/* terminate test */
		TC_ERROR("CMAC test #4 (320 bit msg) failed.\n");
		goto exitTest;
	}
	(void) tc_cmac_setup(&state, key, &sched);
	result = verify_cmac_512_bit_msg(&state);
	if (result == TC_FAIL) {
		/* terminate test */
		TC_ERROR("CMAC test #5  (512 bit msg)failed.\n");
		goto exitTest;
	}

	TC_PRINT("All CMAC tests succeeded!\n");

exitTest:
	TC_END_RESULT(result);
	TC_END_REPORT(result);

	return result;
}
