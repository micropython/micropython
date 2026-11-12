#!/usr/bin/env python3
# Combine boot.bin and app.bin into a single firmware.bin for flashing.
#
# The flash layout is read from the Kconfig .config produced by the build so
# this script works across all Ameba SoCs without hard-coded addresses.
#
# Usage:
#   python makeimg.py <.config> <boot.bin> <app.bin> <firmware.bin>
#
# firmware.bin layout:
#   [0x000000]  boot.bin  padded to boot region size (0xFF fill)
#   [boot_size] app.bin   written verbatim

import os
import sys


def load_config(path):
    config = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, _, val = line.partition("=")
            config[key.strip()] = val.strip()
    return config


def parse_hex(s):
    s = s.strip()
    return int(s, 16) if s.lower().startswith("0x") else int(s)


def main():
    if len(sys.argv) != 5:
        print(
            f"usage: {os.path.basename(sys.argv[0])} <.config> <boot.bin> <app.bin> <firmware.bin>"
        )
        sys.exit(1)

    arg_config = sys.argv[1]
    arg_boot_bin = sys.argv[2]
    arg_app_bin = sys.argv[3]
    arg_output = sys.argv[4]

    for path in (arg_config, arg_boot_bin, arg_app_bin):
        if not os.path.exists(path):
            print(f"error: file not found: {path}")
            sys.exit(1)

    config = load_config(arg_config)

    try:
        boot_offset = parse_hex(config["CONFIG_FLASH_BOOT_OFFSET"])
        boot_size = parse_hex(config["CONFIG_FLASH_BOOT_SIZE"])
        app_offset = parse_hex(config["CONFIG_FLASH_OTA1_OFFSET"])
    except KeyError as e:
        print(f"error: missing key in .config: {e}")
        sys.exit(1)

    # The XIP base equals CONFIG_FLASH_BOOT_OFFSET because the bootloader
    # always sits at the very start of flash.  All other offsets are relative
    # to this base.
    xip_base = boot_offset
    boot_phys = 0  # boot starts at flash offset 0
    app_phys = app_offset - xip_base  # physical offset of app in flash

    if app_phys < boot_size:
        print(
            f"error: app region (0x{app_phys:06x}) overlaps boot region (size 0x{boot_size:06x})"
        )
        sys.exit(1)

    pad_to = app_phys  # boot.bin is padded up to this offset

    with open(arg_boot_bin, "rb") as f:
        boot_data = f.read()

    with open(arg_app_bin, "rb") as f:
        app_data = f.read()

    if len(boot_data) > pad_to:
        print(
            f"error: boot.bin ({len(boot_data)} bytes) exceeds boot region "
            f"(0x{pad_to:06x} = {pad_to} bytes)"
        )
        sys.exit(1)

    padding = b"\xff" * (pad_to - len(boot_data))

    with open(arg_output, "wb") as f:
        f.write(boot_data)
        f.write(padding)
        f.write(app_data)

    total = pad_to + len(app_data)
    print(f"  MAKEIMG  {os.path.basename(arg_output)}")
    print(
        f"           boot @ 0x{boot_phys:06x}  {len(boot_data):>7} B  "
        f"(padded to 0x{pad_to:06x} = {pad_to // 1024} KB)"
    )
    print(f"           app  @ 0x{app_phys:06x}  {len(app_data):>7} B")
    print(f"           total          {total:>7} B  ({total / 1024:.1f} KB)")


if __name__ == "__main__":
    main()
