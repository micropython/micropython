#!/bin/bash
set -e

TEST_DIR=$(dirname $0)
MPREMOTE=${TEST_DIR}/../mpremote.py

# Parse command line options
DEVICE=""
TESTS=""
COVERAGE=false

show_help() {
    echo "Usage: $(basename $0) [-t <device>] [-c] [test_file.sh ...]"
    echo ""
    echo "Run mpremote tests against a MicroPython target."
    echo ""
    echo "Options:"
    echo "  -t <device>   Test instance/device to connect to. Examples:"
    echo "                  -t a0              Connect to /dev/ttyACM0"
    echo "                  -t u0              Connect to /dev/ttyUSB0"
    echo "                  -t c0              Connect to COM0"
    echo "                  -t /dev/ttyACM0    Connect to specified serial port"
    echo "                  -t rfc2217://host:port  Connect via RFC2217"
    echo "                If not specified, uses mpremote's auto-detection."
    echo "  -c            Enable coverage collection"
    echo "  -h            Show this help message"
    echo ""
    echo "Arguments:"
    echo "  test_file.sh  Specific test file(s) to run. If not specified,"
    echo "                all test_*.sh files in the tests directory are run."
    echo ""
    echo "Examples:"
    echo "  $(basename $0) -c                    Run all tests with coverage"
    echo "  $(basename $0) -t a0 -c              Run all tests on /dev/ttyACM0 with coverage"
    echo "  $(basename $0) -c tests/test_fs.sh   Run specific test with coverage"
    exit 0
}

while getopts "t:ch" opt; do
    case $opt in
        t)
            DEVICE="$OPTARG"
            ;;
        c)
            COVERAGE=true
            ;;
        h)
            show_help
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done
shift $((OPTIND-1))

# Convert device shortcuts to full paths (similar to run-tests.py)
if [ -n "$DEVICE" ]; then
    case "$DEVICE" in
        a[0-9]*)
            DEVICE="/dev/ttyACM${DEVICE#a}"
            ;;
        u[0-9]*)
            DEVICE="/dev/ttyUSB${DEVICE#u}"
            ;;
        c[0-9]*)
            DEVICE="COM${DEVICE#c}"
            ;;
    esac
    # Set MPREMOTE_DEVICE for tests to use
    export MPREMOTE_DEVICE="$DEVICE"
    # Update MPREMOTE to include connect command
    MPREMOTE="${MPREMOTE} connect ${DEVICE}"
    echo "Using device: $DEVICE"
fi

if [ -z "$1" ]; then
    # Find tests matching test_*.sh
    TESTS=${TEST_DIR}/test_*.sh
else
    TESTS=("$@")
fi

if [ "$COVERAGE" = true ]; then
    # Check if coverage is installed
    if ! command -v coverage &> /dev/null; then
        echo "Coverage is not installed. Please install it to run tests with coverage."
        exit 1
    fi
    echo "Coverage enabled"
fi

for t in $TESTS; do
    TMP=$(mktemp -d)
    echo -n "${t}: "
    if [ "$COVERAGE" = true ]; then
        # Use test filename as context (e.g., test_fs.sh)
        # Note: Don't use --append with parallel mode (set in pyproject.toml)
        TEST_NAME=$(basename "${t}")
        MPREMOTE="coverage run --context=${TEST_NAME} ${TEST_DIR}/../mpremote.py"
    fi
    # Strip CR and replace the random temp dir with a token.
    if env MPREMOTE="${MPREMOTE}" TMP="${TMP}" MPREMOTE_DEVICE="${DEVICE}" "${t}" 2>&1 | tr -d '\r' | sed "s,${TMP},"'${TMP},g' > "${t}.out"; then
        # Check if test was skipped (matches main test runner convention)
        if grep -q "^SKIP$" "${t}.out"; then
            echo "skip"
        elif diff "${t}.out" "${t}.exp" > /dev/null; then
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

if [ "$COVERAGE" = true ]; then
    echo ""
    echo "Coverage data collected. Run 'coverage combine && coverage report' to see results."
fi
