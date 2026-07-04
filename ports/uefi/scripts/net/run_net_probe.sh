#!/usr/bin/env bash
# Host wrapper: run scripts/net/net_probe.py inside the dev container, where QEMU
# is Linux and its SLIRP user-net reaches the internet through Docker.
#
# Usage:
#   scripts/net/run_net_probe.sh <arch> <nic> <mode> [extra net_probe.py args...]
#     arch : x64 | aa64
#     nic  : virtio-net-pci | e1000 | rtl8139   (default virtio-net-pci)
#     mode : internet | hermetic                (default internet)
#
# Examples:
#   scripts/net/run_net_probe.sh x64  virtio-net-pci internet
#   scripts/net/run_net_probe.sh aa64 virtio-net-pci hermetic
#   scripts/net/run_net_probe.sh aa64 e1000          internet   # expect no driver
set -euo pipefail
cd "$(dirname "$0")/../.."                     # -> ports/uefi
REPO="$(cd ../.. && pwd)"                        # micropython repo root

ARCH="${1:-x64}"; NIC="${2:-virtio-net-pci}"; MODE="${3:-internet}"
shift || true; shift || true; shift || true
DEV_IMG="${DEV_IMG:-mpy-uefi-dev}"

exec docker run --rm -v "$REPO:/work" -w /work/ports/uefi "$DEV_IMG" \
  python3 scripts/net/net_probe.py \
    --arch "$ARCH" --nic "$NIC" --mode "$MODE" "$@"
