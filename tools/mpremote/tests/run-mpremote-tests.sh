#!/bin/bash
set -e

TESTS_DIR="${TESTS_DIR:-$(dirname $0)}"
RESULT_DIR="${RESULT_DIR:-${TESTS_DIR}/results}"
export MPREMOTE="${MPREMOTE:-${TESTS_DIR}/../mpremote.py}"
DIFF="diff --unified --strip-trailing-cr"

if [ -z "$1" ]; then
    # Find tests matching test_*.sh
    declare -a TESTS=("${TESTS_DIR}"/test_*.sh)
else
    # Specific test paths from the command line.
    declare -a TESTS=("$@")
fi

mkdir --parents "${RESULT_DIR}"

for test in "${TESTS[@]}"; do
    export TMP=$(mktemp --directory | tr '\\' /)
    result="${RESULT_DIR}/$(basename "${test}")"

    echo -n "${test}: "

    envsubst <"${test}.exp" >"${result}.exp"
    if "${test}" >"${result}.out" 2>&1; then
        if $DIFF "${result}.out" "${result}.exp" >/dev/null; then
            echo "OK"
            rm "${result}.out" "${result}.exp"
        else
            echo "FAIL"
        fi
    else
        echo "CRASH"
    fi
    rm -r "${TMP}"
done

for test in "${TESTS[@]}"; do
    result="${RESULT_DIR}/$(basename "${test}")"
    if [ -e "${result}.out" ]; then
        echo "FAILURE ${test}"
        $DIFF "${result}.exp" "${result}.out"
    fi
done
