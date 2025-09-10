"""
Process raw qstr file and output qstr data with length, hash and data bytes.

This script works with Python 3.3+.
"""

import re
import sys

bytes_cons = bytes

# fmt: off
codepoint2name = {
    198: "AElig", 193: "Aacute", 194: "Acirc", 192: "Agrave", 913: "Alpha", 197: "Aring", 195: "Atilde",
    196: "Auml", 914: "Beta", 199: "Ccedil", 935: "Chi", 8225: "Dagger", 916: "Delta", 208: "ETH",
    201: "Eacute", 202: "Ecirc", 200: "Egrave", 917: "Epsilon", 919: "Eta", 203: "Euml", 915: "Gamma",
    205: "Iacute", 206: "Icirc", 204: "Igrave", 921: "Iota", 207: "Iuml", 922: "Kappa", 923: "Lambda",
    924: "Mu", 209: "Ntilde", 925: "Nu", 338: "OElig", 211: "Oacute", 212: "Ocirc", 210: "Ograve",
    937: "Omega", 927: "Omicron", 216: "Oslash", 213: "Otilde", 214: "Ouml", 934: "Phi", 928: "Pi",
    8243: "Prime", 936: "Psi", 929: "Rho", 352: "Scaron", 931: "Sigma", 222: "THORN", 932: "Tau",
    920: "Theta", 218: "Uacute", 219: "Ucirc", 217: "Ugrave", 933: "Upsilon", 220: "Uuml", 926: "Xi",
    221: "Yacute", 376: "Yuml", 918: "Zeta", 225: "aacute", 226: "acirc", 180: "acute", 230: "aelig",
    224: "agrave", 8501: "alefsym", 945: "alpha", 38: "amp", 8743: "and", 8736: "ang", 229: "aring",
    8776: "asymp", 227: "atilde", 228: "auml", 8222: "bdquo", 946: "beta", 166: "brvbar", 8226: "bull",
    8745: "cap", 231: "ccedil", 184: "cedil", 162: "cent", 967: "chi", 710: "circ", 9827: "clubs",
    8773: "cong", 169: "copy", 8629: "crarr", 8746: "cup", 164: "curren", 8659: "dArr", 8224: "dagger",
    8595: "darr", 176: "deg", 948: "delta", 9830: "diams", 247: "divide", 233: "eacute", 234: "ecirc",
    232: "egrave", 8709: "empty", 8195: "emsp", 8194: "ensp", 949: "epsilon", 8801: "equiv", 951: "eta",
    240: "eth", 235: "euml", 8364: "euro", 8707: "exist", 402: "fnof", 8704: "forall", 189: "frac12",
    188: "frac14", 190: "frac34", 8260: "frasl", 947: "gamma", 8805: "ge", 62: "gt", 8660: "hArr",
    8596: "harr", 9829: "hearts", 8230: "hellip", 237: "iacute", 238: "icirc", 161: "iexcl", 236: "igrave",
    8465: "image", 8734: "infin", 8747: "int", 953: "iota", 191: "iquest", 8712: "isin", 239: "iuml",
    954: "kappa", 8656: "lArr", 955: "lambda", 9001: "lang", 171: "laquo", 8592: "larr", 8968: "lceil",
    8220: "ldquo", 8804: "le", 8970: "lfloor", 8727: "lowast", 9674: "loz", 8206: "lrm", 8249: "lsaquo",
    8216: "lsquo", 60: "lt", 175: "macr", 8212: "mdash", 181: "micro", 183: "middot", 8722: "minus",
    956: "mu", 8711: "nabla", 160: "nbsp", 8211: "ndash", 8800: "ne", 8715: "ni", 172: "not", 8713: "notin",
    8836: "nsub", 241: "ntilde", 957: "nu", 243: "oacute", 244: "ocirc", 339: "oelig", 242: "ograve",
    8254: "oline", 969: "omega", 959: "omicron", 8853: "oplus", 8744: "or", 170: "ordf", 186: "ordm",
    248: "oslash", 245: "otilde", 8855: "otimes", 246: "ouml", 182: "para", 8706: "part", 8240: "permil",
    8869: "perp", 966: "phi", 960: "pi", 982: "piv", 177: "plusmn", 163: "pound", 8242: "prime",
    8719: "prod", 8733: "prop", 968: "psi", 34: "quot", 8658: "rArr", 8730: "radic", 9002: "rang",
    187: "raquo", 8594: "rarr", 8969: "rceil", 8221: "rdquo", 8476: "real", 174: "reg", 8971: "rfloor",
    961: "rho", 8207: "rlm", 8250: "rsaquo", 8217: "rsquo", 8218: "sbquo", 353: "scaron", 8901: "sdot",
    167: "sect", 173: "shy", 963: "sigma", 962: "sigmaf", 8764: "sim", 9824: "spades", 8834: "sub",
    8838: "sube", 8721: "sum", 8835: "sup", 185: "sup1", 178: "sup2", 179: "sup3", 8839: "supe",
    223: "szlig", 964: "tau", 8756: "there4", 952: "theta", 977: "thetasym", 8201: "thinsp", 254: "thorn",
    732: "tilde", 215: "times", 8482: "trade", 8657: "uArr", 250: "uacute", 8593: "uarr", 251: "ucirc",
    249: "ugrave", 168: "uml", 978: "upsih", 965: "upsilon", 252: "uuml", 8472: "weierp", 958: "xi",
    253: "yacute", 165: "yen", 255: "yuml", 950: "zeta", 8205: "zwj", 8204: "zwnj"
}
# fmt: on

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

# static qstrs, these must maintain a specific order for .mpy compatibility
# See QSTR_LAST_STATIC at the top of py/persistentcode.c

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

# Additional QSTRs that must have index <255 because they are stored as `byte` values.
# These are not part of the .mpy compatibility list, but we place them in the
# fixed unsorted pool (i.e. QDEF0) to ensure their indices are small.
unsorted_qstr_list = {
    # From py/objtype.c: used in the `mp_binary_op_method_name` and `mp_unary_op_method_name` tables.
    "__bool__",
    "__pos__",
    "__neg__",
    "__invert__",
    "__abs__",
    "__float__",
    "__complex__",
    "__sizeof__",
    "__lt__",
    "__gt__",
    "__eq__",
    "__le__",
    "__ge__",
    "__ne__",
    "__contains__",
    "__iadd__",
    "__isub__",
    "__imul__",
    "__imatmul__",
    "__ifloordiv__",
    "__itruediv__",
    "__imod__",
    "__ipow__",
    "__ior__",
    "__ixor__",
    "__iand__",
    "__ilshift__",
    "__irshift__",
    "__add__",
    "__sub__",
    "__mul__",
    "__matmul__",
    "__floordiv__",
    "__truediv__",
    "__mod__",
    "__divmod__",
    "__pow__",
    "__or__",
    "__xor__",
    "__and__",
    "__lshift__",
    "__rshift__",
    "__radd__",
    "__rsub__",
    "__rmul__",
    "__rmatmul__",
    "__rfloordiv__",
    "__rtruediv__",
    "__rmod__",
    "__rpow__",
    "__ror__",
    "__rxor__",
    "__rand__",
    "__rlshift__",
    "__rrshift__",
    "__get__",
    "__set__",
    "__delete__",
    # From py/scope.c: used in `scope_simple_name_table` table.
    # Note: "<module>" is already in `static_qstr_list`.
    "<lambda>",
    "<listcomp>",
    "<dictcomp>",
    "<setcomp>",
    "<genexpr>",
}

# Matches any string that needs no escaping (alphanum + _ only)
RE_NO_ESCAPE = re.compile(r"^[a-zA-Z0-9_]$")


# this must match the equivalent function in qstr.c
def compute_hash(qstr, bytes_hash):
    hash = 5381
    for b in qstr:
        hash = (hash * 33) ^ b
    # Make sure that valid hash is never zero, zero means "hash not computed"
    # if bytes_hash is zero, assume a 16-bit mask (to match qstr.c)
    return (hash & ((1 << (8 * (bytes_hash or 2))) - 1)) or 1


def qstr_escape(qst):
    def esc_char(c):
        if RE_NO_ESCAPE.match(c):
            return c
        c = ord(c)
        try:
            name = codepoint2name[c]
        except KeyError:
            name = "0x%02x" % c
        return "_" + name + "_"

    return "".join(map(esc_char, qst))


static_qstr_list_ident = list(map(qstr_escape, static_qstr_list))


def parse_input_headers(infiles):
    qcfgs = {}
    qstrs = {}

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
                if ident in static_qstr_list_ident:
                    continue
                if ident in qstrs:
                    continue

                qstrs[ident] = (ident, qstr)

    if not qcfgs:
        sys.stderr.write("ERROR: Empty preprocessor output - check for errors above\n")
        sys.exit(1)

    return qcfgs, qstrs


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


def print_qstr_data(qcfgs, qstrs):
    # get config variables
    cfg_bytes_len = int(qcfgs["BYTES_IN_LEN"])
    cfg_bytes_hash = int(qcfgs["BYTES_IN_HASH"])

    # print out the starter of the generated C header file
    print("// This file was automatically generated by makeqstrdata.py")
    print("")

    # add NULL qstr with no hash or data
    print('QDEF0(MP_QSTRnull, 0, 0, "")')

    # add static qstrs to the first unsorted pool
    for qstr in static_qstr_list:
        qbytes = make_bytes(cfg_bytes_len, cfg_bytes_hash, qstr)
        print("QDEF0(MP_QSTR_%s, %s)" % (qstr_escape(qstr), qbytes))

    # add remaining qstrs to the sorted (by value) pool (unless they're in
    # unsorted_qstr_list, in which case add them to the unsorted pool)
    for ident, qstr in sorted(qstrs.values(), key=lambda x: x[1]):
        qbytes = make_bytes(cfg_bytes_len, cfg_bytes_hash, qstr)
        pool = 0 if qstr in unsorted_qstr_list else 1
        print("QDEF%d(MP_QSTR_%s, %s)" % (pool, ident, qbytes))


def do_work(infiles):
    qcfgs, qstrs = parse_input_headers(infiles)
    print_qstr_data(qcfgs, qstrs)


if __name__ == "__main__":
    do_work(sys.argv[1:])
