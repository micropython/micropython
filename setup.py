from datetime import datetime
from setuptools import setup
from pathlib import Path

stub_root = Path("circuitpython-stubs")
stubs = [p.relative_to(stub_root).as_posix() for p in stub_root.glob("*.pyi")]

now = datetime.utcnow()
version = now.strftime("%Y.%m.%d")

setup(
    name="circuitpython-stubs",
    description="PEP 561 type stubs for CircuitPython",
    url="https://github.com/adafruit/circuitpython",
    maintainer="CircuitPythonistas",
    author_email="circuitpython@adafruit.com",
    version=version,
    license="MIT",
    package_data={"circuitpython-stubs": stubs},
    packages=["circuitpython-stubs"],
    setup_requires=["setuptools>=38.6.0"],
)
