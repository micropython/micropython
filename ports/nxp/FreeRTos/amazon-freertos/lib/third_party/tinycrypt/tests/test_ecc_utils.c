/* test_ecc_utils.c - TinyCrypt common functions for ECC tests */

/* Copyright (c) 2014, Kenneth MacKay
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.*/

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
 *
 *  test_ecc_utils.c -- Implementation of some common functions for ECC tests.
 *
 */

#include <test_ecc_utils.h>
#include <tinycrypt/constants.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

int hex2int (char hex)
{
	uint8_t dec;

	if ('0' <= hex && hex <= '9') dec = hex - '0';
	else if ('a' <= hex && hex <= 'f') dec = hex - 'a' + 10;
	else if ('A' <= hex && hex <= 'F') dec = hex - 'A' + 10;
	else return -1;

	return dec;
}

/*
 * Convert hex string to byte string
 * Return number of bytes written to buf, or 0 on error
 */
int hex2bin(uint8_t *buf, const size_t buflen, const char *hex,
	    const size_t hexlen)
{

	int dec;

	if (buflen < hexlen / 2 + hexlen % 2)
	{
		return false;
	}

	/* if hexlen is uneven, insert leading zero nibble */
	if (hexlen % 2)
	{
		dec = hex2int(hex[0]);
		if (dec == -1)
			return false;
		buf[0] = dec;
		buf++;
		hex++;
	}

	/* regular hex conversion */
	for (size_t i = 0; i < hexlen / 2; i++)
	{
		dec = hex2int(hex[2 * i]);
		if (dec == -1)
		{
			return false;
		}
		buf[i] = dec << 4;

		dec = hex2int(hex[ 2 * i + 1]);
		if (dec == -1)
		{
			return false;
		}
		buf[i] += dec;
	}
	return hexlen / 2 + hexlen % 2;
}

/*
 * Convert hex string to zero-padded nanoECC scalar
 */
void string2scalar(unsigned int *scalar, unsigned int num_word32, char *str)
{

	unsigned int num_bytes = 4 * num_word32;
	uint8_t tmp[num_bytes];
	size_t hexlen = strlen(str);

	int padding;

	if (0 > (padding = 2 * num_bytes - strlen(str)))
	{
		printf("Error: 2 * num_bytes(%d) < strlen(hex) (%zu)\n",
		       2 * num_bytes, strlen(str));
		exit(-1);
	}

	memset(tmp, 0, padding / 2);

	if (false == hex2bin(tmp + padding / 2, num_bytes, str, hexlen))
	{
		exit(-1);
	}
	uECC_vli_bytesToNative(scalar, tmp, num_bytes);

}

void vli_print_bytes(uint8_t *vli, unsigned int size)
{
	for(unsigned i = 0; i < size; ++i)
	{
		printf("%02X ", (unsigned)vli[i]);
	}
}

void print_ecc_scalar(const char *label, const unsigned int * p_vli,
		      unsigned int num_word32)
{
	unsigned int i;

	if (label) {
		printf("%s = { ", label);
	}

	for(i = 0; i < num_word32 - 1; ++i) {
		printf("0x%08lX, ", (unsigned long)p_vli[i]);
	}
	printf("0x%08lX", (unsigned long)p_vli[i]);

	if (label) {
		printf(" };\n");
	}
}

int check_ecc_result(const int num, const char *name,
		      const unsigned int *expected, 
		      const unsigned int *computed,
		      const unsigned int num_word32, const bool verbose)
{
  uint32_t num_bytes = 4 * num_word32;
  if (memcmp(computed, expected, num_bytes)) {
    TC_PRINT("\n  Vector #%02d check %s - FAILURE:\n\n", num, name);
    print_ecc_scalar("Expected", expected, num_word32);
    print_ecc_scalar("Computed", computed, num_word32);
    TC_PRINT("\n");
    return TC_FAIL;
  }
  if (verbose) {
    TC_PRINT("  Vector #%02d check %s - success\n", num, name);
  }
  return TC_PASS;
}

int check_code(const int num, const char *name, const int expected,
		const int computed, const int verbose)
{

	if (expected != computed) {
		TC_ERROR("\n  Vector #%02d check %s - FAILURE:\n", num, name);
		TC_ERROR("\n  Expected: %d, computed: %d\n\n", expected, computed);
		return TC_FAIL;
	}

	if (verbose) {
		TC_PRINT("  Vector #%02d check %s - success (%d=%d)\n", num, name,
		       expected, computed);
	}

	return TC_PASS;
}

/* Test ecc_make_keys, and also as keygen part of other tests */
int keygen_vectors(char **d_vec, char **qx_vec, char **qy_vec, int tests,
		    bool verbose)
{

	unsigned int pub[2 * NUM_ECC_WORDS];
	unsigned int d[NUM_ECC_WORDS];
	unsigned int prv[NUM_ECC_WORDS];
	unsigned int result = TC_PASS;

	/* expected outputs (converted input vectors) */
	unsigned int exp_pub[2 * NUM_ECC_WORDS];
	unsigned int exp_prv[NUM_ECC_WORDS];

	for (int i = 0; i < tests; i++) {
		string2scalar(exp_prv, NUM_ECC_WORDS, d_vec[i]);
		string2scalar(exp_pub, NUM_ECC_WORDS, qx_vec[i]);
		string2scalar(exp_pub + NUM_ECC_WORDS, NUM_ECC_WORDS, qy_vec[i]);

		/*
		 * Feed prvkey vector as padded random seed into ecc_make_key.
		 * Internal mod-reduction will be zero-op and generate correct prv/pub
		 */
		memset(d, 0, NUM_ECC_WORDS);
		string2scalar(d, NUM_ECC_WORDS, d_vec[i]);

		uint8_t pub_bytes[2*NUM_ECC_BYTES];
		uint8_t prv_bytes[NUM_ECC_BYTES];

		uECC_make_key_with_d(pub_bytes, prv_bytes, d, uECC_secp256r1());

		uECC_vli_bytesToNative(prv, prv_bytes, NUM_ECC_BYTES);
		uECC_vli_bytesToNative(pub, pub_bytes, NUM_ECC_BYTES);
		uECC_vli_bytesToNative(pub + NUM_ECC_WORDS, pub_bytes + NUM_ECC_BYTES, NUM_ECC_BYTES);

		/* validate correctness of vector conversion and make_key() */
		result = check_ecc_result(i, "prv  ", exp_prv, prv,  NUM_ECC_WORDS, verbose);
		if (result == TC_FAIL) {
		  return result;
		}
		result = check_ecc_result(i, "pub.x", exp_pub, pub,  NUM_ECC_WORDS, verbose);
		if (result == TC_FAIL) {
		  return result;
		}
		result = check_ecc_result(i, "pub.y", exp_pub + NUM_ECC_WORDS, pub + NUM_ECC_WORDS,  NUM_ECC_WORDS, verbose);
		if (result == TC_FAIL) {
		  return result;
		}
	}
	return result;
}
