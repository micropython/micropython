#!/usr/bin/env -S uv run --script
# /// script
# dependencies = [
#     "spdx-tools>=0.8.2",
#     "cyclonedx-python-lib>=11.12",
#     "license-expression>=30.4",
#     "esp-idf-sbom>=1.5",
#     "tomli>=2; python_version<'3.11'",
# ]
# ///
# SPDX-License-Identifier: MIT
#
# Generate an SBOM (SPDX 2.3 and/or CycloneDX 1.6 JSON) for a micropython build from its
# linker map: tools/sbom.py build/firmware.map --spdx fw.spdx.json --cyclonedx fw.cdx.json
#
# A file's licence is the first of: SPDX tag, frozen manifest metadata, REUSE.toml
# annotation, [licenses] table in LICENSE, the component's licence file (as a LicenseRef). Anything unresolved is an error.

import argparse
import base64
import contextlib
import datetime
import fnmatch
import functools
import importlib.util
import json
import os
import posixpath
import re
import subprocess
import sys
import tempfile
import uuid
from dataclasses import dataclass
from pathlib import Path, PurePosixPath
from typing import Any
from collections.abc import Iterable

try:
    import tomllib
except ImportError:  # Python < 3.11
    import tomli as tomllib

LICENSE_FILE_GLOBS = ("LICENSE*", "License*", "COPYING*")

LLVM = "Apache-2.0 WITH LLVM-exception"
GCC = "GPL-3.0-or-later WITH GCC-exception-3.1"

# Toolchain libraries have no source in the repo, so they're identified by the archive that
# supplied the object: (archive path regex, component, SPDX licence, compiler or None)
TOOLCHAIN_LIBS = [
    (r"(^|/)libclang_rt[^/]*\.a$", "compiler-rt", LLVM, "clang"),
    (r"(^|/)libc\+\+(abi)?[^/]*\.a$", "libc++", LLVM, "clang"),
    (r"(^|/)libunwind[^/]*\.a$", "libunwind", LLVM, "clang"),
    (r"(^|/)(libgcc(_eh)?\.a|crt(begin|end)[^/]*\.o)$|/gcc/.*/crt[in]\.o$", "libgcc", GCC, "gcc"),
    (r"(^|/)lib(std|sup)c\+\+[^/]*\.a$", "libstdc++", GCC, "gcc"),
    (
        r"/(picolibc|clang-runtimes)/.*/(lib[cm]\.a|crt0[^/]*\.o)$",
        "picolibc",
        "LicenseRef-picolibc",
        None,
    ),
    (r"musl[^/]*/(.*/)?(lib[cm]\.a|S?crt[1in]\.o)$", "musl", "MIT", None),
    (
        r"(linux-gnu|glibc).*/(lib(c|m|pthread|dl|rt|util|resolv|crypt)(-[0-9.]+)?\.a|S?crt[1in]\.o)$",
        "glibc",
        "LGPL-2.1-or-later",
        None,
    ),
    (r"(^|/)libffi\.a$", "libffi", "MIT", None),
    (
        r"(^|/)lib(c|m|g|nosys|rdimon|rdpmon|gloss[^/]*)(_nano)?\.a$|arm-none-eabi/lib/.*crt0\.o$",
        "newlib",
        "LicenseRef-newlib",
        None,
    ),
]


class SbomError(Exception):
    pass


@dataclass
class Component:
    name: str
    ref: str  # unique id: repo relative directory, "toolchain/<name>" or "esp-idf:<package>"
    license: str | None = None
    version: str | None = None
    commit: str | None = None
    url: str | None = None
    purl: str | None = None
    cpe: str | None = None
    supplier: str | None = None


@dataclass
class Collected:
    components: list[Component]
    unresolved: list[str]  # linked objects that can't be traced to exactly one component
    missing: list[str]  # source files or packages with no licence


# debug info, Xtensa property tables etc are not part of the image
NON_IMAGE_SECTIONS = tuple(
    ".debug .comment .stab .xt .ARM.attr .riscv.attr .gnu.attr .note.GNU".split()
)


def is_object_name(name: str) -> bool:
    return name.endswith((".o", ".obj")) or (name.endswith(")") and ".a(" in name)


def linked_objects(map_path: Path) -> set[str]:
    """Object paths / archive(member) names included in the binary."""
    objs = set()
    gnu = False
    wrapped = ""  # GNU ld puts a long input section name on its own line
    for line in map_path.read_text(errors="replace").splitlines():
        t = line.split()
        if line.startswith("Linker script and memory map"):
            gnu = True
        elif not t or not line[0].isspace():
            continue
        elif gnu and len(t) == 1 and line.startswith(" ."):
            wrapped = t[0]
        else:
            obj, section, size = "", "", 0
            if gnu and len(t) >= 3 and t[-3].startswith("0x") and t[-2].startswith("0x"):
                obj, size, section = t[-1], int(t[-2], 16), t[-4] if len(t) >= 4 else wrapped
            elif len(t) == 5 and t[3].isdigit() and t[4].endswith(")"):
                obj, _, section = t[4][:-1].rpartition(":(")
                size = int(t[2], 16)
            wrapped = ""
            if is_object_name(obj) and size and not section.startswith(NON_IMAGE_SECTIONS):
                objs.add(obj)
    if not objs:
        raise SbomError(f"{map_path}: no linked objects found, not a GNU ld or LLD map")
    return objs


def split_member(obj: str) -> tuple[str, str | None]:
    """("lib/libx.a", "member.o") for "lib/libx.a(member.o)", else (obj, None)."""
    archive, sep, member = obj.partition(".a(")
    return (archive + ".a", member[:-1]) if sep else (obj, None)


def drop_object_suffix(name: str) -> str:
    return re.sub(r"\.(obj|o)$", "", name)


def license_refs(expression: str | None) -> set[str]:
    words = (expression or "").replace("(", " ").replace(")", " ").split()
    return {w for w in words if w.startswith("LicenseRef-")}


def glob_match(path: str, pattern: str) -> bool:
    """REUSE.toml path glob: "**/" is any number of directories, "**" anything including "/",
    "*" anything but "/", "?" one character but "/"."""
    tokens = {"**/": "(?:.*/)?", "**": ".*", "*": "[^/]*", "?": "[^/]"}
    regex = re.sub(r"\*\*/?|\*|\?|[^*?]+", lambda m: tokens.get(m[0]) or re.escape(m[0]), pattern)
    return re.fullmatch(regex, path) is not None


def github_purl(url: str | None, version: str | None, commit: str | None) -> str | None:
    m = re.fullmatch(
        r"(?:https://github\.com/|git@github\.com:)([^/]+)/([^/]+?)(?:\.git)?", url or ""
    )
    if not m or not version:
        return None
    numbers = version.lstrip("v").split(".")
    ref = version if len(numbers) > 1 and all(n.isdigit() for n in numbers) else commit
    return f"pkg:github/{m[1].lower()}/{m[2].lower()}@{ref}"


def path_version(path: str, compiler: str | None) -> str | None:
    """Version segment of a toolchain library path (gcc/<triple>/<version>/, clang/<version>/),
    if it starts with a digit."""
    parts = path.split("/")
    step = 2 if compiler == "gcc" else 1
    return next(
        (
            parts[i + step]
            for i in range(len(parts) - step - 1)
            if parts[i] == compiler and parts[i + step][:1].isdigit()
        ),
        None,
    )


def toolchain_lib(obj: str) -> Component | None:
    """Component for an object from a known toolchain library (archive member or crt file)."""
    archive = posixpath.normpath(split_member(obj)[0].replace("\\", "/"))
    for pattern, name, lic, compiler in TOOLCHAIN_LIBS:
        if re.search(pattern, archive):
            return Component(name, "toolchain/" + name, lic, path_version(archive, compiler))
    return None


def slug(s: str) -> str:
    """Letters and digits only, as in the ids of esp-idf-sbom and SPDX."""
    return "".join(c if c.isascii() and c.isalnum() else "-" for c in s)


@functools.cache
def licensing() -> Any:
    from license_expression import get_spdx_licensing

    return get_spdx_licensing()


def parse_spdx(text: str, where: str) -> str:
    """Normalised SPDX expression; LicenseRef-* are allowed."""
    if not text.strip():
        raise SbomError(f"{where}: empty SPDX licence expression")
    try:
        bad = [
            s
            for s in licensing().validate(text).invalid_symbols
            if not s.startswith("LicenseRef-")
        ]
        parsed = str(licensing().parse(text))
    except Exception as e:  # license-expression raises more than its own error types
        raise SbomError(f"{where}: not an SPDX licence expression ({e}): {text!r}")
    if bad:
        raise SbomError(f"{where}: not an SPDX licence expression ({', '.join(bad)}): {text!r}")
    return parsed


def combine(exprs: Iterable[str | None]) -> str | None:
    """AND together SPDX expressions, simplified; None if there are none."""
    exprs = [e for e in exprs if e]
    return (
        str(licensing().parse(" AND ".join(f"({e})" for e in exprs)).simplify()) if exprs else None
    )


def file_tag(path: Path) -> str | None:
    """SPDX expression from the SPDX-License-Identifier tag(s) in the head of a file."""
    if not path.is_file():  # generated at build time
        return None
    with path.open(errors="replace") as f:
        head = f.read(2048)  # the tag is in the comment at the top of a file
    tags = []
    for chunk in head.split("SPDX-License-Identifier:")[1:]:
        with contextlib.suppress(SbomError):  # prose mentioning the tag, or no expression
            tags.append(parse_spdx(re.split(r"\*/|-->", chunk.partition("\n")[0])[0], str(path)))
    return combine(tags)  # several tags in one file all apply


def identify_license(text: str) -> str | None:
    """SPDX id of a single, short, common licence text (MIT, ISC, BSD-2/3-Clause, Apache-2.0 or
    MPL-2.0), else None. Longer files, which are likely to hold several licences, are not
    recognised."""
    t = " ".join(text.lower().split())
    if t.startswith("apache license version 2.0") or t.startswith("apache license, version 2.0"):
        return "Apache-2.0" if "terms and conditions for use, reproduction" in t else None
    if t.startswith("mozilla public license, version 2.0"):
        return "MPL-2.0"
    if len(t) > 2500 or any(w in t for w in ("commercial", "advertising", "personal benefit")):
        return None  # a long file, or a standard text that someone has added a clause to
    if "permission is hereby granted, free of charge, to any person obtaining a copy" in t:
        return "MIT"
    if "permission to use, copy, modify, and/or distribute this software" in t:
        return "ISC"
    if "redistribution and use in source and binary forms" in t:
        # the standard clauses only: any other (a restriction on use, say) isn't BSD
        clauses = re.split(r"\s\d\.\s", t)[1:]
        starts = ("redistributions of source code", "redistributions in binary form")
        if len(clauses) in (2, 3) and all(c.startswith(starts) for c in clauses[:2]):
            if len(clauses) == 2:
                return "BSD-2-Clause"
            if re.match(r"(neither )?the names? of", clauses[2]):
                return "BSD-3-Clause"
    return None


class LicenseTable:
    """The [licenses] TOML table at the end of the top level LICENSE: {repo path: SPDX expression}.

    Keys are files, directories or fnmatch globs. "." is the repo root."""

    def __init__(self, license_file: Path):
        text = license_file.read_text()
        start = text.find("\n[licenses]")
        if start < 0:
            raise SbomError(f"no [licenses] table in {license_file}")
        try:
            table = tomllib.loads(text[start:])["licenses"]
        except tomllib.TOMLDecodeError as e:
            raise SbomError(f"{license_file}: the [licenses] table is not valid TOML: {e}")
        # glob (or plain path) -> SPDX expression
        self.table: dict[str, str] = {
            k: parse_spdx(v, f"{license_file}: {k}") for k, v in table.items()
        }

    def lookup(self, rel: PurePosixPath, top: PurePosixPath) -> str | None:
        """Entry for rel, else for its nearest parent directory, going up as far as top.

        If several keys match a path the one with the most literal characters is used."""
        for p in (rel, *rel.parents):
            hits = [g for g in self.table if fnmatch.fnmatchcase(str(p), g)]
            if hits:
                return self.table[max(hits, key=lambda g: sum(c not in "*?[" for c in g))]
            if p == top:
                break
        return None


def manifest_inputs(
    root: Path, manifest: str, variables: dict[str, str | None]
) -> tuple[list[tuple[Path, str | None]], list[Path]]:
    """([(frozen source path, licence declared in the manifest metadata or None)],
    [c_module() directories]) for a manifest."""
    spec = importlib.util.spec_from_file_location(
        "manifestfile", root / "tools" / "manifestfile.py"
    )
    manifestfile = importlib.util.module_from_spec(spec)
    sys.modules["manifestfile"] = manifestfile
    spec.loader.exec_module(manifestfile)
    mf = manifestfile.ManifestFile(manifestfile.MODE_FREEZE, variables)
    mf.execute(manifest)
    files = [(Path(r.full_path).resolve(), r.metadata.license) for r in mf.files()]
    return files, [Path(d).resolve() for d in mf.c_modules()]


def compile_commands(build_dir: Path) -> dict[Path, Path] | None:
    """{object: source} from the CMake compile_commands.json in build_dir, None if there is none.

    A source generated inside the build directory is replaced by the file of the same name in
    the source directory of the CMake directory it was generated in (cmake_install.cmake)."""
    try:
        entries = json.loads((build_dir / "compile_commands.json").read_text())
    except FileNotFoundError:
        return None
    build = build_dir.resolve()
    result = {}
    for e in entries:
        if "output" not in e:
            continue
        directory = Path(e["directory"])
        src = Path(os.path.normpath(directory / e["file"]))
        if build in (src, *src.parents):
            install = src.parent / "cmake_install.cmake"
            m = (
                re.search(
                    r"^# Install script for directory: (.+)$", install.read_text(), re.MULTILINE
                )
                if install.is_file()
                else None
            )
            if not m:
                continue
            src = Path(m[1].strip(), src.name)
        result[Path(os.path.normpath(build / e["output"]))] = src.resolve()
    return result


def dep_source(obj: str, port_dir: str | None) -> Path | None:
    """The source named first in the make dependency file (X.P beside X.o), or None."""
    try:
        text = Path(obj.removesuffix(".o") + ".P").read_text()
    except OSError:
        return None
    prereqs = [t for t in text.partition(": ")[2].split() if t != "\\"]
    if not prereqs:
        return None
    src = Path(port_dir or "", prereqs[0])
    return src.resolve() if src.is_file() else None


class CMakeArchives:
    """The objects CMake put in an archive: <archive dir>/CMakeFiles/<target>.dir/**/<member>.
    An archive can also hold the objects of a sibling object library (mbedtls 4 does), which
    are found by widening the search to the parent directories."""

    def __init__(self, base_dirs: list[Path]):
        self.base_dirs = base_dirs
        self.index: dict[Path, dict[str, list[Path]]] = {}  # dir -> {member: [objects]}

    def locate(self, archive: str) -> Path | None:
        return next((p.resolve() for b in self.base_dirs if (p := b / archive).is_file()), None)

    def objects(self, archive: Path, member: str) -> list[Path]:
        build, parent = self.base_dirs[0].resolve(), archive.parent
        # the archive's own directory first, then up to the build directory
        for d in [parent, *(p for p in parent.parents if build in (p, *p.parents))]:
            if d not in self.index:
                index = self.index[d] = {}
                for p in sorted(d.rglob("*")):
                    if p.is_file() and "CMakeFiles" in p.relative_to(d).parts:
                        index.setdefault(p.name, []).append(p)
            if self.index[d].get(member):
                return self.index[d][member]
        return []


def git(repo: Path, *args: str) -> str | None:
    r = subprocess.run(["git", "-C", str(repo), *args], capture_output=True, text=True)
    if "dubious ownership" in r.stderr:
        raise SbomError(f"git won't read {repo}, it is owned by another user: {r.stderr.strip()}")
    return r.stdout.strip() if r.returncode == 0 else None


class Repo:
    def __init__(self, root: Path):
        self.root = root.resolve()
        self.licenses = LicenseTable(self.root / "LICENSE")
        self.submodules: dict[Path, str | None] = {}  # directory -> url
        pending = [self.root]
        while pending:  # submodules can have submodules of their own
            d = pending.pop()
            out = git(d, "config", "--file", ".gitmodules", "--list") or ""
            cfg = dict(line.split("=", 1) for line in out.splitlines())
            for key, path in cfg.items():
                if key.endswith(".path"):
                    self.submodules[d / path] = cfg.get(key.removesuffix("path") + "url")
                    pending.append(d / path)
        self.extracted: dict[str, str] = {}  # LicenseRef-* made from component licence files
        self.roots: set[Path] = set()  # component roots outside the repo
        self._reuse: dict[Path, list[tuple[str, str]]] = {}  # dir -> REUSE.toml (glob, SPDX)
        self.reuse_refs: dict[str, Path] = {}  # LicenseRef-* named in a REUSE.toml -> its dir

    def contains(self, path: Path) -> bool:
        return self.root in path.parents

    @functools.cache
    def license_files(self, d: Path) -> list[Path]:
        return sorted({p for g in LICENSE_FILE_GLOBS for p in d.glob(g) if p.is_file()})

    def add_root(self, d: Path) -> None:
        """Make a directory outside the repo (a --src-root, c_module() or variable directory) the
        top of a component."""
        if d != self.root and not self.contains(d):
            self.roots.add(d)

    def component_dir(self, source: Path) -> Path | None:
        """The directory of the component owning a source file: the nearest parent that is the repo
        root, a submodule or a registered root, or that has a licence file and is inside the repo
        or under a root. None if the file is outside the repo and every root."""
        for d in source.parents:
            if d == self.root or d in self.submodules or d in self.roots:
                return d
            inside = self.contains(d) or any(r in d.parents for r in self.roots)
            if inside and self.license_files(d):
                return d
        return None

    def license_from_file(self, d: Path, name: str) -> str | None:
        """Licence of directory d from its licence files, or None if it has none. A single file
        holding a recognised licence text is that licence, otherwise it's LicenseRef-<name> with
        the file(s) as its text."""
        if not (files := self.license_files(d)):
            return None
        texts = [p.read_text(errors="replace").strip() for p in files]
        if len(files) == 1 and (known := identify_license(texts[0])):
            return known
        ref = "LicenseRef-" + slug(name)
        if ref not in self.extracted:
            self.extracted[ref] = "\n\n".join(
                (p.name + ":\n" if len(files) > 1 else "") + t for p, t in zip(files, texts)
            )
        return ref

    def reuse_license(self, src: Path, comp_dir: Path) -> str | None:
        """Licence from the nearest REUSE.toml annotation matching src, up to the component dir,
        then from the REUSE.toml of a --src-root above it, so a project can declare the licence of
        files in a third party component that doesn't carry its own.
        Only `path` (a string or a list) and `SPDX-License-Identifier` strings are supported."""
        past = False
        for d in src.parents:
            if past and d not in self.roots:
                continue
            if d not in self._reuse:
                self._reuse[d] = []
                if (d / "REUSE.toml").is_file():
                    try:
                        toml = tomllib.loads((d / "REUSE.toml").read_text())
                        for a in toml.get("annotations", []):
                            if "SPDX-License-Identifier" in a:
                                expr = parse_spdx(a["SPDX-License-Identifier"], f"{d}/REUSE.toml")
                                for ref in license_refs(expr):
                                    self.reuse_refs.setdefault(ref, d)
                                paths = a["path"] if isinstance(a["path"], list) else [a["path"]]
                                self._reuse[d] += [(p, expr) for p in paths]
                    except (tomllib.TOMLDecodeError, KeyError) as e:
                        raise SbomError(f"{d}/REUSE.toml: {e!r}")
            rel = src.relative_to(d).as_posix()
            # the last matching annotation wins
            found = next((e for p, e in reversed(self._reuse[d]) if glob_match(rel, p)), None)
            if found:
                return found
            past = past or d == comp_dir
        return None

    def file_license(self, src: Path, comp_dir: Path, declared: str | None = None) -> str | None:
        """SPDX expression for a source file, the first of: its own tag, the manifest's declared
        licence, the REUSE.toml annotation, the licence table entry (files in the repo only), the
        licence file of its component."""
        table = self.contains(src) and self.licenses.lookup(
            PurePosixPath(self.label(src)), PurePosixPath(self.label(comp_dir))
        )
        # outside the repo a component is named by its directory, not a machine specific path
        name = self.label(comp_dir) if self.contains(comp_dir) else comp_dir.name
        return (
            file_tag(src)
            or declared
            or self.reuse_license(src, comp_dir)
            or table
            or (comp_dir != self.root and self.license_from_file(comp_dir, name))
            or None
        )

    def label(self, p: Path) -> str:
        """Path relative to the repo root ("." for the root itself), else absolute."""
        return (p.relative_to(self.root) if p == self.root or self.contains(p) else p).as_posix()

    def component(self, d: Path) -> Component:
        """Component (without licence) for directory d."""
        if d == self.root:
            c = Component("micropython", "micropython")
            c.url = git(d, "config", "--get", "remote.upstream.url") or git(
                d, "config", "--get", "remote.origin.url"
            )
        else:
            c = Component(d.name, self.label(d).lstrip("/"), url=self.submodules.get(d))
        if d == self.root or (d in self.submodules and (d / ".git").exists()):
            c.version = git(d, "describe", "--tags", "--always")
            c.commit = git(d, "rev-parse", "HEAD")
        c.purl = github_purl(c.url, c.version, c.commit)
        return c

    def license_ref_texts(self, components: list[Component]) -> dict[str, str]:
        """{LicenseRef-x: text} for every LicenseRef used: from the licence file of a component,
        else LICENSES/<id>.txt in the repo, else in the REUSE project that declared it."""
        texts = {}
        for ref in sorted(set().union(*(license_refs(c.license) for c in components))):
            if ref in self.extracted:
                texts[ref] = self.extracted[ref]
                continue
            start = self.reuse_refs.get(ref)
            dirs = [self.root, *([start, *start.parents] if start else [])]
            path = next((p for d in dirs if (p := d / "LICENSES" / f"{ref}.txt").is_file()), None)
            if not path:
                where = "LICENSES/ of the repo"
                where += f" or of the REUSE project at {start}" if start else ""
                raise SbomError(f"{ref} is used but there is no {ref}.txt in {where}")
            texts[ref] = path.read_text(errors="replace").strip()
        return texts


class Idf:
    """Packages of an ESP-IDF build: each build component, and the submodules and sub-packages
    inside one. Their metadata comes from `esp-idf-sbom create`, whose bom-refs are
    "COMPONENT-<component>" and "SUBMODULE-" / "SUBPACKAGE-<component>-<path in component>"
    with each non-alphanumeric character replaced by "-"."""

    def __init__(self, project_description: Path, tool: list[str]):
        desc = json.loads(project_description.read_text())
        self.path = Path(desc["idf_path"]).resolve()
        info = desc["build_component_info"]
        dirs = {n: Path(i["dir"]).resolve() for n, i in info.items() if i.get("dir")}
        self.archives = {Path(i["file"]).resolve(): n for n, i in info.items() if i.get("file")}
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp, "out.json")
            cmd = [*tool, "create", "--format", "cyclonedx-json", "--file-tags"]
            cmd += ["--output-file", str(out), str(project_description)]
            r = subprocess.run(cmd, capture_output=True, text=True)
            if r.returncode:
                raise SbomError("{} failed:\n{}".format(" ".join(cmd), r.stderr.strip()))
            cdx = json.loads(out.read_text())
        # longest first, so a name that is a prefix of another doesn't claim its packages
        slugs = {slug(n): n for n in sorted(dirs, key=lambda n: len(slug(n)), reverse=True)}
        self.packages: dict[Path, dict[str, Any]] = {}  # package dir -> esp-idf-sbom component
        self.component_dirs: dict[str, Path] = {}  # component name -> dir
        for c in cdx["components"]:
            kind, _, ref = c["bom-ref"].partition("-")
            if kind not in ("COMPONENT", "SUBMODULE", "SUBPACKAGE"):
                continue  # the framework and toolchain entries are not linked code
            match = next(
                ((s, n) for s, n in slugs.items() if ref == s or ref.startswith(s + "-")), None
            )
            if match is None or match[1] == "main":  # main is the project's own code
                continue
            s, comp = match
            if kind == "COMPONENT":
                if ref != s:
                    continue
                self.component_dirs[comp] = d = dirs[comp]
            else:
                # None if it isn't checked out, then no linked file can be in it either
                d = self.find_dir(dirs[comp], ref[len(s) + 1 :])
            if d:
                self.packages[d] = c

    @staticmethod
    def find_dir(root: Path, want: str) -> Path | None:
        """Sub directory of root whose relative path slugs to want."""
        todo = [root]
        while todo:
            for d in sorted(p for p in todo.pop().iterdir() if p.is_dir()):
                s = slug(d.relative_to(root).as_posix())
                if s == want:
                    return d
                if want.startswith(s + "-"):
                    todo.append(d)
        return None

    def package_for(self, path: Path) -> Path | None:
        """Directory of the package owning a file of the IDF build, or None."""
        return next((d for d in path.parents if d in self.packages), None)

    def archive_package(self, archive: Path) -> Path | None:
        """Package of a component's own archive (members generated in the build dir)."""
        d = self.component_dirs.get(self.archives.get(archive, ""))
        return d if d in self.packages else None

    def component(self, d: Path, repo: Repo) -> Component:
        c = self.packages[d]
        exprs = []
        for e in c.get("licenses", []):
            lic = e.get("license", {})
            ident = e.get("expression") or lic.get("id") or lic.get("name")
            if not ident:
                raise SbomError(f"esp-idf package {c['name']}: licence without id or name: {e}")
            text = lic.get("text", "")
            if isinstance(text, dict):
                raw = text.get("content", "")
                text = base64.b64decode(raw).decode() if text.get("encoding") == "base64" else raw
            if text and ident.startswith("LicenseRef-"):
                repo.extracted.setdefault(ident, text)
            exprs.append(parse_spdx(ident, f"esp-idf package {c['name']}"))
        url = next((r["url"] for r in c.get("externalReferences", []) if r["type"] == "vcs"), None)
        return Component(
            c["name"],
            "esp-idf:" + c["bom-ref"],
            combine(exprs) or repo.license_from_file(d, "esp-idf-" + c["bom-ref"]),
            c.get("version"),
            url=url,
            purl=c.get("purl"),
            cpe=c.get("cpe"),
            supplier=c.get("supplier", {}).get("name"),
        )


def collect(
    repo: Repo,
    map_path: Path,
    src_roots: list[Path],
    frozen: list[str],
    variables: dict[str, str | None],
    idf: Idf | None = None,
    blobs: dict[Path, Path] | None = None,
) -> Collected:
    build_dir = map_path.resolve().parent
    by_dir: dict[Path, set[Path]] = {}  # component dir -> its linked source files
    libs: dict[str, Component] = {}
    idf_pkgs: set[Path] = set()
    unresolved: set[str] = set()
    # Frozen modules aren't objects in the map, they come from the manifests the build froze.
    # c_module() dirs are built as <build>/<module dir name>/<file>.o and CMake builds a
    # submodule in <build>/<submodule name>/, so their parents are source roots for the objects.
    frozen_srcs: list[tuple[Path, str | None, str]] = []  # (file, declared licence, manifest)
    src_roots = [*src_roots, *(sm.parent for sm in repo.submodules)]
    for manifest in frozen:
        files, cmodules = manifest_inputs(repo.root, manifest, variables)
        frozen_srcs += [(p, lic, manifest) for p, lic in files]
        src_roots += [d.parent for d in cmodules]
        for d in cmodules:
            repo.add_root(d)
    archives = CMakeArchives([build_dir, *src_roots])

    def owner(src: Path, archive: Path | None) -> tuple[str, Path] | None:
        """("idf", package) or ("repo", component dir) for a source file, or None."""
        pkg = idf and (idf.package_for(src) or (archive and idf.archive_package(archive)))
        if pkg:
            return ("idf", pkg)
        d = repo.component_dir(src)
        return ("repo", d) if d else None

    compdb = compile_commands(build_dir)
    port_dir = variables.get("PORT_DIR")
    for obj in sorted(linked_objects(map_path)):
        name = drop_object_suffix(PurePosixPath(obj.replace("\\", "/")).name)
        if name in ("frozen_content", "frozen_content.c"):
            if not frozen:
                raise SbomError("the image has frozen modules, give the manifest(s) with --frozen")
            continue  # the frozen files themselves are licensed from the manifest
        archive = None
        archive_name, member = split_member(obj)
        if member is not None:
            archive = archives.locate(archive_name)
            built = [str(p) for p in archives.objects(archive, member)] if archive else []
        else:
            built = [obj]
        srcs = set()
        for o in built:
            if compdb is not None:
                s = compdb.get(Path(os.path.normpath(build_dir / o)))
            else:
                s = dep_source(o, port_dir)
            if s:
                srcs.add(s)
        if not srcs:  # data made with objcopy, or a prebuilt archive kept in the repo
            data = (blobs or {}).get(Path(obj).resolve())
            prebuilt = archive and repo.contains(archive) and build_dir not in archive.parents
            data = data or (archive if prebuilt else None)
            srcs = {data} if data else set()
        owners = {owner(s, archive) for s in srcs}
        if not srcs and idf and archive:  # a prebuilt library of an IDF package
            pkg = idf.package_for(archive) or idf.archive_package(archive)
            owners = {("idf", pkg)} if pkg else set()
        if len(owners) > 1 or None in owners:
            unresolved.add(obj if len(owners) > 1 else archive_name)
        elif owners:
            kind, key = owners.pop()
            if kind == "idf":
                idf_pkgs.add(key)
            else:
                by_dir.setdefault(key, set()).update(srcs)
        elif lib := toolchain_lib(obj):
            libs[lib.name] = lib
        else:
            unresolved.add(archive_name)
    declared: dict[Path, str] = {}
    for path, lic, manifest in frozen_srcs:
        if d := repo.component_dir(path):
            by_dir.setdefault(d, set()).add(path)
            if lic:
                declared[path] = parse_spdx(lic, f"{manifest}: licence of {path}")
        else:
            unresolved.add(str(path))
    components, missing = [], []
    for d, sources in by_dir.items():
        c = repo.component(d)
        per_file = {s: repo.file_license(s, d, declared.get(s)) for s in sources}
        missing += sorted(
            f"{repo.label(s)} in component {repo.label(d)}"
            for s, lic in per_file.items()
            if not lic
        )
        c.license = combine(per_file.values())
        components.append(c)
    if idf:
        for d in sorted(idf_pkgs):
            c = idf.component(d, repo)
            if not c.license:
                missing.append(str(d.relative_to(idf.path)) if idf.path in d.parents else str(d))
            components.append(c)
    components += libs.values()
    return Collected(sorted(components, key=lambda c: c.name.lower()), sorted(unresolved), missing)


def write_spdx(path: Path, name: str, comps: list[Component], ref_texts: dict[str, str]) -> None:
    from spdx_tools.spdx import model as spdx
    from spdx_tools.spdx.parser.jsonlikedict.license_expression_parser import (
        LicenseExpressionParser,
    )
    from spdx_tools.spdx.validation.document_validator import validate_full_spdx_document
    from spdx_tools.spdx.writer.json.json_writer import write_document_to_file

    parse = LicenseExpressionParser().parse_license_expression
    no_assertion = spdx.SpdxNoAssertion()
    category = spdx.ExternalPackageRefCategory

    def package(c: Component) -> spdx.Package:
        refs = [
            spdx.ExternalPackageRef(cat, kind, value)
            for cat, kind, value in (
                (category.PACKAGE_MANAGER, "purl", c.purl),
                (category.SECURITY, "cpe23Type", c.cpe),
            )
            if value
        ]
        return spdx.Package(
            name=c.name,
            spdx_id="SPDXRef-" + slug(c.ref),
            version=c.version,
            supplier=spdx.Actor(spdx.ActorType.ORGANIZATION, c.supplier) if c.supplier else None,
            download_location=c.url or no_assertion,
            files_analyzed=False,
            license_declared=parse(c.license) if c.license else no_assertion,
            license_concluded=no_assertion,
            copyright_text=no_assertion,
            external_references=refs,
        )

    info = spdx.CreationInfo(
        spdx_version="SPDX-2.3",
        spdx_id="SPDXRef-DOCUMENT",
        name=name,
        document_namespace=f"https://micropython.org/spdx/{name}-{uuid.uuid4()}",  # never fetched
        creators=[spdx.Actor(spdx.ActorType.TOOL, "micropython tools/sbom.py")],
        created=datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0),
    )
    pkgs = [package(c) for c in [Component(name, "firmware"), *comps]]
    relationship, kind = spdx.Relationship, spdx.RelationshipType
    rels = [relationship("SPDXRef-DOCUMENT", kind.DESCRIBES, "SPDXRef-firmware")]
    rels += [relationship("SPDXRef-firmware", kind.CONTAINS, p.spdx_id) for p in pkgs[1:]]
    infos = [
        spdx.ExtractedLicensingInfo(license_id=r, extracted_text=t, license_name=r[11:])
        for r, t in ref_texts.items()
    ]
    doc = spdx.Document(info, packages=pkgs, relationships=rels, extracted_licensing_info=infos)
    if messages := validate_full_spdx_document(doc):
        raise SbomError(
            "invalid SPDX document:\n" + "\n".join(m.validation_message for m in messages)
        )
    write_document_to_file(doc, str(path), validate=False)


def write_cyclonedx(
    path: Path, name: str, comps: list[Component], ref_texts: dict[str, str]
) -> None:
    from cyclonedx.model.bom import Bom
    from cyclonedx.model.component import Component as CdxComponent, ComponentType
    from cyclonedx.model import AttachedText, ExternalReference, ExternalReferenceType, XsUri
    from cyclonedx.model.contact import OrganizationalEntity
    from cyclonedx.model.license import DisjunctiveLicense, LicenseExpression
    from cyclonedx.output import make_outputter
    from cyclonedx.schema import OutputFormat, SchemaVersion
    from packageurl import PackageURL

    def licenses(c: Component) -> list[Any]:
        """Licence objects (which carry a LicenseRef's text) for a single licence or an AND of
        them, else an expression, which can't."""
        if not c.license:
            return []
        terms = c.license.split(" AND ")
        if any(set(t) & set(" ()") for t in terms):
            return [LicenseExpression(c.license)]
        return [
            DisjunctiveLicense(name=t, text=AttachedText(content=ref_texts[t]))
            if t.startswith("LicenseRef-")
            else DisjunctiveLicense(id=t)
            for t in terms
        ]

    bom = Bom()
    bom.metadata.component = app = CdxComponent(
        name=name, type=ComponentType.FIRMWARE, bom_ref="firmware"
    )
    deps = [
        CdxComponent(
            name=c.name,
            type=ComponentType.LIBRARY,
            version=c.version,
            bom_ref=c.ref,
            purl=PackageURL.from_string(c.purl) if c.purl else None,
            cpe=c.cpe,
            supplier=OrganizationalEntity(name=c.supplier) if c.supplier else None,
            licenses=licenses(c),
            external_references=[
                ExternalReference(type=ExternalReferenceType.VCS, url=XsUri(c.url))
            ]
            if c.url
            else [],
        )
        for c in comps
    ]
    bom.components.update(deps)
    bom.register_dependency(app, deps)
    outputter = make_outputter(bom, OutputFormat.JSON, SchemaVersion.V1_6)
    path.write_text(outputter.output_as_string(indent=2) + "\n")


def name_value(arg: str) -> tuple[str, str]:
    name, sep, value = arg.partition("=")
    if not sep or not name:
        raise argparse.ArgumentTypeError(f"expected NAME=VALUE, not {arg!r}")
    return name, value


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Generate an SPDX and/or CycloneDX SBOM from a linker map file."
    )
    add = ap.add_argument
    add("map", type=Path, help="GNU ld or LLD (clang) map file of the linked image")
    add("--spdx", type=Path, metavar="FILE", help="write an SPDX 2.3 JSON SBOM")
    add("--cyclonedx", type=Path, metavar="FILE", help="write a CycloneDX 1.6 JSON SBOM")
    add("--repo", type=Path, default=Path(__file__).parent.parent, help="repo root")
    add("--name", help="firmware name (default: map file name)")
    add("--src-root", type=Path, action="append", default=[], help="extra dir objects are under")
    add("--frozen", action="append", default=[], metavar="MANIFEST", help="manifest.py built in")
    add(
        "-v",
        "--var",
        action="append",
        default=[],
        type=name_value,
        metavar="NAME=VALUE",
        help="manifest path variable such as BOARD_DIR, as for makemanifest.py",
    )
    add("--idf", type=Path, metavar="PROJECT_DESCRIPTION", help="ESP-IDF project_description.json")
    add(
        "--blob",
        action="append",
        default=[],
        type=name_value,
        metavar="OBJECT=FILE",
        help="object made from a data FILE (objcopy -I binary)",
    )
    try:
        run(ap.parse_args())
    except SbomError as e:
        sys.exit("\n".join(f"sbom: error: {line}" for line in str(e).splitlines()))


def run(args: argparse.Namespace) -> None:
    writers = [(write_spdx, args.spdx), (write_cyclonedx, args.cyclonedx)]
    writers = [(write, out) for write, out in writers if out]
    if not writers:
        raise SbomError("nothing to do, give --spdx and/or --cyclonedx")
    for _, out in writers:
        out.unlink(missing_ok=True)  # never leave an SBOM from an earlier run behind
    repo = Repo(args.repo)
    # object paths in the map are relative to the port dir
    map_dir = args.map.resolve().parent
    extra_roots = [r.resolve() for r in args.src_root]
    for r in extra_roots:
        repo.add_root(r)
    src_roots = [repo.root, Path.cwd(), map_dir.parent, map_dir, *extra_roots]
    variables: dict[str, str | None] = {  # as passed to makemanifest.py
        "MPY_DIR": str(repo.root),
        "MPY_LIB_DIR": str(repo.root / "lib" / "micropython-lib"),
        "PORT_DIR": str(Path.cwd()),
        "BOARD_DIR": None,
    }
    for key, value in args.var:
        path = Path(value)
        variables[key] = str(path.resolve()) if path.exists() else value
        if path.is_dir():
            repo.add_root(path.resolve())
    idf = Idf(args.idf, [sys.executable, "-m", "esp_idf_sbom"]) if args.idf else None
    blobs = {Path(obj).resolve(): Path(src).resolve() for obj, src in args.blob}
    result = collect(repo, args.map, src_roots, args.frozen, variables, idf, blobs)
    errors = [
        f"no licence for {m} (no SPDX-License-Identifier tag, entry in the LICENSE table or "
        "licence file)"
        for m in result.missing
    ]
    errors += [
        f"{u} is linked but its source is not found or is ambiguous (use --src-root"
        f"{', or build without LTO' if '.ltrans' in u else ''})"
        for u in result.unresolved
    ]
    if errors:
        raise SbomError("\n".join(errors))
    ref_texts = repo.license_ref_texts(result.components)
    name = args.name or args.map.stem
    for write, out in writers:
        write(out, name, result.components, ref_texts)
    print(f"sbom: {len(result.components)} components", file=sys.stderr)


if __name__ == "__main__":
    main()
