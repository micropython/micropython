#!/usr/bin/python

import os.path
import re
import sys


def mark_hn_errors(source_file_name):
    # TODO: refactor to make testable; logic is getting a bit more involved here
    # Specifically, capture the prints in a usable way
    if not os.path.isfile(source_file_name):
        return 0

    with open(source_file_name) as source_file:
        source = source_file.read()
    source = delete_line_continuations(source)
    source = strip_comment(source)
    lines = source.split("\n")
    err_count = 0
    for line in lines:
        if (
            is_var_decl(line) and
            not prefix_is_correct(line)
        ):
            err_count += 1
            print(source_file_name + ":" + str(find_line(source_file_name, line)))
            truncation_length = 90
            if len(line) > truncation_length:
                line = line[:truncation_length] + "..."
            removed_line = "- " + line
            print(removed_line)
            ident = get_identifier(line)
            fixed_ident = get_base_identifier(ident)
            fixed_ident = list(fixed_ident)
            fixed_ident[0] = fixed_ident[0].upper()
            fixed_ident = ''.join(fixed_ident)
            fixed_ident = get_prefix(line) + fixed_ident
            fixed_line = "+ " + line.replace(ident, fixed_ident)
            print(fixed_line)
            print("")
    return err_count


def find_line(source_file_name, text):
    with open(source_file_name) as source_file:
        line_num = 0
        for line in source_file:
            line_num += 1
            if text in line:
                return line_num
    return 0


def delete_line_continuations(source):
    source = re.sub(r"[\r\n]+", "\n", source)
    source = re.sub(r"\\\n", "", source)
    return source


def strip_comment(source):
    source = re.sub(r"/\*.*?\*\/", " ", source, flags=re.S)
    source = re.sub(r"/\*.*", " ", source)
    source = re.sub(r"//.*", " ", source)
    source = re.sub(r"[^\S\n]+", " ", source)
    source = re.sub(r"[^\S\n]+\n", "\n", source)
    return source


def is_var_decl(line):
    line = line.strip()
    line = get_left_hand_side(line)
    line = strip_pointers(line)
    line = strip_qualifiers(line)
    var_decl_format = re.compile(
        r"""
            ^               # assert beginning of string
            \s*             # Any or no whitespace at the start of the string
            [a-zA-Z_]+      # Must start with letter or underscore
            \w*\b           # Then any word character up to a word boundary
            \s+             # Some amount of white space
            [a-zA-Z_]\w*\b  # Foo
        """,
        re.VERBOSE
    )
    if not var_decl_format.match(line):
        return False
    if "(" in line:
        return False
    if line.startswith("return"):
        return False
    if line.startswith("case"):
        return False
    if line.startswith("typedef"):
        return False
    return True


def get_left_hand_side(line):
    line = line.split("=")[0]
    line = line.strip()
    return line


def strip_pointers(line):
    line = re.sub(r"\*", "", line)
    line = re.sub(r"\[.*?\]", "", line)
    line = re.sub(r"\s+", " ", line)
    return line


def strip_qualifiers(line):
    qualifiers = [
        "const",
        "enum",
        "extern",
        "inline",
        "register",
        "restrict",
        "signed",
        "static",
        "struct",
        "union",
        "unsigned",
        "volatile",
    ]
    for qualifier in qualifiers:
        pattern = r"\b" + qualifier + r"\b"
        line = re.sub(pattern, "", line)
    line = re.sub(r"\s+", " ", line)
    line = line.strip()
    return line


def prefix_is_correct(line):
    if is_globally_linked(line):
        return True
    correct_prefix = get_prefix(line)
    identifier = get_identifier(line)
    current_prefix = get_identifier_prefix(identifier)
    if current_prefix == correct_prefix:
        return True
    correct_base_prefix = re.sub("^p+", "", correct_prefix)
    current_base_prefix = re.sub("^p+", "", current_prefix)
    if correct_base_prefix == 'x' and current_base_prefix == 'e':
        # Need to parse typedefs in order to correctly identify enums.
        # So for now we just assume it's correct
        return True
    return False


def is_globally_linked(line):
    if "extern" in line:
        return True
    if "static" in line:
        return False
    if line.startswith(" ") or line.startswith("\t"):
        # If the line starts with a space and is not part of a function, it will
        # fail the style check.  So, if the line starts with a space it is part
        # of a function.
        return False
    return True


def get_identifier(line):
    line = line.strip()
    line = get_left_hand_side(line)
    line = strip_qualifiers(line)
    line = strip_pointers(line)
    identifier = re.findall(r"\b\w+\b", line)[1]
    return identifier


def get_prefix(line):
    line = line.strip()
    line = get_left_hand_side(line)
    unsigned = "unsigned" in line
    line = strip_qualifiers(line)
    indirection = count_indirection(line)
    line = strip_pointers(line)
    base_type = get_base_type(line)
    base_prefix = get_base_prefix(base_type)
    prefix = "p" * indirection + "u" * unsigned + base_prefix
    return prefix

# PKCS #11 types may have the format
# CK_TYPE_PTR or CK_TYPE_PTR_PTR
def count_pkcs11_indirection(line):
    base_prefix = get_base_type(line)
    if (re.search("_PTR_PTR$", base_prefix)):
        return 2
    elif (re.search("_PTR$", base_prefix)):
        return 1
    else:
        return 0

def count_indirection(line):
    pointer_count = line.count('*')
    pointer_count += count_pkcs11_indirection(line)
    return pointer_count


def get_base_type(line):
    line = re.findall(r"^\w+\b", line)[0]
    return line


TYPE_PREFIXES = {
    "BaseType_t":    "x",
    "char":          "c",
    "enum":          "e",
    "int":           "l",
    "int16_t":       "s",
    "int32_t":       "l",
    "int64_t":       "ll",
    "int8_t":        "c",
    "UBaseType_t":   "ux",
    "uint16_t":      "us",
    "uint32_t":      "ul",
    "uint64_t":      "ull",
    "uint8_t":       "uc",
    "void":          "v",
}


def get_base_prefix(type_name):
    try:
        prefix = TYPE_PREFIXES[type_name]
    except KeyError:
        prefix = "x"
    return prefix


def get_identifier_prefix(identifier):
    direct_identifier = re.sub(r"^p+", "", identifier)
    indirection = "p" * (len(identifier) - len(direct_identifier))
    for prefix in TYPE_PREFIXES.values():
        if re.match(r"^" + prefix + r"[0-9,A-Z]", direct_identifier):
            return indirection + prefix
    return ''


def get_base_identifier(identifier):
    prefix = get_identifier_prefix(identifier)
    base_identifier = re.sub(r"^" + prefix, "", identifier)
    return base_identifier


def main():  # pragma: no cover
    files = sys.argv[1:]
    err_count = 0
    for file in files:
        try:
            err_count += mark_hn_errors(file)
        except Exception as e:
            raise Exception(file, e)
    sys.exit(err_count)


if __name__ == "__main__":  # pragma: no cover
    main()
