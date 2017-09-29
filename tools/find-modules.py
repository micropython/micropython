#!/usr/bin/env python
#
# Create a list of modules to be frozen for MicroPython.
# Multiple modules directories can be specified eg for port, board and project.
# Modules can be specified three ways.
#
# 1. Linux Symlinks as used in MicroPython master on GitHub.
# #########################################################
# When running under WINDOWS the path and filename are read from the simlink
#   to find the module. Otherwise the simlink is used normally.
#
# 2. Fake symlinks.
# ################
# On any OS including WINDOWS fake symlinks can be create as follows:
#
#   1. File name must match the module to be frozen, eg foo.py
#   2. File must contain the path and name of the module, eg:
#       frozen/foo.py
#       ../frozen/foo.py
#       ..\frozen\foo.py<CR><LF>
#       Note \ is changed to /, <CR> and <LF> are removed.
#   3. Files must contain only one line.
#
# For files conforming to these rules the file contents are used.
#
# any .py files not conforming to these rules are treated a valid MicroPython
#    modules and their path and filename are used.
#
# 3. modules.lst file in any modules directory.
# ############################################
#
# A list of modules can be placed in a modules.lst file. Rules are:
#
#   1. Comments must be on separate lines starting with #.
#       leading whitespace is stripped.
#   2. Blank lines are ignored.
#   3. Modules must be specified with path and filename, one per line.
#   4. Modules listed in modules.lst must not reside in the same directory
#       as modules.lst or they willl be processed twice.
#   5. Paths may use / or \
# eg:
#   +----------------------------------------+
#   |# This is a list of modules to freeze   |
#   |                                        |
#   |    # first file is - foo.py            |
#   |../modules2/foo.py                      |
#   |../modules2/bar.py                      |
#   |pkg/my_pkg.py                           |
#   +----------------------------------------+
#
# Usage:
# ######
#
# Have one or more directories with modules or links to modules to be frozen.
#
# Multiple module directories are supported,eg for the port, board and project.
# When setting the FROZEN_MPY_DIR variable:
#   Modules lists with spaces must be enclosed in quotes.
#   Paths must use forward slash.
#   Alternatively spaces and backslashes must be escaped.
# eg:
#
# Build port modules. (this is the same as the default make).
# make FROZEN_MPY_DIR=modules
#
# Builds board specific modules.
# make FROZEN_MPY_DIR=boards/PYBV10/modules
#
# Build port and board modules.
# make FROZEN_MPY_DIR="modules boards/PYBV10/modules"
#
# Build port, board and project modules.
# This allows users to incororate the standard port modules, eg onewire.py
#   and add board or project modules in differnt directories
#   in their own project tree.
# make FROZEN_MPY_DIR="modules boards/PYBV10/modules boards/PROJECT1/modules"
#
# Usage:
# ######
#
# A temporary makefile (frozen.mk) is created in the build directory.
# The build directory (eg build-PYBV10) must be specified as the first
#   argument, followed by one or more module directories.
#
# python find-modules.py build modules > frozen-mpy-list
#
# The output is frozen.mk in the build directory and a list of modules
#   printed to stdout.
#
# frozen.mk compiles each .py file to .mpy file with mpy-cross.
# Because there are an unknown number of source directories each module is
#   built by a separate rule in frozen.mk.
#
# The output list is formatted for mpy-tool.
#

from __future__ import print_function
import sys
import os
import ntpath


def module_name(f):
    return f

modules = []
mpy_list = ""

# cmd line requires BUILD directory and at least one MODULES directory
if (len(sys.argv) < 3):
    sys.exit("find-modules.py Not enough arguments")

build = sys.argv[1].rstrip("/").rstrip("\\")
for i in range(2, len(sys.argv)):
    root = sys.argv[i].replace("\\", "/").rstrip("/")
    root_len = len(root)

    for dirpath, dirnames, filenames in os.walk(root):
        for filename in filenames:
            fullpath = dirpath + "/" + filename
            pkg = dirpath[root_len + 1:]
            if filename == "modules.lst":
                with open(fullpath, "r") as lstfile:
                    for line in lstfile:
                        # Windows users may have backslash, CR and/or LF.
                        line = line.replace("\r", '').replace("\n", '').replace("\\", "/")
                        if ((line.lstrip()[:1] != "#") and (line[-3:] == ".py")):
                            # not a comment, is valid link, use link contents.
                            modules.append([root, pkg, line])

            if filename[-3:] == ".py":
                pyfile = open(fullpath, "r")
                # Windows users may have backslash, CR and/or LF.
                line_1 = pyfile.readline().replace("\r", '').replace("\n", '').replace("\\", "/")
                if ((ntpath.basename(filename) == ntpath.basename(line_1)) &(pyfile.readline() == '')):
                    # valid one line link, use the link contents.
                    modules.append([root, pkg, line_1])
                else:
                    # not a valid link, point to the file.
                    modules.append([root, pkg, filename])
try:
    os.makedirs(build)
except OSError:
    if not os.path.isdir(build):
        raise
mkfile = open(build + "/frozen.mk", "w")
mkfile.write('# to build .mpy files from .py files\n\n')
for root, pkg, module in modules:
    if(pkg != ""):
        pkg += "/"
    name = os.path.basename(module)[:-3]
    mpy_list += build + "/frozen_mpy/" + pkg + name + ".mpy "
    mkfile.write('$(BUILD)/frozen_mpy/' + pkg + name + '.mpy: ' + root + '/' + pkg + module + ' $(MPY_CROSS)\n')
    mkfile.write('\t$(Q)$(MKDIR) -p $(dir $@)\n')
    mkfile.write('\t@$(ECHO) "MPY $<"\n')
    mkfile.write('\t$(Q)$(MPY_CROSS) -o $@ -s ' + pkg + os.path.basename(module) + ' $(MPY_CROSS_FLAGS) $<\n')
    mkfile.write('\n')
mkfile.close
print(mpy_list)
