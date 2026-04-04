#!/usr/bin/env python3
import subprocess
import os

idf = os.environ["IDF_PATH"]
confgen = f"{idf}/tools/kconfig_new/confgen.py"

subprocess.check_call([
    "python3", confgen,
    "--kconfig", "Kconfig.auto",
    "--config", ".config",
    "--output", "header", "board_kconfig.h",
    "--output", "cmake", "board_kconfig.cmake"
])
