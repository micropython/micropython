#!/bin/sh

set -eu

if [ -z "${LARGE_SECRET_PASSPHRASE:-}" ]; then
    echo "ERROR: LARGE_SECRET_PASSPHRASE is not set" >&2
    exit 2
fi

gpg --quiet --batch --yes --decrypt --passphrase="$LARGE_SECRET_PASSPHRASE" \
    --output tests/ports/psoc-edge/mp_custom/secrets.py \
    tests/ports/psoc-edge/mp_custom/secrets.py.gpg
