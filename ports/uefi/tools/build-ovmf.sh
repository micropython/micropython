#!/usr/bin/env bash
# Build OVMF (X64) + AAVMF (AArch64) firmware and extract the .fd files.
#
# Usage: tools/build-ovmf.sh [--release] [--net]
#   (default)        NOOPT firmware        -> firmware/
#   --net            NOOPT + network stack -> firmware/net
#   --release        RELEASE firmware      -> firmware/release
#   --release --net  RELEASE + network     -> firmware/release-net
#
# NOOPT firmware is already baked into the Dockerfile's ovmf-builder/ovmf-net-builder
# stages (the same layers `dev`'s COPY --from uses for `make run`), so those two cases
# just extract the image's own build output. RELEASE isn't baked into any stage (nothing
# runs it interactively), so those two cases re-run `build -b RELEASE` explicitly inside
# an ovmf-builder container instead.
#
# Run from ports/uefi/. Rebuild is rare (only on EDK2_TAG / --net change).
set -euo pipefail

cd "$(dirname "$0")/.."                        # -> ports/uefi

release=0 net=0
for arg in "$@"; do
  case "$arg" in
    --release) release=1 ;;
    --net) net=1 ;;
    *) echo "usage: $0 [--release] [--net]" >&2; exit 2 ;;
  esac
done

EDK2_TAG="${EDK2_TAG:-edk2-stable202502}"
NET_FLAGS="-D NETWORK_ENABLE=TRUE -D NETWORK_IP6_ENABLE=TRUE -D NETWORK_TLS_ENABLE=TRUE -D NETWORK_HTTP_BOOT_ENABLE=TRUE -D NETWORK_HTTP_ENABLE=TRUE -D NETWORK_ALLOW_HTTP_CONNECTIONS=TRUE"

if [ "$release" = 1 ]; then
  BUILD_TYPE=RELEASE
  if [ "$net" = 1 ]; then OUT="$(pwd)/firmware/release-net"; else OUT="$(pwd)/firmware/release"; fi
else
  BUILD_TYPE=NOOPT
  if [ "$net" = 1 ]; then OUT="$(pwd)/firmware/net"; else OUT="$(pwd)/firmware"; fi
fi
mkdir -p "$OUT"

if [ "$release" = 0 ]; then
  # NOOPT: baked into the image as part of `docker build` — just extract it.
  stage=ovmf-builder; img="mpy-uefi-ovmf:${EDK2_TAG}"
  if [ "$net" = 1 ]; then stage=ovmf-net-builder; img="mpy-uefi-ovmf-net:${EDK2_TAG}"; fi
  docker build -f tools/Dockerfile --target "$stage" --build-arg EDK2_TAG="$EDK2_TAG" -t "$img" tools
  cid="$(docker create "$img")"
  trap 'docker rm -f "$cid" >/dev/null 2>&1 || true' EXIT
else
  # RELEASE: build the (cached) ovmf-builder base for the edk2 checkout + BaseTools,
  # then explicitly re-run `build -b RELEASE` (optionally with NET_FLAGS).
  img="mpy-uefi-ovmf:${EDK2_TAG}"
  docker build -f tools/Dockerfile --target ovmf-builder --build-arg EDK2_TAG="$EDK2_TAG" -t "$img" tools
  if [ "$net" = 1 ]; then cid="mpy-ovmf-release-net-$$"; run_net_flags="$NET_FLAGS"; else cid="mpy-ovmf-release-$$"; run_net_flags=""; fi
  docker rm -f "$cid" >/dev/null 2>&1 || true
  trap 'docker rm -f "$cid" >/dev/null 2>&1 || true' EXIT
  docker run --name "$cid" -e NET_FLAGS="$run_net_flags" "$img" bash -c '
    set -e
    cd /edk2 && . ./edksetup.sh
    if [ "$TARGETARCH" = arm64 ]; then Paa= ; else Paa=aarch64-linux-gnu- ; fi
    if [ "$TARGETARCH" = amd64 ]; then Px= ; else Px=x86_64-linux-gnu- ; fi
    GCC5_AARCH64_PREFIX=$Paa build -a AARCH64 -t GCC5 -b '"$BUILD_TYPE"' -p ArmVirtPkg/ArmVirtQemu.dsc $NET_FLAGS
    GCC5_BIN=$Px             build -a X64     -t GCC5 -b '"$BUILD_TYPE"' -p OvmfPkg/OvmfPkgX64.dsc     $NET_FLAGS
  '
fi

base_x64=/edk2/Build/OvmfX64/${BUILD_TYPE}_GCC5/FV
base_aa64=/edk2/Build/ArmVirtQemu-AARCH64/${BUILD_TYPE}_GCC5/FV
docker cp "$cid:$base_x64/OVMF_CODE.fd"  "$OUT/OVMF_CODE.fd"
docker cp "$cid:$base_x64/OVMF_VARS.fd"  "$OUT/OVMF_VARS.fd"
docker cp "$cid:$base_aa64/QEMU_EFI.fd"  "$OUT/AAVMF_CODE.fd"
docker cp "$cid:$base_aa64/QEMU_VARS.fd" "$OUT/AAVMF_VARS.fd"
# The QEMU aarch64 `virt` machine wants each pflash bank padded to 64 MiB.
truncate -s 64M "$OUT/AAVMF_CODE.fd" "$OUT/AAVMF_VARS.fd"

echo "Firmware written to $OUT:"
ls -l "$OUT"/*.fd
