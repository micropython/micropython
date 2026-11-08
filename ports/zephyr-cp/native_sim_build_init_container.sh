#!/usr/bin/env bash
# One-time setup to run INSIDE the zephyr-cp-dev container on the first
# launch against a fresh bind-mounted circuitpython checkout.
#
# Usage (inside the container):
#   cd ~/circuitpython/ports/zephyr-cp
#   ./native_sim_build_init_container.sh
#
# Safe to re-run; west/pip/etc. are idempotent.
set -euo pipefail

git config --global --add safe.directory /home/dev/circuitpython

cd "$(dirname "${BASH_SOURCE[0]}")"

echo "==> west init"
if [ ! -d .west ]; then
    west init -l zephyr-config
else
    echo "    (already initialized, skipping)"
fi

echo "==> west update"
west update

echo "==> pip install Zephyr requirements"
pip install -r zephyr/scripts/requirements.txt

echo "==> pip install CircuitPython dev requirements"
pip install -r ../../requirements-dev.txt

echo "==> west zephyr-export"
west zephyr-export

echo "==> west sdk install (x86_64-zephyr-elf)"
west sdk install -t x86_64-zephyr-elf

echo "==> fetch port submodules"
python ../../tools/ci_fetch_deps.py zephyr-cp

echo
echo "First-run setup complete."
echo "You can now build with:  make BOARD=native_native_sim"
