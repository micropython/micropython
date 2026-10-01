# Shared arch mapping + paths for the run/debug/triage scripts. Source me:
#   source "$(dirname "$0")/env.sh" "$ARCH"
# shellcheck shell=bash
ARCH="${1:-${ARCH:-aa64}}"
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"   # ports/uefi
FW="$HERE/firmware"
ESP="$HERE/esp"

case "$ARCH" in
  aa64)
    QEMU=qemu-system-aarch64
    MACHINE=(-machine virt -cpu cortex-a72)
    FW_CODE="$FW/AAVMF_CODE.fd"; FW_VARS="$FW/AAVMF_VARS.fd"
    EFI_NAME=BOOTAA64.EFI ;;
  x64)
    QEMU=qemu-system-x86_64
    MACHINE=(-machine q35)
    FW_CODE="$FW/OVMF_CODE.fd"; FW_VARS="$FW/OVMF_VARS.fd"
    EFI_NAME=BOOTX64.EFI ;;
  *)
    echo "unknown ARCH '$ARCH' (use aa64 or x64)" >&2
    return 2 2>/dev/null || exit 2 ;;
esac

# Prefer the network-enabled firmware (full IP4/TCP/UDP/DNS/TLS stack) if it has been
# built — the lean default firmware lacks the upper network stack. Build it with
# `bash tools/build-ovmf.sh --net`.
if [ -f "$FW/net/$(basename "$FW_CODE")" ]; then
  FW_CODE="$FW/net/$(basename "$FW_CODE")"
  FW_VARS="$FW/net/$(basename "$FW_VARS")"
else
  echo "note: firmware/net not built — networking will be unavailable." >&2
  echo "      build it with:  bash tools/build-ovmf.sh --net" >&2
fi

# QEMU network devices (see README.md):
#  * virtio-rng is MANDATORY — the EDK2 network stack has a DEPEX on the RNG protocol.
#  * virtio-net on SLIRP user-net reaches the outside world (DHCP 10.0.2.15 / gw
#    10.0.2.2 / DNS 10.0.2.3); on the host it NATs to the real internet.
#  * hostfwd maps host tcp/8080 -> guest tcp/8080 so a server demo is reachable.
#  * slots pinned on aa64 so they don't take slot 0x2 and shift the boot disk.
if [ "$ARCH" = "x64" ]; then
  _RNG_ADDR=""; _NIC_ADDR=""
else
  _RNG_ADDR=",addr=0xc"; _NIC_ADDR=",addr=0xb"
fi
NET=(-device "virtio-rng-pci${_RNG_ADDR}"
     -netdev user,id=net0,hostfwd=tcp::8080-:8080
     -device "virtio-net-pci,netdev=net0${_NIC_ADDR}")
