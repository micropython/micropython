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
#   ZCP_NO_DISPLAY   set to 1 to not share the host's X11 display
#
# If DISPLAY is set when the container is first created, the host's X11
# socket and a copy of its X cookie are shared so the simulator can open its
# display window. This works on a Linux desktop and in WSLg.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
CONTAINER_NAME="${ZCP_CONTAINER:-zcp}"
IMAGE="${ZCP_IMAGE:-localhost/zephyr-cp-dev}"
read -r -a EXTRA_ARGS <<< "${ZCP_PODMAN_ARGS:-}"

DISPLAY_ARGS=()
if [ -z "${ZCP_NO_DISPLAY:-}" ] && [ -n "${DISPLAY:-}" ] && [ -d /tmp/.X11-unix ]; then
    # The host's X cookie is tied to its hostname, which the container does
    # not share. Copy it as a wildcard entry on every start, so a cookie
    # renewed by a new desktop session reaches the container too.
    XAUTH_DIR="${XDG_CACHE_HOME:-$HOME/.cache}/zcp-xauth/$CONTAINER_NAME"
    mkdir -p "$XAUTH_DIR"
    rm -f "$XAUTH_DIR/xauth"
    touch "$XAUTH_DIR/xauth"
    if command -v xauth >/dev/null; then
        xauth nlist "$DISPLAY" 2>/dev/null | sed -e 's/^..../ffff/' \
            | xauth -f "$XAUTH_DIR/xauth" nmerge - 2>/dev/null || true
    fi
    DISPLAY_ARGS+=(-v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY
                   -v "$XAUTH_DIR:/home/dev/.zcp-xauth:ro"
                   -e XAUTHORITY=/home/dev/.zcp-xauth/xauth)
fi

if podman container exists "$CONTAINER_NAME"; then
    exec podman start -ai "$CONTAINER_NAME"
else
    exec podman run -it --init --name "$CONTAINER_NAME" \
        -v "$REPO_ROOT:/home/dev/circuitpython:Z" \
        --userns=keep-id \
        ${DISPLAY_ARGS[@]+"${DISPLAY_ARGS[@]}"} \
        ${EXTRA_ARGS[@]+"${EXTRA_ARGS[@]}"} \
        "$IMAGE" "$@"
fi
