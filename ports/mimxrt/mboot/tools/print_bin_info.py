#!/usr/bin/python
import argparse
import struct
from collections import namedtuple


BootData = namedtuple(
    "BootData",
    "start_location size plugin_flag",
)

ImageVectorTable = namedtuple(
    "ImageVectorTable", "header entry dcd_addr boot_data_addr self csf_addr"
)


def print_binary_info(bin_path, ivt_offset, boot_data_offset):
    with open(bin_path, "rb") as bin_file:
        bin_data = bin_file.read()

    ivt = ImageVectorTable._make(
        struct.unpack("<II4xIIII4x", bin_data[ivt_offset : (ivt_offset + 32)])
    )
    print(type(ivt).__name__)
    print(" " * 4, "{0:15s}:".format("header"), "0x{:08X}".format(ivt.header))
    print(" " * 4, "{0:15s}:".format("entry"), "0x{:08X}".format(ivt.entry))
    print(" " * 4, "{0:15s}:".format("dcd_addr"), "0x{:08X}".format(ivt.dcd_addr))
    print(" " * 4, "{0:15s}:".format("boot_data_addr"), "0x{:08X}".format(ivt.boot_data_addr))
    print(" " * 4, "{0:15s}:".format("self"), "0x{:08X}".format(ivt.self))
    print(" " * 4, "{0:15s}:".format("csf_addr"), "0x{:08X}".format(ivt.csf_addr))

    boot_data = BootData._make(
        struct.unpack("<III4x", bin_data[boot_data_offset : (boot_data_offset + 16)])
    )
    print(type(boot_data).__name__)
    print(
        " " * 4, "{0:15s}:".format("start_location"), "0x{:08X}".format(boot_data.start_location)
    )
    print(" " * 4, "{0:15s}:".format("size"), "0x{:08X}".format(boot_data.size))
    print(" " * 4, "{0:15s}:".format("plugin_flag"), "0x{:08X}".format(boot_data.plugin_flag))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("bin_path", help="Input path of binary file")
    parser.add_argument("ivt_offset", help="Offset to IVT inside of image")
    parser.add_argument("boot_data_offset", help="Offset to boot data inside of image")
    args = parser.parse_args()
    #
    print_binary_info(
        args.bin_path, int(args.ivt_offset, base=16), int(args.boot_data_offset, base=16)
    )
