#!/bin/bash
#
# This script check that changes don't lead to code size regressions.
# (Size of the language core (== minimal port should not grow)).
#

REFERENCE=$HOME/persist/firmware.bin
#REFERENCE=/tmp/micropython
#TRAVIS_PULL_REQUEST=false

if [ -f $REFERENCE ]; then
    size_old=$(stat -c%s $REFERENCE)
    size_new=$(stat -c%s minimal/build/firmware.bin)
    echo "Old size: $size_old new size: $size_new"
    if [ $size_new -gt $size_old ]; then
        echo "Validation failure: Core code size increased"
        if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
            exit 1
        fi
    fi
else
    echo "Warning: reference file doesn't exist, code size check didn't run"
fi
