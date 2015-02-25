#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

BOARD=$1
BTYPE=$2

# Build location
# First parameter passed is the board type
BUILD=build/${BOARD}/${BTYPE}

# Generate the MD5 hash
echo -n md5sum --binary $BUILD/application.bin | awk '{ print $1 }' > __md5hash.bin

# Concatenate it with the application binary
cat $BUILD/application.bin __md5hash.bin > $BUILD/MCUIMG.BIN
RET=$?

# Remove the tmp files
rm -f __md5hash.bin

# Remove hte unsigned binary
rm -f $BUILD/application.bin

exit $RET
