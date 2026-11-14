#!/usr/bin/env python3
#
# Unit tests for py/makeqstrdefs.py. Run with
# `python3 tests/tools/test_makeqstrdefs.py`.

import os
import shutil
import subprocess
import sys
import tempfile
import unittest


class TestMakeQstrDefs(unittest.TestCase):
    @unittest.skipUnless(shutil.which("gcc") or shutil.which("cc"), "C preprocessor not found")
    def test_compressed_strings_in_header_use_including_source(self):
        compiler = shutil.which("gcc") or shutil.which("cc")
        repo_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
        script = os.path.join(repo_dir, "py", "makeqstrdefs.py")

        with tempfile.TemporaryDirectory() as tmpdir:
            source = os.path.join(tmpdir, "source.c")
            header = os.path.join(tmpdir, "messages.h")
            preprocessed = os.path.join(tmpdir, "source.i")
            output_dir = os.path.join(tmpdir, "compress")

            with open(header, "w") as f:
                f.write('MP_COMPRESSED_ROM_TEXT("header message")\n')
            with open(source, "w") as f:
                f.write('#include "messages.h"\nMP_COMPRESSED_ROM_TEXT("source message")\n')

            subprocess.run(
                [
                    sys.executable,
                    script,
                    "pp",
                    compiler,
                    "-E",
                    "output",
                    preprocessed,
                    "cflags",
                    "-I" + tmpdir,
                    "cxxflags",
                    "sources",
                    source,
                    "dependencies",
                    "changed_sources",
                    source,
                ],
                check=True,
            )
            subprocess.run(
                [sys.executable, script, "split", "compress", preprocessed, output_dir, "_"],
                check=True,
            )

            output_files = [name for name in os.listdir(output_dir) if name.endswith(".compress")]
            self.assertEqual(len(output_files), 1)
            with open(os.path.join(output_dir, output_files[0])) as f:
                self.assertEqual(
                    f.read().splitlines(),
                    ["header message", "source message"],
                )


if __name__ == "__main__":
    unittest.main()
