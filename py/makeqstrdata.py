"""
Process raw qstr file and output qstr data with length, hash and data bytes.

This script works with Python 2.7, 3.3 and 3.4.

CIRCUITPY-CHANGE:
For documentation about the format of compressed translated strings, see
supervisor/shared/translate/translate.h
"""

from __future__ import print_function

import bisect
import re
import sys

import collections
import gettext
import os.path

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(errors="backslashreplace")

py = os.path.dirname(sys.argv[0])
top = os.path.dirname(py)

# Python 2/3 compatibility:
#   - iterating through bytes is different
#   - codepoint2name lives in a different module
import platform

if platform.python_version_tuple()[0] == "2":
    bytes_cons = lambda val, enc=None: bytearray(val)
    from htmlentitydefs import codepoint2name
elif platform.python_version_tuple()[0] == "3":
    bytes_cons = bytes
    from html.entities import codepoint2name
# end compatibility code

codepoint2name[ord("-")] = "hyphen"

# add some custom names to map characters that aren't in HTML
codepoint2name[ord(" ")] = "space"
codepoint2name[ord("'")] = "squot"
codepoint2name[ord(",")] = "comma"
codepoint2name[ord(".")] = "dot"
codepoint2name[ord(":")] = "colon"
codepoint2name[ord(";")] = "semicolon"
codepoint2name[ord("/")] = "slash"
codepoint2name[ord("%")] = "percent"
codepoint2name[ord("#")] = "hash"
codepoint2name[ord("(")] = "paren_open"
codepoint2name[ord(")")] = "paren_close"
codepoint2name[ord("[")] = "bracket_open"
codepoint2name[ord("]")] = "bracket_close"
codepoint2name[ord("{")] = "brace_open"
codepoint2name[ord("}")] = "brace_close"
codepoint2name[ord("*")] = "star"
codepoint2name[ord("!")] = "bang"
codepoint2name[ord("\\")] = "backslash"
codepoint2name[ord("+")] = "plus"
codepoint2name[ord("$")] = "dollar"
codepoint2name[ord("=")] = "equals"
codepoint2name[ord("?")] = "question"
codepoint2name[ord("@")] = "at_sign"
codepoint2name[ord("^")] = "caret"
codepoint2name[ord("|")] = "pipe"
codepoint2name[ord("~")] = "tilde"

C_ESCAPES = {
    "\a": "\\a",
    "\b": "\\b",
    "\f": "\\f",
    "\n": "\\n",
    "\r": "\\r",
    "\t": "\\t",
    "\v": "\\v",
    "'": "\\'",
    '"': '\\"',
}

# static qstrs, should be sorted
# These are qstrs that are always included and always have the same number. It allows mpy files to omit them.
static_qstr_list = [
    "",
    "__dir__",  # Put __dir__ after empty qstr for builtin dir() to work
    "\n",
    " ",
    "*",
    "/",
    "<module>",
    "_",
    "__call__",
    "__class__",
    "__delitem__",
    "__enter__",
    "__exit__",
    "__getattr__",
    "__getitem__",
    "__hash__",
    "__init__",
    "__int__",
    "__iter__",
    "__len__",
    "__main__",
    "__module__",
    "__name__",
    "__new__",
    "__next__",
    "__qualname__",
    "__repr__",
    "__setitem__",
    "__str__",
    "ArithmeticError",
    "AssertionError",
    "AttributeError",
    "BaseException",
    "EOFError",
    "Ellipsis",
    "Exception",
    "GeneratorExit",
    "ImportError",
    "IndentationError",
    "IndexError",
    "KeyError",
    "KeyboardInterrupt",
    "LookupError",
    "MemoryError",
    "NameError",
    "NoneType",
    "NotImplementedError",
    "OSError",
    "OverflowError",
    "RuntimeError",
    "StopIteration",
    "SyntaxError",
    "SystemExit",
    "TypeError",
    "ValueError",
    "ZeroDivisionError",
    "abs",
    "all",
    "any",
    "append",
    "args",
    "bool",
    "builtins",
    "bytearray",
    "bytecode",
    "bytes",
    "callable",
    "chr",
    "classmethod",
    "clear",
    "close",
    "const",
    "copy",
    "count",
    "dict",
    "dir",
    "divmod",
    "end",
    "endswith",
    "eval",
    "exec",
    "extend",
    "find",
    "format",
    "from_bytes",
    "get",
    "getattr",
    "globals",
    "hasattr",
    "hash",
    "id",
    "index",
    "insert",
    "int",
    "isalpha",
    "isdigit",
    "isinstance",
    "islower",
    "isspace",
    "issubclass",
    "isupper",
    "items",
    "iter",
    "join",
    "key",
    "keys",
    "len",
    "list",
    "little",
    "locals",
    "lower",
    "lstrip",
    "main",
    "map",
    "micropython",
    "next",
    "object",
    "open",
    "ord",
    "pop",
    "popitem",
    "pow",
    "print",
    "range",
    "read",
    "readinto",
    "readline",
    "remove",
    "replace",
    "repr",
    "reverse",
    "rfind",
    "rindex",
    "round",
    "rsplit",
    "rstrip",
    "self",
    "send",
    "sep",
    "set",
    "setattr",
    "setdefault",
    "sort",
    "sorted",
    "split",
    "start",
    "startswith",
    "staticmethod",
    "step",
    "stop",
    "str",
    "strip",
    "sum",
    "super",
    "throw",
    "to_bytes",
    "tuple",
    "type",
    "update",
    "upper",
    "utf-8",
    "value",
    "values",
    "write",
    "zip",
]

# These qstrs have to be sorted early (preferably right after static_qstr_list) because they are required to fit in 8-bit values
# however they should never be *forced* to appear
# repeats len, hash, int from the static qstr list, but this doesn't hurt anything.
eightbit_qstr_list = [
    "__abs__",
    "__add__",
    "__and__",
    "__bool__",
    "__complex__",
    "__contains__",
    "__delete__",
    "__divmod__",
    "__eq__",
    "__float__",
    "__floordiv__",
    "__ge__",
    "__get__",
    "__gt__",
    "__hash__",
    "__iadd__",
    "__iand__",
    "__ifloordiv__",
    "__ilshift__",
    "__imatmul__",
    "__imod__",
    "__imul__",
    "__int__",
    "__invert__",
    "__ior__",
    "__ipow__",
    "__irshift__",
    "__isub__",
    "__itruediv__",
    "__ixor__",
    "__le__",
    "__len__",
    "__lshift__",
    "__lt__",
    "__matmul__",
    "__mod__",
    "__mul__",
    "__ne__",
    "__neg__",
    "__or__",
    "__pos__",
    "__pow__",
    "__radd__",
    "__rand__",
    "__rfloordiv__",
    "__rlshift__",
    "__rmatmul__",
    "__rmod__",
    "__rmul__",
    "__ror__",
    "__rpow__",
    "__rrshift__",
    "__rshift__",
    "__rsub__",
    "__rtruediv__",
    "__rxor__",
    "__set__",
    "__sizeof__",
    "__sub__",
    "__truediv__",
    "__xor__",
]


# this must match the equivalent function in qstr.c
def compute_hash(qstr, bytes_hash):
    hash = 5381
    for b in qstr:
        hash = (hash * 33) ^ b
    # Make sure that valid hash is never zero, zero means "hash not computed"
    return (hash & ((1 << (8 * bytes_hash)) - 1)) or 1


def qstr_escape(qst):
    def esc_char(m):
        c = ord(m.group(0))
        try:
            name = codepoint2name[c]
        except KeyError:
            name = "0x%02x" % c
        return "_" + name + "_"

    return re.sub(r"[^A-Za-z0-9_]", esc_char, qst)


# CIRCUITPY-CHANGE: add translations handling
def parse_input_headers_with_translations(infiles):
    qcfgs = {}
    qstrs = {}
    translations = set()

    # add static qstrs
    for qstr in static_qstr_list:
        # work out the corresponding qstr name
        ident = qstr_escape(qstr)

        # don't add duplicates
        assert ident not in qstrs

        # add the qstr to the list, with order number to retain original order in file
        order = len(qstrs) - 300000
        qstrs[ident] = (order, ident, qstr)

    # read the qstrs in from the input files
    for infile in infiles:
        with open(infile, "rt") as f:
            for line in f:
                line = line.strip()

                # is this a config line?
                match = re.match(r"^QCFG\((.+), (.+)\)", line)
                if match:
                    value = match.group(2)
                    if value[0] == "(" and value[-1] == ")":
                        # strip parenthesis from config value
                        value = value[1:-1]
                    qcfgs[match.group(1)] = value
                    continue

                # CIRCUITPY-CHANGE
                match = re.match(r'^TRANSLATE\("(.*)"\)$', line)
                if match:
                    translations.add(match.group(1))
                    continue

                # is this a QSTR line?
                match = re.match(r"^Q\((.*)\)$", line)
                if not match:
                    continue

                # get the qstr value
                qstr = match.group(1)

                # special cases to specify control characters
                if qstr == "\\n":
                    qstr = "\n"
                elif qstr == "\\r\\n":
                    qstr = "\r\n"

                # work out the corresponding qstr name
                ident = qstr_escape(qstr)

                # don't add duplicates
                if ident in qstrs:
                    continue

                # add the qstr to the list, with order number to retain original order in file
                order = len(qstrs)
                # but put special method names like __add__ at the top of list, so
                # that their id's fit into a byte
                if ident in eightbit_qstr_list:
                    order -= 100000
                qstrs[ident] = (order, ident, qstr)

    if not qcfgs and qstrs:
        sys.stderr.write("ERROR: Empty preprocessor output - check for errors above\n")
        sys.exit(1)

    return qcfgs, qstrs, translations


# CIRCUITPY-CHANGE: Used externally by mpy-tool.py. Don't pass back translations.
def parse_input_headers(infiles):
    qcfgs, qstrs, translations = parse_input_headers_with_translations(infiles)
    return (qcfgs, qstrs)


def escape_bytes(qstr, qbytes):
    if all(32 <= ord(c) <= 126 and c != "\\" and c != '"' for c in qstr):
        # qstr is all printable ASCII so render it as-is (for easier debugging)
        return qstr
    else:
        # qstr contains non-printable codes so render entire thing as hex pairs
        return "".join(("\\x%02x" % b) for b in qbytes)


def make_bytes(cfg_bytes_len, cfg_bytes_hash, qstr):
    qbytes = bytes_cons(qstr, "utf8")
    qlen = len(qbytes)
    qhash = compute_hash(qbytes, cfg_bytes_hash)
    if qlen >= (1 << (8 * cfg_bytes_len)):
        print("qstr is too long:", qstr)
        assert False
    qdata = escape_bytes(qstr, qbytes)
    return '%d, %d, "%s"' % (qhash, qlen, qdata)


# CIRCUITPY-CHANGE: add translations
def print_qstr_data(qcfgs, qstrs, translations):
    # get config variables
    cfg_bytes_len = int(qcfgs["BYTES_IN_LEN"])
    cfg_bytes_hash = int(qcfgs["BYTES_IN_HASH"])

    # print out the starter of the generated C header file
    print("// This file was automatically generated by makeqstrdata.py")
    print("")

    # add NULL qstr with no hash or data
    print('QDEF(MP_QSTRnull, 0, 0, "")')

    total_qstr_size = 0
    # go through each qstr and print it out
    for order, ident, qstr in sorted(qstrs.values(), key=lambda x: x[0]):
        qbytes = make_bytes(cfg_bytes_len, cfg_bytes_hash, qstr)
        print("QDEF(MP_QSTR_%s, %s)" % (ident, qbytes))

        total_qstr_size += len(qstr)

    print(
        "// Enumerate translated texts but don't actually include translations. Instead, the linker will link them in."
    )
    for i, original in enumerate(sorted(translations)):
        print('TRANSLATION("{}", {})'.format(original, i))

    print()
    print("// {} bytes worth of qstr".format(total_qstr_size))


def do_work(infiles):
    qcfgs, qstrs, translations = parse_input_headers_with_translations(infiles)
    print_qstr_data(qcfgs, qstrs, translations)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="Process QSTR definitions into headers for compilation"
    )
    parser.add_argument(
        "infiles", metavar="N", type=str, nargs="+", help="an integer for the accumulator"
    )

    args = parser.parse_args()

    do_work(args.infiles)
