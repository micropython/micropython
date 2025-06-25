import pathlib
import sys
import textwrap
import yaml

with open(sys.argv[1]) as f:
    defs = yaml.safe_load(f)

if len(sys.argv) > 2:
    modname = sys.argv[2]
else:
    modname = pathlib.Path(sys.argv[1]).stem

signed_integer_types = {'uint8_t', 'uint16_t', 'uint32_t', 'Fixed', 'GrafVerb', 'CharParameter'}
unsigned_integer_types = {'int8_t', 'int16_t', 'int32_t'}


class PointConverter:
    def emit_to_c(self, name_py, name_c):
        return f"Point {name_c} = Point_to_c({name_py});\n"

    def emit_to_py(self, name_c):
        return f"NEW_TUPLE({name_c}.x, {name_c}.y)"


converters = {
    'Point': PointConverter,
}


class SimpleConverter:
    def __init__(self, type_c, to_c, to_py):
        self.type_c = type_c
        self.to_c = to_c
        self.to_py = to_py

    def emit_decls(self):
        pass

    def emit_to_c(self, name_py, name_c):
        return f"{self.type_c} {name_c} = {self.to_c}({name_py});\n"

    def emit_to_py(self, name_c):
        return f"{self.to_py}({name_c})"


class PtrConverter:
    def __init__(self, type_c):
        self.type_c = type_c

    def emit_to_c(self, name_py, name_c):
        is_const = +self.type_c.startswith("const ")  # to get 0/1, not True/False
        return f"{self.type_c} {name_c} = to_c_helper({name_py}, sizeof(*{name_c}), {is_const});\n"

    def emit_to_py(self, name_c):
        is_signed_hint = not self.type_c.lower().startswith('u')
        return f"from_c_helper({name_c}, sizeof(*{name_c}), {+is_signed_hint})"


def make_converter(type_c):
    if converter := converters.get(type_c):
        return converter()
    if type_c in signed_integer_types:
        return SimpleConverter(type_c, "mp_obj_get_int", "mp_obj_new_int")
    if type_c in unsigned_integer_types:
        return SimpleConverter(type_c, "mp_obj_get_int_truncated", "mp_obj_new_int_from_uint")
    if type_c.endswith("*") or type_c.endswith("Handle") or type_c.endswith("Ptr"):
        return PtrConverter(type_c)
    raise ValueError(f"no converter possible for {type_c}")


class Processor:
    def __init__(self, modname):
        self.modname = modname
        self.decls = []
        self.body = []
        self.locals = []
        self.info = []
        self.unknowns = set()
        self.types = {
            'Byte': 'uint8_t',
            'Boolean': 'uint8_t',
            'SignedByte': 'int8_t',
            'Sint8': 'int8_t',
            'Uint8': 'uint8_t',
            'Sint16': 'int16_t',
            'Uint16': 'uint16_t',
            'Sint32': 'int32_t',
            'Uint32': 'uint32_t',
            'ULONGINT': 'uint32_t',
            'INTEGER': 'int16_t',
            'LONGINT': 'int32_t',
        }
        self.body_dedent("""
        #include "py/obj.h"
        #include "py/runtime.h"
        #include <Multiverse.h>

        // Relies on gcc Variadic Macros and Statement Expressions
        #define NEW_TUPLE(...) \
            ({mp_obj_t _z[] = {__VA_ARGS__}; mp_obj_new_tuple(MP_ARRAY_SIZE(_z), _z); })

        void *to_c_helper(mp_obj_t obj, size_t objsize, bool is_const) {
            if (mp_obj_is_int(obj)) {
                return (void*)mp_obj_get_int_truncated(obj);
            }
            mp_buffer_info_t bufinfo = {0};
            mp_get_buffer_raise(obj, &bufinfo, is_const ? MP_BUFFER_READ : MP_BUFFER_READ | MP_BUFFER_WRITE);
            if (bufinfo.len != objsize) {
                mp_raise_ValueError(MP_ERROR_TEXT("buffer has wrong length"));
            }
            return bufinfo.buf;
        }

        Point Point_to_c(mp_obj_t obj) {
            Point result;
            if (mp_obj_len_maybe(obj) == MP_OBJ_NEW_SMALL_INT(2)) {
                result.h = mp_obj_get_int(mp_obj_subscr(obj, mp_obj_new_int(0), MP_OBJ_SENTINEL));
                result.v = mp_obj_get_int(mp_obj_subscr(obj, mp_obj_new_int(1), MP_OBJ_SENTINEL));
            } else {
                result = *(Point*)to_c_helper(obj, sizeof(Point), true);
            }
            return result;
        }

        mp_obj_t from_c_helper(void *buf, size_t objsize, bool is_signed_hint) {
            return mp_obj_new_int_from_uint(*(unsigned long*)buf);
        }

        mp_obj_t LMGet_common(long address, size_t objsize, mp_obj_t arg) {
            if (arg == mp_const_none) {
                return mp_obj_new_bytearray(objsize, (void*)address);
            }
            mp_buffer_info_t bufinfo = {0};
            mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_WRITE);
            if (bufinfo.len != objsize) {
                mp_raise_ValueError(MP_ERROR_TEXT("buffer has wrong length"));
            }
            memcpy(bufinfo.buf, (void*)address, objsize);
            return arg;
        }
        void LMSet_common(long address, size_t objsize, mp_obj_t arg) {
            mp_buffer_info_t bufinfo = {0};
            mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
            if (bufinfo.len != objsize) {
                mp_raise_ValueError(MP_ERROR_TEXT("buffer has wrong length"));
            }
            memcpy((void*)address, bufinfo.buf, objsize);
        }
        """)

    def resolve_type(self, typename):
        is_const = self.is_const(typename)
        typename = typename.removeprefix("const ")

        while typename in self.types:
            typename = self.types[typename]

        if is_const and self.is_ptr(typename):
            return f"const {typename}"

        return typename

    def is_ptr(self, typename):
        return self.resolve_type(typename).endswith("*")

    def is_const(self, typename):
        return typename.startswith("const ")

    def body_dedent(self, text):
        self.body.append(textwrap.dedent(text.rstrip()))

    def process(self, defs):
        for d in defs:
            self.only_for = d.pop('only-for', None)
            self.not_for = d.pop('not-for', None)
            self.api = d.pop('api', None)

            if len(d) != 1:
                raise ValueError(f"Definition with more than one key: {defs!r}")
            k, v = d.popitem()
            meth = getattr(self, 'handle_' + k, lambda v: self.handle_unknown(k, v))
            meth(v)

    def handle_unknown(self, k, v):
        if k in self.unknowns:
            return
        self.unknowns.add(k)
        self.info.append(f"# Unknown {k} {v!r:.55s}...")

    def handle_enum(self, e):
        for v in e['values']:
            if 'value' in v:
                self.locals.append(
                    f"{{ MP_ROM_QSTR(MP_QSTR_{v['name']}), MP_ROM_INT({v['value']}) }},"
                )
            # else:
            # self.info.append(f"enumerant without value: {v['name']}")

    def handle_lowmem(self, lm):
        name = lm['name']
        address = lm['address']
        typename = lm['type']
        self.body_dedent(f"""
            static mp_obj_t LMGet{name}_fn(size_t n_args, const mp_obj_t *args) {{
                return LMGet_common({address}, sizeof({typename}), n_args == 0 ? mp_const_none : args[0]);
            }}
            MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(LMGet{name}_obj, 0, 1, LMGet{name}_fn);

            static mp_obj_t LMSet{name}_fn(mp_obj_t value) {{
                LMSet_common({address}, sizeof({typename}), value);
                return mp_const_none;
            }}
            MP_DEFINE_CONST_FUN_OBJ_1(LMSet{name}_obj, LMSet{name}_fn);
            """)
        self.locals.append(
            f"{{ MP_ROM_QSTR(MP_QSTR_LMGet{name}), MP_ROM_PTR(&LMGet{name}_obj) }},"
        )
        self.locals.append(
            f"{{ MP_ROM_QSTR(MP_QSTR_LMSet{name}), MP_ROM_PTR(&LMSet{name}_obj) }},"
        )

    def handle_typedef(self, v):
        pass  # ignore typedefs for now ??

    def handle_verbatim(self, v):
        pass  # Ignore verbatim blocks

    # {'args': [{'name': 'src_bitmap', 'type': 'BitMap*'},
    #          {'name': 'dst_bitmap', 'type': 'BitMap*'},
    #          {'name': 'src_rect', 'type': 'const Rect*'},
    #          {'name': 'dst_rect', 'type': 'const Rect*'},
    #          {'name': 'mode', 'type': 'INTEGER'},
    #          {'name': 'mask', 'type': 'RgnHandle'}],
    # 'executor': 'C_',
    # 'name': 'CopyBits',
    # 'trap': 43244}

    def fun_declare_args_enum(self, args):
        if args:
            args = ", ".join(f"ARG_{arg['name']}" for arg in args)
            return f"enum {{ {args} }};"
        return ""

    @staticmethod
    def fun_declare_allowed_arg(arg):
        name = arg['name']
        return f"{{ MP_QSTR_{name}, MP_ARG_OBJ | MP_ARG_REQUIRED, {{0}}, }},"

    def fun_parse_args(self, args):
        body = [
            self.fun_declare_args_enum(args),
            "static const mp_arg_t allowed_args[] = {",
            *(self.fun_declare_allowed_arg(a) for a in args),
            "};",
            "mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];",
            "mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);",
        ]
        return "\n".join(f"    {line}" for line in body)

    def make_converter(self, typename):
        print(f"make_converter {typename}", file=sys.stderr)
        if not typename.endswith('*'):
            typename = self.resolve_type(typename)
        print(f" -> {typename}", file=sys.stderr)
        return make_converter(typename)

    def fun_convert_arg(self, idx, arg):
        return self.make_converter(arg['type']).emit_to_c(f"args[{idx}].u_obj", arg['name'])

    def fun_convert_args(self, args):
        return "".join("    " + self.fun_convert_arg(i, a) for i, a in enumerate(args))

    def fun_call_fun(self, fun):
        return_type = fun.get('return', None)
        args = fun.get('args', [])
        fun_args = ", ".join(arg['name'] for arg in args)
        funcall = f"{fun['name']}({fun_args});"
        if return_type:
            funcall = f"{return_type} retval = {funcall}"
        return "    " + funcall

    def fun_convert_return(self, fun):
        return_type = fun.get('return', None)
        if return_type:
            converter = self.make_converter(return_type)
            return f"    return {converter.emit_to_py('retval')};// TODO"
        else:
            return "    return mp_const_none;"

    def handle_function(self, fun):
        name = fun['name']
        args = fun.get('args', [])
        print(f"## API={self.api}", file=sys.stderr)
        if self.api == 'carbon':
            return
        self.body_dedent(f"""
        mp_obj_t {name}_fn(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {{
             {self.fun_parse_args(args)}
             {self.fun_convert_args(args)}
             {self.fun_call_fun(fun)}
             {self.fun_convert_return(fun)}
        }}
        MP_DEFINE_CONST_FUN_OBJ_KW({name}_obj, {len(args)}, {name}_fn);
        """)
        self.locals.append(f"{{ MP_ROM_QSTR(MP_QSTR_{name}), MP_ROM_PTR(&{name}_obj) }},")

    def handle_funptr(self, fun):
        pass  # Ignore function pointers for now

    def emit(self):
        for row in self.decls:
            print(row)
        for row in self.body:
            print(row)
            print()
        print("static const mp_rom_map_elem_t module_globals_table[] = {")
        print(f"    {{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_{self.modname}) }},")
        for row in self.locals:
            print(f"    {row}")
        print("};")
        print("static MP_DEFINE_CONST_DICT(module_globals, module_globals_table);")
        print(
            textwrap.dedent(f"""
            const mp_obj_module_t {self.modname}_module = {{
                .base = {{ &mp_type_module }},
                .globals = (mp_obj_dict_t *)&module_globals,
            }};

            MP_REGISTER_MODULE(MP_QSTR_{self.modname}, {self.modname}_module);""")
        )

        for row in self.info:
            print(row, file=sys.stderr)


p = Processor(modname)
p.process(defs)
p.emit()
