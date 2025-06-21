#!/usr/bin/env python
# MIT license; Copyright (c) 2024 Angus Gratton
#
# This is a utility script for MicroPython maintainers, similar to tools/metrics.py
# but particular to this port. It's for measuring the impact of an ESP-IDF update or
# config change at a high level.
#
# Specifically, it builds the esp32 MicroPython port for a collection of boards
# and outputs a Markdown table of binary sizes, static IRAM size, and static
# DRAM size (the latter generally inversely correlates to free heap at runtime.)
#
# To use:
#
# 1) Need to not be in an ESP-IDF venv already (i.e. don't source export.sh),
#    but IDF_PATH has to be set.
#
# 2) Choose the versions you want to test and the board/variant pairs by
#    editing the tuples below.
#
# 3) The IDF install script sometimes fails if it has to downgrade a package
#    within a minor version. The "nuclear option" is to delete all the install
#    environments and have this script recreate them as it runs:
#    rm -rf ~/.espressif/python_env/*
#
# 4) Run this script from the ports/esp32 directory, i.e.:
#    ./tools/metrics_esp32.py
#
# 5) If all goes well, it will run for a while and then print a Markdown
#    formatted table of binary sizes, sorted by board+variant.
#
# Note that for ESP32-S3 and C3, IRAM and DRAM are exchangeable so the IRAM size
# column of the table is really D/IRAM.
import os
import re
import shutil
import sys
import subprocess
from dataclasses import dataclass

IDF_VERS = ("v5.4.1",)

BUILDS = (
    ("ESP32_GENERIC", ""),
    ("ESP32_GENERIC", "D2WD"),
    ("ESP32_GENERIC", "SPIRAM"),
    ("ESP32_GENERIC_S3", ""),
    ("ESP32_GENERIC_S3", "SPIRAM_OCT"),
)


def rmtree(path):
    try:
        shutil.rmtree(path)
    except FileNotFoundError:
        pass


@dataclass
class BuildSizes:
    idf_ver: str
    board: str
    variant: str
    bin_size: str = ""
    dram_size: str = ""
    iram_size: str = ""

    def print_summary(self, include_ver=False):
        print(f"BOARD={self.board} BOARD_VARIANT={self.variant}")
        if include_ver:
            print(f"IDF_VER {self.idf_ver}")
        print(f"Binary size: {self.bin_size}")
        print(f"IRAM size: {self.iram_size}")
        print(f"DRAM size: {self.dram_size}")

    def print_table_heading():
        print(
            "| BOARD | BOARD_VARIANT | IDF Version | Binary Size | Static IRAM Size | Static DRAM Size |"
        )
        print(
            "|-------|---------------|-------------|-------------|------------------|------------------|"
        )

    def print_table_row(self, print_board):
        print(
            "| "
            + " | ".join(
                (
                    self.board if print_board else "",
                    self.variant if print_board else "",
                    self.idf_ver,
                    self.bin_size,
                    self.iram_size,
                    self.dram_size,
                )
            )
            + " |"
        )

    def __lt__(self, other):
        """sort by board, then variant, then IDF version to get an easy
        to compare table"""
        return (self.board, self.variant, self.idf_ver) < (
            other.board,
            other.variant,
            other.idf_ver,
        )

    def build_dir(self):
        if self.variant:
            return f"build-{self.board}-{self.variant}"
        else:
            return f"build-{self.board}"

    def run_make(self, target):
        env = dict(os.environ)
        env["BOARD"] = self.board
        env["BOARD_VARIANT"] = self.variant

        try:
            # IDF version changes as we go, so re-export the environment each time
            cmd = f"source $IDF_PATH/export.sh; make {target}"
            return subprocess.check_output(
                cmd, shell=True, env=env, stderr=subprocess.STDOUT
            ).decode()
        except subprocess.CalledProcessError as e:
            err_file = f"{self.build_dir()}/make-{target}-failed-{self.idf_ver}.log"
            print(f"'make {target}' failed, writing to log to {err_file}", file=sys.stderr)
            with open(err_file, "w") as f:
                f.write(e.output.decode())
            raise

    def make_size(self):
        try:
            size_out = self.run_make("size")
            try:
                # pre IDF v5.4 size output
                # "Used static DRAM:" or "Used stat D/IRAM:"
                RE_DRAM = r"Used stat(?:ic)? D.*: *(\d+) bytes"
                RE_IRAM = r"Used static IRAM: *(\d+) bytes"
                self.dram_size = re.search(RE_DRAM, size_out).group(1)
                self.iram_size = re.search(RE_IRAM, size_out).group(1)
            except AttributeError:
                # IDF v5.4 size output is much nicer formatted
                # Note the pipes in these expressions are not the ASCII/RE |
                RE_DRAM = r"│ *DI?RAM *│ *(\d+)"
                RE_IRAM = r"│ *IRAM *│ *(\d+)"
                self.dram_size = re.search(RE_DRAM, size_out).group(1)
                self.iram_size = re.search(RE_IRAM, size_out).group(1)

            # This line is the same on before/after versions
            RE_BIN = r"Total image size: *(\d+) bytes"
            self.bin_size = re.search(RE_BIN, size_out).group(1)
        except subprocess.CalledProcessError:
            self.bin_size = "build failed"


def main(do_clean):
    if "IDF_PATH" not in os.environ:
        raise RuntimeError("IDF_PATH must be set")

    if not os.path.exists("Makefile"):
        raise RuntimeError(
            "This script must be run from the ports/esp32 directory, i.e. as ./tools/metrics_esp32.py"
        )

    if "IDF_PYTHON_ENV_PATH" in os.environ:
        raise RuntimeError(
            "Run this script without any existing ESP-IDF environment active/exported."
        )

    sizes = []
    for idf_ver in IDF_VERS:
        switch_ver(idf_ver)
        rmtree("managed_components")
        for board, variant in BUILDS:
            print(f"Building '{board}'/'{variant}'...", file=sys.stderr)
            result = BuildSizes(idf_ver, board, variant)
            # Rather than running the 'clean' target, delete the build directory to avoid
            # environment version mismatches, etc.
            rmtree(result.build_dir())
            result.make_size()
            result.print_summary()
            sizes.append(result)

    # print everything again as a table sorted by board+variant
    last_bv = ""
    BuildSizes.print_table_heading()
    for build_sizes in sorted(sizes):
        bv = (build_sizes.board, build_sizes.variant)
        build_sizes.print_table_row(last_bv != bv)
        last_bv = bv


def idf_git(*commands):
    try:
        subprocess.check_output(
            ["git"] + list(commands), cwd=os.environ["IDF_PATH"], stderr=subprocess.STDOUT
        )
    except subprocess.CalledProcessError as e:
        print(f"git {' '.join(commands)} failed:")
        print(e.output.decode())
        raise


def idf_install():
    try:
        subprocess.check_output(
            ["bash", "install.sh"], cwd=os.environ["IDF_PATH"], stderr=subprocess.STDOUT
        )
    except subprocess.CalledProcessError as e:
        print("IDF install.sh failed:")
        print(e.output.decode())
        raise


def switch_ver(idf_ver):
    print(f"Switching version to {idf_ver}...", file=sys.stderr)
    idf_git("switch", "--detach", idf_ver)
    idf_git("submodule", "update", "--init", "--recursive")
    idf_install()


if __name__ == "__main__":
    main("--no-clean" not in sys.argv)
