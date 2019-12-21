#!/bin/sh
#
# This script updates all of the default configuration files for any changes
# in Kconfig files.
#

set -e

for f in configs/*_config; do
    KCONFIG_CONFIG=$(mktemp --tmpdir micropython.config.XXXXXXXXXX)
    export KCONFIG_CONFIG
    ./tools/Kconfiglib/defconfig.py "$f"
    ./tools/Kconfiglib/savedefconfig.py --out "$f"
    rm "$KCONFIG_CONFIG"
    rm "$KCONFIG_CONFIG.old"
done
