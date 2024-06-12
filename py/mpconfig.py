#!/usr/bin/env python

# This tool is used to provide visibility into mpconfig
# Currently it parses configured source files and uses the
# pre-processor to parse and present all configured
# MICROPY_* definitions

import ast
import functools
import io
import json
import os
import operator as op
import re
import subprocess
import sys
import multiprocessing, multiprocessing.dummy

from numpy import source


file_header = r'^# \d+ "(.*)"(?: \d+)*$'
pattern = r"(MICROPY_[A-Z_0-9]+(?:\(.*?\))?)"
pattern_detail = r"(#[ifdefinels]+ )?(.*?)(MICROPY_[A-Z_0-9]+(?:\(.*?\))?)(.*)$"
pattern_ignore = r"_*MICROPY_INCLUDED_\S+_H_*"


class Args:
    mpy = ["."]
    debug = []
    pp = []
    output = []
    cflags = []
    cxxflags = []
    sources = []
    pp_output = None


args = Args()


@functools.total_ordering
class ifdef:
    def __init__(self, val):
        self.s = str(val)

    def __str__(self):
        return self.s

    def __lt__(self, other):
        return self.s < str(other)


DEFINED = ifdef(1)

UNDEFINED = ifdef(0)


def find_mpconfig_defines_in_files(files):
    """Find any MICROPY_* definitions in the provided c file.

    :param str c_file: path to c file to check
    :return: List[(module_name, obj_module, enabled_define)]
    """
    global pattern_detail

    details = {}

    def find_mpconfig_defines(fname):
        with io.open(fname, encoding="utf-8") as file_obj:
            content = file_obj.read()
            # unwrap line-continuation
            content = re.sub("\\\\\n\s*", "", content, flags=re.MULTILINE)
            definitions = re.findall(pattern_detail, content, flags=re.MULTILINE)
            return fname, [d for d in definitions if not re.match(pattern_ignore, d[2])]

    try:
        cpus = multiprocessing.cpu_count()
    except NotImplementedError:
        cpus = 1
    p = multiprocessing.dummy.Pool(cpus)

    for name, matches in p.imap(find_mpconfig_defines, files):
        for match in matches:
            line_type = match[0]
            between = match[1]
            conf = match[2]
            value = match[3].strip()

            if conf not in details:
                details[conf] = dict(defined=set(), used=set(), value=UNDEFINED)

            if "#define " == line_type:
                if " " in between:
                    # the detected conf is in the value, not the define name
                    details[conf]["used"].add(name)
                else:
                    if not value:
                        value = DEFINED
                    details[conf]["defined"].add((name, value))
            else:
                details[conf]["used"].add(name)

    return details


def preprocess():
    sources = args.sources
    csources = []
    cxxsources = []
    for source in sources:
        if source.endswith(".cpp"):
            cxxsources.append(source)
        elif source.endswith(".c"):
            csources.append(source)
    try:
        os.makedirs(os.path.dirname(args.output))
    except OSError:
        pass

    def pp(flags):
        def run(files):
            return subprocess.check_output(args.pp + flags + files)

        return run

    try:
        cpus = multiprocessing.cpu_count()
    except NotImplementedError:
        cpus = 1
    p = multiprocessing.dummy.Pool(cpus)
    with open(args.pp_output, "wb") as out_file:
        for flags, sources in (
            (args.cflags, csources),
            (args.cxxflags, cxxsources),
        ):
            batch_size = (len(sources) + cpus - 1) // cpus
            chunks = [sources[i : i + batch_size] for i in range(0, len(sources), batch_size or 1)]
            for output in p.imap(pp(flags), chunks):
                out_file.write(output)


def eval_expr(expr):
    """
    Safely evaluate maths expressions in C define strings.
    """
    expr = expr.replace("||", "|").replace("&&", "&").replace("! ", " not").replace("!(", " not (")
    try:
        return eval_(ast.parse(expr, mode="eval").body)
    except (TypeError, SyntaxError):
        return expr


def eval_(node):
    # supported operators
    operators = {
        ast.Add: op.add,
        ast.Sub: op.sub,
        ast.Mult: op.mul,
        ast.Div: op.truediv,
        ast.Pow: op.pow,
        ast.BitXor: op.xor,
        ast.BitAnd: op.and_,
        ast.USub: op.neg,
        ast.BitOr: op.or_,
        ast.GtE: op.ge,
        ast.Gt: op.gt,
        ast.LtE: op.le,
        ast.Lt: op.lt,
        ast.Eq: op.eq,
        ast.NotEq: op.ne,
        ast.LShift: op.lshift,
        ast.RShift: op.rshift,
        ast.Not: op.not_,
    }

    ret = None
    if isinstance(node, ast.Num):  # <number>
        ret = node.n
    elif isinstance(node, ast.BinOp):  # <left> <operator> <right>
        ret = operators[type(node.op)](eval_(node.left), eval_(node.right))
    elif isinstance(node, ast.UnaryOp):  # <operator> <operand> e.g., -1
        ret = operators[type(node.op)](eval_(node.operand))
    elif isinstance(node, ast.Compare):  # <operator> <operand> e.g., -1
        ret = True
        left = eval_(node.left)
        for opr, right in zip(node.ops, [eval_(c) for c in node.comparators]):
            ret &= operators[type(opr)](left, right)
    else:
        raise TypeError(node)

    return 1 if ret is True else 0 if ret is False else ret


def parse_values(mpconfig_in):
    mpconfig = {}

    for key, value in mpconfig_in.items():
        if isinstance(value, str):
            if key == value:
                mpconfig[key] = UNDEFINED
                continue

            if value == "":
                mpconfig[key] = DEFINED
                continue

            def _fill_mpconfig(match):
                return str(mpconfig_in.get(match.group(0), 0))

            value = re.sub(pattern_detail, _fill_mpconfig, value)

            value = eval_expr(value)

        mpconfig[key] = value
    return mpconfig


def mpypath(p):
    return os.path.relpath(p, args.mpy)


def main():
    args.command = sys.argv[1]

    named_args = {s: [] for s in dir(args) if not s.startswith("_")}

    for arg in sys.argv[1:]:
        if arg in named_args:
            current_tok = arg
        else:
            named_args[current_tok].append(arg)

    if not named_args["pp"] or len(named_args["output"]) != 1:
        print("usage: %s %s ..." % (sys.argv[0], " ... ".join(named_args)))
        sys.exit(2)

    for k, v in named_args.items():
        if v:
            setattr(args, k, v)

    for arg in ("mpy", "output"):
        val = getattr(args, arg)
        if isinstance(val, list):
            setattr(args, arg, val[0])

    if args.debug:
        args.debug = str(args.debug[0]) == "1"

    os.makedirs(os.path.dirname(args.output), exist_ok=True)

    # Add preprocessor flag to keep all #define lines in output
    args.cflags.extend(["-dD", "-Wp,-w"])
    args.cxxflags.extend(["-dD", "-Wp,-w"])

    args.pp_output = args.output + ".i"

    preprocess()

    with open(args.pp_output, "r") as ppi:
        preproc_out = ppi.read()
    if not args.debug:
        os.unlink(args.pp_output)

    # Find all the defines and header files in the pre-processor output
    micropy_all = set(
        (d for d in re.findall(pattern, preproc_out) if not re.match(pattern_ignore, d))
    )

    files_all = set(re.findall(file_header, preproc_out, flags=re.MULTILINE))
    headers_all = set(
        (
            os.path.relpath(f, ".")
            for f in files_all
            if f.endswith(".h") and not f.startswith("/") and os.path.exists(f)
        )
    )

    src_files = set(args.sources)

    # Create a dummy C file with all configs to pre-process the final config values
    def_c_file = args.output + ".c"

    _mark_start = "___MPCONFIG_DEFINES_START___\n"
    _mark_end = "___MPCONFIG_DEFINES_END___\n"

    with open(def_c_file, "w") as cdef:
        cdef.write(f"#include <py/mpconfig.h>\n")
        for h in sorted(headers_all):
            cdef.write(f'#include "{h}"\n')
        cdef.write("#define STR(a) STR_(a)\n")
        cdef.write("#define STR_(a...) #a\n\n")
        cdef.write(_mark_start)
        cdef.write("{\n")
        cdef.write(",\n".join(('"%s": STR(%s)' % (d, d) for d in micropy_all)))
        cdef.write("}\n")
        cdef.write(_mark_end)

    args.sources = [def_c_file]

    args.pp_output = def_c_file + ".i"

    preprocess()

    with open(args.pp_output, "r") as ppi:
        preproc_out = ppi.read()

    if not args.debug:
        os.unlink(def_c_file)
        os.unlink(args.pp_output)

    # Snip out the json we've crafted above
    pp_json = (
        re.search(f"{_mark_start}(.*){_mark_end}", preproc_out, flags=re.DOTALL).group(1).strip()
    )
    mpconfig_json = json.loads(pp_json)

    mpconfig_json_eval = None

    while True:
        # re-run eval on values to process any micropy dependant on other micropy
        mpconfig_json_eval = parse_values(mpconfig_json)
        if mpconfig_json_eval == mpconfig_json:
            break
        mpconfig_json = mpconfig_json_eval

    # scan through all C and H files to gather usage locations and original definitions
    micropy_details = find_mpconfig_defines_in_files(headers_all | src_files)

    outfile = args.output

    with open(outfile, "w") as out_file:
        for key, value in sorted(mpconfig_json.items()):
            if value is UNDEFINED:
                out_file.write(f"# {key} is not defined\n")
            elif value is DEFINED:
                out_file.write(f'{key} = ""  # is defined\n')
            elif isinstance(value, str):
                value = value.strip('"')
                out_file.write(f'{key} = "{value}"\n')
            else:
                out_file.write(f"{key} = {value}\n")

    # re-sort details by the file they're initially defined in.
    micropy_by_loc = {}
    for conf, details in micropy_details.items():
        defined = details["defined"]
        used = details["used"]
        value = mpconfig_json.get(conf, UNDEFINED)
        for location, orig_val in defined:
            if location not in micropy_by_loc:
                micropy_by_loc[location] = dict()
            key = (conf, value)
            if key not in micropy_by_loc[location]:
                micropy_by_loc[location][key] = dict(used=set(), orig_val=set())

            micropy_by_loc[location][key]["used"] |= used
            if isinstance(orig_val, str):
                orig_val = re.sub(r"/\*.*?\*/", "", orig_val)
            micropy_by_loc[location][key]["orig_val"].add(orig_val)

    # Create out.h header file with full details
    outheader = args.output + ".h"

    with open(outheader, "w") as header_file:
        for location, defines in reversed(sorted(micropy_by_loc.items())):
            header_file.write(f"/** Definitions from: {mpypath(location)} */\n")

            for (conf, value), details in sorted(defines.items()):
                uses = details["used"]
                if uses:
                    header_file.write(f"\n/** Used in:\n")
                    header_file.write("\n".join((f" * {mpypath(use)}" for use in sorted(uses))))
                    header_file.write(f"\n */\n")
                else:
                    header_file.write(f"\n/** No uses found */\n")

                orig_vals = [str(v).strip("() ") for v in details["orig_val"]]
                if str(value) not in orig_vals:
                    header_file.write(f"/** Definition here:\n")
                    header_file.write("\n".join((f" * {v}" for v in sorted(details["orig_val"]))))
                    header_file.write(f"\n */\n")

                if value is UNDEFINED:
                    header_file.write(f"#undef {conf}\n")
                elif value is DEFINED:
                    header_file.write(f"#define {conf}\n")
                elif isinstance(value, str):
                    header_file.write(f"#define {conf} {value}\n")
                else:
                    header_file.write(f"#define {conf} {value}\n")

            header_file.write(f"\n")


if __name__ == "__main__":
    main()
