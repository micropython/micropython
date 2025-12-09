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
import json
import os
import re
import shutil
import sys
import subprocess
from dataclasses import dataclass

IDF_VERS = ("v5.5.1", "v5.4.2")

BUILDS = (
    ("ESP32_GENERIC", ""),
    ("ESP32_GENERIC", "D2WD"),
    ("ESP32_GENERIC", "SPIRAM"),
    ("ESP32_GENERIC_C2", ""),
    ("ESP32_GENERIC_C6", ""),
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
    bin_size: int | str = ""
    dram_size: int = 0
    iram_size: int = 0

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

    def print_table_row(self, print_board, baseline=None):
        def compare(field):
            this = getattr(self, field)
            if baseline and isinstance(this, int):
                base = getattr(baseline, field)
                delta = this - base
                pct = ((this / base) * 100) - 100
                plus = "+" if delta >= 0 else ""
                return f"{this} ({plus}{delta}/{pct:.1f})"
            else:
                return str(this)

        print(
            "| "
            + " | ".join(
                (
                    self.board if print_board else "",
                    self.variant if print_board else "",
                    self.idf_ver,
                    compare("bin_size"),
                    compare("iram_size"),
                    compare("dram_size"),
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
            size_out = self.run_make("size SIZE_FLAGS='--format json'")
            size_out = size_out[size_out.rindex("{") :]
            size_out = json.loads(size_out)

            def sum_sizes(*keys):
                return sum(size_out.get(k, 0) for k in keys)

            # Different targets report DRAM, IRAM, and/or D/IRAM separately
            self.dram_size = sum_sizes(
                "used_dram", "diram_data", "diram_bss", "diram_rodata", "diram_other"
            )
            self.iram_size = sum_sizes("used_iram", "diram_text", "diram_vectors")
            self.bin_size = size_out["total_size"]
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
    baseline_sizes = None
    BuildSizes.print_table_heading()
    for build_sizes in sorted(sizes):
        bv = (build_sizes.board, build_sizes.variant)
        new_board = last_bv != bv
        if new_board:
            baseline_sizes = None
        build_sizes.print_table_row(last_bv != bv, baseline_sizes)
        last_bv = bv
        baseline_sizes = build_sizes


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
