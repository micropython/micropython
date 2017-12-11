/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Linaro Limited
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
#include <string.h>

#include "py/mphal.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/compile.h"
#include "upytesthelper.h"

static const char *test_exp_output;
static int test_exp_output_len, test_rem_output_len;
static int test_failed;
static void *heap_start, *heap_end;

void upytest_set_heap(void *start, void *end) {
    heap_start = start;
    heap_end = end;
}

void upytest_set_expected_output(const char *output, unsigned len) {
    test_exp_output = output;
    test_exp_output_len = test_rem_output_len = len;
    test_failed = false;
}

bool upytest_is_failed(void) {
    if (test_failed) {
        return true;
    }
    #if 0
    if (test_rem_output_len != 0) {
        printf("remaining len: %d\n", test_rem_output_len);
    }
    #endif
    return test_rem_output_len != 0;
}

// MP_PLAT_PRINT_STRN() should be redirected to this function.
// It will pass-thru any content to mp_hal_stdout_tx_strn_cooked()
// (the dfault value of MP_PLAT_PRINT_STRN), but will also match
// it to the expected output as set by upytest_set_expected_output().
// If mismatch happens, upytest_is_failed() returns true.
void upytest_output(const char *str, mp_uint_t len) {
    if (!test_failed) {
        if (len > test_rem_output_len) {
            test_failed = true;
        } else {
            test_failed = memcmp(test_exp_output, str, len);
            #if 0
            if (test_failed) {
                printf("failed after char %u, within %d chars, res: %d\n",
                    test_exp_output_len - test_rem_output_len, (int)len, test_failed);
                for (int i = 0; i < len; i++) {
                    if (str[i] != test_exp_output[i]) {
                        printf("%d %02x %02x\n", i, str[i], test_exp_output[i]);
                    }
                }
            }
            #endif
            test_exp_output += len;
            test_rem_output_len -= len;
        }
    }
    mp_hal_stdout_tx_strn_cooked(str, len);
}

void upytest_execute_test(const char *src) {
    // To provide clean room for each test, interpreter and heap are
    // reinitialized before running each.
    gc_init(heap_start, heap_end);
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_init(mp_sys_argv, 0);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, false);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        mp_obj_t exc = (mp_obj_t)nlr.ret_val;
        if (mp_obj_is_subclass_fast(mp_obj_get_type(exc), &mp_type_SystemExit)) {
            // Assume that sys.exit() is called to skip the test.
            // TODO: That can be always true, we should set up convention to
            // use specific exit code as skip indicator.
            tinytest_set_test_skipped_();
            goto end;
        }
        mp_obj_print_exception(&mp_plat_print, exc);
        tt_abort_msg("Uncaught exception\n");
    }

    if (upytest_is_failed()) {
        tinytest_set_test_failed_();
    }

end:
    mp_deinit();
}
