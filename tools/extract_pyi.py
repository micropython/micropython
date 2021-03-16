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


IMPORTS_IGNORE = frozenset(
    {
        "int",
        "float",
        "bool",
        "str",
        "bytes",
        "tuple",
        "list",
        "set",
        "dict",
        "bytearray",
        "slice",
        "file",
        "buffer",
        "range",
        "array",
        "struct_time",
    }
)
IMPORTS_TYPING = frozenset(
    {
        "Any",
        "Optional",
        "Union",
        "Tuple",
        "List",
        "Sequence",
        "NamedTuple",
        "Iterable",
        "Iterator",
        "Callable",
        "AnyStr",
        "overload",
        "Type",
    }
)
IMPORTS_TYPES = frozenset({"TracebackType"})
CPY_TYPING = frozenset(
    {"ReadableBuffer", "WriteableBuffer", "AudioSample", "FrameBuffer", "Alarm"}
)


def is_typed(node, allow_any=False):
    if node is None:
        return False
    if allow_any:
        return True
    elif isinstance(node, ast.Name) and node.id == "Any":
        return False
    elif (
        isinstance(node, ast.Attribute)
        and type(node.value) == ast.Name
        and node.value.id == "typing"
        and node.attr == "Any"
    ):
        return False
    return True


def find_stub_issues(tree):
    for node in ast.walk(tree):
        if isinstance(node, ast.AnnAssign):
            if not is_typed(node.annotation):
                yield ("WARN", f"Missing attribute type on line {node.lineno}")
            if isinstance(node.value, ast.Constant) and node.value.value == Ellipsis:
                yield ("WARN", f"Unnecessary Ellipsis assignment (= ...) on line {node.lineno}.")
        elif isinstance(node, ast.Assign):
            if isinstance(node.value, ast.Constant) and node.value.value == Ellipsis:
                yield ("WARN", f"Unnecessary Ellipsis assignment (= ...) on line {node.lineno}.")
        elif isinstance(node, ast.arguments):
            allargs = list(node.args + node.kwonlyargs)
            if sys.version_info >= (3, 8):
                allargs.extend(node.posonlyargs)
            for arg_node in allargs:
                if not is_typed(arg_node.annotation) and (
                    arg_node.arg != "self" and arg_node.arg != "cls"
                ):
                    yield (
                        "WARN",
                        f"Missing argument type: {arg_node.arg} on line {arg_node.lineno}",
                    )
            if node.vararg and not is_typed(node.vararg.annotation, allow_any=True):
                yield (
                    "WARN",
                    f"Missing argument type: *{node.vararg.arg} on line {node.vararg.lineno}",
                )
            if node.kwarg and not is_typed(node.kwarg.annotation, allow_any=True):
                yield (
                    "WARN",
                    f"Missing argument type: **{node.kwarg.arg} on line {node.kwarg.lineno}",
                )
        elif isinstance(node, ast.FunctionDef):
            if not is_typed(node.returns):
                yield ("WARN", f"Missing return type: {node.name} on line {node.lineno}")


def extract_imports(tree):
    modules = set()
    typing = set()
    types = set()
    cpy_typing = set()

    def collect_annotations(anno_tree):
        if anno_tree is None:
            return
        for node in ast.walk(anno_tree):
            if isinstance(node, ast.Name):
                if node.id in IMPORTS_IGNORE:
                    continue
                elif node.id in IMPORTS_TYPING:
                    typing.add(node.id)
                elif node.id in IMPORTS_TYPES:
                    types.add(node.id)
                elif node.id in CPY_TYPING:
                    cpy_typing.add(node.id)
            elif isinstance(node, ast.Attribute):
                if isinstance(node.value, ast.Name):
                    modules.add(node.value.id)

    for node in ast.walk(tree):
        if isinstance(node, (ast.AnnAssign, ast.arg)):
            collect_annotations(node.annotation)
        elif isinstance(node, ast.Assign):
            collect_annotations(node.value)
        elif isinstance(node, ast.FunctionDef):
            collect_annotations(node.returns)
            for deco in node.decorator_list:
                if isinstance(deco, ast.Name) and (deco.id in IMPORTS_TYPING):
                    typing.add(deco.id)

    return {
        "modules": sorted(modules),
        "typing": sorted(typing),
        "types": sorted(types),
        "cpy_typing": sorted(cpy_typing),
    }


def find_references(tree):
    for node in ast.walk(tree):
        if isinstance(node, ast.arguments):
            for node in ast.walk(node):
                if isinstance(node, ast.Attribute):
                    if isinstance(node.value, ast.Name) and node.value.id[0].isupper():
                        yield node.value.id


def convert_folder(top_level, stub_directory):
    ok = 0
    total = 0
    filenames = sorted(os.listdir(top_level))
    stub_fragments = []
    references = set()

    for filename in filenames:
        full_path = os.path.join(top_level, filename)
        file_lines = []
        if os.path.isdir(full_path):
            (mok, mtotal) = convert_folder(full_path, os.path.join(stub_directory, filename))
            ok += mok
            total += mtotal
        elif filename.endswith(".c"):
            with open(full_path, "r", encoding="utf-8") as f:
                for line in f:
                    line = line.rstrip()
                    if line.startswith("//|"):
                        if len(line) == 3:
                            line = ""
                        elif line[3] == " ":
                            line = line[4:]
                        else:
                            line = line[3:]
                            print("[WARN] There must be at least one space after '//|'")
                        file_lines.append(line)
        elif filename.endswith(".pyi"):
            with open(full_path, "r") as f:
                file_lines.extend(line.rstrip() for line in f)

        fragment = "\n".join(file_lines).strip()
        try:
            tree = ast.parse(fragment)
        except SyntaxError as e:
            print(f"[ERROR] Failed to parse a Python stub from {full_path}")
            traceback.print_exception(type(e), e, e.__traceback__)
            return (ok, total + 1)
        references.update(find_references(tree))

        if fragment:
            name = os.path.splitext(os.path.basename(filename))[0]
            if name == "__init__" or (name in references):
                stub_fragments.insert(0, fragment)
            else:
                stub_fragments.append(fragment)

    if not stub_fragments:
        return (ok, total)

    stub_filename = os.path.join(stub_directory, "__init__.pyi")
    print(stub_filename)
    stub_contents = "\n\n".join(stub_fragments)

    # Validate the stub code.
    try:
        tree = ast.parse(stub_contents)
    except SyntaxError as e:
        traceback.print_exception(type(e), e, e.__traceback__)
        return (ok, total)

    error = False
    for (level, msg) in find_stub_issues(tree):
        if level == "ERROR":
            error = True
        print(f"[{level}] {msg}")

    total += 1
    if not error:
        ok += 1

    # Add import statements
    imports = extract_imports(tree)
    import_lines = ["from __future__ import annotations"]
    if imports["types"]:
        import_lines.append("from types import " + ", ".join(imports["types"]))
    if imports["typing"]:
        import_lines.append("from typing import " + ", ".join(imports["typing"]))
    if imports["cpy_typing"]:
        import_lines.append("from _typing import " + ", ".join(imports["cpy_typing"]))
    import_lines.extend(f"import {m}" for m in imports["modules"])
    import_body = "\n".join(import_lines)
    m = re.match(r'(\s*""".*?""")', stub_contents, flags=re.DOTALL)
    if m:
        stub_contents = m.group(1) + "\n\n" + import_body + "\n\n" + stub_contents[m.end() :]
    else:
        stub_contents = import_body + "\n\n" + stub_contents

    # Code formatting
    stub_contents = isort.code(stub_contents)
    stub_contents = black.format_str(stub_contents, mode=black.FileMode(is_pyi=True))

    os.makedirs(stub_directory, exist_ok=True)
    with open(stub_filename, "w") as f:
        f.write(stub_contents)

    return (ok, total)


if __name__ == "__main__":
    top_level = sys.argv[1].strip("/")
    stub_directory = sys.argv[2]

    (ok, total) = convert_folder(top_level, stub_directory)

    print(f"Parsing .pyi files: {total - ok} failed, {ok} passed")

    if ok != total:
        sys.exit(total - ok)
