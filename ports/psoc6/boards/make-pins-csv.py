from __future__ import print_function
from itertools import islice
import argparse
import sys
import csv
import re
import os


def get_pin_addr_helper(pin_def):
    pattern = r"CYHAL_PORT_(\d+),\s*(\d+)"
    match = re.search(pattern, pin_def)
    port_number = match.group(1)
    pin_number = match.group(2)
    return (int(port_number) << 3) + int(pin_number)


def get_pin_package_path(filename):
    root_dir = "./mtb_shared/mtb-hal-cat1"
    mid_dir = "COMPONENT_CAT1A/include/pin_packages"
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for dirname in dirnames:
            if dirname.startswith("release-"):
                release_version = dirname
                file_path = os.path.join(root_dir, release_version, mid_dir, filename)
                if os.path.isfile(file_path):
                    return file_path
    return None


def generate_pins_csv(pin_package_filename, pins_csv_filename):
    file_path = get_pin_package_path(pin_package_filename)

    if file_path is None:
        sys.exit(1)

    # Read the header file
    with open(file_path, "r") as file:
        content = file.read()

    # Find the starting and ending points of the enum declaration
    enum_start = content.find("typedef enum {")
    enum_end = content.find("}")

    if enum_start != -1 and enum_end != -1:
        enum_content = content[enum_start:enum_end]

        # Extract enum values using regex
        enum_values = re.findall(r"\b(\w+)\s*=", enum_content)
        # Write enum values to a CSV file
        with open("./" + pins_csv_filename, "w", newline="") as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerows(
                [
                    [value, value]
                    for value in enum_values
                    if value.startswith("P") or value.startswith("N")
                ]
            )
        print("// pins.csv generated successfully")
    else:
        print("// Error: pins.csv generation failed")


def generate_af_pins_csv(pin_package_filename, pins_af_csv_filename):
    file_path = get_pin_package_path(pin_package_filename)

    if file_path is None:
        sys.exit(1)
    # Read the header file
    with open(file_path, "r") as file:
        content = file.read()

    # Find the starting and ending points of the enum declaration
    enum_start = content.find("typedef enum {")
    enum_end = content.find("}")

    if enum_start != -1 and enum_end != -1:
        enum_content = content[enum_start:enum_end]

        # Extract enum values using regex
        pin_name = re.findall(r"\b(?!NC\b)(\w+)\s*=", enum_content)
        # pin_name = re.findall(r"\b(\w+)\s*=", enum_content)
        pin_def = re.findall(r"=\s*(.*?\))", enum_content)
        # pin_def.insert(0, "255")
        # print(pin_def)

        # Write enum values to a CSV file
        with open("./" + pins_af_csv_filename, "w", newline="") as csv_file:
            NC_pin = ["NC", 255, "CYHAL_GET_GPIO(CYHAL_PORT_31, 7)"]  # non-existent port
            csv_writer = csv.writer(csv_file)
            csv_writer.writerow(NC_pin)
            for pname, pdef in zip(pin_name, pin_def):
                # for pname, pdef in zip(islice(pin_name, 1, None), pin_def):
                # if pin_name[pname].startswith('P'):
                val = get_pin_addr_helper(pdef)
                csv_writer.writerow([pname, val, pdef.strip('"')])

        print("// pins_af.csv generated successfully")
    else:
        print("Error: pins_af.csv generation failed")


def main():
    parser = argparse.ArgumentParser(
        prog="make-pins-csv.py",
        usage="%(prog)s [options] [command]",
        description="Generate intermediate board specific pin csv files to be used by make-pins script",
    )

    parser.add_argument(
        "-g",
        "--gen-pin-for",
        dest="pin_package_filename",
        help="Specifies the pin package file from mtb assets to generate pins.csv",
    )

    parser.add_argument(
        "-p",
        "--save-pins-csv-at",
        dest="pins_csv",
        help="Specifies name of generated pins csv file",
    )

    parser.add_argument(
        "-gaf",
        "--save-pins-af-csv-at",
        dest="pins_af_csv",
        help="Specifies name of generated alternate functions pins csv file",
    )

    args = parser.parse_args(sys.argv[1:])

    if args.pin_package_filename and args.pins_csv:
        print("// Generating pins csv file")
        print("// - --gen-pin-for {:s}".format(args.pin_package_filename))
        print("// - --save-pins-csv-at {:s}".format(args.pins_csv))
        generate_pins_csv(args.pin_package_filename, args.pins_csv)

    if args.pin_package_filename and args.pins_af_csv:
        print("// Generating alternate functions pins csv file")
        print("// - --save-pins-csv-at {:s}".format(args.pins_af_csv))
        generate_af_pins_csv(args.pin_package_filename, args.pins_af_csv)


if __name__ == "__main__":
    main()
