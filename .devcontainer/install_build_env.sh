#!/bin/bash
# -----------------------------------------------------------------------------
# install_build_env.sh: install build-environment for CircuitPython
#
# Normally, this should run directly as postCreateCommand during container
# creation. Due to an unresolved bug on how Github-codespaces creates a clone,
# this script is started from $HOME/.bashrc instead.
#
# The script delegates parts to other scripts for reuse across toolchains.
# This has the added benefit that they can be called independently later again
# if necessary.
#
# The scripts expect the environment-variables CP_TOOLCHAIN and CP_PORT to be set
# to valid values. This is normally done from within
#   .devcontainer/<port>/devcontainer.json
#
# Author: Bernhard Bablok
#
# -----------------------------------------------------------------------------
REPO_ROOT="/workspaces/circuitpython"

# --- install exit-handler for cleanup   --------------------------------------

on_exit() {
  rc=$?
  if [ -f /workspaces/install_build_env.log.active ]; then
    mv /workspaces/install_build_env.log.active /workspaces/install_build_env.log
  fi
  rm -rf /tmp/install_build_env
  exit $rc
}

# --- test prerequisites for installation   ------------------------------------

while ! test -f /workspaces/post_create.finished; do
  echo -e "[install_build_env.sh] waiting for /workspaces/post_create.finished ..."
  sleep 1
done

if [ -f /workspaces/install_build_env.log ]; then
  echo -e "[install_build_env.sh] installation already done"
  exit 0
elif ! mkdir /tmp/install_build_env 2>/dev/null; then
  # mkdir is atomic, so we know we are already running
  echo -e "[install_build_env.sh] install already running with PID $(cat /tmp/install_build_env/pid.txt)"
  exit 0
else
  echo -e "$$" > /tmp/install_build_env/pid.txt
  trap 'on_exit' EXIT
fi

echo -e "[install_build_env.sh] starting install"

# --- delegate install steps to other scripts   -------------------------------
(
"$REPO_ROOT/.devcontainer/fetch-port-submodules.sh" || exit 3
"$REPO_ROOT/.devcontainer/common_tools.sh" || exit 3
"$REPO_ROOT/.devcontainer/$CP_TOOLCHAIN-toolchain.sh" || exit 3
"$REPO_ROOT/.devcontainer/make-mpy-cross.sh" || exit 3
echo -e "Setup complete!\nStart a new terminal and build CircuitPython!\n"
) |& tee /workspaces/install_build_env.log.active

echo -e "[install_build_env.sh] Setup complete!"
exit 0
