/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/persistentcode.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "genhdr/mpversion.h"
#ifdef _WIN32
#include "ports/windows/fmode.h"
#endif

// Command line options, with their defaults
STATIC uint emit_opt = MP_EMIT_OPT_NONE;
mp_uint_t mp_verbose_flag = 0;

// Heap size of GC heap (if enabled)
// Make it larger on a 64 bit machine, because pointers are larger.
long heap_size = 1024 * 1024 * (sizeof(mp_uint_t) / 4);

STATIC void stderr_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    ssize_t dummy = write(STDERR_FILENO, str, len);
    (void)dummy;
}

STATIC const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};

STATIC int compile_and_save(const char *file, const char *output_file, const char *source_file) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_file(file);

        qstr source_name;
        if (source_file == NULL) {
            source_name = lex->source_name;
        } else {
            source_name = qstr_from_str(source_file);
        }

        #if MICROPY_PY___FILE__
        mp_store_global(MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
        #endif

        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_compiled_module_t cm;
        cm.context = m_new_obj(mp_module_context_t);
        mp_compile_to_raw_code(&parse_tree, source_name, false, &cm);

        vstr_t vstr;
        vstr_init(&vstr, 16);
        if (output_file == NULL) {
            vstr_add_str(&vstr, file);
            vstr_cut_tail_bytes(&vstr, 2);
            vstr_add_str(&vstr, "mpy");
        } else {
            vstr_add_str(&vstr, output_file);
        }
        mp_raw_code_save_file(&cm, vstr_null_terminated_str(&vstr));
        vstr_clear(&vstr);

        nlr_pop();
        return 0;
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_stderr_print, (mp_obj_t)nlr.ret_val);
        return 1;
    }
}

STATIC int usage(char **argv) {
    printf(
        "usage: %s [<opts>] [-X <implopt>] <input filename>\n"
        "Options:\n"
        "--version : show version information\n"
        "-o : output file for compiled bytecode (defaults to input with .mpy extension)\n"
        "-s : source filename to embed in the compiled bytecode (defaults to input file)\n"
        "-v : verbose (trace various operations); can be multiple\n"
        "-O[N] : apply bytecode optimizations of level N\n"
        "\n"
        "Target specific options:\n"
        "-msmall-int-bits=number : set the maximum bits used to encode a small-int\n"
        "-march=<arch> : set architecture for native emitter; x86, x64, armv6, armv6m, armv7m, armv7em, armv7emsp, armv7emdp, xtensa, xtensawin\n"
        "\n"
        "Implementation specific options:\n", argv[0]
        );
    int impl_opts_cnt = 0;
    printf(
        #if MICROPY_EMIT_NATIVE
        "  emit={bytecode,native,viper} -- set the default code emitter\n"
        #else
        "  emit=bytecode -- set the default code emitter\n"
        #endif
        );
    impl_opts_cnt++;
    printf(
        "  heapsize=<n> -- set the heap size for the GC (default %ld)\n"
        , heap_size);
    impl_opts_cnt++;

    if (impl_opts_cnt == 0) {
        printf("  (none)\n");
    }

    return 1;
}

// Process options which set interpreter init options
STATIC void pre_process_options(int argc, char **argv) {
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-X") == 0) {
                if (a + 1 >= argc) {
                    exit(usage(argv));
                }
                if (strcmp(argv[a + 1], "emit=bytecode") == 0) {
                    emit_opt = MP_EMIT_OPT_BYTECODE;
                #if MICROPY_EMIT_NATIVE
                } else if (strcmp(argv[a + 1], "emit=native") == 0) {
                    emit_opt = MP_EMIT_OPT_NATIVE_PYTHON;
                } else if (strcmp(argv[a + 1], "emit=viper") == 0) {
                    emit_opt = MP_EMIT_OPT_VIPER;
                #endif
                } else if (strncmp(argv[a + 1], "heapsize=", sizeof("heapsize=") - 1) == 0) {
                    char *end;
                    heap_size = strtol(argv[a + 1] + sizeof("heapsize=") - 1, &end, 0);
                    // Don't bring unneeded libc dependencies like tolower()
                    // If there's 'w' immediately after number, adjust it for
                    // target word size. Note that it should be *before* size
                    // suffix like K or M, to avoid confusion with kilowords,
                    // etc. the size is still in bytes, just can be adjusted
                    // for word size (taking 32bit as baseline).
                    bool word_adjust = false;
                    if ((*end | 0x20) == 'w') {
                        word_adjust = true;
                        end++;
                    }
                    if ((*end | 0x20) == 'k') {
                        heap_size *= 1024;
                    } else if ((*end | 0x20) == 'm') {
                        heap_size *= 1024 * 1024;
                    }
                    if (word_adjust) {
                        heap_size = heap_size * MP_BYTES_PER_OBJ_WORD / 4;
                    }
                } else {
                    exit(usage(argv));
                }
                a++;
            }
        }
    }
}

STATIC char *backslash_to_forwardslash(char *path) {
    for (char *p = path; p != NULL && *p != '\0'; ++p) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    return path;
}

MP_NOINLINE int main_(int argc, char **argv) {
    mp_stack_set_limit(40000 * (sizeof(void *) / 4));

    pre_process_options(argc, argv);

    char *heap = malloc(heap_size);
    gc_init(heap, heap + heap_size);

    mp_init();
    #ifdef _WIN32
    set_fmode_binary();
    #endif

    #if MICROPY_EMIT_NATIVE
    // Set default emitter options
    MP_STATE_VM(default_emit_opt) = emit_opt;
    #else
    (void)emit_opt;
    #endif

    // set default compiler configuration
    mp_dynamic_compiler.small_int_bits = 31;
    // don't support native emitter unless -march is specified
    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_NONE;
    mp_dynamic_compiler.nlr_buf_num_regs = 0;

    const char *input_file = NULL;
    const char *output_file = NULL;
    const char *source_file = NULL;

    // parse main options
    for (int a = 1; a < argc; a++) {
        if (argv[a][0] == '-') {
            if (strcmp(argv[a], "-X") == 0) {
                a += 1;
            } else if (strcmp(argv[a], "--version") == 0) {
                printf("MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE
                    "; mpy-cross emitting mpy v" MP_STRINGIFY(MPY_VERSION) "." MP_STRINGIFY(MPY_SUB_VERSION) "\n");
                return 0;
            } else if (strcmp(argv[a], "-v") == 0) {
                mp_verbose_flag++;
            } else if (strncmp(argv[a], "-O", 2) == 0) {
                if (unichar_isdigit(argv[a][2])) {
                    MP_STATE_VM(mp_optimise_value) = argv[a][2] & 0xf;
                } else {
                    MP_STATE_VM(mp_optimise_value) = 0;
                    for (char *p = argv[a] + 1; *p && *p == 'O'; p++, MP_STATE_VM(mp_optimise_value)++) {;
                    }
                }
            } else if (strcmp(argv[a], "-o") == 0) {
                if (a + 1 >= argc) {
                    exit(usage(argv));
                }
                a += 1;
                output_file = argv[a];
            } else if (strcmp(argv[a], "-s") == 0) {
                if (a + 1 >= argc) {
                    exit(usage(argv));
                }
                a += 1;
                source_file = backslash_to_forwardslash(argv[a]);
            } else if (strncmp(argv[a], "-msmall-int-bits=", sizeof("-msmall-int-bits=") - 1) == 0) {
                char *end;
                mp_dynamic_compiler.small_int_bits =
                    strtol(argv[a] + sizeof("-msmall-int-bits=") - 1, &end, 0);
                if (*end) {
                    return usage(argv);
                }
                // TODO check that small_int_bits is within range of host's capabilities
            } else if (strncmp(argv[a], "-march=", sizeof("-march=") - 1) == 0) {
                const char *arch = argv[a] + sizeof("-march=") - 1;
                if (strcmp(arch, "x86") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X86;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_X86;
                } else if (strcmp(arch, "x64") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X64;
                    mp_dynamic_compiler.nlr_buf_num_regs = MAX(MICROPY_NLR_NUM_REGS_X64, MICROPY_NLR_NUM_REGS_X64_WIN);
                } else if (strcmp(arch, "armv6") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV6;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
                } else if (strcmp(arch, "armv6m") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV6M;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP; // need to be conservative so this code can run on armv7emdp
                } else if (strcmp(arch, "armv7m") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7M;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
                } else if (strcmp(arch, "armv7em") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7EM;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
                } else if (strcmp(arch, "armv7emsp") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7EMSP;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
                } else if (strcmp(arch, "armv7emdp") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7EMDP;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
                } else if (strcmp(arch, "xtensa") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_XTENSA;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_XTENSA;
                } else if (strcmp(arch, "xtensawin") == 0) {
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_XTENSAWIN;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_XTENSAWIN;
                } else if (strcmp(arch, "host") == 0) {
                    #if defined(__i386__) || defined(_M_IX86)
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X86;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_X86;
                    #elif defined(__x86_64__) || defined(_M_X64)
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X64;
                    mp_dynamic_compiler.nlr_buf_num_regs = MAX(MICROPY_NLR_NUM_REGS_X64, MICROPY_NLR_NUM_REGS_X64_WIN);
                    #elif defined(__arm__) && !defined(__thumb2__)
                    mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV6;
                    mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
                    #else
                    mp_printf(&mp_stderr_print, "unable to determine host architecture for -march=host\n");
                    exit(1);
                    #endif
                } else {
                    return usage(argv);
                }
            } else {
                return usage(argv);
            }
        } else {
            if (input_file != NULL) {
                mp_printf(&mp_stderr_print, "multiple input files\n");
                exit(1);
            }
            input_file = backslash_to_forwardslash(argv[a]);
        }
    }

    if (input_file == NULL) {
        mp_printf(&mp_stderr_print, "no input file\n");
        exit(1);
    }

    int ret = compile_and_save(input_file, output_file, source_file);

    #if MICROPY_PY_MICROPYTHON_MEM_INFO
    if (mp_verbose_flag) {
        mp_micropython_mem_info(0, NULL);
    }
    #endif

    mp_deinit();

    return ret & 0xff;
}

int main(int argc, char **argv) {
    mp_stack_ctrl_init();
    return main_(argc, argv);
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

void nlr_jump_fail(void *val) {
    fprintf(stderr, "FATAL: uncaught NLR %p\n", val);
    exit(1);
}
