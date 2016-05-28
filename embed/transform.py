"""
This script transforms MicroPython source files so the mp_state_ctx global
variable is passed as the first argument to (almost) all functions.

Be warned, there are many dragons here!

TODO: convert qstr to mp_qstr_t
"""

import sys
import re

mp_prefix = "micropy_"
state_arg = "struct _mp_state_ctx_t *mp_state"
state_var = "mp_state"

def process_do_nothing(match):
    return match.group(0)

def process_fun_decl_defn_incomplete(match):
    d = match.groupdict()
    fun = d["fun"]

    if fun.startswith("mp_"):
        fun = fun[3:]

    args = "%s, %s" % (state_arg, d["args"])

    return "%s%s%s%s(%s," % (d["static"], d["rettype"], mp_prefix, fun, args)

def process_fun_decl_defn(match, trailing):
    d = match.groupdict()
    fun = d["fun"]

    if fun in ("DEBUG_printf",) or fun.startswith("utf8_") or fun.startswith("unichar_") or fun.startswith("MP_"):
        return match.group(0)

    if fun == "set_clear":
        # because mp_set_clear is distinct from set_clear
        fun = "obj_set_clear"

    if fun.startswith("mp_"):
        fun = fun[3:]

    if d["args"] == "void":
        args = state_arg
    else:
        args = "%s, %s" % (state_arg, d["args"])

    #if d["static"] is None:
    #    return "%s%s%s(%s)%s" % (d["rettype"], mp_prefix, fun, args, trailing)
    #else:
    return "%s%s%s%s(%s)%s" % (d["static"], d["rettype"], mp_prefix, fun, args, trailing)

def process_fun_decl(match):
    return process_fun_decl_defn(match, ";")

def process_fun_defn(match):
    return process_fun_decl_defn(match, " {")

def process_fun_defn_oneline(match):
    return process_fun_decl_defn(match, match.groupdict()["body"])

def process_struct_init(match):
    d = match.groupdict()
    value = d["value"]

    if value in ("false", "true"):
        return match.group(0)

    if value.startswith("mp_"):
        value = value[3:]

    return "    .%s = %s%s,%s" % (d["member"], mp_prefix, value, d["end"])

def process_type_fun_typedef(match):
    d = match.groupdict()

    if d["args"] == "void":
        args = state_arg
    else:
        args = "%s, %s" % (state_arg, d["args"])

    return "%s(%s);" % (d["head"], args)

def process_fun_obj_defn(match):
    d = match.groupdict()
    fun = d["fun"]
    #if fun in ("mp_globals_get", "mp_locals_get"):
    #    return "%s(%s%s);" % (d["head"], d["args"], fun)
    if fun == "set_clear":
        fun = "obj_set_clear"
    elif fun.startswith("mp_"):
        fun = fun[3:]
    return "%s(%s%s%s);" % (d["head"], d["args"], mp_prefix, fun)

def process_compile_def(match):
    return "#define c(f) %scompile_##f" % mp_prefix

def process_gc_def(match):
    d = match.groupdict()
    fun1 = d["fun1"]
    fun2 = d["fun2"]
    if fun1 == "free":
        return "#define free(p) %sgc_free(%s, (p))" % (mp_prefix, state_var)
    else:
        return "#define realloc(p, n) %sgc_realloc(%s, (p), (n))" % (mp_prefix, state_var)

def process_locals_globals(match):
    d = match.groupdict()
    dct = d["dict"]
    op = d["op"]
    if d["op"] == "get":
        return "static inline mp_obj_dict_t *%s%s_get(%s) { return %s->dict_%s; }" % (mp_prefix, dct, state_arg, state_var, dct)
    else:
        return "static inline void %s%s_set(%s, mp_obj_dict_t *d) { %s->dict_%s = d; }" % (mp_prefix, dct, state_arg, state_var, dct)

c_functions = (
    "open",
    "read",
    "write",
    "close",
    "sizeof",
    "offsetof",
    "va_start",
    "va_end",
    "va_copy",
    "va_arg",
    "assert",
    "longjmp",
    "setjmp",
    "strlen",
    "strchr",
    "strncmp",
    "memset",
    "memcpy",
    "memmove",
    "memcmp",
    "alloca",
    "malloc",
    "malloc_with_finaliser",
    "realloc",
    "free",
    "printf",
    "vprintf",
    "sprintf",
    "snprintf",
    "vsnprintf",
    "isnan",
    "isinf",
    "signbit",
    "fpclassify",
    "DEBUG_printf",
    "QDEF", # macro for qstrs
    "op", # argument for str_caseconv
    "allocator", # argument for emit_write_uint
    "f", # argument for apply_to_single_or_list
    "comp_fun", # variable in compile_node
    "re1_5_recursiveloopprog",
    "re1_5_sizecode",
    "re1_5_compilecode",
    "re1_5_dumpcode",
    "re1_5_fatal",
    "sha256_init",
    "sha256_update",
    "sha256_final",
)

def x1_vm(m):
    return "(%s)->vm.%s" % (state_var, m.groupdict()["field"])

def x1_ctx(m):
    return "(%s)->%s" % (state_var, m.groupdict()["field"])

def x1_mem(m):
    return "(%s)->mem.%s" % (state_var, m.groupdict()["field"])

def x2(m):
    fun = m.groupdict()["fun"]
    if fun in c_functions:
        return m.group(0)
    if fun.startswith("utf8_") or fun.startswith("unichar_"):
        return m.group(0)
    assert not fun.startswith(mp_prefix)

    if fun in ("stream_next_byte", "stream_close", "io_func"):
        # special cases
        return "%s(%s, " % (fun, state_var)

    if fun == "set_clear":
        # because mp_set_clear is distinct from set_clear
        fun = "obj_set_clear"
    if fun.startswith("mp_"):
        fun = fun[3:]
    if m.groupdict()["closeparen"] is None:
        return "%s%s(%s, " % (mp_prefix, fun, state_var)
    else:
        return "%s%s(%s)" % (mp_prefix, fun, state_var)

def ins_state_var(m):
    d = m.groupdict()
    if d["closeparen"] is None:
        return "%s(%s, " % (d["prefix"], state_var)
    else:
        return "%s(%s)" % (d["prefix"], state_var)

def tr_mp_fun(m):
    fun = m.group(0)
    if fun.startswith("mp_"):
        fun = fun[3:]
    return mp_prefix + fun

def tr_lo_gl_get(m):
    return "%s->dict_%s" % (state_var, m.group(1))

def tr_lo_gl_set(m):
    return "%s->dict_%s = %s" % (state_var, m.group(1), m.group(2))

xxx = (
    (x1_vm, re.compile(r"MP_STATE_VM\((?P<field>[a-z0-9_]+)\)")),
    (x1_ctx, re.compile(r"MP_STATE_CTX\((?P<field>[a-z0-9_]+)\)")),
    (x1_mem, re.compile(r"MP_STATE_MEM\((?P<field>[a-z0-9_]+)\)")),

    # globals/locals
    (tr_lo_gl_get, re.compile(r"mp_(locals|globals)_get\(\)")),
    (tr_lo_gl_set, re.compile(r"mp_(locals|globals)_set\(([a-z_\->]+)\)")),

    (x2, re.compile(r"(?<=[ ()!*])(?P<fun>[a-z][a-z0-9_]+)\((?P<closeparen>\))?")),

    (ins_state_var, re.compile(r"(?P<prefix>(\.|->)(print|make_new|call|unary_op|binary_op|attr|subscr|getiter|iternext|buffer_p\.get_buffer))\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>stream_p->(read|write|ioctl))\((?P<closeparen>\))?")),
    #(ins_state_var, re.compile(r"(?P<prefix>\(mp_fun_(0|1|2|3|var|kw)_t\)self->fun\))\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>self->fun\.(_0|_1|_2|_3|var|kw))\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>print->print_strn)\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>lex->stream_next_byte)\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>lex->stream_close)\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>emit_method_table->[a-z_]+)\((?P<closeparen>\))?")),
    #(ins_state_var, re.compile(r"(?P<prefix>\(\*allocator\))\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>allocator)\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix> f)\((?P<closeparen>\))?")),
    (ins_state_var, re.compile(r"(?P<prefix>comp_fun)\((?P<closeparen>\))?")),

    # these functions are used as pointers within expressions
    (tr_mp_fun, re.compile(r"(?<!_)(plat_print_strn|mp_obj_tuple_getiter|dict_make_new|dict_it_iternext|list_it_iternext|set_it_iternext|bytes_it_iternext|str_it_iternext|tuple_it_iternext|mp_obj_str_binary_op|vstr_printf|mp_obj_str_get_buffer|mp_obj_instance_call|mp_obj_exception_make_new|mp_obj_instance_make_new|str_buf_next_byte|str_buf_free|file_buf_next_byte|file_buf_close|mp_stream_write|mp_stream_write_adaptor|array_subscr)(?!_)")),
    (tr_mp_fun, re.compile(r"vstr_add_strn(?!\()")),
    # these are used as pointers, specifically to set up a dynamic mp_obj_type_t struct
    (tr_mp_fun, re.compile(r"instance_print|instance_unary_op|instance_binary_op|mp_obj_instance_attr|instance_subscr|instance_getiter|instance_get_buffer")),
    (tr_mp_fun, re.compile(r"namedtuple_(print|make_new|attr)|mp_obj_tuple_unary_op|mp_obj_tuple_binary_op|mp_obj_tuple_subscr|mp_obj_tuple_getiter")),
    # for emit_bc and compiler
    (x2, re.compile(r"(?P<fun>mp_emit_bc_##fun)\((?P<closeparen>\))?")),
    (tr_mp_fun, re.compile(r"mp_emit_bc_[a-z_]+")),
    (tr_mp_fun, re.compile(r"mp_emit_bc_##fun")),
    (tr_mp_fun, re.compile(r"(?<!_)emit_get_cur_to_write_[a-z_]+")),
    # these are used as pointers in the compiler
    (tr_mp_fun, re.compile(r"(?<!_)(compile_funcdef_lambdef_param|c_del_stmt|compile_dotted_as_name|compile_string|compile_bytes|compile_const_object|compile_scope_func_param|compile_scope_func_annotations|compile_scope_lambda_param)")),
)

def process_generic(line):
    if line.startswith("#define MP_STATE_"):
        return line
    elif line.startswith("#define NORETURN"):
        return line
    elif line.startswith("#define "):
        pass
    elif line.startswith("static inline "):
        pass
    elif line.startswith("extern const mp_emit_method_table_id_ops_t mp_emit_bc_method"):
        pass
    elif line.startswith("const mp_emit_method_table_id_ops_t mp_emit_bc_method"):
        pass
    elif line.startswith("const mp_print_t "):
        pass
    elif not line.startswith("    "):
        return line

    # TODO: don't process strings

    for xx, rr in xxx:
        last_idx = 0
        parts = []
        for m in rr.finditer(line):
            parts.append(line[last_idx:m.start()])
            parts.append(xx(m))
            last_idx = m.end()
        parts.append(line[last_idx:])
        line = "".join(parts)

    return line

regex_type = r"(const )?(void|int|uint|long long|size_t|unsigned int|char|byte|bool|unichar|vstr_t|qstr|scope_t|id_info_t|emit_t|mpz_t|mp_[a-z_]+_t) \*?"
line_regexs = (
    (process_locals_globals, re.compile(r"static inline (mp_obj_dict_t \*|void )mp_(?P<dict>locals|globals)_(?P<op>[gs]et)\(.+\) \{ .+; \}$")),
    (process_fun_decl, re.compile(r"(?P<static>(STATIC )?(NORETURN )?)(?P<rettype>" + regex_type + r")(?P<fun>[A-Za-z0-9_]+)\((?P<args>.+)\);$")),
    (process_fun_defn, re.compile(r"(?P<static>((STATIC|static) (inline )?)?(NORETURN )?)(?P<rettype>" + regex_type + r")(?P<fun>[A-Za-z0-9_]+)\((?P<args>.+)\) {$")),
    (process_fun_defn_oneline, re.compile(r"(?P<static>static inline )(?P<rettype>" + regex_type + r")(?P<fun>[A-Za-z0-9_]+)\((?P<args>.+)\)(?P<body> {.*})$")),
    (process_fun_decl_defn_incomplete, re.compile(r"(?P<static>(STATIC )?(NORETURN )?)(?P<rettype>" + regex_type + r")(?P<fun>[A-Za-z0-9_]+)\((?P<args>.+),$")),
    #(process_fun_defn, re.compile(r"(?P<static>static inline )(?P<rettype>" + regex_type + r")(?P<fun>mp_(locals|globals)_(get|set))\((?P<args>.+)\) {$")),
    (process_do_nothing, re.compile(r" *#(el)?if .+$")),
    (process_do_nothing, re.compile(r"static inline .+$")),
    (process_do_nothing, re.compile(r"#define (or|and|and_ident|and_blank|tok|rule|opt_rule)\(.+$")),
    (process_compile_def, re.compile(r"#define c\(f\) compile_##f$")),
    (process_gc_def, re.compile(r"#define (?P<fun1>free|realloc) (?P<fun2>gc_[a-z]+)$")),
    (process_struct_init, re.compile(r"    \.(?P<member>[a-z_]+) = (?P<value>[a-z_]+),(?P<end>( \\)?)$")),
    (process_struct_init, re.compile(r"    \.(?P<member>[a-z_]+ = \{ \.[a-z_]+) = (?P<value>[a-z_]+)(?P<end> \},)$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>typedef " + regex_type + r"\(\*mp_(print_strn|fun_0|fun_1|fun_2|fun_3|fun_var|fun_kw|print_fun|make_new_fun|call_fun|unary_op_fun|binary_op_fun|attr_fun|subscr_fun|lexer_stream_next_byte|lexer_stream_close)_t\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>typedef " + regex_type + r"\(\*(emit_allocator|apply_list_fun)_t\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>typedef void \(\*compile_function_t\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>typedef void \(\*list_fun_t\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>    typedef mp_uint_t \(\*io_func_t\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>    void \(\*print_strn\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>    mp_int_t \(\*get_buffer\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>    mp_uint_t \(\*(read|write|ioctl)\))\((?P<args>.+)\);$")),
    (process_type_fun_typedef, re.compile(r"(?P<head>    (void|bool) \(\*[a-z_]+\))\((?P<args>emit_t \*emit.*)\);$")),
    (process_fun_obj_defn, re.compile(r"(?P<head>(STATIC )?MP_DEFINE_CONST_FUN_OBJ([A-Z0-9_]*))\((?P<args>.+, )(?P<fun>[a-z0-9_]+)\);$")),
    (lambda m: "", re.compile(r"    \.globals = \(mp_obj_dict_t\*\)&MP_STATE_VM\(dict_main\),$")), # XXX

    # sys.path, sys.argv, sys.modules handling
    # at the moment we remove these from the sys module, but they still exist in the state
    (lambda m:
        r"",
        re.compile(r"    \{ MP_ROM_QSTR\(MP_QSTR_path\), MP_ROM_PTR\(&MP_STATE_VM\(mp_sys_path_obj\)\) \},$")),
    (lambda m:
        r"",
        re.compile(r"    \{ MP_ROM_QSTR\(MP_QSTR_argv\), MP_ROM_PTR\(&MP_STATE_VM\(mp_sys_argv_obj\)\) \},$")),
    (lambda m:
        r"",
        re.compile(r"    \{ MP_ROM_QSTR\(MP_QSTR_modules\), MP_ROM_PTR\(&MP_STATE_VM\(mp_loaded_modules_dict\)\) \},$")),

    (lambda m: "    ((defval == MP_OBJ_NULL) ? " + mp_prefix + "load_method : " + mp_prefix + "load_method_maybe)(" + state_var + ", base, attr, dest);", re.compile(r"    \(\(defval == MP_OBJ_NULL\) \? mp_load_method : mp_load_method_maybe\)\(base, attr, dest\);$")),
    (lambda m: "    void **ptrs = (void**)(void*)&" + state_var + "->dict_locals;", re.compile(r"    void \*\*ptrs = \(void\*\*\)\(void\*\)&mp_state_ctx;$")),
    (process_generic, None),
)

def process_line(line):
    m = re.search(r" *//", line)
    if m:
        comment = line[m.start():]
        line = line[:m.start()]
    else:
        comment = ""

    for fun, regex in line_regexs:
        if regex is None:
            line = fun(line)
            break
        else:
            m = regex.match(line)
            if m:
                line = fun(m)
                break

    return line + comment

def do_work(filename):
    with open(filename) as f:
        lines = f.readlines()

    for f, l in (
            ("py/misc.h", 37),
            ("py/parse.h", 33),
            ("py/mphal.h", 36),
            ("py/mpprint.h", 42),
            ("py/formatfloat.c", 30)):
        if filename.endswith(f):
            lines.insert(l, "struct _mp_state_ctx_t;\n")

    for line in lines:
        line = line[:-1] # strip trailing \n
        print(process_line(line))

do_work(sys.argv[1])
