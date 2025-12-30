// SPDX-FileCopyrightText: 2014 Roger Ferrer Ibanez
// SPDX-FileCopyrightText: 2020 Eddy S
// SPDX-FileCopyrightText: 2025 Jeff Epler
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <cassert>
#include <set>
#include <utility>
#include <algorithm>

// This is the first gcc header to be included
#include "gcc-plugin.h"
#include "plugin-version.h"

// gcc headers have order, don't blindly re-order
#include "cp/cp-tree.h"
#include "context.h"
#include "function.h"
#include "internal-fn.h"
#include "is-a.h"
#include "predict.h"
#include "basic-block.h"
#include "tree.h"
#include "tree-ssa-alias.h"
#include "gimple-expr.h"
#include "gimple.h"
#include "gimple-ssa.h"
#include "tree-pretty-print.h"
#include "tree-pass.h"
#include "tree-ssa-operands.h"
#include "tree-phinodes.h"
#include "print-tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "gimple-pretty-print.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "diagnostic.h"
#include "stringpool.h"

#include "ssa-iterators.h"

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

static struct plugin_info my_gcc_plugin_info = {
    "1.0",
    "Validate MicroPython mp_printf argument types against format string"};

namespace {
const pass_data micropython_checks_data = {
    GIMPLE_PASS,
    "micropython_checks", /* name */
    OPTGROUP_NONE,        /* optinfo_flags */
    TV_NONE,              /* tv_id */
    PROP_gimple_any,      /* properties_required */
    0,                    /* properties_provided */
    0,                    /* properties_destroyed */
    0,                    /* todo_flags_start */
    0                     /* todo_flags_finish */
};

struct micropython_checks : gimple_opt_pass {
    micropython_checks(gcc::context *ctx)
        : gimple_opt_pass(micropython_checks_data, ctx) {}

    virtual unsigned int execute(function *fun) override {
        // This phase has two steps, first we remove redundant LHS from
        // GIMPLE_CALLs
        walk(fun);

        return 0;
    }

    virtual micropython_checks *clone() override {
        // We do not clone ourselves
        return this;
    }

    static bool is_mpprint_call(const tree fn) {
        if (!fn)
            return false;
        if (fn->base.code != ADDR_EXPR)
            return false;
        if (!fn->base.constant_flag)
            return false;
        const tree op = fn->exp.operands[0];
        if (op->base.code != FUNCTION_DECL)
            return false;
        const tree name = op->decl_minimal.name;
        if (name->base.code != IDENTIFIER_NODE)
            return false;
        return strcmp((const char *)name->identifier.id.str, "mp_printf") == 0;
    }

    static const char *mpprint_format_arg(gimple *stmt) {
        tree fn = gimple_call_fn(stmt);
        if (!is_mpprint_call(fn))
            return NULL;
        unsigned nargs = gimple_call_num_args(stmt);
        if (nargs < 2)
            return NULL;
        tree arg = gimple_call_arg(stmt, 1);
        if (!arg->base.constant_flag)
            return NULL;
        if (arg->base.code != ADDR_EXPR)
            return NULL;
        tree op = arg->exp.operands[0];
        if (op->base.code != STRING_CST)
            return NULL;
        return op->string.str;
    }

    void walk(function *fun) {
        basic_block bb;
        FOR_ALL_BB_FN(bb, fun) {
            gimple_stmt_iterator gsi;
            for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
                gimple *stmt = gsi_stmt(gsi);

                // location_t loc = gimple_location (stmt);
                switch (gimple_code(stmt)) {
                case GIMPLE_CALL: {
                    const char *fmt = mpprint_format_arg(stmt),
                               *fmt_start = nullptr;

                    if (!fmt) {
                        break;
                    }

                    unsigned nargs = gimple_call_num_args(stmt);
                    unsigned argno = 2;

                    auto require_double = [&](int argno) {
                        auto arg = gimple_call_arg(stmt, argno);
                        location_t loc = gimple_location(stmt);

                        auto tp = arg ? arg->exp.typed.type : 0;
                        if (tp)
                            tp = TYPE_CANONICAL(tp);
                        if (!tp 
                                || tp->base.code != REAL_TYPE
                                || tp->type_common.precision != double_type_node->type_common.precision) {
                            warning_at(loc, OPT_Wformat_,
                                       "argument %d: Format %q.*s requires a "
                                       "%<double%> argument, not %qE",
                                       argno + 1, (int)(fmt - fmt_start + 1),
                                       fmt_start, arg);
                        }
                    };

                    auto require_ptr = [&](int argno) {
                        auto arg = gimple_call_arg(stmt, argno);
                        if (!arg ||
                            arg->exp.typed.type->base.code != POINTER_TYPE) {
                            volatile location_t loc = gimple_location(stmt);
                            warning_at(
                                loc, OPT_Wformat_,
                                "argument %d: Format %q.*s requires a pointer "
                                "argument, not %qE",
                                argno + 1, (int)(fmt - fmt_start + 1),
                                fmt_start, arg);
                        }
                    };

                    auto require_sized_int = [&](int argno, tree type1,
                                                 tree type2) {
                        auto arg = gimple_call_arg(stmt, argno);
                        location_t loc = gimple_location(stmt);

                        auto tp = arg ? arg->exp.typed.type : 0;
                        if (tp && (tp->base.code != INTEGER_TYPE ||
                                   tp->type_common.precision !=
                                       type1->type_common.precision)) {
                            warning_at(
                                loc, OPT_Wformat_,
                                "argument %d: Format %q.*s requires a %qT or "
                                "%qT (%d bits), not %qT [size %d]",
                                argno + 1, (int)(fmt - fmt_start + 1),
                                fmt_start, type1, type2,
                                type2->type_common.precision, tp,
                                tp->type_common.precision);
                        }
                    };

                    auto require_int = [&require_sized_int](int argno) {
                        require_sized_int(argno, integer_type_node,
                                          unsigned_type_node);
                    };
                    auto require_long = [&require_sized_int](int argno) {
                        require_sized_int(argno, long_integer_type_node,
                                          long_unsigned_type_node);
                    };
                    auto require_long_long = [&require_sized_int](int argno) {
                        require_sized_int(argno, long_long_integer_type_node,
                                          long_long_unsigned_type_node);
                    };
                    auto require_qstr = [&require_sized_int](int argno) {
                        require_sized_int(argno, size_type_node,
                                          signed_size_type_node);
                    };

                    for (;;) {
                        while (*fmt != '\0' && *fmt != '%') {
                            ++fmt;
                        }

                        if (*fmt == '\0') {
                            break;
                        }

                        fmt_start = fmt;

                        // move past % character
                        ++fmt;

                        if (*fmt == '%') {
                            ++fmt;
                            continue;
                        }

                        // parse flags, if they exist
                        while (*fmt != '\0') {
                            if (*fmt == '-') {
                            } else if (*fmt == '+') {
                            } else if (*fmt == ' ') {
                            } else if (*fmt == '0') {
                            } else {
                                break;
                            }
                            ++fmt;
                        }

                        // parse width, if it exists
                        int width = 0;
                        for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
                            width = width * 10 + *fmt - '0';
                        }

                        // parse precision, if it exists
                        int prec = -1;
                        if (*fmt == '.') {
                            ++fmt;
                            if (*fmt == '*') {
                                ++fmt;
                                require_int(argno++);
                            } else {
                                prec = 0;
                                for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
                                    prec = prec * 10 + *fmt - '0';
                                }
                            }
                            if (prec < 0) {
                                prec = 0;
                            }
                        }

                        // parse long specifiers

                        bool long_arg = false;
                        if (*fmt == 'l') {
                            ++fmt;
                            long_arg = true;
                        }
                        switch (*fmt) {
                        case 'b':
                        case 'B':
                        case 'i':
                        case 'u':
                        case 'd':
                        case 'x':
                        case 'X':
                        case 'o':
                        case 'O':
                            if (long_arg)
                                require_long(argno++);
                            else
                                require_int(argno++);
                            break;

                        case 'c':
                            require_int(argno++);
                            break;

                        case 'q':
                            require_qstr(argno++);
                            break;

                        case 's':
                        case 'p':
                        case 'P':
                            require_ptr(argno++);
                            break;

                        case 'e':
                        case 'E':
                        case 'f':
                        case 'F':
                        case 'g':
                        case 'G':
                            require_double(argno++);
                            break;

                        case 'l':
                            ++fmt;
                            if (*fmt != 'u' && *fmt != 'd' && *fmt != 'x' && *fmt != 'X') {
                                location_t loc = gimple_location(stmt);
                                warning_at(loc, OPT_Wformat_,
                                           "Bad format specification with ll%c",
                                           *fmt);
                            }
                            require_long_long(argno++);
                            break;
                        default: {
                            location_t loc = gimple_location(stmt);
                            warning_at(loc, OPT_Wformat_,
                                       "Bad format specification with %c",
                                       *fmt);
                        } break;
                        }
                    }
                    if (argno != nargs) {
                        location_t loc = gimple_location(stmt);
                        warning_at(loc, OPT_Wformat_,
                                   "Wrong # arguments. Supplied %d, used %d",
                                   nargs, argno);
                    }
                } break;
                case GIMPLE_ASSIGN:
                case GIMPLE_ASM:
                case GIMPLE_COND:
                case GIMPLE_GOTO:
                case GIMPLE_LABEL:
                case GIMPLE_NOP:
                case GIMPLE_OMP_ATOMIC_LOAD:
                case GIMPLE_OMP_ATOMIC_STORE:
                case GIMPLE_OMP_CONTINUE:
                case GIMPLE_OMP_CRITICAL:
                case GIMPLE_OMP_FOR:
                case GIMPLE_OMP_MASTER:
                case GIMPLE_OMP_ORDERED:
                case GIMPLE_OMP_PARALLEL:
                case GIMPLE_OMP_RETURN:
                case GIMPLE_OMP_SECTION:
                case GIMPLE_OMP_SECTIONS:
                case GIMPLE_OMP_SECTIONS_SWITCH:
                case GIMPLE_OMP_SINGLE:
                case GIMPLE_PHI:
                case GIMPLE_RESX:
                case GIMPLE_RETURN:
                case GIMPLE_SWITCH:
                    // TODO: complete the remaining trees
                    break;
                default:
                    // TODO: even more trees in newer gcc versions
                    break;
                    gcc_unreachable();
                }
            }
        }
    }

    void micropython_checks_lhs(const std::set<tree> &unused_lhs,
                                function *fun) {
        basic_block bb;
        FOR_ALL_BB_FN(bb, fun) {
            gimple_stmt_iterator gsi;
            for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
                gimple *stmt = gsi_stmt(gsi);

                switch (gimple_code(stmt)) {
                case GIMPLE_CALL: {
                    tree lhs = gimple_call_lhs(stmt);
                    if (unused_lhs.find(lhs) != unused_lhs.end()) {
                        // Deliberately similar to the code in tree-cfg.c
                        tree fdecl = gimple_call_fndecl(stmt);
                        tree ftype = gimple_call_fntype(stmt);

                        if (lookup_attribute("micropython_checks",
                                             TYPE_ATTRIBUTES(ftype))) {
                            location_t loc = gimple_location(stmt);

                            if (fdecl)
                                warning_at(loc, OPT_Wunused_result,
                                           "ignoring return value of %qD, "
                                           "declared with attribute warn "
                                           "unused result",
                                           fdecl);
                            else
                                warning_at(loc, OPT_Wunused_result,
                                           "ignoring return value of function "
                                           "declared with attribute warn "
                                           "unused result");
                        }
                    }
                    break;
                }
                default:
                    // Do nothing
                    break;
                }
            }
        }
    }
};
} // namespace

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version) {
    // We check the current gcc loading this plugin against the gcc we used to
    // created this plugin
    if (!plugin_default_version_check(version, &gcc_version)) {
        std::cerr << "This GCC plugin is for version "
                  << GCCPLUGIN_VERSION_MAJOR << "." << GCCPLUGIN_VERSION_MINOR
                  << "\n";
        return 1;
    }

    register_callback(plugin_info->base_name,
                      /* event */ PLUGIN_INFO,
                      /* callback */ NULL, /* user_data */ &my_gcc_plugin_info);

    // Register the phase right after cfg
    struct register_pass_info pass_info;

    pass_info.pass = new micropython_checks(g);
    pass_info.reference_pass_name = "cfg";
    pass_info.ref_pass_instance_number = 1;
    pass_info.pos_op = PASS_POS_INSERT_AFTER;

    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL,
                      &pass_info);

    return 0;
}
