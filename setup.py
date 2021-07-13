# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT
import os
import site
from datetime import datetime
from typing import Dict, List

from setuptools import setup
from pathlib import Path

STD_PACKAGES = set(('array', 'math', 'os', 'random', 'struct', 'sys', 'ssl', 'time'))

stub_root = Path("circuitpython-stubs")
stubs = [p.relative_to(stub_root).as_posix() for p in stub_root.glob("*.pyi")]

def local_scheme(version):
    return ""

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
    license="MIT",
    packages=list(package_data.keys()),
    package_data=package_data,
    package_dir = package_dir,
    setup_requires=["setuptools_scm", "setuptools>=38.6.0"],
    use_scm_version={"local_scheme": local_scheme},
    zip_safe=False,
)
