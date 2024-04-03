#!/bin/bash
# -----------------------------------------------------------------------------
# common_tools.sh: install tools and requirements for CircuitPython
#
# This script installs tools common to all builds.
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------

REPO_ROOT="/workspaces/circuitpython"

echo -e "[common_tools.sh] starting install"
cd "$REPO_ROOT"

# --- repositories and tools   ------------------------------------------------

echo -e "[common_tools.sh] adding pybricks/ppa"
sudo add-apt-repository -y ppa:pybricks/ppa
echo -e "[common_tools.sh] installing uncrustify and mtools"
sudo apt-get -y install uncrustify mtools

# dosfstools >= 4.2 needed, standard repo only has 4.1
echo -e "[common_tools.sh] downloading and installing dosfstools"
wget https://github.com/dosfstools/dosfstools/releases/download/v4.2/dosfstools-4.2.tar.gz
tar -xzf dosfstools-4.2.tar.gz
(cd dosfstools-4.2/
 ./configure
 make -j $(nproc)
 sudo make install
)
rm -fr dosfstools-4.2 dosfstools-4.2.tar.gz

# --- circuitpython setup   --------------------------------------------------

# additional python requirements
echo -e "[common_tools.sh] pip-installing requirements"
pip install --upgrade -r requirements-dev.txt
pip install --upgrade -r requirements-doc.txt

# add pre-commit
echo -e "[common_tools.sh] installing pre-commit"
pre-commit install
