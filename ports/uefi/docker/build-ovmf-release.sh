#!/usr/bin/env bash
# Build RELEASE firmware for BOTH targets into ./firmware/release/:
#   OVMF (X64):      OVMF_CODE.fd  / OVMF_VARS.fd
#   AAVMF (AArch64): AAVMF_CODE.fd / AAVMF_VARS.fd
#
# RELEASE (vs the default NOOPT) firmware is optimized and, crucially, emits no DEBUG
# log on the serial line — so it boots faster and doesn't drown a raw-REPL sync. Used
# by the REPL-driven test runner (fast per-test app relaunches — no QEMU reset). Reuses
# the cached `ovmf-builder` stage (edk2 checkout + BaseTools). Run from ports/uefi/.
set -euo pipefail

cd "$(dirname "$0")/.."                       # -> ports/uefi
EDK2_TAG="${EDK2_TAG:-edk2-stable202502}"
OUT="$(pwd)/firmware/release"; mkdir -p "$OUT"
IMG="mpy-uefi-ovmf:${EDK2_TAG}"

# Reuse the builder image if present; build it (rare) if not.
docker build -f Dockerfile --target ovmf-builder \
  --build-arg EDK2_TAG="$EDK2_TAG" -t "$IMG" .

cid="mpy-ovmf-release-$$"
docker rm -f "$cid" >/dev/null 2>&1 || true
trap 'docker rm -f "$cid" >/dev/null 2>&1 || true' EXIT

# Build RELEASE for both arches (same native/cross prefix logic as the Dockerfile).
docker run --name "$cid" "$IMG" bash -c '
  set -e
  cd /edk2 && . ./edksetup.sh
  if [ "$TARGETARCH" = arm64 ]; then Paa= ; else Paa=aarch64-linux-gnu- ; fi
  if [ "$TARGETARCH" = amd64 ]; then Px= ; else Px=x86_64-linux-gnu- ; fi
  GCC5_AARCH64_PREFIX=$Paa build -a AARCH64 -t GCC5 -b RELEASE -p ArmVirtPkg/ArmVirtQemu.dsc
  GCC5_BIN=$Px             build -a X64     -t GCC5 -b RELEASE -p OvmfPkg/OvmfPkgX64.dsc
'

base_x64=/edk2/Build/OvmfX64/RELEASE_GCC5/FV
base_aa64=/edk2/Build/ArmVirtQemu-AARCH64/RELEASE_GCC5/FV
docker cp "$cid:$base_x64/OVMF_CODE.fd"   "$OUT/OVMF_CODE.fd"
docker cp "$cid:$base_x64/OVMF_VARS.fd"   "$OUT/OVMF_VARS.fd"
docker cp "$cid:$base_aa64/QEMU_EFI.fd"   "$OUT/AAVMF_CODE.fd"
docker cp "$cid:$base_aa64/QEMU_VARS.fd"  "$OUT/AAVMF_VARS.fd"
# The QEMU aarch64 `virt` machine wants each pflash bank padded to 64 MiB.
truncate -s 64M "$OUT/AAVMF_CODE.fd" "$OUT/AAVMF_VARS.fd"

echo "RELEASE firmware written to $OUT:"
ls -l "$OUT"/*.fd