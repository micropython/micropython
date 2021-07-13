#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import argparse
import os
import os.path
from pathlib import Path
import semver
import subprocess

# Compatible with Python 3.4 due to travis using trusty as default.


def version_string(path=None, *, valid_semver=False):
    version = None
    try:
        tag = subprocess.check_output("git describe --tags --exact-match", shell=True, cwd=path)
        version = tag.strip().decode("utf-8", "strict")
    except subprocess.CalledProcessError:
        describe = subprocess.check_output("git describe --tags", shell=True, cwd=path)
        tag, additional_commits, commitish = (
            describe.strip().decode("utf-8", "strict").rsplit("-", maxsplit=2)
        )
        commitish = commitish[1:]
        if valid_semver:
            version_info = semver.parse_version_info(tag)
            if not version_info.prerelease:
                version = (
                    semver.bump_patch(tag)
                    + "-alpha.0.plus."
                    + additional_commits
                    + "+"
                    + commitish
                )
            else:
                version = tag + ".plus." + additional_commits + "+" + commitish
        else:
            version = commitish
    return version


# Visit all the .py files in topdir. Replace any __version__ = "0.0.0-auto.0" type of info
# with actual version info derived from git.
def copy_and_process(in_dir, out_dir):
    for root, subdirs, files in os.walk(in_dir):

        # Skip library examples directory and subfolders.
        relative_path_parts = Path(root).relative_to(in_dir).parts
        if relative_path_parts and relative_path_parts[0] in ["examples", "docs", "tests"]:
            del subdirs[:]
            continue

        for file in files:
            # Skip top-level setup.py (module install info) and conf.py (sphinx config),
            # which are not part of the library
            if (root == in_dir) and file in ("conf.py", "setup.py"):
                continue

            input_file_path = Path(root, file)
            output_file_path = Path(out_dir, input_file_path.relative_to(in_dir))

            if file.endswith(".py"):
                if not output_file_path.parent.exists():
                    output_file_path.parent.mkdir(parents=True)
                with input_file_path.open("r") as input, output_file_path.open("w") as output:
                    for line in input:
                        if line.startswith("__version__"):
                            module_version = version_string(root, valid_semver=True)
                            line = line.replace("0.0.0-auto.0", module_version)
                        output.write(line)


if __name__ == "__main__":
    argparser = argparse.ArgumentParser(
        description="""\
    Copy and pre-process .py files into output directory, before freezing.
    1. Remove top-level repo directory.
    2. Update __version__ info.
    3. Remove examples.
    4. Remove non-library setup.py and conf.py"""
    )
    argparser.add_argument(
        "in_dirs",
        metavar="input-dir",
        nargs="+",
        help="top-level code dirs (may be git repo dirs)",
    )
    argparser.add_argument("-o", "--out_dir", help="output directory")
    args = argparser.parse_args()

    for in_dir in args.in_dirs:
        copy_and_process(in_dir, args.out_dir)
