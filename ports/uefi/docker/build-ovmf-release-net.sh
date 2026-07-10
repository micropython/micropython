#!/usr/bin/env bash
# Build RELEASE + NETWORK firmware for BOTH targets into ./firmware/release-net/:
#   OVMF (X64):      OVMF_CODE.fd  / OVMF_VARS.fd
#   AAVMF (AArch64): AAVMF_CODE.fd / AAVMF_VARS.fd
#
# This is the "all protocols" firmware for the serial test runner: RELEASE (optimized,
# no DEBUG on the serial line — so it doesn't drown the raw-REPL sync) AND carrying the
# full UEFI network stack (SNP/MNP/ARP/IP4+IP6/UDP/TCP/DHCP/DNS/HTTP/TLS + VirtioNetDxe).
# The GOP driver is already in the base DSC, so with a display adapter attached GOP is
# published too. Selected by `run_uefi_tests.py --net`.
#
# Distinct from ./firmware/net (NOOPT/debug net firmware, used by the network diagnostic
# tooling in scripts/net + the samples): that one spams DEBUG on the pl011, which would
# corrupt the byte-clean raw REPL. Run from ports/uefi/. Reuses the cached `ovmf-builder`
# stage; rebuild is rare.
set -euo pipefail

cd "$(dirname "$0")/.."                       # -> ports/uefi
EDK2_TAG="${EDK2_TAG:-edk2-stable202502}"
OUT="$(pwd)/firmware/release-net"; mkdir -p "$OUT"
IMG="mpy-uefi-ovmf:${EDK2_TAG}"

# The network feature switches (same as the Dockerfile ovmf-net-builder stage).
NET_FLAGS="-D NETWORK_ENABLE=TRUE -D NETWORK_IP6_ENABLE=TRUE -D NETWORK_TLS_ENABLE=TRUE \
-D NETWORK_HTTP_BOOT_ENABLE=TRUE -D NETWORK_HTTP_ENABLE=TRUE -D NETWORK_ALLOW_HTTP_CONNECTIONS=TRUE"

# Reuse the builder image if present; build it (rare) if not.
docker build -f Dockerfile --target ovmf-builder \
  --build-arg EDK2_TAG="$EDK2_TAG" -t "$IMG" .

cid="mpy-ovmf-release-net-$$"
docker rm -f "$cid" >/dev/null 2>&1 || true
trap 'docker rm -f "$cid" >/dev/null 2>&1 || true' EXIT

docker run --name "$cid" -e NET_FLAGS="$NET_FLAGS" "$IMG" bash -c '
  set -e
  cd /edk2 && . ./edksetup.sh
  if [ "$TARGETARCH" = arm64 ]; then Paa= ; else Paa=aarch64-linux-gnu- ; fi
  if [ "$TARGETARCH" = amd64 ]; then Px= ; else Px=x86_64-linux-gnu- ; fi
  GCC5_AARCH64_PREFIX=$Paa build -a AARCH64 -t GCC5 -b RELEASE -p ArmVirtPkg/ArmVirtQemu.dsc $NET_FLAGS
  GCC5_BIN=$Px             build -a X64     -t GCC5 -b RELEASE -p OvmfPkg/OvmfPkgX64.dsc     $NET_FLAGS
'

base_x64=/edk2/Build/OvmfX64/RELEASE_GCC5/FV
base_aa64=/edk2/Build/ArmVirtQemu-AARCH64/RELEASE_GCC5/FV
docker cp "$cid:$base_x64/OVMF_CODE.fd"   "$OUT/OVMF_CODE.fd"
docker cp "$cid:$base_x64/OVMF_VARS.fd"   "$OUT/OVMF_VARS.fd"
docker cp "$cid:$base_aa64/QEMU_EFI.fd"   "$OUT/AAVMF_CODE.fd"
docker cp "$cid:$base_aa64/QEMU_VARS.fd"  "$OUT/AAVMF_VARS.fd"
# The QEMU aarch64 `virt` machine wants each pflash bank padded to 64 MiB.
truncate -s 64M "$OUT/AAVMF_CODE.fd" "$OUT/AAVMF_VARS.fd"

echo "RELEASE+network firmware written to $OUT:"
ls -l "$OUT"/*.fd
