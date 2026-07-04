#!/usr/bin/env bash
# Build NETWORK-ENABLED firmware for BOTH targets inside the `ovmf-net-builder`
# Docker stage and drop the .fd files into ./firmware/net:
#   OVMF (X64):      OVMF_CODE.fd  / OVMF_VARS.fd
#   AAVMF (AArch64): AAVMF_CODE.fd / AAVMF_VARS.fd
#
# These carry the full UEFI network stack (SNP/MNP/ARP/IP4+IP6/UDP/TCP/DHCP/DNS/
# HTTP/TLS + VirtioNetDxe) — see the Dockerfile `ovmf-net-builder` stage. They are
# kept separate from the lean default firmware in ./firmware; the test harness
# selects them with `--net` (Makefile: `make test NET=1` / `make test-net`).
#
# Run from ports/uefi/.  Rebuild is rare (only on EDK2_TAG / NET_FLAGS change).
set -euo pipefail

cd "$(dirname "$0")/.."                       # -> ports/uefi
EDK2_TAG="${EDK2_TAG:-edk2-stable202502}"
OUT="$(pwd)/firmware/net"; mkdir -p "$OUT"
IMG="mpy-uefi-ovmf-net:${EDK2_TAG}"

docker build -f Dockerfile --target ovmf-net-builder \
  --build-arg EDK2_TAG="$EDK2_TAG" -t "$IMG" .

cid="$(docker create "$IMG")"
trap 'docker rm -f "$cid" >/dev/null 2>&1 || true' EXIT

base_x64=/edk2/Build/OvmfX64/NOOPT_GCC5/FV
base_aa64=/edk2/Build/ArmVirtQemu-AARCH64/NOOPT_GCC5/FV
docker cp "$cid:$base_x64/OVMF_CODE.fd"  "$OUT/OVMF_CODE.fd"
docker cp "$cid:$base_x64/OVMF_VARS.fd"  "$OUT/OVMF_VARS.fd"
docker cp "$cid:$base_aa64/QEMU_EFI.fd"  "$OUT/AAVMF_CODE.fd"
docker cp "$cid:$base_aa64/QEMU_VARS.fd" "$OUT/AAVMF_VARS.fd"
# The QEMU aarch64 `virt` machine wants each pflash bank padded to 64 MiB.
truncate -s 64M "$OUT/AAVMF_CODE.fd" "$OUT/AAVMF_VARS.fd"

echo "Network firmware written to $OUT:"
ls -l "$OUT"/*.fd
