#!/usr/bin/env bash
# Boot with a gdbstub on :1234 for gdb-multiarch / CLion to attach. No -S: by the
# time you interrupt, the CPU is already in long mode (x64) / EL1 (aa64), which keeps
# the debugger's arch detection sane.
set -euo pipefail
cd "$(dirname "$0")/.."
# shellcheck source=scripts/env.sh
source "scripts/env.sh" "${1:-aa64}"

echo "gdbstub on tcp::1234 — attach with: gdb-multiarch -ex 'target remote :1234'"
exec "$QEMU" "${MACHINE[@]}" -m 512M \
  -drive if=pflash,format=raw,readonly=on,file="$FW_CODE" \
  -drive if=pflash,format=raw,file="$FW_VARS" \
  -drive format=raw,file=fat:rw:"$ESP" \
  -nographic -no-reboot \
  -serial mon:stdio \
  -gdb tcp::1234
