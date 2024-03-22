#!/bin/bash
# -----------------------------------------------------------------------------
# esp-idf-toolchain.sh: install toolchain for CircuitPython
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------
REPO_ROOT="/workspaces/circuitpython"

echo -e "[esp-idf-toolchain.sh] starting install"

# --- tooling   --------------------------------------------------------------

echo -e "[esp-idf-toolchain.sh] fetch packages"
sudo apt-get update
sudo apt-get -y install ninja-build cmake libusb-1.0-0

# --- esp-idf   --------------------------------------------------------------

echo -e "[esp-idf-toolchain.sh] installing esp-idf"
cd "$REPO_ROOT/ports/espressif"
esp-idf/install.sh
source esp-idf/export.sh

# --- re-install our packages in venv created by export.sh   -----------------

echo -e "[esp-idf-toolchain.sh] updating python-packages"
cd "$REPO_ROOT"
pip3 install --upgrade -r requirements-dev.txt
pip3 install --upgrade -r requirements-doc.txt

# --- and again install esp-idf (needs other versions)   ----------------------

echo -e "[esp-idf-toolchain.sh] installing esp-idf (2nd iteration)"
cd "$REPO_ROOT/ports/espressif"
esp-idf/install.sh

# --- update $HOME/.bashrc   --------------------------------------------------

echo -e "[esp-idf-toolchain.sh] update environment in .bashrc"

echo -e "\nsource $REPO_ROOT/ports/espressif/esp-idf/export.sh &> /dev/null\n" >> "$HOME"/.bashrc
