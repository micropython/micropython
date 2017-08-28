#!/usr/bin/env python

from __future__ import print_function

import subprocess
import argparse
import json
import errno
import os
import shutil
import fnmatch
import hashlib


ARDUINO_H = """/**
 * This is a generated file required by the Arduino build system."
 */

#include "pumbaa.h"
"""

SIMBA_GEN_C_FMT = """
#include "simba.h"

const FAR char sysinfo[] = "app:    myapp built - by -.\\r\\n"
                           "board:  {board}\\r\\n"
                           "mcu:    {mcu}\\r\\n";
"""

INTERACTIVE_INO = """#
# The interactive interpreter starts automatically 
# when this script ends.
#
"""

def mkdir_p(path):
    """Recursivly create directories.

    """

    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def create_database():
    """Generate the pumbaa database with baord and mcu information.

    """

    return json.loads(subprocess.check_output(["bin/dbgen.py"]))


def copy_tools():
    """Copy the frozen generation scripts to the tools folder.

    """

    pumbaa_root = os.environ["PUMBAA_ROOT"]

    os.makedirs("tools")
    shutil.copy(os.path.join(pumbaa_root, "bin", "compile_ino.py"), "tools")
    shutil.copy(os.path.join(pumbaa_root, "bin", "make_frozen.py"), "tools")

    
def generate_cores(family, database):
    """Generate the cores directory, shared among all boards.

    """

    cores_dir = os.path.join("cores", "pumbaa")

    # Create the cores directory.
    mkdir_p(cores_dir)

    with open(os.path.join(cores_dir, "Arduino.h"), "w") as fout:
        fout.write(ARDUINO_H)
        
    pumbaa_root = os.environ["PUMBAA_ROOT"]
    cores_srcs = None

    for board in database["boards"].values():
        mcu = board["mcu"]

        if database["mcus"][mcu]["family"] != family:
            continue

        if cores_srcs is None:
            cores_srcs = set(board["src"])
        else:
            cores_srcs = cores_srcs & set(board["src"])

    if family == "avr":
        board = "arduino_pro_micro"
    elif family == "sam":
        board = "arduino_due"
    elif family == "esp":
        board = "esp01"
    else:
        raise ValueError("{}: bad family".format(family))

    # Copy all source files, except those in boards and mcus that are
    # variant specific. Use any board in given family
    for src in cores_srcs:
        dst_dir = os.path.join(cores_dir, os.path.dirname(src))
        mkdir_p(dst_dir)
        shutil.copy(os.path.join(pumbaa_root, src), dst_dir)

    # Copy all header files.
    for inc in database["boards"][board]["inc"]:
        inc_dir = os.path.join(pumbaa_root, inc)
        for root, _, filenames in os.walk(inc_dir):
            for filename in fnmatch.filter(filenames, '*.[hi]'):
                file_path = os.path.join(root, filename)
                file_dir = os.path.dirname(file_path)
                cores_file_dir = file_dir.replace(pumbaa_root + "/", "")
                mkdir_p(os.path.join(cores_dir, cores_file_dir))
                shutil.copy(file_path,
                            os.path.join(cores_dir, cores_file_dir))

    # Various files.
    root_files = [
        "LICENSE",
        "README.rst",
        "VERSION.txt"
    ]

    for root_file in root_files:
        shutil.copy(os.path.join(pumbaa_root, root_file), ".")

    return cores_srcs


def generate_variants(family, database, cores_srcs):
    """Generate the variants directory with board unique information.

    """

    pumbaa_root = os.environ["PUMBAA_ROOT"]

    print("Generating variants for family", family)

    for board_name, config in database['boards'].items():
        if database["mcus"][config["mcu"]]["family"] != family:
            continue

        variant_dir = os.path.join("variants", board_name)

        # Create the variant directory.
        mkdir_p(variant_dir)

        # Copy variant specific source files; those in "boards" and
        # "mcus". Other source files are copies in cores.
        for src in config["src"]:
            if src in cores_srcs:
                continue
            dst_dir = os.path.join(variant_dir, os.path.dirname(src))
            mkdir_p(dst_dir)
            shutil.copy(os.path.join(pumbaa_root, src), dst_dir)

        # Copy all linker script files.
        for libpath in config["libpath"]:
            libpath_dir = os.path.join(pumbaa_root, libpath)
            for root, _, filenames in os.walk(libpath_dir):
                for filename in fnmatch.filter(filenames, '*.ld'):
                    file_path = os.path.join(root, filename)
                    file_dir = os.path.dirname(file_path)
                    variant_file_dir = file_dir.replace(pumbaa_root + "/",
                                                        "")
                    mkdir_p(os.path.join(variant_dir, variant_file_dir))
                    shutil.copy(file_path,
                                os.path.join(variant_dir, variant_file_dir))

        with open(os.path.join(variant_dir, "pumbaa_gen.c"), "w") as fout:
            fout.write(SIMBA_GEN_C_FMT.format(name="my_app",
                                              board=board_name,
                                              mcu=config["mcu"]))

        # Generate the QSTR file for the default configuration.
        default_configuration_dir = os.path.join(pumbaa_root,
                                                 "examples",
                                                 "default-configuration")
        subprocess.check_call(["make",
                               "-s",
                               "BOARD=" + board_name],
                              cwd=default_configuration_dir)
        qstr_file = os.path.join(default_configuration_dir,
                                 "build",
                                 board_name,
                                 "genhdr",
                                 "qstrdefs.generated.h")
        genhdr_dir = os.path.join(variant_dir, "genhdr")
        os.makedirs(genhdr_dir)
        shutil.copy(qstr_file, genhdr_dir)
        

def generate_examples():
    """Generate the examples directory.

    libraries/Pumbaa/examples/<example folder>

    """

    pumbaa_root = os.environ["PUMBAA_ROOT"]

    pumbaa_examples_dir = os.path.join(pumbaa_root, 'examples')
    arduino_pumbaa_path = os.path.join('libraries', 'Pumbaa')
    arduino_examples_path = os.path.join(arduino_pumbaa_path, 'examples')

    os.makedirs(arduino_examples_path)

    with open(os.path.join(arduino_pumbaa_path, "Pumbaa.h"), "w") as fout:
        fout.write("/* Generated file required by Arduino IDE. */")

    examples = [
        "blink",
        "hello_world",
        "interactive"
    ]

    for example in examples:
        # Create the .ino-file.
        pumbaa_example_path = os.path.join(pumbaa_examples_dir, example)
        arduino_example_path = os.path.join(arduino_examples_path, example)
        os.makedirs(arduino_example_path)
        main_py = os.path.join(pumbaa_example_path, "main.py")
        ino_file = os.path.join(arduino_example_path, example + ".ino")
        with open(ino_file, "w") as fout:
            if os.path.exists(main_py):
                with open(main_py) as fin:
                    fout.write(fin.read())
            else:
                fout.write(INTERACTIVE_INO)


def get_c_extra_flags(board, database):
    """Get include path, defines and flags to the compiler.

    """

    incs = database["boards"][board]["inc"]
    cdefs = database["boards"][board]["cdefs"]
    cflags = database["boards"][board]["cflags"]

    return " ".join(cflags
                    + ["\"-I{runtime.platform.path}/cores/pumbaa/" + inc + "\""
                       for inc in incs]
                    + ["-D" + d for d in cdefs])


def get_cxx_extra_flags(board, database):
    """Get include path, defines and flags to the compiler.

    """

    incs = database["boards"][board]["inc"]
    cdefs = database["boards"][board]["cdefs"]
    cxxflags = database["boards"][board]["cxxflags"]

    return " ".join(cxxflags
                    + ["\"-I{runtime.platform.path}/cores/pumbaa/" + inc + "\""
                       for inc in incs]
                    + ["-D" + d for d in cdefs])


def get_c_elf_extra_flags(board, database):
    """Get library path, defines and flags to the linker.

    """

    libpaths = database["boards"][board]["libpath"]
    ldflags = database["boards"][board]["ldflags"]

    ldflags = [ldflag for ldflag in ldflags if "-Wl,-Map" not in ldflag]

    return " ".join(ldflags
                    + ["\"-L{runtime.platform.path}/variants/" + board + "/" + libpath + "\""
                       for libpath in libpaths])


def get_c_elf_extra_flags_after(board, database):
    """Get library path, defines and flags to the linker.

    """

    ldflags_after = database["boards"][board]["ldflags_after"]

    return " ".join(ldflags_after)


def generate_boards_txt_avr(database, boards_txt_fmt):
    """Generate boards.txt for AVR.

    """

    return boards_txt_fmt.format(
        mega2560_compiler_c_extra_flags=get_c_extra_flags("arduino_mega",
                                                          database),
        nano_compiler_c_extra_flags=get_c_extra_flags("arduino_nano",
                                                      database),
        uno_compiler_c_extra_flags=get_c_extra_flags("arduino_uno",
                                                     database),
        pro_micro_compiler_c_extra_flags=get_c_extra_flags("arduino_pro_micro",
                                                           database), 
        mega2560_compiler_cxx_extra_flags=get_cxx_extra_flags("arduino_mega",
                                                              database),
        nano_compiler_cxx_extra_flags=get_cxx_extra_flags("arduino_nano",
                                                          database),
        uno_compiler_cxx_extra_flags=get_cxx_extra_flags("arduino_uno",
                                                         database),
        pro_micro_compiler_cxx_extra_flags=get_cxx_extra_flags("arduino_pro_micro",
                                                               database))


def generate_boards_txt_sam(database, boards_txt_fmt):
    """Generate boards.txt for SAM.

    """

    return boards_txt_fmt.format(
        arduino_due_x_dbg_compiler_c_extra_flags=get_c_extra_flags(
            "arduino_due",
            database),
        arduino_due_x_dbg_compiler_cxx_extra_flags=get_cxx_extra_flags(
            "arduino_due",
            database),
        arduino_due_x_dbg_compiler_c_elf_extra_flags=get_c_elf_extra_flags(
            "arduino_due",
            database))


def generate_boards_txt_esp(database, boards_txt_fmt):
    """Generate boards.txt for ESP.

    """

    # ESP SDK libraries are copied to this location.
    libpath = "-L{runtime.platform.path}/lib"

    esp01_compiler_c_elf_extra_flags = get_c_elf_extra_flags("esp01", database)
    esp01_compiler_c_elf_extra_flags += " "
    esp01_compiler_c_elf_extra_flags += libpath

    esp12e_compiler_c_elf_extra_flags = get_c_elf_extra_flags("esp12e", database)
    esp12e_compiler_c_elf_extra_flags += " "
    esp12e_compiler_c_elf_extra_flags += libpath

    return boards_txt_fmt.format(
        esp01_compiler_c_extra_flags=get_c_extra_flags("esp01", database),
        esp01_compiler_cxx_extra_flags=get_cxx_extra_flags("esp01", database),
        esp01_compiler_c_elf_extra_flags=esp01_compiler_c_elf_extra_flags,
        esp01_compiler_c_elf_extra_flags_after=get_c_elf_extra_flags_after("esp01", database),
        esp12e_compiler_c_extra_flags=get_c_extra_flags("esp12e", database),
        esp12e_compiler_cxx_extra_flags=get_cxx_extra_flags("esp12e", database),
        esp12e_compiler_c_elf_extra_flags=esp12e_compiler_c_elf_extra_flags,
        esp12e_compiler_c_elf_extra_flags_after=get_c_elf_extra_flags_after("esp12e", database))


def generate_configuration_files(family, database):
    """Generate copy configuration files.

    """

    pumbaa_root = os.environ["PUMBAA_ROOT"]
    family_dir = os.path.join(pumbaa_root,
                            "make",
                            "arduino",
                            family)
    configuration_files = [
        "platform.txt"
    ]

    for configuration_file in configuration_files:
        shutil.copy(os.path.join(family_dir, configuration_file), ".")

    with open("boards.txt", "w") as fout:
        with open(os.path.join(family_dir, "boards.txt"), "r") as fin:
            if family == "avr":
                boards_txt = generate_boards_txt_avr(database, fin.read())
            elif family == "sam":
                boards_txt = generate_boards_txt_sam(database, fin.read())
            elif family == "esp":
                boards_txt = generate_boards_txt_esp(database, fin.read())
            else:
                raise ValueError("Unsupported family {}.".format(family))

            fout.write(boards_txt)


def generate_extra(family, database):
    """Generate extra files that do not fit into any other generation
    function.

    """

    pumbaa_root = os.environ["PUMBAA_ROOT"]

    if family == "esp":
        # Copy all libraries.
        libpaths = database["boards"]["esp01"]["libpath"]
        mkdir_p("lib")

        for lib in database["boards"]["esp01"]["lib"]:
            for libpath in libpaths:
                libpath_dir = os.path.join(pumbaa_root, libpath)
                for root, _, filenames in os.walk(libpath_dir):
                    for filename in filenames:
                        if filename != "lib" + lib + ".a":
                            continue
                        file_path = os.path.join(root, filename)
                        shutil.copy(file_path, "lib")
                        break

        # Copt eboot (bootloader).
        eboot_dir = os.path.join("bootloaders", "eboot")
        mkdir_p(eboot_dir)
        shutil.copy(os.path.join(pumbaa_root,
                                 "simba",
                                 "3pp",
                                 "esp8266Arduino",
                                 "2.3.0",
                                 "bootloaders",
                                 "eboot",
                                 "eboot.elf"),
                    eboot_dir)


def generate_files_and_folders(family, database, outdir):
    """Generate files and folders.

    """

    os.makedirs(outdir)
    cwd = os.getcwd()
    os.chdir(outdir)

    copy_tools()
    cores_srcs = generate_cores(family, database)
    generate_variants(family, database, cores_srcs)
    generate_examples()
    generate_configuration_files(family, database)
    generate_extra(family, database)

    os.chdir(cwd)


def main():
    """Package Pumbaa for the Arduino IDE.

    """

    parser = argparse.ArgumentParser()
    parser.add_argument("--remove-outdir", "-r", action="store_true")
    parser.add_argument("--outdir", default="pumbaa-arduino")
    parser.add_argument("--version", required=True)
    args = parser.parse_args()

    if args.remove_outdir:
        if os.path.exists(args.outdir):
            print("Removing", args.outdir)
            shutil.rmtree(args.outdir)

    print("Creating software database.")
    database = create_database()
    
    print("Writing to " + args.outdir + ".")

    for family in ["sam", "esp"]:
        packages_family_dir = os.path.join(args.outdir,
                                           "packages",
                                           "Pumbaa",
                                           family)
        generate_files_and_folders(family,
                                   database,
                                   packages_family_dir)

        # Create release archives and their sha256 sum.
        temporary_family_dir = os.path.join(
            args.outdir,
            "pumbaa-arduino-" + family)
        shutil.copytree(packages_family_dir, temporary_family_dir)
        archive_path_no_suffix = os.path.join(
            args.outdir,
            "pumbaa-arduino-{family}-{version}".format(family=family,
                                                      version=args.version))
        shutil.make_archive(archive_path_no_suffix,
                            "zip",
                            args.outdir,
                            "pumbaa-arduino-" + family)
        shutil.rmtree(temporary_family_dir)

        with open(archive_path_no_suffix + ".zip.sha256", "w") as fout:
            with open(archive_path_no_suffix + ".zip", "rb") as fin:
                fout.write(hashlib.sha256(fin.read()).hexdigest())


if __name__ == "__main__":
    main()
