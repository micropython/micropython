# This script will parse the build output files to extract build flags from the ModusToolbox
# build command that needs to be used as well in the MicroPython src builds.
# Ideally, this flags would be retievable directly from a Makefile command or variable of
# the ModusToolbox Makefile

import argparse, json, os


def get_file_content(file):
    with open(file, "r") as f:
        f_content = f.read()

    return f_content


def get_ccxx_build_flags(cycompiler_file):
    # Extract the flags located between '-c' option and
    # the first (.rsp) response file
    # Warning: is the response file a proper delimiter (?)
    def find_flags_start(build_cmd_list):
        return build_cmd_list.index("-c") + 1  # next is the first element

    def find_flags_end(build_cmd_list):
        file_opt = [item for item in build_cmd_list if item.startswith("@")]
        return build_cmd_list.index(file_opt[0])

    build_cmd = get_file_content(cycompiler_file)
    build_cmd_list = build_cmd.split()

    start_idx = find_flags_start(build_cmd_list)
    end_idx = find_flags_end(build_cmd_list)

    ccxx_flags = build_cmd_list[start_idx:end_idx]
    print(*ccxx_flags)


def get_ld_linker_flags(cylinker_file, mtb_libs_path):
    # Get flags only until the linker script after the compiler argument -T
    def find_flags_start(link_cmd_list):
        gcc_cmd = [item for item in link_cmd_list if item.endswith("arm-none-eabi-g++")]
        return link_cmd_list.index(gcc_cmd[0]) + 1

    def find_flags_end(link_cmd_list):
        link_script_file_param = [item for item in link_cmd_list if item.startswith("-T")]
        return link_cmd_list.index(link_script_file_param[0]) + 1

    def set_path_of_linker_script(link_cmd_list, inker_script_param_index, mtb_libs_path):
        link_cmd_list[inker_script_param_index] = (
            "-T" + str(mtb_libs_path) + "/" + link_cmd_list[inker_script_param_index][len("-T") :]
        )

    link_cmd = get_file_content(cylinker_file)
    link_cmd_list = link_cmd.split()

    start_idx = find_flags_start(link_cmd_list)
    end_idx = find_flags_end(link_cmd_list)
    set_path_of_linker_script(link_cmd_list, end_idx - 1, mtb_libs_path)

    ld_flags = link_cmd_list[start_idx:end_idx]
    print(*ld_flags)


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parser_get_ccxxflags_func(args):
        get_ccxx_build_flags(args.cycompiler_file)

    def parser_get_ldflags_func(args):
        get_ld_linker_flags(args.cylinker_file, args.mtb_libs_path)

    parser = argparse.ArgumentParser(description="Utility to retrieve ModusToolbox build info")
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    parser_ccxx = subparser.add_parser("ccxxflags", description="Get C and CXX build flags")
    parser_ccxx.add_argument("cycompiler_file", type=str, help=".cycompiler file")
    parser_ccxx.set_defaults(func=parser_get_ccxxflags_func)

    parser_ld = subparser.add_parser("ldflags", description="Get linker flags")
    parser_ld.add_argument("cylinker_file", type=str, help=".cylinker file")
    parser_ld.add_argument("mtb_libs_path", type=str, help="Path to mtb-libs folder")
    parser_ld.set_defaults(func=parser_get_ldflags_func)

    # Parser call
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    parser()
