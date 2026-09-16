#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Andrew Leech
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
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Generate a clang JSON compilation database for a MicroPython make build.

Runs the build verbosely and serially (``make V=1 -j1``) and records every C and
C++ compiler invocation as an entry in a ``compile_commands.json`` file, which
static analysis tools (clang-tidy, clangd, cppcheck, and GitLab Advanced SAST)
consume to see the exact flags, include paths and defines each translation unit
was compiled with.

The build is not restructured. ``make V=1`` sets the port makefiles' ``$(Q)`` to
empty so the real compiler command is printed rather than the ``CC file.c``
summary line, and ``-j1`` keeps those lines from interleaving. The build recurses
into ports and subdirectories with ``make -C``; each sub-make prints
``Entering directory`` / ``Leaving directory``, which this script tracks to
resolve the working directory of each compile so relative paths in an entry are
correct. ``-w`` forces those directory messages to print even if ``MAKEFLAGS``
carries ``-s``/``--no-print-directory`` from the caller's environment, and
``MAKEFLAGS``/``MFLAGS`` are stripped from the child environment so an inherited
``-j`` or ``-s`` cannot silently defeat the ``-w -j1`` this script requires.

Because it observes the real build, an incremental build compiles nothing and
produces no entries; pass a target that builds from clean, or clean first. Only
translation units the build actually compiles are recorded, so the database is a
faithful record of what is built into the image for the given configuration.

Coverage is intentionally narrow rather than guessed: only a compiler whose
basename matches ``--compiler-re`` (default: gcc/g++/clang/clang++/cc/c++, with
optional version/target suffixes such as ``arm-none-eabi-gcc`` or ``gcc-12``) is
recorded, and a command line prefixed by an unrecognised wrapper (``ccache``,
``distcc``, ``env FOO=bar``, a ``cd x &&`` chain) is not unwrapped; it is either
matched by widening ``--compiler-re`` deliberately or it is not a compile command
by this script's definition. Silent partial coverage is worse than an explicit
gap, so a build that emits nothing but wrapped invocations reports zero entries,
which the caller must treat as a hard failure.
"""

import argparse
import json
import os
import re
import shlex
import subprocess
import sys

# Compilers whose invocations are recorded. Matched against the command's first
# token, by basename, so both "gcc" and "arm-none-eabi-gcc" match, and also
# "gcc-12"/"clang-17" via the optional trailing digits. A caller whose build
# wraps the compiler in ccache/distcc, or uses a name this does not cover
# (armclang, a vendor compiler), must pass --compiler-re to extend it or accept
# that those lines are not recorded; an unmatched compiler line is silently not
# a compile command, by design, so extending coverage is explicit rather than
# guessed at.
_DEFAULT_COMPILER_RE = r"(^|-)(gcc|g\+\+|clang|clang\+\+|cc|c\+\+)(-[0-9]+(\.[0-9]+)*)?$"

# A source file a compile command operates on. Assembly is deliberately excluded:
# a compilation database is for C/C++ frontends, which do not consume .s units.
_SOURCE_RE = re.compile(r"\.(c|cc|cpp|cxx|c\+\+|C)$")

# make prints these when recursing with `make -C`; the level number, when
# present, lets a directory-stack inconsistency be detected rather than
# silently mis-tracked.
_ENTER_RE = re.compile(r"^make(?:\[(\d+)\])?: Entering directory ['\"](.+)['\"]")
_LEAVE_RE = re.compile(r"^make(?:\[(\d+)\])?: Leaving directory ['\"](.+)['\"]")


def _is_compile_command(tokens, compiler_re):
    """True if the token list is a compiler invocation that compiles a source.

    A compile command runs a compiler, has ``-c`` (compile, do not link), and
    names a source file. Linking, archiving and preprocessing-only steps are not
    compile commands and are skipped. No shell-noise prefix (``ccache``, ``env
    X=Y``, ``cd d &&``) is stripped: ``tokens[0]`` must itself match
    ``compiler_re``, so a wrapped invocation is either matched by widening
    ``--compiler-re`` or is correctly not recorded rather than guessed at.
    """
    if not tokens:
        return False
    if not compiler_re.search(os.path.basename(tokens[0])):
        return False
    if "-c" not in tokens:
        return False
    return any(_SOURCE_RE.search(tok) for tok in tokens)


# Flags that consume the following token as their own value, so that value is
# never mistaken for the positional source argument. This must stay in sync
# with any flag MicroPython's build passes with a separate-token value that
# could itself carry a C-suffixed name (-include and -o are the two that
# realistically could; the rest are here for completeness and safety).
_VALUE_FLAGS = {
    "-o",
    "-include",
    "-MF",
    "-MT",
    "-MQ",
    "-I",
    "-isystem",
    "-iquote",
    "-idirafter",
    "-D",
    "-U",
    "-x",
}


def _source_of(tokens):
    """Return the source file a compile command compiles, or None.

    GCC's calling convention places the file(s) being compiled as positional
    (non-flag) arguments; a flag's own value (for example ``-include foo.h``,
    or a C-suffixed name given to ``-o``) is never a source even though it may
    itself match the C-suffix pattern. This walks the token list tracking
    which tokens are flag values to skip, then returns the *last* remaining
    C-suffixed positional token, matching where the compiler expects it and
    avoiding a leading ``-include`` argument being mistaken for the compiled
    file.
    """
    candidates = []
    skip_next = False
    for tok in tokens:
        if skip_next:
            skip_next = False
            continue
        if tok in _VALUE_FLAGS:
            skip_next = True
            continue
        if tok.startswith("-"):
            continue
        if _SOURCE_RE.search(tok):
            candidates.append(tok)
    return candidates[-1] if candidates else None


def _output_of(tokens):
    """Return the object file a compile command writes (-o value), or None."""
    for i, tok in enumerate(tokens):
        if tok == "-o" and i + 1 < len(tokens):
            return tokens[i + 1]
    return None


def parse_build_log(lines, root, compiler_re):
    """Parse make output into compilation-database entries.

    ``root`` is the directory make was launched from; it seeds the directory
    stack so a compile printed before any ``Entering directory`` is resolved
    against the launch directory. GNU make's printed recursion level is not a
    reliable depth indicator through shell-invoked sub-makes, so transitions are
    validated by directory path instead: every leave must match the active enter
    and the stream must finish back at ``root``. An ambiguous directory field is
    a hard error rather than a plausible but incorrect database entry.
    """
    entries = []
    seen = {}
    # Directory context: a stack mirroring make's recursion.
    root = os.path.normpath(root)
    dir_stack = [root]
    stack_errors = []

    for raw in lines:
        line = raw.rstrip("\n")

        m = _ENTER_RE.match(line)
        if m:
            entered = os.path.normpath(m.group(2))
            # make invoked from root prints an outer enter/leave pair. The root
            # is already seeded, so it is a no-op rather than a recursive level.
            if entered != root or len(dir_stack) != 1:
                dir_stack.append(entered)
            continue
        m = _LEAVE_RE.match(line)
        if m:
            leaving = os.path.normpath(m.group(2))
            if leaving == root and len(dir_stack) == 1:
                continue
            if len(dir_stack) == 1 or leaving != dir_stack[-1]:
                stack_errors.append(f"leaving {leaving!r} with active directory {dir_stack[-1]!r}")
                continue
            dir_stack.pop()
            continue

        # A compile command may be prefixed by shell noise; try to tokenise the
        # whole line and test it. shlex handles quoted -D"..." defines, yielding
        # the token as the compiler receives it (quotes resolved), which is the
        # argv form a compilation database's "arguments" is defined to hold.
        try:
            tokens = shlex.split(line)
        except ValueError:
            # Unbalanced quotes: not a command line we can trust, skip it. A
            # line that merely resembles a compile command but cannot be
            # tokenised is reported, never silently dropped without a trace,
            # so a build whose commands are all unparseable is visible rather
            # than yielding a quietly empty database.
            print(f"gen_compile_commands: skipping unparseable line: {line!r}", file=sys.stderr)
            continue
        if not _is_compile_command(tokens, compiler_re):
            continue

        directory = dir_stack[-1]
        source = _source_of(tokens)
        output = _output_of(tokens)
        # De-duplicate on the output object, canonicalised against the
        # directory the compile actually ran in: two recursive makes can
        # legally emit the same *relative* -o value from different working
        # directories, which would otherwise collide onto one dropped entry.
        # Fall back to the (directory, source) pair only when no -o is
        # present. The last occurrence wins, matching a from-scratch build's
        # final state.
        if output:
            out_abs = (
                output
                if os.path.isabs(output)
                else os.path.normpath(os.path.join(directory, output))
            )
            key = ("o", out_abs)
        else:
            key = ("s", directory, source)
        entry = {
            "directory": directory,
            "file": source,
            "arguments": tokens,
        }
        if key in seen:
            entries[seen[key]] = entry
        else:
            seen[key] = len(entries)
            entries.append(entry)

    if len(dir_stack) != 1:
        stack_errors.append(f"unterminated directory stack: {dir_stack[1:]!r}")
    if stack_errors:
        raise ValueError("directory-stack inconsistency: " + "; ".join(stack_errors))
    return entries


def run_make(make_args, cwd, env, compiler_re_str):
    """Run the verbose serial build and return its output lines.

    ``V=1`` empties MicroPython's ``$(Q)`` so the real compiler command
    prints; ``-j1`` keeps compile lines from interleaving; ``-w`` forces
    Entering/Leaving directory messages even if the caller's ``MAKEFLAGS``
    carries ``-s``/``--no-print-directory``. These three are placed last so a
    caller-supplied conflicting flag earlier in ``make_args`` (for example a
    stray ``-j4``) does not silently defeat them: GNU make takes the last
    occurrence of a flag it does not accumulate. ``MAKEFLAGS``/``MFLAGS`` are
    removed from the child environment first, because an inherited ``-j8`` or
    ``-s`` there is honoured *in addition to* the command line and would
    otherwise defeat ``-j1``/``-w`` regardless of argument order.
    """
    env = dict(env)
    env.pop("MAKEFLAGS", None)
    env.pop("MFLAGS", None)
    cmd = ["make", *make_args, "V=1", "-w", "-j1"]
    proc = subprocess.run(
        cmd,
        cwd=cwd,
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        sys.stderr.write(proc.stdout[-4000:])
        raise SystemExit(f"gen_compile_commands: make failed (exit {proc.returncode})")
    return proc.stdout.splitlines()


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "-C",
        dest="directory",
        default=".",
        help="run make from this directory (make -C)",
    )
    parser.add_argument(
        "-o",
        dest="output",
        default="compile_commands.json",
        help="path to write the compilation database to",
    )
    parser.add_argument(
        "--log",
        help="parse a previously captured make V=1 -w -j1 log instead of running make",
    )
    parser.add_argument(
        "--compiler-re",
        default=_DEFAULT_COMPILER_RE,
        help=(
            "regex matched against a compile command's first token's basename "
            "(default covers gcc/g++/clang/clang++/cc/c++ and versioned/"
            "cross-prefixed variants); widen this to cover a compiler wrapper "
            "or vendor toolchain this build uses instead of guessing at one"
        ),
    )
    parser.add_argument(
        "make_args",
        nargs=argparse.REMAINDER,
        help="arguments passed to make after `--` (targets, BOARD=..., etc.)",
    )
    args = parser.parse_args()

    make_args = args.make_args
    if make_args and make_args[0] == "--":
        make_args = make_args[1:]

    root = os.path.abspath(args.directory)
    compiler_re = re.compile(args.compiler_re)

    if args.log:
        with open(args.log) as f:
            lines = f.read().splitlines()
    else:
        env = os.environ.copy()
        lines = run_make(make_args, cwd=root, env=env, compiler_re_str=args.compiler_re)

    entries = parse_build_log(lines, root, compiler_re)

    if not entries:
        raise SystemExit(
            "gen_compile_commands: no compile commands found; the build produced zero "
            "entries, which almost always means an incremental build compiled nothing "
            "(clean first), the build wraps the compiler in a way --compiler-re does "
            "not match (ccache, distcc, a vendor name), or the compiler name needs "
            "--compiler-re for another reason"
        )

    with open(args.output, "w") as f:
        json.dump(entries, f, indent=1)

    print(f"gen_compile_commands: wrote {len(entries)} entries to {args.output}")


if __name__ == "__main__":
    main()
