#!/bin/bash
# -----------------------------------------------------------------------------
# fetch-port-submodules.sh: fetch port specific submodules
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------

REPO_ROOT="/workspaces/circuitpython"
cd "$REPO_ROOT"

if [ -z "$CP_PORT" ]; then
  echo -e "[fetch-port-submodules.sh] CP_PORT not set. Cannot fetch submodules!"
  exit 3
fi

cd "ports/$CP_PORT"
echo -e "[fetch-port-submodules.sh] fetching necessary submodules"
make fetch-port-submodules
