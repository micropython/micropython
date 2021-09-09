#!/usr/bin/python
import sys
import struct
import argparse
import binascii
from collections import namedtuple
from pprint import pprint


FirmwareImageHeader = namedtuple(
    "FirmwareImageHeader",
    "magic crc size entry_addr header_crc",
)

format_specifier_header = "<4s4xIII8xI"


def convert_size(byte_size):
    if byte_size > (1024 * 1024 * 1024):
        format_specifier = "GB"
        byte_size = byte_size / (1024 * 1024 * 1024)
    elif byte_size > (1024 * 1024):
        format_specifier = "MB"
        byte_size = byte_size / (1024 * 1024)
    elif byte_size > 1024:
        format_specifier = "KB"
        byte_size = byte_size / 1024
    else:
        format_specifier = "B"
        byte_size = byte_size

    return "{0}{1}".format(
        str(round(byte_size, 1) if byte_size % 1 else int(byte_size)), format_specifier
    )


def patch_binary(bin_data, offset, fill_to):
    format_specifier_header = "<4s4xIII8xI"
    format_specifier_header_crc = format_specifier_header[:-1]
    header_size = struct.calcsize(format_specifier_header)
    #
    raw_header = FirmwareImageHeader._make(
        struct.unpack(format_specifier_header, bin_data[0:header_size])
    )
    # Set image size attribute
    size = len(bin_data)
    crc = binascii.crc32(bin_data[header_size:]) & 0xFFFFFFFF

    patched_header = struct.pack(
        format_specifier_header_crc,
        raw_header.magic,
        crc,
        size,
        raw_header.entry_addr,
    )
    header_crc = binascii.crc32(patched_header)
    patched_header += struct.pack("<I", header_crc)

    # Create processed header
    patched_header_dict = FirmwareImageHeader(
        magic=raw_header.magic,
        crc=crc,
        size=size,
        entry_addr=raw_header.entry_addr,
        header_crc=header_crc,
    )._asdict()
    print("Header Fields:")
    print("\t{0:20s}: {1}".format("magic", str(patched_header_dict["magic"], "utf8")))
    print(
        "\t{0:20s}: 0x{1:08X} (range: [0x{2:08X}, 0x{3:08X}])".format(
            "crc",
            patched_header_dict["crc"],
            offset + header_size,
            offset + (len(bin_data) - header_size),
        )
    )
    print(
        "\t{0:20s}: {1} ({2:d})".format(
            "size", convert_size(patched_header_dict["size"]), patched_header_dict["size"]
        )
    )
    print("\t{0:20s}: 0x{1:08X}".format("entry_addr", patched_header_dict["entry_addr"]))
    print(
        "\t{0:20s}: 0x{1:08X} (range: [0x{2:08X}, 0x{3:08X}])".format(
            "header_crc", patched_header_dict["header_crc"], offset, offset + header_size
        )
    )

    # Create patched binary
    return patched_header + bin_data[header_size:fill_to]


def path_binary_main(in_path, out_path, fill_to):
    with open(in_path, "rb") as bin_file:
        bin_data = bin_file.read()

    print("Firmware Header:")

    header_offset = bin_data.find(b"MPFW")
    print("\t{0:20s}: 0x{1:08X}".format("offset", header_offset))

    patched_bin_data = patch_binary(bin_data[header_offset:], header_offset, fill_to)

    print("Output binary:\n\t{0:20s}: 0x{1:08X}".format("size", len(patched_bin_data)))

    with open(out_path, "wb") as bin_file:
        bin_file.write(patched_bin_data)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("in_path", help="Path of input binary")
    parser.add_argument("out_path", help="Path of output binary")
    parser.add_argument("fill_to", type=int, help="Fill output up to N bytes")
    parser.add_argument("buffer_size", type=int, help="Size of download buffer")
    args = parser.parse_args()
    #
    path_binary_main(args.in_path, args.out_path, args.fill_to)
