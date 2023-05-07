import csv
import argparse
import string


def parse_pins(filename):
    """Parse CSV file given in filename.
    The CSV file should have the following columns:
    mcu_name, board_name, port (integer), pin (integer)
    The CSV file should also have a header row"""
    pins = {}
    with open(filename, newline="") as csv_file:
        reader = csv.reader(csv_file)
        reader.__next__()
        for row in reader:
            entry = (row[0].strip(), row[1].strip(), int(row[2].strip()), int(row[3].strip()))
            pins[row[0].lower()] = entry
    return pins


def parse_pin_functions(filename):
    """Parse a CSV file with peripheral pin mappings.
    The CSV file should have the following columns
    func_name, LOC0,LOC1,...,LOC31
    There should not be any header row"""
    functions = {}
    with open(filename, newline="") as csv_file:
        reader = csv.reader(csv_file)
        for row in reader:
            entry = row[1:]
            functions[row[0].strip()] = entry
    return functions


def make_pin_name(pin):
    """Create pin name"""
    return "pin_" + pin[0]


def make_mcu_dict_entry(pin):
    """Create a pin mcu dictionary entry"""
    entry = "{ MP_ROM_QSTR(MP_QSTR_" + pin[0] + "), MP_ROM_PTR(&" + make_pin_name(pin) + ") },"
    return entry


def make_mcu_dict_entry2(pin):
    """Create a pin mcu dictionary entry"""
    entry = (
        "{ MP_ROM_QSTR(MP_QSTR_" + pin[1] + "), \t\t\tMP_ROM_PTR(&" + make_pin_name(pin) + ") },"
    )
    return entry


def make_mcu_dict(pins):
    """Create the mcu dictionary"""
    decl = "\n\nSTATIC const mp_rom_map_elem_t board_module_globals_table[] = {\n"
    decl += "\tCIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS\n"
    for pin in pins.values():
        decl += "\t" + make_mcu_dict_entry(pin) + "\n"

    decl += "\n"
    for pin in pins.values():
        if pin[1] != "":
            decl += "\t" + make_mcu_dict_entry2(pin) + "\n"

    decl += "\t{ MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },\n"
    decl += "\t{ MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },\n"
    decl += "};\n"
    decl += "MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);\n"
    return decl


def make_board_dict_entry(pin):
    """Create a pin board dictionary entry"""
    entry = "{ MP_OBJ_NEW_QSTR(MP_QSTR_" + pin[1] + "), (mp_obj_t)&" + make_pin_name(pin) + " },"
    return entry


def make_pin_function_list_decl(pin, fcns):
    """Create a pin function list declaration"""
    decl = "\nconst uint8_t pin_" + pin + "_functions[] = { \n"
    if len(fcns) > 0:
        decl += str(fcns[0])
        for i in fcns[1:]:
            decl += ", " + str(i)
    decl += "\n};\n"
    return decl


def make_pin_declaration(pin):
    """Create a pin declaration"""
    decl = (
        "\nconst mcu_pin_obj_t "
        + make_pin_name(pin)
        + " = PIN("
        + str(pin[2])
        + ","
        + str(pin[3])
        + ","
        + make_pin_name(pin).lower()
        + "_functions"
        ");"
    )
    return decl


def define_pin_exists(pin):
    ret = "\n#define GPIO_" + pin[0] + "_EXISTS\t1"
    return ret


def make_pin_function_lists(functions, pins):
    """Create lists of pin functions from the parsed CSV data"""
    fcn_list = {}
    decl = ""
    i = 0
    for fcn, fcn_pins in sorted(functions.items()):
        for j in range(0, len(fcn_pins)):
            pin = fcn_pins[j].lower()
            if pin == "":
                continue
            if pin not in fcn_list:
                fcn_list[pin] = [255] * len(functions)
            fcn_list[pin][i] = 1
        i += 1
    for pin in pins.keys():
        if not pin in fcn_list:
            fcn_list[pin] = []

        decl += make_pin_function_list_decl(pin, fcn_list[pin])

    return decl


def make_source_file(src_file, pins, fcn):
    """Make pins.c"""
    with open(src_file, "w") as f:
        f.write('#include "shared-bindings/board/__init__.h"\n')
        f.write('#include "pin_functions.h"\n')

        f.write(make_pin_function_lists(fcn, pins))

        for pin in pins.values():
            f.write(make_pin_declaration(pin))
        f.write(make_mcu_dict(pins))
        f.close()
    return


def make_header_file(hdr_file, pins, fcns):
    """Make pins.h"""
    hdr_file_name = hdr_file.split("/")[-1]
    guard_name = "__" + hdr_file_name.replace(".", "_").upper() + "__"
    with open(hdr_file, "w") as f:
        f.write("#ifndef " + guard_name + "\n")
        f.write("#define " + guard_name + "\n\n")

        fcn_names = sorted(fcns.keys())
        for i in range(len(fcn_names)):
            f.write("#define " + "FN_" + fcn_names[i] + "\t\t\t(" + str(i) + ")\n")

        for pin in pins.values():
            f.write(define_pin_exists(pin))

        f.write("\n\n")
        f.write("\n\n#endif /*" + guard_name + "*/\n")
        f.close()

    return


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="make_pins",
        usage="%(prog)s [options]",
        description="Parse a CSV file with pin data and store as corresponding C source and header files.",
    )
    parser.add_argument(
        "-s",
        "--source_file",
        dest="source_file",
        action="store",
        help="Name of the output source file",
        default="pins.c",
    )
    parser.add_argument(
        "-e",
        "--header_file",
        dest="header_file",
        action="store",
        help="Name of the output header file",
        default="pin_functions.h",
    )

    parser.add_argument(
        "csv_file",
        action="store",
        help="Name of the input csv file",
    )
    parser.add_argument(
        "fcn_csv",
        action="store",
        help="Name of the csv file with pin functions",
    )
    args = parser.parse_args()

    src_file = args.source_file
    hdr_file = args.header_file

    csv_file = args.csv_file
    fcn_csv = args.fcn_csv

    pins = parse_pins(csv_file)
    fcns = parse_pin_functions(fcn_csv)
    make_source_file(src_file, pins, fcns)
    make_header_file(hdr_file, pins, fcns)
