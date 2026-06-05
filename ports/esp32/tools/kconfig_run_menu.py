#!/usr/bin/env python3
import subprocess
import os

idf = os.environ["IDF_PATH"]
menu = f"{idf}/tools/kconfig_new/menuconfig.py"

subprocess.check_call(["python", "-m" "menuconfig", "Kconfig.auto"])
