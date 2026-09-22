#!/usr/bin/env bash
# Interactive serial REPL on the host terminal (serial -> stdio, with QEMU monitor).
set -euo pipefail
cd "$(dirname "$0")/.."
# shellcheck source=scripts/env.sh
source "scripts/env.sh" "${1:-aa64}"

exec "$QEMU" "${MACHINE[@]}" -m 512M \
  -drive if=pflash,format=raw,readonly=on,file="$FW_CODE" \
  -drive if=pflash,format=raw,file="$FW_VARS" \
  -drive format=raw,file=fat:rw:"$ESP" \
  "${NET[@]}" \
  -nographic -no-reboot \
  -serial mon:stdio
