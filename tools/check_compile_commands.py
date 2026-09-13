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

"""Verify that a compilation database covers a MicroPython build.

A compilation database that looks valid but silently omits translation units
yields an analysis over a fraction of the code while reporting success, which is
worse than no analysis because it manufactures false assurance. This check does
not trust the capture tool's own summary. It derives an independent oracle from
the build's own artefacts: the object files the build actually produced, and,
for each object, the compiler-generated dependency file (`.P`, MicroPython's
`py/mkrules.mk` rewrite of the raw `-MD -MF` `.d` gcc emits) naming the real
primary source that produced it. Matching a database entry to an object by its
output path alone only proves a same-named entry exists; it does not prove the
entry actually describes that object's translation unit. This check requires
both: the object must have a database entry, and that entry's `file` must
resolve to the same source the object's own dependency file records, not
merely a same-named entry chosen by path guessing.

Every object compiled from a C or C++ translation unit must have such a
matching entry. Objects assembled from assembly sources are expected to be
absent (no C frontend consumes them, and assembly has no `-MD` dependency
file) and are reported separately, not as errors. An object with no
dependency file and no assembly source of the same stem is unclassifiable and
is treated as a failure, never a silent pass. A database entry whose claimed
output does not exist among the scanned objects, or whose output lies outside
every given build directory, is reported as an extraneous/out-of-scope entry
so unexpected contamination (for example a host-gcc tool build leaking into a
target database) does not pass unnoticed just because it does not cause a
missing-object failure.

Exit status:
  0   every C/C++ object has a database entry whose source matches its .P
      record (or, absent a .P, its recorded output at least resolves under a
      given source root), and no entry is extraneous
  1   one or more C/C++ objects are missing, mismatched, or unclassifiable, or
      an entry is extraneous/out-of-scope
  2   usage or environment error (no objects found, database unreadable)

Usage:
  check_compile_commands.py --db compile_commands.json --build-dir DIR [--build-dir DIR ...]
                      [--source-root DIR ...] [--json-out FILE]

  --db DB             The compilation database to check.
  --build-dir DIR     A directory tree to scan for produced *.o (repeatable).
  --source-root DIR   A root under which a dependency-less match is trusted,
                       and used for the legacy absolute-path-mirror case when
                       no .P file exists (repeatable).
  --json-out FILE     Also write a machine-readable summary to FILE.
"""

from __future__ import annotations

import argparse
import glob
import hashlib
import json
import os
import shlex
import sys

C_SUFFIXES = (".c", ".cc", ".cpp", ".cxx", ".c++")
ASM_SUFFIXES = (".s", ".S", ".asm")


def entry_args(entry: dict) -> list[str]:
    if "arguments" in entry:
        return entry["arguments"]
    return shlex.split(entry["command"])


def entry_output(entry: dict) -> str | None:
    """Absolute, normalised path of the object an entry produced.

    From the entry's `output` field, or its `-o` argument resolved against
    `directory` when relative. Symlinks are resolved (`realpath`) so a
    prefix/containment check against a build directory cannot be defeated by
    a symlinked intermediate component.
    """
    out = entry.get("output")
    args = entry_args(entry)
    if out is None:
        for i, a in enumerate(args):
            if a == "-o" and i + 1 < len(args):
                out = args[i + 1]
                break
    if out is None:
        return None
    if not os.path.isabs(out):
        out = os.path.join(entry["directory"], out)
    return os.path.realpath(out)


def entry_source(entry: dict) -> str | None:
    """Absolute, normalised path of the source an entry compiles (`file`)."""
    src = entry.get("file")
    if not src:
        return None
    if not os.path.isabs(src):
        src = os.path.join(entry["directory"], src)
    return os.path.realpath(src)


def under_any_root(path: str, roots: list[str]) -> bool:
    """True if `path` is `roots[i]` itself or strictly under it.

    Uses `os.path.commonpath` on realpaths so a lexical prefix match (e.g.
    `/tmp/build-old` "under" `/tmp/build`) cannot pass.
    """
    ap = os.path.realpath(path)
    for r in roots:
        rp = os.path.realpath(r)
        if ap == rp:
            return True
        try:
            if os.path.commonpath([ap, rp]) == rp:
                return True
        except ValueError:
            continue
    return False


def dep_file_for(obj_abs: str) -> str | None:
    """Path of the retained dependency file for an object, if it exists.

    MicroPython's py/mkrules.mk (lines 89-96, 101-108) compiles with
    `-MD -MF $(@:.o=.d)`, then copies the raw .d to a sibling .P (a
    make-friendly phony-rule rewrite of the same dependency data, consumed by
    the build itself at line 306's `-include $(OBJ:.o=.P)`) and deletes the
    .d. The .P's first (possibly backslash-continued) line is
    `<object>: <source> <header> ...`; the first token after the
    colon-suffixed object is always the real primary source that produced the
    object, independent of the compilation database or any path-guessing.
    Assembly objects have no `-MD` step and so never have a .P file.
    """
    if not obj_abs.endswith(".o"):
        return None
    p = obj_abs[: -len(".o")] + ".P"
    return p if os.path.exists(p) else None


def primary_source_from_dep_file(dep_path: str) -> str | None:
    """Parse a .P file's first dependency rule and return its primary source.

    Joins backslash-continued physical lines before splitting, so a
    dependency list wrapped across many lines (the normal case; mkrules.mk
    wraps every path) still yields the first token after the object.
    """
    try:
        with open(dep_path) as f:
            joined = ""
            for raw in f:
                line = raw.rstrip("\n")
                cont = line.endswith("\\")
                joined += line[:-1] if cont else line
                joined += " "
                if not cont:
                    break
    except OSError:
        return None
    if ":" not in joined:
        return None
    _, _, rest = joined.partition(":")
    tokens = rest.split()
    return tokens[0] if tokens else None


def resolve_dep_source(dep_path: str, compile_dir: str) -> str | None:
    """Absolute, normalised path of a dependency file's primary source.

    make/gcc write the .d relative to the directory the compile actually ran
    in, which is the claiming database entry's own `directory` field, not
    the object's build-output directory (those differ whenever the build
    recurses via `make -C`, which this build always does).
    """
    src = primary_source_from_dep_file(dep_path)
    if not src:
        return None
    if not os.path.isabs(src):
        src = os.path.join(compile_dir, src)
    return os.path.realpath(src)


def classify_no_dep_object(obj_abs: str, build_dir: str, source_roots: list[str]) -> str:
    """Classify an object with no .P file as 'c', 'asm', or 'unknown'.

    Only assembly objects legitimately have no dependency file (no `-MD`
    step), so this path exists solely to identify those (and the rare
    dependency-less C case, matched only under an explicit source root, never
    guessed via the host filesystem root). MicroPython's build mirrors
    several source layouts under build-*/:
      - sources under a source root (build-*/py/obj.o  <- <root>/py/obj.c)
      - generated C emitted into the build dir itself
        (build-*/frozen_content.o <- build-*/frozen_content.c)
      - objects for a source given by absolute path, mirrored verbatim under
        the build dir (build-*/<abs source path>.o <- /<abs source path>.c)
    The absolute-mirror probe is constrained to reconstructed paths that lie
    under a given source root, so it never probes host filesystem state
    outside the analysed tree.
    """
    rel = os.path.relpath(obj_abs, build_dir)
    stem = rel.removesuffix(".o")
    roots = [build_dir] + source_roots

    def found(suffixes: tuple[str, ...]) -> bool:
        for root in roots:
            for suf in suffixes:
                cand = os.path.join(root, stem + suf)
                if os.path.exists(cand):
                    return True
        for suf in suffixes:
            cand = os.path.join("/", stem + suf)
            if under_any_root(cand, source_roots) and os.path.exists(cand):
                return True
        return False

    if found(ASM_SUFFIXES):
        return "asm"
    if found(C_SUFFIXES):
        return "c"
    return "unknown"


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--db", required=True)
    ap.add_argument("--build-dir", action="append", required=True, dest="build_dirs")
    ap.add_argument("--source-root", action="append", default=[], dest="source_roots")
    ap.add_argument("--json-out")
    args = ap.parse_args()

    try:
        with open(args.db) as f:
            db = json.load(f)
    except (OSError, ValueError) as exc:
        print(f"check_compile_commands: cannot read database {args.db}: {exc}", file=sys.stderr)
        return 2

    build_dirs_abs = [os.path.realpath(b) for b in args.build_dirs]

    # Source roots default to the distinct entry directories, plus any given,
    # so a caller that omits --source-root still gets the historical
    # behaviour for a single-port capture.
    source_roots = list(args.source_roots)
    for e in db:
        d = e.get("directory")
        if d and d not in source_roots:
            source_roots.append(d)

    # Map every entry's output to the entry(ies) claiming it, and separately
    # validate the entry's shape (both output and source resolvable). An
    # entry with no resolvable output or source is malformed and is reported,
    # never silently ignored.
    db_by_output: dict[str, list[dict]] = {}
    malformed_entries = []
    extraneous_entries = []
    for e in db:
        out = entry_output(e)
        src = entry_source(e)
        if out is None or src is None:
            malformed_entries.append(e.get("file") or e.get("output") or "<unknown>")
            continue
        if not under_any_root(out, build_dirs_abs):
            extraneous_entries.append(out)
            continue
        db_by_output.setdefault(out, []).append(e)

    objects = []
    for bd in build_dirs_abs:
        objects.extend(os.path.realpath(p) for p in glob.glob(f"{bd}/**/*.o", recursive=True))
    objects = sorted(set(objects))

    if not objects:
        print("check_compile_commands: no object files found under build dirs", file=sys.stderr)
        return 2
    missing_output_objects = sorted(set(db_by_output) - set(objects))

    missing_c = []
    missing_asm = []
    missing_unknown = []
    mismatched = []
    matched = []
    for obj in objects:
        bd = next((b for b in build_dirs_abs if under_any_root(obj, [b])), build_dirs_abs[0])
        candidates = db_by_output.get(obj, [])
        dep_path = dep_file_for(obj)

        if not candidates:
            if dep_path is not None:
                # Has a real dependency file (so it is a C/C++ object) but no
                # database entry at all: missing, not merely unclassifiable.
                missing_c.append(obj)
                continue
            kind = classify_no_dep_object(obj, bd, source_roots)
            if kind == "asm":
                missing_asm.append(obj)
            elif kind == "c":
                missing_c.append(obj)
            else:
                missing_unknown.append(obj)
            continue

        if len(candidates) != 1:
            mismatched.append((obj, [entry_source(e) for e in candidates], ["duplicate output"]))
            continue

        if dep_path is None:
            # No dependency file to cross-check against (should not happen
            # for a real C/C++ object under this build, but do not silently
            # trust a same-named entry with nothing independent behind it:
            # require the entry's source to at least resolve to a real file
            # under a source root).
            ok = any(
                os.path.exists(entry_source(e)) and under_any_root(entry_source(e), source_roots)
                for e in candidates
            )
            if ok:
                matched.append(obj)
            else:
                mismatched.append((obj, [entry_source(e) for e in candidates], None))
            continue

        # The .P file's dependency paths are relative to the compile's own
        # working directory, i.e. the claiming entry's own `directory` field,
        # not the build-output directory. Resolve against each candidate's
        # directory (ordinarily there is exactly one) and accept a match
        # against any of them.
        dep_sources = {
            resolve_dep_source(dep_path, e["directory"]) for e in candidates if e.get("directory")
        }
        entry_sources = [entry_source(e) for e in candidates]
        if dep_sources & set(entry_sources):
            matched.append(obj)
        else:
            mismatched.append((obj, entry_sources, sorted(dep_sources)))

    summary = {
        "database": os.path.abspath(args.db),
        "database_sha256": hashlib.sha256(open(args.db, "rb").read()).hexdigest(),
        "db_entries": len(db),
        "db_distinct_outputs": len(db_by_output),
        "db_malformed_entries": malformed_entries,
        "db_extraneous_entries": extraneous_entries,
        "db_outputs_missing_on_disk": missing_output_objects,
        "objects_on_disk": len(objects),
        "objects_matched": len(matched),
        "objects_missing_c": missing_c,
        "objects_missing_asm": missing_asm,
        "objects_missing_unknown": missing_unknown,
        "objects_mismatched": [
            {"object": o, "db_sources": s, "dep_sources": d} for o, s, d in mismatched
        ],
        "complete": not (
            missing_c
            or missing_unknown
            or mismatched
            or malformed_entries
            or extraneous_entries
            or missing_output_objects
        ),
    }

    print(f"database entries:        {len(db)}")
    print(f"distinct object outputs:  {len(db_by_output)}")
    print(f"objects on disk:          {len(objects)}")
    print(f"matched (source-verified):{len(matched):>3}")
    print(f"assembly (expected out):  {len(missing_asm)}")
    if missing_asm:
        for o in missing_asm:
            print(f"    asm  {o}")
    if missing_unknown:
        print(f"unclassifiable objects:   {len(missing_unknown)} (treated as failures)")
        for o in missing_unknown:
            print(f"    ????  {o}")
    if missing_c:
        print(f"MISSING C/C++ TUs:        {len(missing_c)}")
        for o in missing_c:
            print(f"    MISSING  {o}")
    if mismatched:
        print(f"MISMATCHED entries:       {len(mismatched)}")
        for o, s, d in mismatched:
            print(f"    MISMATCH  {o}  db_source={s}  dep_sources={d}")
    if malformed_entries:
        print(f"MALFORMED db entries:     {len(malformed_entries)}")
        for m in malformed_entries:
            print(f"    MALFORMED  {m}")
    if extraneous_entries:
        print(f"EXTRANEOUS db entries:    {len(extraneous_entries)} (output outside build dirs)")
        for o in extraneous_entries:
            print(f"    EXTRANEOUS  {o}")
    if missing_output_objects:
        print(f"MISSING output objects:   {len(missing_output_objects)}")
        for o in missing_output_objects:
            print(f"    MISSING OUTPUT  {o}")

    if args.json_out:
        with open(args.json_out, "w") as f:
            json.dump(summary, f, indent=2)

    if not summary["complete"]:
        print(
            "check_compile_commands: INCOMPLETE - database does not faithfully cover the build",
            file=sys.stderr,
        )
        return 1
    print("check_compile_commands: COMPLETE - every C/C++ object has a verified database entry")
    return 0


if __name__ == "__main__":
    sys.exit(main())
