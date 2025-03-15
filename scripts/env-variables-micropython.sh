#!/bin/bash

# Set environment variables for MicroPython development

BUILD_VERBOSE=1

MICROPYTHON=$(pwd)
echo "MICROPYTHON=$MICROPYTHON"
if [[ "$MICROPYTHON" == */scripts ]]; then
    MICROPYTHON="${MICROPYTHON%/scripts}"
fi
echo "MICROPYTHON=$MICROPYTHON"