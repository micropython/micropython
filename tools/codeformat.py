#!/usr/bin/env python3

import sys, os, glob

FILES = [
    "drivers/**/*.py",
    "examples/**/*.py",
    "extmod/**/*.py",
    "lib/**/*.py",
    "ports/**/*.py",
    "py/**/*.py",
    "tools/**/*.py",
]

EXCLUSIONS = []


def list_files():
    files = set()
    for pattern in FILES:
        files.update(glob.glob(pattern, recursive=True))
    for pattern in EXCLUSIONS:
        files.difference_update(glob.fnmatch.filter(files, pattern))
    return sorted(files)


def main():
    try:
        os.stat("./tools/codeformat.py")
    except OSError:
        print(
            "Run this from the top-level directory, e.g. ./tools/codeformat.py",
            file=sys.stderr,
        )
        sys.exit(1)

    files = list_files()
    for file in files:
        print(file)
        os.system("black -q --fast " + file)


if __name__ == "__main__":
    main()
