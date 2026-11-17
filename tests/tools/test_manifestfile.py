#!/usr/bin/env python3
#
# Unit tests for tools/manifestfile.py. Run with
# `python3 tests/tools/test_manifestfile.py`.

import os
import sys
import tempfile
import unittest

# Locate tools/manifestfile.py relative to this file.
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "tools"))
from manifestfile import (
    ManifestFile,
    ManifestFileError,
    MODE_COMPILE,
    MODE_FREEZE,
    MODE_PYPROJECT,
)


class TestCModule(unittest.TestCase):
    def setUp(self):
        self.tmpdir = tempfile.mkdtemp()
        self.mf = ManifestFile(MODE_FREEZE, {"MPY_DIR": self.tmpdir})

    def tearDown(self):
        import shutil

        shutil.rmtree(self.tmpdir, ignore_errors=True)

    def _make_path(self, *parts):
        path = os.path.join(self.tmpdir, *parts)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        return path

    def test_missing_path(self):
        with self.assertRaisesRegex(ManifestFileError, "C module path does not exist"):
            self.mf.c_module(os.path.join(self.tmpdir, "no_such_dir"))

    def test_path_is_file_not_dir(self):
        f = os.path.join(self.tmpdir, "regular_file")
        open(f, "w").close()
        with self.assertRaisesRegex(ManifestFileError, "C module path must be a directory"):
            self.mf.c_module(f)

    def test_dir_missing_module_files(self):
        d = os.path.join(self.tmpdir, "mymod")
        os.makedirs(d)
        with self.assertRaisesRegex(
            ManifestFileError, "must contain micropython.mk or micropython.cmake"
        ):
            self.mf.c_module(d)

    def test_accepts_dir_with_mk(self):
        d = os.path.join(self.tmpdir, "mymod_mk")
        os.makedirs(d)
        open(os.path.join(d, "micropython.mk"), "w").close()
        self.mf.c_module(d)
        self.assertIn(d, self.mf.c_modules())

    def test_accepts_dir_with_cmake(self):
        d = os.path.join(self.tmpdir, "mymod_cmake")
        os.makedirs(d)
        open(os.path.join(d, "micropython.cmake"), "w").close()
        self.mf.c_module(d)
        self.assertIn(d, self.mf.c_modules())

    def test_path_var_substitution(self):
        d = os.path.join(self.tmpdir, "submod")
        os.makedirs(d)
        open(os.path.join(d, "micropython.mk"), "w").close()
        self.mf.c_module("$(MPY_DIR)/submod")
        self.assertIn(d, self.mf.c_modules())

    def test_unresolved_path_var(self):
        with self.assertRaisesRegex(
            ManifestFileError, r"Unresolved variable in c_module\(\) path"
        ):
            self.mf.c_module("$(BOARD_DIR)/nope")


class TestCModuleNonFreezeModes(unittest.TestCase):
    # In MODE_COMPILE/MODE_PYPROJECT, c_module() is a silent no-op so a manifest
    # using it can be evaluated in any mode without raising NameError.

    def setUp(self):
        self.tmpdir = tempfile.mkdtemp()

    def tearDown(self):
        import shutil

        shutil.rmtree(self.tmpdir, ignore_errors=True)

    def _no_op_in_mode(self, mode):
        mf = ManifestFile(mode, {"MPY_DIR": self.tmpdir})
        # Path does not exist; would raise in MODE_FREEZE.
        mf.c_module(os.path.join(self.tmpdir, "no_such_dir"))
        self.assertEqual(mf.c_modules(), [])

    def test_compile_mode_noop(self):
        self._no_op_in_mode(MODE_COMPILE)

    def test_pyproject_mode_noop(self):
        self._no_op_in_mode(MODE_PYPROJECT)


if __name__ == "__main__":
    unittest.main()
