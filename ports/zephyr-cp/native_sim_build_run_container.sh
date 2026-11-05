#!/usr/bin/env bash
# Launch (or re-attach to) the zephyr-cp-dev container with the enclosing
# circuitpython checkout bind-mounted at /home/dev/circuitpython. Works from
# any CWD — the mount is resolved relative to this script's location.
#
# On first invocation, creates a persistent container named "zcp". On
# subsequent invocations, re-starts the same container so installed state
# (e.g. the Zephyr SDK under /home/dev/zephyr-sdk-*) survives across sessions.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
CONTAINER_NAME="zcp"
IMAGE="localhost/zephyr-cp-dev"

if podman container exists "$CONTAINER_NAME"; then
    exec podman start -ai "$CONTAINER_NAME"
else
    exec podman run -it --name "$CONTAINER_NAME" \
        -v "$REPO_ROOT:/home/dev/circuitpython:Z" \
        --userns=keep-id \
        "$IMAGE" "$@"
fi
