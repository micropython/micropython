#!/bin/sh
HERE="$(readlink -f "$(dirname "$0")")"
TOP="$(readlink -f "${HERE}/../..")"

if [ $# -eq 0 ]; then it=-it; else it=; fi

docker run \
    -w /work/ports/m68kmac --rm \
    -v "${TOP}":/work \
    ${it} ghcr.io/autc04/retro68 "$@"
