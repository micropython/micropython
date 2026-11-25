#! /usr/bin/env python3

import argparse
import importlib
import json
import subprocess
import sys
import unittest

import test_utils

# Can't 'import run-tests' because the dash is a syntax error.
run_tests = importlib.import_module("run-tests")
base_path = run_tests.base_path
CPYTHON3_CMD = run_tests.CPYTHON3_CMD


class PathComparisonTests(unittest.TestCase):
    def setUp(self) -> None:
        self.args = argparse.Namespace
        self.args.result_dir = base_path("results")
        self.run_tests_path = base_path("run-tests.py")
        self.base_path = base_path()
        self.not_base_path = base_path("..")

    def get_test_result(self, test: str) -> str:
        """Result ("pass" or "skip etc) of a single test, from _results.json."""
        with open(test_utils.get_results_filename(self.args), "r") as f:
            results = json.load(f)["results"]
        return next((result[1] for result in results if result[0] == test), "not in results.json")

    def test_discovery_relative_to_base_path(self):
        """Not passing any paths runs test discovery."""
        specs = [
            (self.base_path, "run-tests.py"),
            (base_path("../ports/unix"), "../../tests/run-tests.py"),
            (self.not_base_path, self.run_tests_path),
        ]
        for spec in specs:
            cwd, run_tests_path = spec
            with self.subTest(cwd=cwd, run_tests_path=run_tests_path):
                result = subprocess.run(
                    CPYTHON3_CMD + [run_tests_path, "--dry-run"],
                    cwd=cwd,
                    stdout=subprocess.DEVNULL,
                )
                self.assertEqual(result.returncode, 0)
                self.assertEqual(self.get_test_result("basics/0prelim.py"), "found")
                # Port test discovery used to refer to cwd so check this works.
                if sys.platform == "linux":
                    self.assertEqual(
                        self.get_test_result("ports/unix/time_mktime_localtime.py"), "found"
                    )

    def test_discovery_runs_if_tests_path_passed(self):
        """Passing the tests/ directory runs test discovery."""
        specs = [
            (self.base_path, "."),
            (self.base_path, "../tests"),
            (base_path("../ports/unix"), "../../tests"),
        ]
        if sys.platform == "win32":
            specs.append((self.base_path, "../TESTS"))
        for spec in specs:
            cwd, directory = spec
            with self.subTest(cwd=cwd, directory=directory):
                result = subprocess.run(
                    CPYTHON3_CMD
                    + [self.run_tests_path, "--dry-run", "-d", "otherdir", "-d", directory],
                    cwd=cwd,
                    stdout=subprocess.DEVNULL,
                )
                self.assertEqual(result.returncode, 0)
                self.assertEqual(self.get_test_result("basics/0prelim.py"), "found")
                if sys.platform == "linux":
                    self.assertEqual(
                        self.get_test_result("ports/unix/time_mktime_localtime.py"), "found"
                    )

    def test_special_tests_compares_real_paths(self):
        """Tests in cmdline/ and feature_check/ must must have their output tested with a regex."""
        specs = [
            ("feature_check/self.py", self.base_path, "feature_check/self.py"),
            ("cmdline/self.py", self.base_path, "cmdline/self.py"),
            ("cmdline/self.py", self.base_path, "cmdline/../cmdline/self.py"),
            ("cmdline/self.py", self.not_base_path, base_path("cmdline/self.py")),
        ]
        # Must be case-insensitive plus ignore separator.
        if sys.platform == "win32":
            specs.append(("cmdline/self.py", self.base_path, "CMDLINE\\self.py"))
            specs.append(("cmdline/self.py", self.not_base_path, base_path("CMDLINE\\self.py")))
        for spec in specs:
            name, cwd, file = spec
            with self.subTest(cwd=cwd, file=file):
                result = subprocess.run(
                    CPYTHON3_CMD + [self.run_tests_path, file], cwd=cwd, stdout=subprocess.DEVNULL
                )
                self.assertEqual(result.returncode, 0)
                # Since we access test by canonical name here this also verifies any non-canonical
                # name passed in gets properly converted.
                self.assertEqual(self.get_test_result(name), "pass")

    def test_tests_with_regex_output_compares_real_paths(self):
        """Tests in tests_with_regex_output must have their output tested with a regex."""
        name = "basics/bytes_compare3.py"
        specs = [
            (name, self.base_path, name),
            (name, self.not_base_path, base_path(name)),
            (name, self.base_path, "basics/../basics/bytes_compare3.py"),
        ]
        if sys.platform == "win32":
            specs.append((name, self.base_path, "BASICS\\bytes_compare3.py"))
            specs.append((name, self.not_base_path, base_path("BASICS\\bytes_compare3.py")))
        for spec in specs:
            name, cwd, file = spec
            with self.subTest(cwd=cwd, file=file):
                result = subprocess.run(
                    CPYTHON3_CMD + [self.run_tests_path, file], cwd=cwd, stdout=subprocess.DEVNULL
                )
                self.assertEqual(result.returncode, 0)
                self.assertEqual(self.get_test_result(name), "pass")

    def test_skip_thread_mutate_compares_real_paths(self):
        """Tests in thread/ starting with mutate_ must be skipped on unix."""
        if sys.platform != "linux":
            return
        specs = [
            (self.base_path, "thread"),
            (self.not_base_path, base_path("thread")),
            (self.base_path, "thread/../thread"),
        ]
        for spec in specs:
            cwd, directory = spec
            result = subprocess.run(
                CPYTHON3_CMD + [self.run_tests_path, "-d", directory, "--dry-run"],
                cwd=cwd,
                stdout=subprocess.DEVNULL,
            )
            self.assertEqual(result.returncode, 0)
            self.assertEqual(self.get_test_result("thread/mutate_list.py"), "skip")


if __name__ == "__main__":
    unittest.main()
