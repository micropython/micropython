
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_original
import sys
import os
import setuptools
import subprocess

import sysconfig

target_is_windows = sys.platform == "win32"

if target_is_windows:
    extra_objects = ["../windows/build-standard/micropython.lib"]
    extra_link_args = ["Bcrypt.lib"]
else:
    extra_objects = ["../unix/build-standard/libmicropython.a"]
    extra_link_args = []


def get_msvc_platform():
    arch = sysconfig.get_platform()  # e.g. 'win32', 'win-amd64', 'win-arm64'
    return {
        "win32": "Win32",
        "win-amd64": "x64",
        "win-arm64": "ARM64",
    }[arch]

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""

    def __str__(self):
        import pybind11
        return pybind11.get_include()

class build_ext(build_ext_original):
    """
    Run neccesary make steps before Python module build

    This enables to use pip install for the entire build flow,
    such as when doing building from a git repo. Example:
    pip install "git+https://github.com/micropython/micropython.git@main#subdirectory=ports/unix"
    """
    def run(self):

        if target_is_windows:
            # For Windows, build with MSVC
            msbuild_platform = get_msvc_platform()
            subprocess.run(
                [
                    "msbuild",
                    "libmicropython.vcxproj",
                    "/p:Configuration=Release",
                    f"/p:Platform={msbuild_platform}",
                ],
                cwd="../windows",
                check=True,
            )

        else:
            extra_args = [
                # btree uses headers not available in emscripten
                # btree #include <sys/cdefs.h> fails gives warning/erro on musl
                "MICROPY_PY_BTREE=0",
            ]
            # Unix port, build with make
            subprocess.check_call(["make", "submodules"], cwd="../unix")
            subprocess.check_call(["make", "clean", "libmicropython",
                "V=1",
                "CFLAGS_EXTRA=-fPIC -fno-omit-frame-pointer -DMICROPY_UNIX_NO_MAIN=1",
                "MICROPY_PY_FFI=0", # libffi causes linking error
                "VARIANT=standard",
            ] + extra_args, cwd="../unix")

        super().run()

ext_modules = [
    Extension(
        "micropython_module",
        ["micropython_module.cpp"],
        include_dirs=[
            get_pybind_include(),
        ],
        extra_objects=extra_objects,
        extra_link_args=extra_link_args,
        language="c++"
    ),
]

# NOTE: static metadata is in pyproject.toml
setup(
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
