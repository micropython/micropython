#!/usr/bin/env python3
import kconfiglib.core as kconfiglib
import glob
import os

# 1. Collect all component Kconfig files
kconfig_files = glob.glob("components/**/Kconfig", recursive=True)
kconfig_files += glob.glob("managed_components/**/Kconfig", recursive=True)
# 2. Create a top-level Kconfig file dynamically
with open("Kconfig.auto", "w") as f:
    for path in kconfig_files:
        f.write(f'source "{path}"\n')
