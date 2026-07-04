#!/usr/bin/env bash
# One-shot failure context after a red `make test`: serial + OVMF debug logs.
set -uo pipefail
cd "$(dirname "$0")/.."
# shellcheck source=scripts/env.sh
source "scripts/env.sh" "${1:-aa64}" 2>/dev/null || true

echo "===== firmware/serial.log (tail) ====="
tail -n 80 firmware/serial.log 2>/dev/null || echo "(none)"
echo
echo "===== firmware/debug.log (OVMF DEBUG(), tail) ====="
tail -n 80 firmware/debug.log 2>/dev/null || echo "(none)"
