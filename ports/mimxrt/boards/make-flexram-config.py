#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2021 Philipp Ebensberger
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""Evaluate FlexRAM configuration and generate startup code."""

import re
import argparse

# Regex for linker script configuration
ocram_regex = r"^\s*ocrm_size\s*=\s*(?P<size>.*);"
dtcm_regex = r"^\s*dtcm_size\s*=\s*(?P<size>.*);"
itcm_regex = r"^\s*itcm_size\s*=\s*(?P<size>.*);"

# Regex for GPR register base define in NXL hal
gpr_base_regex = r"^.*IOMUXC_GPR_BASE\s*\((?P<base_addr>\w*)u\)"

# Regex for FlexRAM parameters in NXP HAL
fsl_ram_bank_size_regex = r"^.*FSL_FEATURE_FLEXRAM_INTERNAL_RAM_BANK_SIZE\s*\((?P<size>\w*)\)"
fsl_bank_nbr_regex = (
    r"^.*FSL_FEATURE_FLEXRAM_INTERNAL_RAM_TOTAL_BANK_NUMBERS\s*\((?P<number>\w*)\)"
)


"""
According to AN12077:
    The minimum configuration of OCRAM is 64 KB. This is required
    due to ROM code requires at least 64 KB of RAM for its execution.

    2.1.1.1. Static configuration - Page 4
"""
ocram_min_size = 0x00010000  # 64 KB


# Value parser
def mimxrt_default_parser(defines_file, features_file, ld_script):
    with open(ld_script, "r") as input_file:
        input_str = input_file.read()
    #
    ocram_match = re.search(ocram_regex, input_str, re.MULTILINE)
    dtcm_match = re.search(dtcm_regex, input_str, re.MULTILINE)
    itcm_match = re.search(itcm_regex, input_str, re.MULTILINE)

    with open(defines_file, "r") as input_file:
        input_str = input_file.read()
    mcu_define_file_match = re.search(gpr_base_regex, input_str, re.MULTILINE)

    with open(features_file, "r") as input_file:
        input_str = input_file.read()
    fsl_ram_bank_size_match = re.search(fsl_ram_bank_size_regex, input_str, re.MULTILINE)
    fsl_bank_nbr_match = re.search(fsl_bank_nbr_regex, input_str, re.MULTILINE)
    #
    extract = {
        "ocram_size": int(ocram_match.group("size"), 16),
        "dtcm_size": int(dtcm_match.group("size"), 16),
        "itcm_size": int(itcm_match.group("size"), 16),
        "gpr_base_addr": int(mcu_define_file_match.group("base_addr"), 16),
        "fsl_ram_bank_size": int(fsl_ram_bank_size_match.group("size")),
        "fsl_bank_nbr": int(fsl_bank_nbr_match.group("number")),
    }
    # Evaluate configuration
    if extract["ocram_size"] < ocram_min_size:
        raise ValueError("OCRAM size must be at least {:08X}!".format(ocram_min_size))

    if (extract["ocram_size"] % extract["fsl_ram_bank_size"]) != 0:
        raise ValueError("Configuration invalid!")

    # Check if DTCM and ITCM size is either multiple of 32k or 4k,8k or 16k
    if extract["dtcm_size"] != 0x0:
        if extract["dtcm_size"] % extract["fsl_ram_bank_size"] != 0:
            if extract["dtcm_size"] not in (0x00000000, 0x00001000, 0x00002000, 0x00004000):
                raise ValueError("Configuration invalid!")

    if extract["itcm_size"] != 0x0:
        if extract["itcm_size"] % extract["fsl_ram_bank_size"] != 0:
            if extract["itcm_size"] not in (0x00000000, 0x00001000, 0x00002000, 0x00004000):
                raise ValueError("Configuration invalid!")
    #
    return extract


# Code generators
def mimxrt_default_gen_code(extract_dict):
    flexram_bank_cfg = "0b"
    avail_flexram = extract_dict["fsl_ram_bank_size"] * extract_dict["fsl_bank_nbr"]

    if (
        extract_dict["ocram_size"] + extract_dict["dtcm_size"] + extract_dict["itcm_size"]
    ) > avail_flexram:
        raise ValueError("Configuration exceeds available FlexRAM!")

    bit_patterns = (
        (extract_dict["ocram_size"], "01"),
        (extract_dict["dtcm_size"], "10"),
        (extract_dict["itcm_size"], "11"),
    )

    for size, pattern in bit_patterns:
        for _ in range(0, size, extract_dict["fsl_ram_bank_size"]):
            flexram_bank_cfg += pattern

    # Generate GPR Register config
    print(".equ __iomux_gpr14_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x38))
    print(".equ __iomux_gpr16_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x40))
    print(".equ __iomux_gpr17_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x44))
    print(
        ".equ __iomux_gpr17_value, 0x{:08X} /* {}k OCRAM, {}k DTCM, {}k ITCM */".format(
            int(flexram_bank_cfg, 2),
            extract_dict["ocram_size"] // 1024,
            extract_dict["dtcm_size"] // 1024,
            extract_dict["itcm_size"] // 1024,
        )
    )


def mimxrt_106x_gen_code(extract_dict):
    flexram_bank_cfg = "0b"
    avail_flexram = extract_dict["fsl_ram_bank_size"] * extract_dict["fsl_bank_nbr"]
    flexram_configurable_ocram = (
        extract_dict["ocram_size"] % 524288
    )  # 512kB OCRAM are not part of FlexRAM configurable memory

    if (
        flexram_configurable_ocram + extract_dict["dtcm_size"] + extract_dict["itcm_size"]
    ) > avail_flexram:
        raise ValueError("Configuration exceeds available FlexRAM!")

    for size, pattern in (
        (flexram_configurable_ocram, "01"),
        (extract_dict["dtcm_size"], "10"),
        (extract_dict["itcm_size"], "11"),
    ):
        for _ in range(0, size, extract_dict["fsl_ram_bank_size"]):
            flexram_bank_cfg += pattern

    # Generate GPR Register config
    print(".equ __iomux_gpr14_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x38))
    print(".equ __iomux_gpr16_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x40))
    print(".equ __iomux_gpr17_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x44))
    print(
        ".equ __iomux_gpr17_value, 0x{:08X} /* {}k OCRAM (512k OCRAM, {}k from FlexRAM), {}k DTCM, {}k ITCM */".format(
            int(flexram_bank_cfg, 2),
            extract_dict["ocram_size"] // 1024,
            flexram_configurable_ocram // 1024,
            extract_dict["dtcm_size"] // 1024,
            extract_dict["itcm_size"] // 1024,
        )
    )


def mimxrt_1176_gen_code(extract_dict):
    flexram_bank_cfg = "0b"
    avail_flexram = extract_dict["fsl_ram_bank_size"] * extract_dict["fsl_bank_nbr"]
    flexram_configurable_ocram = (
        extract_dict["ocram_size"] % 524288
    )  # 512kB OCRAM are not part of FlexRAM configurable memory

    if (
        flexram_configurable_ocram + extract_dict["dtcm_size"] + extract_dict["itcm_size"]
    ) > avail_flexram:
        raise ValueError("Configuration exceeds available FlexRAM!")

    for size, pattern in (
        (flexram_configurable_ocram, "01"),
        (extract_dict["dtcm_size"], "10"),
        (extract_dict["itcm_size"], "11"),
    ):
        for _ in range(0, size, extract_dict["fsl_ram_bank_size"]):
            flexram_bank_cfg += pattern

    # Generate GPR Register config
    print(".equ __iomux_gpr14_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x38))
    print(".equ __iomux_gpr16_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x40))
    print(".equ __iomux_gpr17_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x44))
    print(".equ __iomux_gpr18_adr, 0x{:08X}".format(extract_dict["gpr_base_addr"] + 0x48))
    print(
        ".equ __iomux_gpr17_value, 0x{:08X} /* {}k OCRAM (512k OCRAM, {}k from FlexRAM), {}k DTCM, {}k ITCM */".format(
            int(flexram_bank_cfg, 2) & 0xFFFF,
            extract_dict["ocram_size"] // 1024,
            flexram_configurable_ocram // 1024,
            extract_dict["dtcm_size"] // 1024,
            extract_dict["itcm_size"] // 1024,
        )
    )
    print(".equ __iomux_gpr18_value, 0x{:08X}".format((int(flexram_bank_cfg, 2) >> 16) & 0xFFFF))


def main(defines_file, features_file, ld_script, controller):
    dispatcher = {
        "MIMXRT1011": (mimxrt_default_parser, mimxrt_default_gen_code),
        "MIMXRT1015": (mimxrt_default_parser, mimxrt_default_gen_code),
        "MIMXRT1021": (mimxrt_default_parser, mimxrt_default_gen_code),
        "MIMXRT1052": (mimxrt_default_parser, mimxrt_default_gen_code),
        "MIMXRT1062": (mimxrt_default_parser, mimxrt_106x_gen_code),
        "MIMXRT1064": (mimxrt_default_parser, mimxrt_106x_gen_code),
        "MIMXRT1176": (mimxrt_default_parser, mimxrt_1176_gen_code),
    }

    extractor, code_generator = dispatcher[controller]

    extract_dict = extractor(defines_file, features_file, ld_script)
    code_generator(extract_dict)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="make-flexram-ld.py",
        usage="%(prog)s [options] [command]",
        description="Evaluate FlexRAM configuration and generate startup code.",
    )
    parser.add_argument(
        "-d",
        "--defines_file",
        dest="defines_file",
        help="Path to MCU defines file",
        default="../../../lib/nxp_driver/sdk/devices/MIMXRT1021/MIMXRT1021.h",
    )
    parser.add_argument(
        "-f",
        "--features_file",
        dest="features_file",
        help="Path to MCU features file",
        default="../../../lib/nxp_driver/sdk/devices/MIMXRT1021/MIMXRT1021_features.h",
    )
    parser.add_argument(
        "-l",
        "--ld_file",
        dest="linker_file",
        help="Path to the aggregated linker-script",
        default="MIMXRT1021.ld",
    )
    parser.add_argument(
        "-c", "--controller", dest="controller", help="Controller name", default="MIMXRT1021"
    )
    #
    args = parser.parse_args()
    main(args.defines_file, args.features_file, args.linker_file, args.controller)
