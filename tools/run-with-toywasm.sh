#! /bin/sh

if [ ! -e "${MICROPY_MICROPYTHON_WASM}" ]; then
    echo "MICROPY_MICROPYTHON_WASM is not set properly" >&2
    exit 1
fi

exec ${TOYWASM:-toywasm} --wasi --wasi-dir=/ \
${MICROPY_MICROPYTHON_WASM} \
-- "$@"
