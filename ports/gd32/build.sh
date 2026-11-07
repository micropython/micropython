#!/bin/bash
# Convenience build script for the GD32 port.
# Usage: ./build.sh [GD32F407|GD32F470|all]
set -e
cd "$(dirname "$0")"

TARGET="${1:-all}"
JOBS="${JOBS:-1}"

build_one() {
    local board="$1"
    echo "====> Building BOARD=$board"
    make BOARD="$board" -j"$JOBS" dist
    echo
}

case "$TARGET" in
    all)
        build_one GD32F407
        build_one GD32F470
        ;;
    GD32F407|GD32F470)
        build_one "$TARGET"
        ;;
    clean)
        rm -rf build build-* dist
        echo "cleaned"
        ;;
    *)
        echo "Usage: $0 [GD32F407|GD32F470|all|clean]"
        exit 1
        ;;
esac

echo "Artifacts:"
find dist -type f -name "firmware.*" -exec ls -lh {} \; 2>/dev/null | awk '{print "  "$5"\t"$9}'
