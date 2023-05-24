"""
This script reads in the given CMSIS device include file (eg stm32f405xx.h),
extracts relevant peripheral constants, and creates qstrs, mpz's and constants
for the stm module.
"""

# Use a pattern rule here so that make will only call make-stmconst.py once to
# make both modstm_const.h and modstm_qstr.h
# $(HEADER_BUILD)/%_const.h $(BUILD)/%_qstr.h: $(CMSIS_MCU_HDR) make-stmconst.py | $(HEADER_BUILD)
#         $(ECHO) "GEN stmconst $@"
#         $(Q)$(PYTHON) make-stmconst.py --qstr $(GEN_STMCONST_QSTR) --mpz $(GEN_STMCONST_MPZ) $(CMSIS_MCU_HDR) > $(GEN_STMCONST_HDR)

from __future__ import print_function

import argparse
import re

# Python 2/3 compatibility
import platform

def parse_file(filename):

    regex_pattern = '(\w+)\s\=\s*(?:"([^"]*)"|(\S+))'

    with open(filename, 'r') as file:
        content = file.read()

    shared_variables = {}

    matches = re.findall(regex_pattern, content)

    for m in matches:

        variable = m[0]
        address = m[2]
        # Check if the key starts with "ulp_"
        if variable.startswith("ulp_"):
            # Remove "ulp_" from the beginning of the key
            variable = variable[4:].upper()

            shared_variables[variable.strip()] = address.strip()

    return shared_variables

def main():
    cmd_parser = argparse.ArgumentParser(description="Extract ULP constants from a C header file.")
    cmd_parser.add_argument("file", nargs=1, help="input file")
    cmd_parser.add_argument(
        "-q",
        "--qstr",
        dest="qstr_filename",
        default="build/esp32_ulpconst_qstr.h",
        help="Specified the name of the generated qstr header file",
    )
    # cmd_parser.add_argument(
    #     "--mpz",
    #     dest="mpz_filename",
    #     default="build/esp32_ulpconst_mpz.h",
    #     help="the destination file of the generated mpz header",
    # )
    args = cmd_parser.parse_args()

    shared_variables = parse_file(args.file[0])

    print("// Automatically generated from %s by make-esp32ulpconst.py" % args.file[0])
    print("")

    with open("../../genhdr/esp32_ulpconst_qstr.h", "wt") as h_file:
        for key, value in shared_variables.items():
            assert 0 <= int(value, 16) <= 0xFFFFFFFF
            print(
                "STATIC const mp_obj_int_t mpz_%08x = {{&mp_type_int}, "
                "{.neg=0, .fixed_dig=1, .alloc=2, .len=2, "
                ".dig=(uint16_t*)(const uint16_t[]){%#x, %#x}}};"
                % (int(value, 16), int(value, 16) & 0xFFFF, (int(value, 16) >> 16) & 0xFFFF),
                file=h_file,
            )

    with open("../../genhdr/esp32_ulpconst_mpz.h", "wt") as mpz_file:
        for key, value in shared_variables.items():
            print(
                "{MP_ROM_QSTR(MP_QSTR_%s), MP_ROM_PTR( & mpz_%08x)},"
                % (key, int(value, 16)),
                file=mpz_file,
            )

    #{MP_ROM_QSTR(MP_QSTR_VOLTAGE), MP_ROM_PTR( & mpz_50000130)},

    return shared_variables

if __name__ == "__main__":
    main()
