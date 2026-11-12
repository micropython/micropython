#!/usr/bin/env -S uv run --script
# /// script
# dependencies = [
#     "spdx-tools>=0.8.2",
#     "cyclonedx-python-lib>=11.12",
#     "license-expression>=30.4",
#     "tomli>=2; python_version<'3.11'",
# ]
# ///
#
# Run with `uv run tests/tools/test_sbom.py`.

import argparse
import base64
import importlib.util
import json
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

TOOLS = Path(__file__).resolve().parents[2] / "tools"
spec = importlib.util.spec_from_file_location("sbom", TOOLS / "sbom.py")
sbom = importlib.util.module_from_spec(spec)
sys.modules["sbom"] = sbom  # dataclasses looks the module up
spec.loader.exec_module(sbom)

GNU_MAP = """\
Discarded input sections

 .text          0x0000000000000000        0x4 build/unused.o

Linker script and memory map

.text           0x0000000000001000     0x2000
 .text          0x0000000000001000      0x100 build/py/vm.o
 .text.foo      0x0000000000001100        0x0 build/py/empty.o
 .text          0x0000000000001100       0x40 /usr/lib/gcc/x86_64-linux-gnu/13/libgcc.a(_udivdi3.o)
 .text.a_function_with_a_long_name
                0x0000000000001140       0x10 build/py/wrapped.o
 .debug_info    0x0000000000000000       0x20 build/py/debug_only.o
 .debug_str
                0x0000000000000000       0x20 build/py/debug_only.o
"""

LLD_MAP = """\
     VMA              LMA     Size Align Out     In      Symbol
    1000             1000     2000    16 .text
    1000             1000      100     1         build/py/vm.o:(.text)
    1100             1100        0     1         build/py/empty.o:(.text)
    1100             1100       40     1         /usr/lib/libclang_rt.builtins.a(udivdi3.o):(.text)
    1140             1140       10     1         build/py/wrapped.o:(.text.a_function)
       0                0       20     1         build/py/debug_only.o:(.debug_info)
"""


class TestMapParsing(unittest.TestCase):
    def check(self, text: str, member: str) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp, "fw.map")
            path.write_text(text)
            self.assertEqual(
                sbom.linked_objects(path), {"build/py/vm.o", "build/py/wrapped.o", member}
            )

    def test_gnu_ld(self) -> None:
        self.check(GNU_MAP, "/usr/lib/gcc/x86_64-linux-gnu/13/libgcc.a(_udivdi3.o)")

    def test_lld(self) -> None:
        self.check(LLD_MAP, "/usr/lib/libclang_rt.builtins.a(udivdi3.o)")


class TestCommandLine(unittest.TestCase):
    def test_name_value(self) -> None:
        self.assertEqual(sbom.name_value("A=b=c"), ("A", "b=c"))
        for bad in ("A", "=x"):
            with self.assertRaises(argparse.ArgumentTypeError):
                sbom.name_value(bad)

    def test_failed_run_leaves_no_old_output(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            old = Path(d) / "old.json"
            old.write_text("{}")
            (Path(d) / "x.map").write_text("")  # no objects in it
            args = argparse.Namespace(
                spdx=old,
                cyclonedx=None,
                repo=TOOLS.parent,
                map=Path(d) / "x.map",
                src_root=[],
                var=[],
                idf=None,
                blob=[],
                frozen=[],
            )
            with self.assertRaises(sbom.SbomError):
                sbom.run(args)
            self.assertFalse(old.exists())


class TestToolchain(unittest.TestCase):
    def lib(self, obj: str):
        c = sbom.toolchain_lib(obj)
        return c and (c.name, c.version, c.license)

    def test_gcc_and_clang_versions(self) -> None:
        self.assertEqual(
            self.lib("/usr/lib/gcc/x86_64-linux-gnu/13/libgcc.a(_udivdi3.o)"),
            ("libgcc", "13", "GPL-3.0-or-later WITH GCC-exception-3.1"),
        )
        self.assertEqual(
            self.lib(
                "/usr/lib/llvm-18/lib/clang/18/lib/linux/libclang_rt.builtins-x86_64.a(udivdi3.o)"
            ),
            ("compiler-rt", "18", "Apache-2.0 WITH LLVM-exception"),
        )

    def test_startup_objects(self) -> None:
        gcc = "/opt/arm-gnu/lib/gcc/arm-none-eabi/12.2.1/thumb/v8-m.main+fp/hard/"
        newlib = "/opt/arm-gnu/arm-none-eabi/lib/thumb/v8-m.main+fp/hard/"
        self.assertEqual(self.lib(gcc + "crti.o")[0], "libgcc")
        self.assertEqual(self.lib(gcc + "crtn.o")[0], "libgcc")
        self.assertEqual(self.lib(newlib + "crt0.o")[0], "newlib")
        self.assertEqual(self.lib("/usr/lib/x86_64-linux-gnu/crti.o")[0], "glibc")

    def test_clang_runtimes_is_picolibc(self) -> None:
        obj = "/opt/ATfE-21.1.1/lib/clang-runtimes/arm-none-eabi/armv7m/lib/libc.a(isinf.c.o)"
        self.assertEqual(self.lib(obj)[0], "picolibc")

    def test_unknown_archive(self) -> None:
        self.assertIsNone(self.lib("build/lib/foo.a(bar.o)"))

    def test_unnormalised_path(self) -> None:
        obj = "/usr/lib/gcc/x86_64-linux-gnu/13/../../../x86_64-linux-gnu/crti.o"
        self.assertEqual(self.lib(obj)[0], "glibc")

    def test_libc_names_only_match_the_libraries(self) -> None:
        self.assertEqual(self.lib("/usr/lib/musl/lib/libc.a(x.o)")[0], "musl")
        self.assertIsNone(self.lib("build/musl_helpers/foo.a(x.o)"))
        self.assertIsNone(self.lib("build/picolibc_glue/foo.a(x.o)"))


class TestSpdx(unittest.TestCase):
    def test_normalise_and_licenseref(self) -> None:
        self.assertEqual(sbom.parse_spdx("BSD-3-clause", "t"), "BSD-3-Clause")
        self.assertEqual(sbom.parse_spdx("MIT AND LicenseRef-x", "t"), "MIT AND LicenseRef-x")

    def test_invalid(self) -> None:
        with self.assertRaises(sbom.SbomError):
            sbom.parse_spdx("Not-A-Licence", "t")

    def test_combine(self) -> None:
        self.assertEqual(
            set(sbom.combine(["MIT", None, "MIT", "ISC"]).split(" AND ")), {"MIT", "ISC"}
        )
        self.assertIsNone(sbom.combine([None]))

    def test_empty_and_incomplete_are_errors(self) -> None:
        for text in ("", "  ", "MIT OR", "MIT,BSD-3-Clause"):
            with self.assertRaises(sbom.SbomError):
                sbom.parse_spdx(text, "t")

    def test_tag_on_the_next_line_is_not_a_licence(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "x.c"
            path.write_text("/* SPDX-License-Identifier:\n * MIT */\n")
            self.assertIsNone(sbom.file_tag(path))


class RepoTestCase(unittest.TestCase):
    """A tiny repo: root MIT, lib/a has its own LICENSE file and a table entry, lib/b is only in
    the table and lib/e only has a LICENSE file."""

    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name).resolve()
        self.write(
            "LICENSE",
            'text\n\n[licenses]\n"." = "MIT"\n"lib/a" = "BSD-3-Clause"\n"lib/b" = "Apache-2.0"\n'
            '"lib/b/x.*" = "ISC"\n"lib/b/x.c" = "Zlib"\n',
        )
        self.write("py/vm.c", "// SPDX-License-Identifier: MIT\n")
        self.write("py/untagged.c", "int x;\n")
        self.write("lib/a/LICENSE", "whatever\n")
        self.write("lib/a/a.c", "int a;\n")
        self.write("lib/a/tagged.c", "/* SPDX-License-Identifier: BSD-2-Clause */\n")
        self.write("lib/b/b.c", "int b;\n")
        self.write("lib/b/x.c", "int x;\n")
        self.write("lib/b/x.h", "int x;\n")
        self.write("lib/c/c.c", "int c;\n")  # no component licence file and no table entry
        self.write("lib/e/LICENSE", "The e licence\n")
        self.write("lib/e/License.RP", "Extra terms\n")
        self.write("lib/e/e.c", "int e;\n")
        subprocess.run(["git", "-C", str(self.root), "init", "-q"], check=True)
        self.repo = sbom.Repo(self.root)

    def tearDown(self) -> None:
        self.tmp.cleanup()

    def write(self, path: str, text: str) -> Path:
        full = self.root / path
        full.parent.mkdir(parents=True, exist_ok=True)
        full.write_text(text)
        return full

    def lic(self, path: str) -> str | None:
        full = self.root / path
        return self.repo.file_license(full, self.repo.component_dir(full))

    def write_map(self, path: str, *lines: str) -> Path:
        return self.write(path, "Linker script and memory map\n" + "".join(lines))

    def dep(self, obj: str, src: str) -> str:
        """Make style dependency file for an object; returns the object's absolute path."""
        path = self.root / obj
        path.parent.mkdir(parents=True, exist_ok=True)
        path.with_suffix(".P").write_text(f"{path}: \\\n {self.root / src}\n")
        return str(path)


class TestLicenses(RepoTestCase):
    def test_tag_beats_table(self) -> None:
        self.assertEqual(self.lic("lib/a/tagged.c"), "BSD-2-Clause")

    def test_directory_entry(self) -> None:
        self.assertEqual(self.lic("lib/a/a.c"), "BSD-3-Clause")
        self.assertEqual(self.lic("lib/b/b.c"), "Apache-2.0")

    def test_exact_beats_glob_beats_directory(self) -> None:
        self.assertEqual(self.lic("lib/b/x.c"), "Zlib")
        self.assertEqual(self.lic("lib/b/x.h"), "ISC")

    def test_reuse_toml_beats_table(self) -> None:
        self.write(
            "lib/a/sub/REUSE.toml",
            'version = 1\n[[annotations]]\npath = "**/*.bin"\nSPDX-License-Identifier = "Zlib"\n',
        )
        self.write("lib/a/sub/deep/x.bin", "")
        self.write("lib/a/sub/y.c", "")
        self.assertEqual(self.lic("lib/a/sub/deep/x.bin"), "Zlib")
        self.assertEqual(self.lic("lib/a/sub/y.c"), "BSD-3-Clause")

    def test_component_licence_file_is_the_last_resort(self) -> None:
        ref = self.lic("lib/e/e.c")
        self.assertEqual(ref, "LicenseRef-lib-e")
        text = self.repo.license_ref_texts([sbom.Component("e", "lib/e", ref)])[ref]
        self.assertIn("The e licence", text)
        self.assertIn("Extra terms", text)

    def test_common_licence_texts_are_recognised(self) -> None:
        mit = (
            "Permission is hereby granted, free of charge, to any person obtaining a copy of this"
        )
        bsd = "Redistribution and use in source and binary forms, with or without modification"
        src = "Redistributions of source code must retain the above notice."
        binary = "Redistributions in binary form must reproduce the above notice."
        for text, want in [
            (f"MIT License\n\nCopyright (c) 2020 A\n\n{mit}\nsoftware", "MIT"),
            ("Permission to use, copy, modify, and/or distribute this software for any", "ISC"),
            (
                f"{bsd}.\n1. {src}\n2. {binary}\n3. Neither the name of X may endorse",
                "BSD-3-Clause",
            ),
            (f"{bsd}.\n1. {src}\n2. {binary}", "BSD-2-Clause"),
            (f"{bsd}.\n1. Use only with an RP2040.\n2. {src}\n3. {binary}", None),
            (f"{bsd}.\n1. {src}\n2. {binary}\n3. Use is for personal benefit only", None),
            (f"{bsd}.\n1. {src}\n2. {binary}\nAll advertising materials mention", None),
            (f"{mit}\n" + "other licence terms " * 200, None),
            (
                "Apache License\nVersion 2.0, January 2004\nTERMS AND CONDITIONS FOR USE, REPRODUCTION",
                "Apache-2.0",
            ),
            ("Some custom terms", None),
        ]:
            self.assertEqual(sbom.identify_license(text), want, text[:40])

    def test_root_entry_only_covers_the_repo_itself(self) -> None:
        self.assertEqual(self.lic("py/untagged.c"), "MIT")
        # lib/c has no LICENSE file so it belongs to the repo root component
        self.assertEqual(self.lic("lib/c/c.c"), "MIT")
        # a submodule without licence information never falls back to the root entry
        self.write("lib/d/d.c", "int d;\n")
        self.repo.submodules[self.root / "lib/d"] = None
        self.assertIsNone(self.lic("lib/d/d.c"))

    def test_bad_table_value(self) -> None:
        self.write("LICENSE", 'text\n[licenses]\n"." = "Nope-1.0"\n')
        with self.assertRaises(sbom.SbomError):
            sbom.Repo(self.root)

    def test_bad_toml_is_an_error(self) -> None:
        self.write("LICENSE", 'text\n[licenses]\n"." = "MIT"\n"lib/a" = \n')
        with self.assertRaisesRegex(sbom.SbomError, "not valid TOML"):
            sbom.Repo(self.root)


class TestCollect(RepoTestCase):
    def test_components_and_missing(self) -> None:
        mapfile = self.write_map(
            "out.map",
            f" .text 0x1000 0x10 {self.dep('build/py/vm.o', 'py/vm.c')}\n",
            f" .text 0x1010 0x10 {self.dep('build/lib/a/a.o', 'lib/a/a.c')}\n",
            f" .text 0x1020 0x10 {self.dep('build/lib/a/tagged.o', 'lib/a/tagged.c')}\n",
            " .text 0x1030 0x10 /usr/lib/gcc/x86_64-linux-gnu/13/libgcc.a(x.o)\n",
            " .text 0x1040 0x10 nowhere/else.o\n",
        )
        result = sbom.collect(self.repo, mapfile, [self.root], [], {})
        self.assertEqual(
            {c.name: c.license for c in result.components},
            {
                "a": "BSD-2-Clause AND BSD-3-Clause",
                "libgcc": "GPL-3.0-or-later WITH GCC-exception-3.1",
                "micropython": "MIT",
            },
        )
        self.assertEqual(result.unresolved, ["nowhere/else.o"])
        self.assertEqual(result.missing, [])

    def test_missing_licence_is_reported(self) -> None:
        self.write("lib/d/d.c", "int d;\n")
        self.repo.submodules[self.root / "lib/d"] = None
        obj = self.dep("build/lib/d/d.o", "lib/d/d.c")
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {obj}\n")
        result = sbom.collect(self.repo, mapfile, [self.root], [], {})
        self.assertEqual(result.missing, ["lib/d/d.c in component lib/d"])

    def test_blobs_and_prebuilt_archives_are_sources(self) -> None:
        self.write("lib/a/fw.bin", "data")
        self.write("lib/a/prebuilt.a", "!<arch>\n")
        blob = self.root / "out" / "fw.o"
        mapfile = self.write_map(
            "out/out.map",
            f" .rodata 0x1000 0x10 {blob}\n",
            " .text 0x1010 0x10 {}(x.o)\n".format(self.root / "lib/a/prebuilt.a"),
        )
        args = (self.repo, mapfile, [self.root], [], {})
        self.assertEqual(sbom.collect(*args).unresolved, [str(blob)])
        result = sbom.collect(*args, blobs={blob: self.root / "lib/a/fw.bin"})
        self.assertEqual(result.unresolved, [])
        self.assertEqual([c.name for c in result.components], ["a"])

    def test_c_module_objects_come_from_the_manifest(self) -> None:
        # a C module is traced through its .P file like any other object
        (self.root / "tools").mkdir()
        shutil.copy(TOOLS / "manifestfile.py", self.root / "tools" / "manifestfile.py")
        self.write("mods/foo/micropython.mk", "")
        self.write("mods/foo/foo.c", "/* SPDX-License-Identifier: BSD-2-Clause */\n")
        manifest = self.write("manifest.py", 'c_module("$(TOP)/mods/foo")\n')
        obj = self.dep("build/foo/foo.o", "mods/foo/foo.c")
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {obj}\n")
        result = sbom.collect(self.repo, mapfile, [], [str(manifest)], {"TOP": str(self.root)})
        self.assertEqual(result.unresolved, [])
        self.assertEqual(
            {c.name: c.license for c in result.components}, {"micropython": "BSD-2-Clause"}
        )

    def test_frozen_content_needs_the_manifest(self) -> None:
        mapfile = self.write_map("out.map", " .text 0x1000 0x10 build/frozen_content.o\n")
        with self.assertRaisesRegex(sbom.SbomError, "--frozen"):
            sbom.collect(self.repo, mapfile, [self.root], [], {})


class TestOutsideTheRepo(RepoTestCase):
    def setUp(self) -> None:
        super().setUp()
        ext = tempfile.TemporaryDirectory()
        self.addCleanup(ext.cleanup)
        self.ext = Path(ext.name).resolve()
        (self.ext / "mod").mkdir()
        (self.ext / "mod" / "mod.c").write_text("int m;\n")
        self.obj = self.dep("build/mod/mod.o", "x")
        (self.root / "build/mod/mod.P").write_text(f"{self.obj}: {self.ext}/mod/mod.c\n")
        self.mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {self.obj}\n")

    def collect(self) -> sbom.Collected:
        return sbom.collect(self.repo, self.mapfile, [self.ext / "mod"], [], {})

    def test_src_root_is_a_component(self) -> None:
        (self.ext / "mod" / "LICENSE").write_text("Module licence\n")
        self.repo.add_root(self.ext / "mod")
        result = self.collect()
        self.assertEqual(result.unresolved, [])
        self.assertEqual(
            [(c.name, c.license) for c in result.components],
            [("mod", "LicenseRef-mod")],
        )

    def test_missing_licence_is_reported(self) -> None:
        self.assertEqual(self.collect().unresolved, [self.obj])  # no root given
        self.repo.add_root(self.ext / "mod")
        self.assertEqual(
            self.collect().missing,
            [f"{self.ext / 'mod' / 'mod.c'} in component {self.ext / 'mod'}"],
        )

    def test_tag_is_enough(self) -> None:
        self.repo.add_root(self.ext / "mod")
        (self.ext / "mod" / "mod.c").write_text("// SPDX-License-Identifier: MIT\n")
        result = self.collect()
        self.assertEqual([(c.name, c.license) for c in result.components], [("mod", "MIT")])


class TestDependencyFile(RepoTestCase):
    def test_out_of_tree_objects_use_the_first_prerequisite(self) -> None:
        build = Path(tempfile.mkdtemp()).resolve()
        self.addCleanup(shutil.rmtree, build)
        self.write("lib/b/dup.c", "// SPDX-License-Identifier: MIT\n")
        self.write(
            "lib/a/dup.c", "// SPDX-License-Identifier: ISC\n"
        )  # same name, so ambiguous alone
        (build / "dup.o").write_text("")
        (build / "dup.P").write_text(f"{build}/dup.o: dup.c \\\n  {self.root}/py/vm.c \\\n")
        (build / "vm.o").write_text("")
        (build / "vm.P").write_text(f"{build}/vm.o: \\\n {self.root}/lib/a/a.c\n")
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {build}/dup.o\n")
        variables = {"PORT_DIR": str(self.root / "lib/b")}
        result = sbom.collect(self.repo, mapfile, [self.root], [], variables)
        self.assertEqual(result.unresolved, [])
        self.assertEqual([c.license for c in result.components], ["MIT"])
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {build}/vm.o\n")
        result = sbom.collect(self.repo, mapfile, [self.root], [], variables)
        self.assertEqual([c.license for c in result.components], ["BSD-3-Clause"])

    def test_object_without_a_dep_file_is_unresolved_by_name(self) -> None:
        obj = str(self.root / "build/lib/a/a.o")  # lib/a/a.c exists but nothing names it
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {obj}\n")
        result = sbom.collect(self.repo, mapfile, [self.root], [], {})
        self.assertEqual((result.components, result.unresolved), ([], [obj]))

    def test_relative_prerequisites_are_relative_to_port_dir(self) -> None:
        obj = self.root / "build/vm.o"
        obj.parent.mkdir()
        obj.with_suffix(".P").write_text("build/vm.o: ../py/vm.c\n")
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {obj}\n")
        port = {"PORT_DIR": str(self.root / "lib")}
        result = sbom.collect(self.repo, mapfile, [self.root], [], port)
        self.assertEqual([c.name for c in result.components], ["micropython"])


class BuildTestCase(RepoTestCase):
    """Adds a CMake build dir with a compile_commands.json."""

    def setUp(self) -> None:
        super().setUp()
        self.build = self.root / "build"
        self.entries: list[dict[str, str]] = []

    def archive(self, archive: str, sources: list[Path], source_dir: Path | None = None) -> None:
        """Archive whose members CMake built from the given sources. Sources generated in the
        build dir get a cmake_install.cmake naming source_dir."""
        self.write("build/" + archive, "")
        parent = Path(archive).parent
        if source_dir:
            self.write(
                f"build/{parent}/cmake_install.cmake",
                f"# Install script for directory: {source_dir}\n",
            )
        for src in sources:
            output = parent / "CMakeFiles/t.dir" / (str(src).lstrip("/") + ".obj")
            self.write(f"build/{output}", "")
            self.entries.append(
                {"directory": str(self.build / parent), "file": str(src), "output": str(output)}
            )

    def collect(self, *objs: str, idf: sbom.Idf | None = None) -> sbom.Collected:
        self.write("build/compile_commands.json", json.dumps(self.entries))
        mapfile = self.write_map("build/out.map", *(f" .text 0x1000 0x10 {o}\n" for o in objs))
        return sbom.collect(self.repo, mapfile, [self.root], [], {}, idf=idf)


class TestCompileCommands(BuildTestCase):
    def test_plain_object_member_and_generated_source(self) -> None:
        # a plain object, with a source relative to the entry's directory
        self.entries.append(
            {
                "directory": str(self.build / "lib/a"),
                "file": "../../../lib/a/a.c",
                "output": "lib/a/CMakeFiles/fw.dir/a.c.o",
            }
        )
        # an archive member
        self.archive("x/libx.a", [self.root / "py/vm.c"])
        # a source generated in the build dir, named by the directory's cmake_install.cmake
        self.write(
            "build/boot/cmake_install.cmake",
            f"# Install script for directory: {self.root}/lib/b\n",
        )
        self.entries.append(
            {
                "directory": str(self.build / "boot"),
                "file": str(self.build / "boot/gen_padded.S"),
                "output": "boot/CMakeFiles/boot.dir/gen_padded.S.o",
            }
        )
        self.entries.append({"directory": str(self.build), "file": "ignored.c"})  # no output
        result = self.collect(
            "lib/a/CMakeFiles/fw.dir/a.c.o",
            "x/libx.a(vm.c.obj)",
            "boot/CMakeFiles/boot.dir/gen_padded.S.o",
        )
        self.assertEqual(result.unresolved, [])
        lics = {c.name: set(c.license.split(" AND ")) for c in result.components}
        self.assertEqual(lics, {"a": {"BSD-3-Clause"}, "micropython": {"MIT", "Apache-2.0"}})

    def test_dep_files_are_not_used_when_there_is_a_compile_commands(self) -> None:
        obj = self.dep("build/lib/a/a.o", "lib/a/a.c")
        self.assertEqual(self.collect(obj).unresolved, [obj])


class TestCMakeArchives(BuildTestCase):
    def test_member_is_traced_through_cmakefiles(self) -> None:
        self.archive("x/libx.a", [self.root / "lib/a/a.c"])
        result = self.collect("x/libx.a(a.c.obj)")
        self.assertEqual(result.unresolved, [])
        self.assertEqual({c.name: c.license for c in result.components}, {"a": "BSD-3-Clause"})

    def test_member_from_a_sibling_object_library(self) -> None:
        self.write("build/x/core/libx.a", "")
        self.archive("x/sub/libother.a", [self.root / "lib/a/a.c"])
        result = self.collect("x/core/libx.a(a.c.obj)")
        self.assertEqual(result.unresolved, [])
        self.assertEqual({c.name for c in result.components}, {"a"})

    def test_member_name_shared_by_two_components_is_an_error(self) -> None:
        self.write("py/a.c", "// SPDX-License-Identifier: MIT\n")
        self.archive("x/libx.a", [self.root / "lib/a/a.c", self.root / "py/a.c"])
        result = self.collect("x/libx.a(a.c.obj)")
        self.assertEqual((result.components, result.unresolved), ([], ["x/libx.a(a.c.obj)"]))


FAKE_ESP_IDF_SBOM = """\
import shutil, sys
assert sys.argv[1:3] == ["create", "--format"] and "--file-tags" in sys.argv
shutil.copy({!r}, sys.argv[sys.argv.index("--output-file") + 1])
"""


class TestIdf(BuildTestCase):
    """IDF tree in the test repo, with a fake esp-idf-sbom."""

    def setUp(self) -> None:
        super().setUp()
        idf = self.root / "idf" / "components"
        comps = {
            "freertos": (idf / "freertos", "esp-idf/freertos/libfreertos.a"),
            "esp_wifi": (idf / "esp_wifi", "esp-idf/esp_wifi/libesp_wifi.a"),
            "main": (self.root / "main", "esp-idf/main/libmain.a"),
        }
        info = {n: {"dir": str(d), "file": str(self.build / f)} for n, (d, f) in comps.items()}
        desc = {
            "idf_path": str(idf.parent),
            "build_dir": str(self.build),
            "build_component_info": info,
        }
        self.write("build/project_description.json", json.dumps(desc))
        for src in ("freertos/port.c", "freertos/FreeRTOS-Kernel/tasks.c"):
            self.write("idf/components/" + src, "")
        self.write("idf/components/esp_wifi/lib/esp32/libnet80211.a", "")
        self.write("idf/components/esp_wifi/lib/LICENSE", "Wifi blob licence\n")
        self.write("build/gen.c", "")
        self.archive(
            "esp-idf/freertos/libfreertos.a",
            [idf / "freertos/port.c", idf / "freertos/FreeRTOS-Kernel/tasks.c"],
        )
        self.archive("esp-idf/freertos/libfreertos.a", [self.build / "gen.c"], idf / "freertos")
        self.archive("esp-idf/main/libmain.a", [self.root / "py/vm.c"])
        # a library built by the package's own CMake
        self.write("idf/components/freertos/FreeRTOS-Kernel/list.c", "")
        self.archive(
            "esp-idf/freertos/FreeRTOS-Kernel/libk.a", [idf / "freertos/FreeRTOS-Kernel/list.c"]
        )

        def lic(expr: str) -> list[dict[str, str]]:
            return [{"expression": expr, "acknowledgement": "concluded"}]

        cdx = {
            "components": [
                {"bom-ref": "FRAMEWORK-esp-idf", "name": "esp-idf", "licenses": lic("MIT")},
                {
                    "bom-ref": "COMPONENT-freertos",
                    "name": "component-freertos",
                    "version": "v5",
                    "licenses": lic("Apache-2.0 AND MIT"),
                },
                {
                    "bom-ref": "SUBPACKAGE-freertos-FreeRTOS-Kernel",
                    "name": "freertos",
                    "version": "10.5.1",
                    "cpe": "cpe:2.3:o:amazon:freertos:10.5.1:*:*:*:*:*:*:*",
                    "supplier": {"name": "Amazon"},
                    "licenses": lic("MIT"),
                },
                {"bom-ref": "COMPONENT-esp-wifi", "name": "component-esp_wifi"},
                {"bom-ref": "SUBMODULE-esp-wifi-lib", "name": "submodule-esp_wifi-lib"},
                {"bom-ref": "COMPONENT-main", "name": "component-main", "licenses": lic("MIT")},
            ]
        }
        self.write("cdx.json", json.dumps(cdx))
        self.write("fake.py", FAKE_ESP_IDF_SBOM.format(str(self.root / "cdx.json")))
        self.objs = (
            "esp-idf/freertos/libfreertos.a(port.c.obj)",
            "esp-idf/freertos/libfreertos.a(tasks.c.obj)",
            "esp-idf/freertos/libfreertos.a(gen.c.obj)",
            f"{idf}/esp_wifi/lib/esp32/libnet80211.a(ieee80211.o)",
            "esp-idf/main/libmain.a(vm.c.obj)",
            "esp-idf/freertos/FreeRTOS-Kernel/libk.a(list.c.obj)",
        )

    def idf(self) -> sbom.Idf:
        return sbom.Idf(
            self.build / "project_description.json", [sys.executable, str(self.root / "fake.py")]
        )

    def test_packages(self) -> None:
        result = self.collect(*self.objs, idf=self.idf())
        self.assertEqual((result.unresolved, result.missing), ([], []))
        self.assertEqual(
            {c.name: (c.license, c.version) for c in result.components},
            {
                # gen.c is generated in the build dir, cmake_install.cmake maps it to its component
                "component-freertos": ("Apache-2.0 AND MIT", "v5"),
                "freertos": ("MIT", "10.5.1"),
                # esp-idf-sbom found no licence, so it's the package's own licence file
                "submodule-esp_wifi-lib": ("LicenseRef-esp-idf-SUBMODULE-esp-wifi-lib", None),
                # the main component is the project's own code
                "micropython": ("MIT", None),
            },
        )
        kernel = next(c for c in result.components if c.name == "freertos")
        cpe = "cpe:2.3:o:amazon:freertos:10.5.1:*:*:*:*:*:*:*"
        self.assertEqual((kernel.cpe, kernel.supplier), (cpe, "Amazon"))

    def test_package_without_licence(self) -> None:
        (self.root / "idf/components/esp_wifi/lib/LICENSE").unlink()
        result = self.collect(*self.objs, idf=self.idf())
        self.assertEqual(result.missing, ["components/esp_wifi/lib"])

    def one_package(self, licenses: list[dict]) -> tuple[sbom.Idf, Path]:
        cdx = {
            "components": [{"bom-ref": "COMPONENT-freertos", "name": "f", "licenses": licenses}]
        }
        self.write("cdx.json", json.dumps(cdx))
        idf = self.idf()
        return idf, idf.component_dirs["freertos"]

    def test_licence_with_name_and_text_but_no_id(self) -> None:
        b64 = base64.b64encode(b"encoded text").decode()
        for text, expected in (
            ({"content": "blob text"}, "blob text"),
            ({"content": b64, "encoding": "base64"}, "encoded text"),
            ("plain text", "plain text"),
        ):
            with self.subTest(text=text):
                idf, d = self.one_package([{"license": {"name": "LicenseRef-Blob", "text": text}}])
                c = idf.component(d, self.repo)
                self.assertEqual(c.license, "LicenseRef-Blob")
                self.assertEqual(self.repo.license_ref_texts([c]), {"LicenseRef-Blob": expected})
                self.repo.extracted.clear()

    def test_licence_with_no_expression_id_or_name_is_an_error(self) -> None:
        for entry in ({}, {"license": {}}, {"license": {"text": {"content": "x"}}}):
            with self.subTest(entry=entry):
                idf, d = self.one_package([entry])
                with self.assertRaises(sbom.SbomError):
                    idf.component(d, self.repo)

    def test_component_name_that_is_a_prefix_of_another(self) -> None:
        comps = self.root / "idf" / "components"
        desc_path = self.build / "project_description.json"
        desc = json.loads(desc_path.read_text())
        desc["build_component_info"]["esp_wifi_remote"] = {
            "dir": str(comps / "esp_wifi_remote"),
            "file": str(self.build / "esp-idf/esp_wifi_remote/libesp_wifi_remote.a"),
        }
        desc_path.write_text(json.dumps(desc))
        self.write("idf/components/esp_wifi_remote/lib/x.c", "")
        cdx = {
            "components": [
                {"bom-ref": "COMPONENT-esp-wifi", "name": "wifi"},
                {"bom-ref": "COMPONENT-esp-wifi-remote", "name": "remote"},
                {"bom-ref": "SUBMODULE-esp-wifi-remote-lib", "name": "remote-lib"},
            ]
        }
        self.write("cdx.json", json.dumps(cdx))
        idf = self.idf()
        self.assertEqual(
            idf.component_dirs,
            {"esp_wifi": comps / "esp_wifi", "esp_wifi_remote": comps / "esp_wifi_remote"},
        )
        self.assertEqual(
            {d: c["name"] for d, c in idf.packages.items()},
            {
                comps / "esp_wifi": "wifi",
                comps / "esp_wifi_remote": "remote",
                comps / "esp_wifi_remote" / "lib": "remote-lib",
            },
        )


class TestVersions(unittest.TestCase):
    def test_no_objects_in_the_map_is_an_error(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "x.map"
            path.write_text("Archive member included to satisfy reference by file (symbol)\n")
            with self.assertRaises(sbom.SbomError):
                sbom.linked_objects(path)

    def test_libgcc_version_comes_from_its_directory(self) -> None:
        lib = sbom.toolchain_lib("/o/lib/gcc/arm-none-eabi/12.2.1/libgcc.a(_udivdi3.o)")
        self.assertEqual((lib.name, lib.version), ("libgcc", "12.2.1"))
        lib = sbom.toolchain_lib("/o/lib/gcc/arm-none-eabi/libgcc.a(_udivdi3.o)")
        self.assertIsNone(lib.version)

    def test_combine_parenthesises_or_terms(self) -> None:
        result = sbom.combine(["MIT OR ISC", "Zlib"])
        lic = sbom.licensing()
        self.assertTrue(lic.is_equivalent(result, "(MIT OR ISC) AND Zlib"))
        self.assertFalse(lic.is_equivalent(result, "MIT OR (ISC AND Zlib)"))


class TestGlobMatch(unittest.TestCase):
    def test_patterns(self) -> None:
        for path, pattern, expected in (
            ("b.c", "*.c", True),
            ("a/b.c", "*.c", False),
            ("a/b.c", "a/*.c", True),
            ("a/b/c.c", "a/*.c", False),
            ("a/b/c.c", "a/**", True),
            ("a/b/c.c", "**/*.c", True),
            ("c.c", "**/*.c", True),
            ("x.a", "**/*.a", True),
            ("a.c", "a?c", True),
            ("a/c", "a?c", False),
            ("xc.c", "c.c", False),
        ):
            with self.subTest(path=path, pattern=pattern):
                self.assertEqual(sbom.glob_match(path, pattern), expected)


class TestReuseToml(RepoTestCase):
    def reuse(self, body: str) -> None:
        self.write("lib/a/REUSE.toml", "version = 1\n" + body)

    def test_annotation_without_a_licence_is_skipped(self) -> None:
        self.reuse('[[annotations]]\npath = "**"\nSPDX-FileCopyrightText = "me"\n')
        self.assertEqual(self.lic("lib/a/a.c"), "BSD-3-Clause")  # from the table

    def test_path_may_be_a_list(self) -> None:
        self.reuse('[[annotations]]\npath = ["a.c", "b.c"]\nSPDX-License-Identifier = "Zlib"\n')
        self.assertEqual(self.lic("lib/a/a.c"), "Zlib")

    def test_search_stops_at_the_component_dir(self) -> None:
        self.write(
            "REUSE.toml",
            'version = 1\n[[annotations]]\npath = "**"\nSPDX-License-Identifier = "Zlib"\n',
        )
        a = self.root / "lib/a"
        self.assertIsNone(self.repo.reuse_license(a / "a.c", a))
        self.assertEqual(self.repo.reuse_license(a / "a.c", self.root), "Zlib")

    def test_licenseref_text_comes_from_the_declaring_project(self) -> None:
        self.reuse('[[annotations]]\npath = "a.c"\nSPDX-License-Identifier = "LicenseRef-Foo"\n')
        ref = self.lic("lib/a/a.c")
        self.assertEqual(ref, "LicenseRef-Foo")
        self.assertEqual(self.repo.reuse_refs, {ref: self.root / "lib/a"})
        comp = [sbom.Component("a", "lib/a", ref)]
        with self.assertRaises(sbom.SbomError):
            self.repo.license_ref_texts(comp)
        self.write("lib/LICENSES/LicenseRef-Foo.txt", "Foo terms\n")  # an ancestor directory
        self.assertEqual(self.repo.license_ref_texts(comp), {ref: "Foo terms"})
        self.write("lib/a/LICENSES/LicenseRef-Foo.txt", "Closer terms\n")
        self.assertEqual(self.repo.license_ref_texts(comp), {ref: "Closer terms"})

    def test_undeclared_licenseref_text_must_be_in_the_repo(self) -> None:
        comp = [sbom.Component("x", "x", "MIT AND LicenseRef-Nowhere")]
        with self.assertRaises(sbom.SbomError):
            self.repo.license_ref_texts(comp)
        self.write("LICENSES/LicenseRef-Nowhere.txt", "text")
        self.assertEqual(self.repo.license_ref_texts(comp), {"LicenseRef-Nowhere": "text"})

    def test_src_root_declares_files_of_a_nested_component(self) -> None:
        ext = tempfile.TemporaryDirectory()
        self.addCleanup(ext.cleanup)
        e = Path(ext.name).resolve()
        (e / "libs/dep").mkdir(parents=True)
        (e / "REUSE.toml").write_text(
            'version = 1\n[[annotations]]\npath = ["libs/dep/a.c"]\nSPDX-License-Identifier = "Zlib"\n'
        )
        self.repo.add_root(e)
        self.repo.add_root(e / "libs/dep")
        dep = e / "libs/dep"
        self.assertEqual(self.repo.reuse_license(dep / "a.c", dep), "Zlib")
        self.assertIsNone(self.repo.reuse_license(dep / "b.c", dep))


class TestSources(RepoTestCase):
    def test_component_dir_outside_the_repo_needs_a_root(self) -> None:
        ext = tempfile.TemporaryDirectory()
        self.addCleanup(ext.cleanup)
        e = Path(ext.name).resolve()
        for d in ("mod", "mod/sub", "other"):
            (e / d).mkdir()
            (e / d / "LICENSE").write_text("l")
            (e / d / "f.c").write_text("")
        self.assertIsNone(self.repo.component_dir(e / "mod/f.c"))  # a LICENSE isn't enough
        self.repo.add_root(e / "mod")
        self.assertEqual(self.repo.component_dir(e / "mod/f.c"), e / "mod")
        self.assertEqual(self.repo.component_dir(e / "mod/sub/f.c"), e / "mod/sub")
        self.assertIsNone(self.repo.component_dir(e / "other/f.c"))


class TestFrozen(RepoTestCase):
    def setUp(self) -> None:
        super().setUp()
        (self.root / "tools").mkdir()
        shutil.copy(TOOLS / "manifestfile.py", self.root / "tools" / "manifestfile.py")
        self.write("lib/a/m.py", "x = 1\n")
        self.mapfile = self.write_map(
            "out.map", f" .text 0x1000 0x10 {self.dep('py/vm.o', 'py/vm.c')}\n"
        )

    def collect(self, manifest_text: str, **variables: str) -> sbom.Collected:
        manifest = self.write("manifest.py", manifest_text)
        variables = {"TOP": str(self.root), **variables}
        return sbom.collect(self.repo, self.mapfile, [self.root], [str(manifest)], variables)

    def test_declared_licence_order(self) -> None:
        src, comp = self.root / "lib/a/m.py", self.root / "lib/a"
        self.assertEqual(self.repo.file_license(src, comp, "Zlib"), "Zlib")  # beats the table
        src.write_text("# SPDX-License-Identifier: ISC\n")
        self.assertEqual(self.repo.file_license(src, comp, "Zlib"), "ISC")  # the tag beats it
        src.write_text("x = 1\n")
        self.write(
            "lib/a/REUSE.toml",
            'version = 1\n[[annotations]]\npath = "m.py"\nSPDX-License-Identifier = "MIT"\n',
        )
        self.assertEqual(self.repo.file_license(src, comp, "Zlib"), "Zlib")  # beats closest

    def test_metadata_licence_of_a_frozen_file(self) -> None:
        result = self.collect(
            'metadata(license="Zlib")\nmodule("m.py", base_path="$(TOP)/lib/a")\n'
        )
        self.assertEqual((result.unresolved, result.missing), ([], []))
        self.assertEqual(
            {c.name: c.license for c in result.components}, {"a": "Zlib", "micropython": "MIT"}
        )

    def test_invalid_metadata_licence_names_the_manifest(self) -> None:
        text = 'metadata(license="Nope-1.0")\nmodule("m.py", base_path="$(TOP)/lib/a")\n'
        with self.assertRaisesRegex(sbom.SbomError, re.escape(str(self.root / "manifest.py"))):
            self.collect(text)

    def test_frozen_file_outside_every_component_is_unresolved(self) -> None:
        ext = tempfile.TemporaryDirectory()
        self.addCleanup(ext.cleanup)
        e = Path(ext.name).resolve()
        (e / "m.py").write_text("x = 1\n")
        result = self.collect('module("m.py", base_path="$(EXT)")\n', EXT=str(e))
        self.assertEqual(result.unresolved, [str(e / "m.py")])

    def test_frozen_content_is_not_looked_up_through_its_dep_file(self) -> None:
        obj = self.dep("build/frozen_content.o", "py/vm.c")
        mapfile = self.write_map("out.map", f" .text 0x1000 0x10 {obj}\n")
        manifest = self.write("manifest.py", 'module("m.py", base_path="$(TOP)/lib/a")\n')
        variables = {"TOP": str(self.root)}
        result = sbom.collect(self.repo, mapfile, [self.root], [str(manifest)], variables)
        self.assertEqual({c.name for c in result.components}, {"a"})  # not micropython (vm.c)


class TestWriters(unittest.TestCase):
    def test_invalid_spdx_leaves_no_file(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "fw.spdx.json"
            comps = [sbom.Component("x", "x", "LicenseRef-x")]
            with self.assertRaises(sbom.SbomError):
                sbom.write_spdx(path, "fw", comps, {"LicenseRef-x": ""})  # no licence text
            self.assertFalse(path.exists())


class TestRun(RepoTestCase):
    def setUp(self) -> None:
        super().setUp()
        self.out = self.root / "o"
        self.out.mkdir()
        self.write(".gitmodules", '[submodule "d"]\n\tpath = lib/d\n\turl = https://x.org/d\n')
        self.write("lib/d/d.c", "int d;\n")  # a submodule with no licence
        self.spdx, self.cdx = self.out / "fw.out", self.out / "fw.cdx.json"

    def run_sbom(self, *objs: str) -> None:
        mapfile = self.write_map("out.map", *(f" .text 0x1000 0x10 {o}\n" for o in objs))
        args = argparse.Namespace(
            spdx=self.spdx,
            cyclonedx=self.cdx,
            repo=self.root,
            map=mapfile,
            name="fw",
            src_root=[],
            frozen=[],
            var=[],
            idf=None,
            blob=[],
        )
        sbom.run(args)

    def test_both_formats_are_written(self) -> None:
        self.run_sbom(
            self.dep("py/vm.o", "py/vm.c"),
            self.dep("lib/a/a.o", "lib/a/a.c"),
            self.dep("lib/e/e.o", "lib/e/e.c"),
        )
        spdx = json.loads(self.spdx.read_text())  # JSON whatever the extension
        self.assertEqual({p["name"] for p in spdx["packages"]}, {"fw", "micropython", "a", "e"})
        infos = {i["licenseId"]: i["extractedText"] for i in spdx["hasExtractedLicensingInfos"]}
        self.assertEqual(list(infos), ["LicenseRef-lib-e"])
        self.assertIn("The e licence", infos["LicenseRef-lib-e"])
        cdx = json.loads(self.cdx.read_text())
        licenses = {c["name"]: c["licenses"] for c in cdx["components"]}
        self.assertEqual(set(licenses), {"micropython", "a", "e"})
        e = licenses["e"][0]["license"]
        self.assertEqual(e["name"], "LicenseRef-lib-e")
        self.assertIn("The e licence", e["text"]["content"])

    def test_a_failed_run_removes_the_old_outputs(self) -> None:
        vm = self.dep("py/vm.o", "py/vm.c")
        self.run_sbom(vm)
        self.assertTrue(self.spdx.exists() and self.cdx.exists())
        for objs in ((vm, "nowhere/else.o"), (vm, self.dep("lib/d/d.o", "lib/d/d.c"))):
            with self.subTest(objs=objs):
                self.run_sbom(vm)
                with self.assertRaises(sbom.SbomError):
                    self.run_sbom(*objs)
                self.assertEqual(list(self.out.iterdir()), [])

    def test_nothing_to_do(self) -> None:
        args = argparse.Namespace(spdx=None, cyclonedx=None)
        with self.assertRaises(sbom.SbomError):
            sbom.run(args)


class TestRepoLicenseTexts(unittest.TestCase):
    def test_every_licenseref_of_the_repo_has_a_text(self) -> None:
        repo = sbom.Repo(TOOLS.parent)
        refs = set()
        for expr in repo.licenses.table.values():
            refs |= sbom.license_refs(expr)
        for _, _, lic, _ in sbom.TOOLCHAIN_LIBS:
            refs |= sbom.license_refs(lic)
        self.assertTrue(refs)
        for ref in sorted(refs):
            with self.subTest(ref=ref):
                self.assertTrue((TOOLS.parent / "LICENSES" / (ref + ".txt")).is_file())


if __name__ == "__main__":
    unittest.main()
