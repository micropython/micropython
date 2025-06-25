#!/bin/sh
HERE="$(dirname "$0")"
TOP="$(readlink -f "${HERE}/../..")"
docker run -w /work/ports/m68kmac --rm --mount type=bind,source=${TOP},destination=/work -it ghcr.io/autc04/retro68 "$@"
