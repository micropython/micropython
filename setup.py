# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT
import os
import site
from datetime import datetime
from typing import Dict, List

from setuptools import setup
from pathlib import Path
import subprocess
import re

STD_PACKAGES = set(('array', 'math', 'os', 'random', 'struct', 'sys', 'ssl', 'time'))

stub_root = Path("circuitpython-stubs")
stubs = [p.relative_to(stub_root).as_posix() for p in stub_root.glob("*.pyi")]

git_out = subprocess.check_output(["git", "describe", "--tags"])
version = git_out.strip().decode("utf-8")

# Detect a development build and mutate it to be valid semver and valid python version.
pieces = version.split("-")
if len(pieces) > 2:
    # Merge the commit portion onto the commit count since the tag.
    pieces[-2] += "+" + pieces[-1]
    pieces.pop()
    # Merge the commit count and build to the pre-release identifier.
    pieces[-2] += ".dev." + pieces[-1]
    pieces.pop()
version = "-".join(pieces)

packages = set(os.listdir("circuitpython-stubs")) - STD_PACKAGES
package_dir = dict((f"{package}-stubs", f"circuitpython-stubs/{package}")
    for package in packages)
print("package dir is", package_dir)

def build_package_data() -> Dict[str, List[str]]:
    result = {}
    for package in packages:
        result[f"{package}-stubs"] = ["*.pyi", "*/*.pyi"]
    return result

package_data=build_package_data()
setup(
    name="circuitpython-stubs",
    description="PEP 561 type stubs for CircuitPython",
    url="https://github.com/adafruit/circuitpython",
    maintainer="CircuitPythonistas",
    maintainer_email="circuitpython@adafruit.com",
    author_email="circuitpython@adafruit.com",
    version=version,
    license="MIT",
    packages=list(package_data.keys()),
    package_data=package_data,
    package_dir = package_dir,
    setup_requires=["setuptools>=38.6.0"],
    zip_safe=False,
)
