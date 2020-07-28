# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# Run with 'python tools/extract_pyi.py shared-bindings/ path/to/stub/dir
# You can also test a specific library in shared-bindings by putting the path
# to that directory instead

import ast
import os
import re
import sys
import traceback

import isort
import black


IMPORTS_IGNORE = frozenset({'int', 'float', 'bool', 'str', 'bytes', 'tuple', 'list', 'set', 'dict', 'bytearray', 'slice', 'file', 'buffer', 'range', 'array', 'struct_time'})
IMPORTS_TYPING = frozenset({'Any', 'Optional', 'Union', 'Tuple', 'List', 'Sequence', 'Iterable', 'Iterator', 'overload'})
IMPORTS_TYPESHED = frozenset({'ReadableBuffer', 'WriteableBuffer'})


def is_any(node):
    node_type = type(node)
    if node is None:
        return True
    if node_type == ast.Name and node.id == "Any":
        return True
    if (node_type == ast.Attribute and type(node.value) == ast.Name
            and node.value.id == "typing" and node.attr == "Any"):
        return True
    return False


def report_missing_annotations(tree):
    for node in ast.walk(tree):
        node_type = type(node)
        if node_type == ast.AnnAssign:
            if is_any(node.annotation):
                print(f"Missing attribute type on line {node.lineno}")
        elif node_type == ast.arg:
            if is_any(node.annotation) and node.arg != "self":
                print(f"Missing argument type: {node.arg} on line {node.lineno}")
        elif node_type == ast.FunctionDef:
            if is_any(node.returns) and node.name != "__init__":
                print(f"Missing return type: {node.name} on line {node.lineno}")


def extract_imports(tree):
    modules = set()
    typing = set()
    typeshed = set()

    def collect_annotations(anno_tree):
        if anno_tree is None:
            return
        for node in ast.walk(anno_tree):
            node_type = type(node)
            if node_type == ast.Name:
                if node.id in IMPORTS_IGNORE:
                    continue
                elif node.id in IMPORTS_TYPING:
                    typing.add(node.id)
                elif node.id in IMPORTS_TYPESHED:
                    typeshed.add(node.id)
            if node_type == ast.Attribute:
                if type(node.value) == ast.Name:
                    modules.add(node.value.id)

    for node in ast.walk(tree):
        node_type = type(node)
        if (node_type == ast.AnnAssign) or (node_type == ast.arg):
            collect_annotations(node.annotation)
        elif node_type == ast.FunctionDef:
            collect_annotations(node.returns)
            for deco in node.decorator_list:
                if deco.id in IMPORTS_TYPING:
                    typing.add(deco.id)

    return {
        "modules": sorted(modules),
        "typing": sorted(typing),
        "typeshed": sorted(typeshed),
    }


def convert_folder(top_level, stub_directory):
    ok = 0
    total = 0
    filenames = sorted(os.listdir(top_level))
    pyi_lines = []

    for filename in filenames:
        full_path = os.path.join(top_level, filename)
        file_lines = []
        if os.path.isdir(full_path):
            (mok, mtotal) = convert_folder(full_path, os.path.join(stub_directory, filename))
            ok += mok
            total += mtotal
        elif filename.endswith(".c"):
            with open(full_path, "r") as f:
                for line in f:
                    if line.startswith("//|"):
                        if line[3] == " ":
                            line = line[4:]
                        elif line[3] == "\n":
                            line = line[3:]
                        else:
                            continue
                        file_lines.append(line)
        elif filename.endswith(".pyi"):
            with open(full_path, "r") as f:
                file_lines.extend(f.readlines())

        # Always put the contents from an __init__ first.
        if filename.startswith("__init__."):
            pyi_lines = file_lines + pyi_lines
        else:
            pyi_lines.extend(file_lines)

    if not pyi_lines:
        return (ok, total)

    stub_filename = os.path.join(stub_directory, "__init__.pyi")
    print(stub_filename)
    stub_contents = "".join(pyi_lines)

    # Validate that the module is a parseable stub.
    total += 1
    try:
        tree = ast.parse(stub_contents)
        imports = extract_imports(tree)
        report_missing_annotations(tree)
        ok += 1
    except SyntaxError as e:
        traceback.print_exception(type(e), e, e.__traceback__)
        return (ok, total)

    # Add import statements
    import_lines = ["from __future__ import annotations"]
    if imports["typing"]:
        import_lines.append("from typing import " + ", ".join(imports["typing"]))
    if imports["typeshed"]:
        import_lines.append("from _typeshed import " + ", ".join(imports["typeshed"]))
    import_lines.extend(f"import {m}" for m in imports["modules"])
    import_body = "\n".join(import_lines)
    m = re.match(r'(\s*""".*?""")', stub_contents, flags=re.DOTALL)
    if m:
        stub_contents = m.group(1) + "\n\n" + import_body + "\n\n" + stub_contents[m.end():]
    else:
        stub_contents = import_body + "\n\n" + stub_contents

    # Code formatting
    stub_contents = isort.code(stub_contents)
    stub_contents = black.format_str(stub_contents, mode=black.FileMode(is_pyi=True))

    os.makedirs(stub_directory, exist_ok=True)
    with open(stub_filename, "w") as f:
        f.write(stub_contents)

    print()
    return (ok, total)


if __name__ == "__main__":
    top_level = sys.argv[1].strip("/")
    stub_directory = sys.argv[2]

    (ok, total) = convert_folder(top_level, stub_directory)

    print(f"Parsing .pyi files: {total - ok} failed, {ok} passed")

    if ok != total:
        sys.exit(total - ok)
