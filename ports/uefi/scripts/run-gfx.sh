#!/usr/bin/env bash
# Interactive REPL on the host terminal WITH a graphics window, so GOP comes up on
# a usable screen. A window needs HOST QEMU (a cocoa window can't come from the
# Linux dev container), so this assumes a prior staged build, e.g.:
#     make docker-stage ARCH=aa64   # build + stage the REPL .efi in the container
#     make run-gfx      ARCH=aa64   # then launch it here, on the host
# Serial + QEMU monitor stay on your terminal (drive the REPL there); the
# framebuffer shows in the window. Resolution: GFX_XRES/GFX_YRES (default 1280x800).
# Display backend: QEMU_DISPLAY (default cocoa on macOS; e.g. gtk/sdl elsewhere).
set -euo pipefail
cd "$(dirname "$0")/.."
# shellcheck source=scripts/env.sh
source "scripts/env.sh" "${1:-aa64}"

XRES="${GFX_XRES:-1280}"
YRES="${GFX_YRES:-800}"
DISPLAY_BACKEND="${QEMU_DISPLAY:-cocoa}"

# A USB keyboard makes the graphical console accept input: the firmware folds it
# into ConIn (which the port polls) alongside serial. aa64 `virt` has no keyboard
# at all by default, so without it the window is input-dead.
if [ "$ARCH" = "x64" ]; then
  # q35's default adapter is std-VGA; replace it so we own the EDID sizing.
  GFX=(-vga none -device "VGA,edid=on,xres=$XRES,yres=$YRES"
       -device qemu-xhci -device usb-kbd)
else
  # 'virt' has no legacy VGA; virtio-gpu-pci supports EDID-driven sizing. Pin the
  # PCIe slots (GPU 0x9, xHCI 0xa) so they don't shift the boot disk's address
  # (which would drop us into the UEFI Shell instead of auto-launching).
  GFX=(-device "virtio-gpu-pci,edid=on,xres=$XRES,yres=$YRES,addr=0x9"
       -device qemu-xhci,addr=0xa -device usb-kbd)
fi

if [ ! -f "$ESP/EFI/BOOT/$EFI_NAME" ]; then
  echo "No staged image at $ESP/EFI/BOOT/$EFI_NAME — run 'make docker-stage ARCH=$ARCH' first." >&2
  exit 1
fi

exec "$QEMU" "${MACHINE[@]}" -m 512M \
  -drive if=pflash,format=raw,readonly=on,file="$FW_CODE" \
  -drive if=pflash,format=raw,file="$FW_VARS" \
  -drive format=raw,file=fat:rw:"$ESP" \
  "${GFX[@]}" "${NET[@]}" -display "$DISPLAY_BACKEND" \
  -no-reboot \
  -serial mon:stdio
