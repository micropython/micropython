#!/bin/bash
set -e

TEST_DIR=$(dirname $0)
MPREMOTE=${TEST_DIR}/../mpremote.py

if [ -z "$1" ]; then
    # Find tests matching test_*.sh
    TESTS=${TEST_DIR}/test_*.sh
else
    # Specific test path from the command line.
    TESTS="$1"
fi

for t in $TESTS; do
    TMP=$(mktemp -d)
    echo -n "${t}: "
    # Strip CR and replace the random temp dir with a token.
    if env MPREMOTE=${MPREMOTE} TMP="${TMP}" "${t}" 2>&1 | tr -d '\r' | sed "s,${TMP},"'${TMP},g' > "${t}.out"; then
        if diff "${t}.out" "${t}.exp" > /dev/null; then
            echo "OK"
        else
            echo "FAIL"
            diff "${t}.out" "${t}.exp" || true
        fi
    else
        echo "CRASH"
    fi
    rm -r "${TMP}"
done
