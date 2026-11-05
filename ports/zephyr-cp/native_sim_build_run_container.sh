#!/usr/bin/env bash
# Launch (or re-attach to) the zephyr-cp-dev container with the enclosing
# circuitpython checkout bind-mounted at /home/dev/circuitpython. Works from
# any CWD — the mount is resolved relative to this script's location.
#
# On first invocation, creates a persistent container named "zcp". On
# subsequent invocations, re-starts the same container so installed state
# (e.g. the Zephyr SDK under /home/dev/zephyr-sdk-*) survives across sessions.
#
# Optional environment variables:
#   ZCP_CONTAINER    container name (default: zcp)
#   ZCP_IMAGE        image name (default: localhost/zephyr-cp-dev)
#   ZCP_PODMAN_ARGS  extra options for "podman run", used only when the
#                    container is first created (e.g. "--runtime runc")
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
CONTAINER_NAME="${ZCP_CONTAINER:-zcp}"
IMAGE="${ZCP_IMAGE:-localhost/zephyr-cp-dev}"
read -r -a EXTRA_ARGS <<< "${ZCP_PODMAN_ARGS:-}"

if podman container exists "$CONTAINER_NAME"; then
    exec podman start -ai "$CONTAINER_NAME"
else
    exec podman run -it --init --name "$CONTAINER_NAME" \
        -v "$REPO_ROOT:/home/dev/circuitpython:Z" \
        --userns=keep-id \
        ${EXTRA_ARGS[@]+"${EXTRA_ARGS[@]}"} \
        "$IMAGE" "$@"
fi
