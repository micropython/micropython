#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Damien P. George
# Copyright (c) 2020 Jim Mussared
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import argparse
import glob
import fnmatch
import itertools
import os
import pathlib
import re
import sys
import subprocess

# Relative to top-level repo dir.
PATHS = [
    # C
    "devices/**/*.[ch]",
    "drivers/bus/*.[ch]",
    "extmod/*.[ch]",
    "lib/netutils/*.[ch]",
    "lib/timeutils/*.[ch]",
    "lib/utils/*.[ch]",
    "mpy-cross/**/*.[ch]",
    "ports/**/*.[ch]",
    "py/**/*.[ch]",
    "shared-bindings/**/*.[ch]",
    "shared-module/**/*.[ch]",
    "supervisor/**/*.[ch]",
    # Python
    "extmod/*.py",
    "ports/**/*.py",
    "py/**/*.py",
    "tools/**/*.py",
    "tests/**/*.py",
]

EXCLUSIONS = [
    # STM32 build includes generated Python code.
    "ports/*/build*",
    # gitignore in ports/unix ignores *.py, so also do it here.
    "ports/unix/*.py",
    # not real python files
    "tests/**/repl_*.py",
    # needs careful attention before applying automatic formatting
    "tests/basics/*.py",
]

# None of the standard Python path matching routines implement the matching
# we want, which is most like git's "pathspec" version of globs.
# In particular, we want "**/" to match all directories.
# This routine is sufficient to work with the patterns we have, but
# subtle cases like character classes that contain meta-characters
# are not covered
def git_glob_to_regex(pat):
    def transform(m):
        m = m.group(0)
        if m == "*":
            return "[^/]*"
        if m == "**/":
            return "(.*/)?"
        if m == "?":
            return "[^/]"
        if m == ".":
            return r"\."
        return m

    result = [transform(part) for part in re.finditer(r"(\*\*/|[*?.]|[^*?.]+)", pat)]
    return "(^" + "".join(result) + "$)"


# Create a single, complicated regular expression that matches exactly the
# files we want, accounting for the PATHS as well as the EXCLUSIONS.
path_re = (
    ""
    # First a negative lookahead assertion that it doesn't match
    # any of the EXCLUSIONS
    + "(?!"
    + "|".join(git_glob_to_regex(pat) for pat in EXCLUSIONS)
    + ")"
    # Then a positive match for any of the PATHS
    + "(?:"
    + "|".join(git_glob_to_regex(pat) for pat in PATHS)
    + ")"
)
path_rx = re.compile(path_re)

# Path to repo top-level dir.
TOP = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

UNCRUSTIFY_CFG = os.path.join(TOP, "tools/uncrustify.cfg")

C_EXTS = (
    ".c",
    ".h",
)
PY_EXTS = (".py",)

# Transform a filename argument relative to the current directory into one
# relative to the TOP directory, which is what we need when checking against
# path_rx.
def relative_filename(arg):
    return str(pathlib.Path(arg).resolve().relative_to(TOP))


def list_files(args):
    return sorted(arg for arg in args if path_rx.match(relative_filename(arg)))


def fixup_c(filename):
    # Read file.
    with open(filename) as f:
        lines = f.readlines()

    # Write out file with fixups.
    with open(filename, "w", newline="") as f:
        dedent_stack = []
        i = 0
        while lines:
            # Get next line.
            i += 1
            l = lines.pop(0)

            # Revert "// |" back to "//| "
            if l.startswith("// |"):
                l = "//|" + l[4:]

            # Dedent #'s to match indent of following line (not previous line).
            m = re.match(r"( +)#(if |ifdef |ifndef |elif |else|endif)", l)
            if m:
                indent = len(m.group(1))
                directive = m.group(2)
                if directive in ("if ", "ifdef ", "ifndef "):
                    l_next = lines[0]
                    indent_next = len(re.match(r"( *)", l_next).group(1))
                    if indent - 4 == indent_next and re.match(r" +(} else |case )", l_next):
                        # This #-line (and all associated ones) needs dedenting by 4 spaces.
                        l = l[4:]
                        dedent_stack.append(indent - 4)
                    else:
                        # This #-line does not need dedenting.
                        dedent_stack.append(-1)
                elif dedent_stack:
                    if dedent_stack[-1] >= 0:
                        # This associated #-line needs dedenting to match the #if.
                        indent_diff = indent - dedent_stack[-1]
                        assert indent_diff >= 0
                        l = l[indent_diff:]
                    if directive == "endif":
                        dedent_stack.pop()

            # Write out line.
            f.write(l)

        assert not dedent_stack, filename


def main():
    cmd_parser = argparse.ArgumentParser(
        description="Auto-format C and Python files -- to be used via pre-commit only."
    )
    cmd_parser.add_argument("-c", action="store_true", help="Format C code only")
    cmd_parser.add_argument("-p", action="store_true", help="Format Python code only")
    cmd_parser.add_argument("-v", action="store_true", help="Enable verbose output")
    cmd_parser.add_argument("--dry-run", action="store_true", help="Print, don't act")
    cmd_parser.add_argument("files", nargs="+", help="Run on specific globs")
    args = cmd_parser.parse_args()

    if args.dry_run:
        print(" ".join(sys.argv))

    # Setting only one of -c or -p disables the other. If both or neither are set, then do both.
    format_c = args.c or not args.p
    format_py = args.p or not args.c

    # Expand the arguments passed on the command line, subject to the PATHS and EXCLUSIONS
    files = list_files(args.files)

    # Extract files matching a specific language.
    def lang_files(exts):
        for file in files:
            if os.path.splitext(file)[1].lower() in exts:
                yield file

    # Run tool on N files at a time (to avoid making the command line too long).
    def batch(cmd, files, N=200):
        while True:
            file_args = list(itertools.islice(files, N))
            if not file_args:
                break
            if args.dry_run:
                print(" ".join(cmd + file_args))
            else:
                subprocess.call(cmd + file_args)

    # Format C files with uncrustify.
    if format_c:
        command = ["uncrustify", "-c", UNCRUSTIFY_CFG, "-lC", "--no-backup"]
        if not args.v:
            command.append("-q")
        batch(command, lang_files(C_EXTS))
        for file in lang_files(C_EXTS):
            fixup_c(file)

    # Format Python files with black.
    if format_py:
        command = ["black", "--fast", "--line-length=99"]
        if args.v:
            command.append("-v")
        else:
            command.append("-q")
        batch(command, lang_files(PY_EXTS))


if __name__ == "__main__":
    main()
