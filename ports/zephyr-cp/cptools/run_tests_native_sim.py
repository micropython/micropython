#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Vladimír Smitka
# SPDX-License-Identifier: MIT

"""Run the core test suite (tests/run-tests.py) on the native_sim build.

Each simulator gets a flash image that already holds what the tests import
from the device: tests/testlib and micropython-lib's unittest, in /lib.
run-tests.py runs one test at a time on a device, so the tests are split
over several simulators (-j) and the results are merged afterwards. When a
test takes the simulator down, it is counted as failed and run-tests.py is
started again with the rest of the tests.
"""

import argparse
import collections
import concurrent.futures
import glob
import os
import pathlib
import re
import shutil
import subprocess
import sys
import time

ZEPHYR_CP = pathlib.Path(__file__).resolve().parent.parent
TOP = ZEPHYR_CP.parent.parent
TESTS = TOP / "tests"
UNITTEST = TOP / "lib/micropython-lib/python-stdlib/unittest/unittest/__init__.py"
FLASH_SIZE = 2 * 1024 * 1024
# The other directories still have tests that fail on the simulator.
DEFAULT_DIRS = ("basics", "float", "misc", "unicode")
RESULT_LINE = re.compile(r"^(pass|FAIL|skip|lrge) +(\S+)")
RESULTS = {"pass": "pass", "FAIL": "fail", "skip": "skip", "lrge": "skip"}


def make_flash_image(path):
    path.write_bytes(b"\xff" * FLASH_SIZE)
    subprocess.run(["mformat", "-i", path, "::"], check=True)
    subprocess.run(["mmd", "-i", path, "::lib", "::lib/unittest"], check=True)
    testlib = sorted(f for f in (TESTS / "testlib").iterdir() if f.is_file())
    subprocess.run(["mcopy", "-i", path, *testlib, "::lib/"], check=True)
    subprocess.run(["mcopy", "-i", path, UNITTEST, "::lib/unittest/"], check=True)


def run_shard(exe, flash, shard, tests, env):
    """Run tests on one simulator, restarting it after a crash.

    Returns {test: result} and an error message if run-tests.py could not run at all."""
    shard.mkdir()
    shutil.copy(flash, shard / "flash.bin")
    sim = f"{exe} --flash={shard / 'flash.bin'} -no-rt -wait_uart -display_headless -i2s_earless"
    cmd = [sys.executable, "run-tests.py", "-t", "execpty:" + sim, "-r", str(shard)]
    results = {}
    attempt = 0
    while True:
        remaining = [test for test in tests if test not in results]
        if not remaining:
            return results, None
        log_file = shard / f"run-tests-{attempt}.log"
        with open(log_file, "w") as log:
            proc = subprocess.run(
                cmd + remaining, cwd=TESTS, env=env, stdout=log, stderr=subprocess.STDOUT
            )
        log = log_file.read_text()
        if proc.returncode < 0:
            # Killed by a signal, for example Ctrl-C.
            return results, f"run-tests.py was stopped by signal {-proc.returncode}"
        if "platform=" not in log:
            return results, f"run-tests.py did not start, see {log_file}:\n{log[-2000:]}"
        for line in log.splitlines():
            m = RESULT_LINE.match(line)
            if m:
                results[m.group(2)] = RESULTS[m.group(1)]
        # run-tests.py stopped early, most likely because the test after the
        # last one it reported took the simulator down.
        crashed = next((test for test in remaining if test not in results), None)
        if crashed is not None:
            results[crashed] = "fail"
            out = shard / (crashed.replace("/", "_") + ".out")
            out.write_text(f"run-tests.py stopped during this test, see {log_file}\n")
        attempt += 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--build",
        default=ZEPHYR_CP / "build-native_native_sim",
        type=pathlib.Path,
        help="native_sim build directory (default: %(default)s)",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        # A simulator without -rt keeps a CPU busy even while it waits for input.
        default=max(1, (os.cpu_count() or 2) // 2),
        type=int,
        help="number of simulators to run at once (default: %(default)s)",
    )
    parser.add_argument(
        "-d",
        "--test-dirs",
        nargs="*",
        help="test directories, relative to tests/ (default: %s)" % " ".join(DEFAULT_DIRS),
    )
    parser.add_argument("files", nargs="*", help="test files, relative to tests/")
    args = parser.parse_args()

    exe = args.build.resolve() / "zephyr-cp/zephyr/zephyr.exe"
    if not exe.exists():
        sys.exit(f"{exe} not found, build it with: make BOARD=native_native_sim")
    if not UNITTEST.exists():
        sys.exit(f"{UNITTEST} not found, run: git submodule update --init lib/micropython-lib")

    tests = list(args.files)
    for test_dir in args.test_dirs or ([] if tests else DEFAULT_DIRS):
        tests += sorted(
            os.path.relpath(f, TESTS) for f in glob.glob(str(TESTS / test_dir / "*.py"))
        )
    if not tests:
        sys.exit("no tests found")

    work = args.build.resolve() / "run-tests"
    # run-tests.py splits the execpty command on whitespace.
    if any(c.isspace() for c in str(work) + str(exe)):
        sys.exit(f"paths must not contain whitespace: {work}")
    shutil.rmtree(work, ignore_errors=True)
    results_dir = work / "results"
    results_dir.mkdir(parents=True)
    flash = work / "flash.bin"
    make_flash_image(flash)

    env = dict(os.environ)
    env.setdefault("SDL_VIDEODRIVER", "dummy")
    jobs = max(1, min(args.jobs, len(tests)))
    start = time.monotonic()
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        futures = [
            pool.submit(run_shard, exe, flash, work / f"shard{i}", tests[i::jobs], env)
            for i in range(jobs)
        ]
    elapsed = time.monotonic() - start

    results = {}
    errors = []
    for i, future in enumerate(futures):
        shard_results, error = future.result()
        results.update(shard_results)
        if error:
            errors.append(error)
        for out in glob.glob(str(work / f"shard{i}" / "*.exp")) + glob.glob(
            str(work / f"shard{i}" / "*.out")
        ):
            shutil.move(out, results_dir)

    counts = collections.defaultdict(collections.Counter)
    for test, result in results.items():
        counts[os.path.dirname(test)][result] += 1
    print(f"{'directory':<20} {'pass':>6} {'fail':>6} {'skip':>6}")
    for test_dir, count in sorted(counts.items()):
        print(f"{test_dir:<20} {count['pass']:>6} {count['fail']:>6} {count['skip']:>6}")
    print(f"{len(results)} of {len(tests)} tests run in {elapsed:.0f} s on {jobs} simulators")

    failed = sorted(test for test, result in results.items() if result == "fail")
    if failed:
        print(f"{len(failed)} tests failed: {' '.join(failed)}")
        print(f"Show the diffs with: tests/run-tests.py --print-failures -r {results_dir}")
    for error in errors:
        print(error)
    if failed or errors:
        sys.exit(1)


if __name__ == "__main__":
    main()
