# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT
import os
import site
from datetime import datetime
from typing import Dict, List

from setuptools import setup
from pathlib import Path

def local_scheme(version):
    return ""

STD_PACKAGES = set(('array', 'math', 'os', 'random', 'struct', 'sys', 'ssl', 'time'))

stub_root = Path(".")
stubs = [p.relative_to(stub_root) for p in stub_root.glob("*/*.pyi")]
packages = set(stub.parent.as_posix() for stub in stubs) - STD_PACKAGES
package_dir = dict((f"{package}-stubs", package)
    for package in packages)

def build_package_data() -> Dict[str, List[str]]:
    result = {}
    for package in packages:
        result[f"{package}-stubs"] = ["*.pyi", "*/*.pyi"]
    result['circuitpython_setboard'] = ["*.py", "*/*.py"]
    result['board_definitions'] = ["*.pyi", "*/*.pyi"]
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
    entry_points = {
      'console_scripts': [
          'circuitpython_setboard = circuitpython_setboard:set_board'
      ]
    },
    packages=list(package_data.keys()),
    package_data=package_data,
    package_dir = package_dir,
    setup_requires=["setuptools_scm", "setuptools>=38.6.0"],
    use_scm_version = {
        "root": "..",
        "relative_to": __file__,
        "local_scheme": local_scheme,
        "git_describe_command": "tools/describe --long",
    },
    zip_safe=False,
)
