#!/bin/bash
# -----------------------------------------------------------------------------
# make-mpy-cross.sh: fetch tags and prereqs, then build mpy-cross
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------

REPO_ROOT="/workspaces/circuitpython"
cd "$REPO_ROOT"

# fetch tags and tools for mpy-cross
echo -e "[make-mpy-cross.sh] fetching tags"
make fetch-tags
echo -e "[make-mpy-cross.sh] fetching prerequisites"
python3 tools/ci_fetch_deps.py mpy-cross

# create cross-compiler
echo -e "[make-mpy-cross.sh] building mpy-cross"
if ! make -j $(nproc) -C mpy-cross; then                   # time: about 36 sec
  echo -e "[make-mpy-cross.sh] make mpy-cross failed"
  exit 3
fi
exit 0
