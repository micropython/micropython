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

#ifndef SMALL_AES_INCLUDE_AES_H
#define SMALL_AES_INCLUDE_AES_H

#define AES_128_ROUNDS 10
#define AES_192_ROUNDS 12
#define AES_256_ROUNDS 14

#define AES_MAX_SUBKEYS 15

typedef union aes_state_word {
    uint8_t b[4];
    uint32_t w;
} aes_state_word_t;

typedef union aes_round_state {
    aes_state_word_t words[4];
    uint8_t flat[16];
} aes_round_state_t;

typedef union aes_expanded_key {
    aes_state_word_t rounds[AES_MAX_SUBKEYS][4];
    aes_state_word_t array[AES_MAX_SUBKEYS * 4];
    uint8_t flat[AES_MAX_SUBKEYS * 16];
} aes_expanded_key_t;

typedef struct aes_key_info {
    uint16_t key_length;
    uint16_t n_rounds;
    aes_expanded_key_t expanded_key;
} aes_key_info_t;

void aes_expand_key(aes_key_info_t *dest, const uint8_t *key, int keylen);
void aes_encrypt_block(uint8_t *block, aes_key_info_t *key_info);
void aes_decrypt_block(uint8_t *block, aes_key_info_t *key_info);

// Requires input_length % 16 == 0
void aes_crypt_cbc(aes_key_info_t *key_info, int encrypt, size_t input_length, uint8_t *iv, const uint8_t *in, uint8_t *out);

#endif
