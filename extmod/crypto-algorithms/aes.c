/*
 * A small, lightweight AES implmentation suitable for microcontrollers.
 *
 * SPDX-License-Identifier: MIT
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Nicko van Someren
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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "aes.h"

#define ROUNDS_FOR_KEYLEN(l) \
    ((l == 16) ? AES_128_ROUNDS : \
    ((l == 24) ? AES_192_ROUNDS : \
    ((l == 32) ? AES_256_ROUNDS : \
    0)))

static const uint8_t sbox[256] = {
    // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint8_t rsbox[256] = {
    // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// For AES (as distinct from Rijndael), we only need the round constants 1 through 10
static const uint8_t round_constants[11] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Perform s-box substitution and row shift on round state
static void shift_sub_state(aes_state_word_t *state) {
    uint8_t temp;

    // The first row only needs substitution, not rotation
    state[0].b[0] = sbox[state[0].b[0]];
    state[1].b[0] = sbox[state[1].b[0]];
    state[2].b[0] = sbox[state[2].b[0]];
    state[3].b[0] = sbox[state[3].b[0]];

    // The second row is rotated one column left
    temp = sbox[state[0].b[1]];
    state[0].b[1] = sbox[state[1].b[1]];
    state[1].b[1] = sbox[state[2].b[1]];
    state[2].b[1] = sbox[state[3].b[1]];
    state[3].b[1] = temp;

    // The third row is rotated two columns left
    temp = sbox[state[0].b[2]];
    state[0].b[2] = sbox[state[2].b[2]];
    state[2].b[2] = temp;
    temp = sbox[state[1].b[2]];
    state[1].b[2] = sbox[state[3].b[2]];
    state[3].b[2] = temp;

    // The fourth row is rotated three columns left
    temp = sbox[state[3].b[3]];
    state[3].b[3] = sbox[state[2].b[3]];
    state[2].b[3] = sbox[state[1].b[3]];
    state[1].b[3] = sbox[state[0].b[3]];
    state[0].b[3] = temp;
}

// Perform reverse row shift and s-box substitution on round state
static void rsub_shift_state(aes_state_word_t *state) {
    uint8_t temp;

    state[0].b[0] = rsbox[state[0].b[0]];
    state[1].b[0] = rsbox[state[1].b[0]];
    state[2].b[0] = rsbox[state[2].b[0]];
    state[3].b[0] = rsbox[state[3].b[0]];

    temp = rsbox[state[3].b[1]];
    state[3].b[1] = rsbox[state[2].b[1]];
    state[2].b[1] = rsbox[state[1].b[1]];
    state[1].b[1] = rsbox[state[0].b[1]];
    state[0].b[1] = temp;

    temp = rsbox[state[0].b[2]];
    state[0].b[2] = rsbox[state[2].b[2]];
    state[2].b[2] = temp;
    temp = rsbox[state[1].b[2]];
    state[1].b[2] = rsbox[state[3].b[2]];
    state[3].b[2] = temp;

    temp = rsbox[state[0].b[3]];
    state[0].b[3] = rsbox[state[1].b[3]];
    state[1].b[3] = rsbox[state[2].b[3]];
    state[2].b[3] = rsbox[state[3].b[3]];
    state[3].b[3] = temp;
}

// Add round key to state
static void add_round_key(aes_state_word_t *state, aes_state_word_t *key) {
    for (int i = 0; i < 4; i++) {
        state[i].w ^= key[i].w;
    }
}

static inline uint8_t gf_double(uint8_t x) {
    return (x << 1) ^ (((x >> 7) & 1) * 0x1b);
}

static void mix_columns(aes_state_word_t *state) {
    uint8_t temp, all_sum;
    for (int i = 0; i < 4; i++) {
        temp = state[i].b[0];
        all_sum = state[i].b[0] ^ state[i].b[1] ^ state[i].b[2] ^ state[i].b[3];
        state[i].b[0] ^= (gf_double(state[i].b[0] ^ state[i].b[1]) ^ all_sum);
        state[i].b[1] ^= (gf_double(state[i].b[1] ^ state[i].b[2]) ^ all_sum);
        state[i].b[2] ^= (gf_double(state[i].b[2] ^ state[i].b[3]) ^ all_sum);
        state[i].b[3] ^= (gf_double(state[i].b[3] ^ temp) ^ all_sum);
    }
}

// Compute the GF weighted sum 14a + 11b + 13c + 9d
// 14 = 0b1110, 11 = 0b1011, 13 = 0b1101, 9 = 0b1001
static const uint8_t poly_reduce_mask[8] = {0, 27, 54, 45, 108, 119, 90, 65};

static uint8_t gf_ploy_inv(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    uint8_t ac = a ^ c;
    uint8_t bd = b ^ d;
    uint8_t ab = a ^ b;
    uint16_t sum = ((ac ^ bd) << 3) ^ (ac << 2) ^ (ab << 1) ^ (bd ^ c);
    return (uint8_t)(sum ^ poly_reduce_mask[sum >> 8]);
}

static void inv_mix_columns(aes_state_word_t *state) {
    uint8_t a, b, c, d;

    for (int i = 0; i < 4; i++) {
        a = state[i].b[0];
        b = state[i].b[1];
        c = state[i].b[2];
        d = state[i].b[3];

        state[i].b[0] = gf_ploy_inv(a, b, c, d);
        state[i].b[1] = gf_ploy_inv(b, c, d, a);
        state[i].b[2] = gf_ploy_inv(c, d, a, b);
        state[i].b[3] = gf_ploy_inv(d, a, b, c);
    }
}

// Key length is given in bytes, not bits
void aes_expand_key(aes_key_info_t *dest, const uint8_t *key, int keylen) {
    int n_rounds = ROUNDS_FOR_KEYLEN(keylen);
    int key_words = keylen / 4;
    int subkey_words = (n_rounds + 1) * 4;
    aes_state_word_t *array = dest->expanded_key.array;

    dest->key_length = keylen;
    dest->n_rounds = n_rounds;

    memcpy(dest->expanded_key.flat, key, keylen);

    aes_state_word_t prev_word = array[key_words - 1];

    for (int i = key_words; i < subkey_words; i++) {
        aes_state_word_t this_word = array[i - key_words];

        if ((i % key_words) == 0) {
            this_word.b[0] ^= round_constants[i / key_words];

            uint8_t temp = sbox[prev_word.b[0]];
            prev_word.b[0] = sbox[prev_word.b[1]];
            prev_word.b[1] = sbox[prev_word.b[2]];
            prev_word.b[2] = sbox[prev_word.b[3]];
            prev_word.b[3] = temp;
        } else if (key_words > 6 && (i % key_words) == 4) {
            prev_word.b[0] = sbox[prev_word.b[0]];
            prev_word.b[1] = sbox[prev_word.b[1]];
            prev_word.b[2] = sbox[prev_word.b[2]];
            prev_word.b[3] = sbox[prev_word.b[3]];
        }
        this_word.w ^= prev_word.w;
        prev_word = array[i] = this_word;
    }
}

void aes_encrypt_block(uint8_t *block, aes_key_info_t *key_info) {
    uint32_t n_rounds = key_info->n_rounds;
    aes_expanded_key_t *key = &key_info->expanded_key;
    aes_round_state_t *state = (aes_round_state_t *)block;

    add_round_key(state->words, key->rounds[0]);

    uint32_t i;
    for (i = 1; i < n_rounds; i++) {
        shift_sub_state(state->words);
        mix_columns(state->words);
        add_round_key(state->words, key->rounds[i]);
    }

    shift_sub_state(state->words);
    add_round_key(state->words, key->rounds[i]);
}

void aes_decrypt_block(uint8_t *block, aes_key_info_t *key_info) {
    uint32_t n_rounds = key_info->n_rounds;
    aes_expanded_key_t *key = &key_info->expanded_key;
    aes_round_state_t *state = (aes_round_state_t *)block;

    add_round_key(state->words, key->rounds[n_rounds]);
    rsub_shift_state(state->words);

    for (int i = n_rounds - 1; i > 0; i--) {
        add_round_key(state->words, key->rounds[i]);
        inv_mix_columns(state->words);
        rsub_shift_state(state->words);
    }

    add_round_key(state->words, key->rounds[0]);
}

// Requires input_length % 16 == 0
void aes_crypt_cbc(aes_key_info_t *key_info, int encrypt, size_t input_length, uint8_t *iv, const uint8_t *in, uint8_t *out) {
    uint32_t *iv_words = (uint32_t *)iv;
    uint32_t *in_words = (uint32_t *)in;
    uint32_t *out_words = (uint32_t *)out;

    if (encrypt) {
        while (input_length > 0) {
            for (int i = 0; i < 4; i++) {
                iv_words[i] ^= *(in_words++);
            }
            aes_encrypt_block((uint8_t *)iv_words, key_info);
            memcpy(out_words, iv_words, 16);
            out_words += 4;
            input_length -= 16;
        }
    } else {
        uint32_t block_words[4];
        while (input_length > 0) {
            memcpy(block_words, in_words, 16);
            aes_decrypt_block((uint8_t *)block_words, key_info);
            for (int i = 0; i < 4; i++) {
                *(out_words++) = iv_words[i] ^ block_words[i];
                iv_words[i] = *(in_words++);
            }
            input_length -= 16;
        }
    }
}
