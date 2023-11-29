/*
 * Simple LZ77 streaming compressor.
 *
 * The scheme implemented here doesn't use a hash table and instead does a brute
 * force search in the history for a previous string.  It is relatively slow
 * (but still O(N)) but gives good compression and minimal memory usage.  For a
 * small history window (eg 256 bytes) it's not too slow and compresses well.
 *
 * MIT license; Copyright (c) 2021 Damien P. George
 */

#include "uzlib.h"

#include "defl_static.c"

#define MATCH_LEN_MIN (3)
#define MATCH_LEN_MAX (258)

// hist should be a preallocated buffer of hist_max size bytes.
// hist_max should be greater than 0 a power of 2 (ie 1, 2, 4, 8, ...).
// It's possible to pass in hist=NULL, and then the history window will be taken from the
// src passed in to uzlib_lz77_compress (this is useful when not doing streaming compression).
void uzlib_lz77_init(uzlib_lz77_state_t *state, uint8_t *hist, size_t hist_max) {
    memset(state, 0, sizeof(uzlib_lz77_state_t));
    state->hist_buf = hist;
    state->hist_max = hist_max;
    state->hist_start = 0;
    state->hist_len = 0;
}

// Search back in the history for the maximum match of the given src data,
// with support for searching beyond the end of the history and into the src buffer
// (effectively the history and src buffer are concatenated).
static size_t uzlib_lz77_search_max_match(uzlib_lz77_state_t *state, const uint8_t *src, size_t len, size_t *longest_offset) {
    size_t longest_len = 0;
    for (size_t hist_search = 0; hist_search < state->hist_len; ++hist_search) {
        // Search for a match.
        size_t match_len;
        for (match_len = 0; match_len <= MATCH_LEN_MAX && match_len < len; ++match_len) {
            uint8_t hist;
            if (hist_search + match_len < state->hist_len) {
                hist = state->hist_buf[(state->hist_start + hist_search + match_len) & (state->hist_max - 1)];
            } else {
                hist = src[hist_search + match_len - state->hist_len];
            }
            if (src[match_len] != hist) {
                break;
            }
        }

        // Take this match if its length is at least the minimum, and larger than previous matches.
        // If the length is the same as the previous longest then take this match as well, because
        // this match will be closer (more recent in the history) and take less bits to encode.
        if (match_len >= MATCH_LEN_MIN && match_len >= longest_len) {
            longest_len = match_len;
            *longest_offset = state->hist_len - hist_search;
        }
    }

    return longest_len;
}

// Compress the given chunk of data.
void uzlib_lz77_compress(uzlib_lz77_state_t *state, const uint8_t *src, unsigned len) {
    const uint8_t *top = src + len;
    while (src < top) {
        // Look for a match in the history window.
        size_t match_offset = 0;
        size_t match_len = uzlib_lz77_search_max_match(state, src, top - src, &match_offset);

        // Encode the literal byte or the match.
        if (match_len == 0) {
            uzlib_literal(state, *src);
            match_len = 1;
        } else {
            uzlib_match(state, match_offset, match_len);
        }

        // Push the bytes into the history buffer.
        size_t mask = state->hist_max - 1;
        while (match_len--) {
            uint8_t b = *src++;
            state->hist_buf[(state->hist_start + state->hist_len) & mask] = b;
            if (state->hist_len == state->hist_max) {
                state->hist_start = (state->hist_start + 1) & mask;
            } else {
                ++state->hist_len;
            }
        }
    }
}
