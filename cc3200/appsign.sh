#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: appsign.sh *board type* *build type*"
    exit 1
fi

BOARD=$1
BTYPE=$2

# Build location
# Based on build type and board type
BUILD=build/${BOARD}/${BTYPE}

# Generate the MD5 hash
# md5 on Darwin, md5sum on Unix
if [ `uname -s` = "Darwin" ]; then
echo -n `md5 $BUILD/application.bin | awk '{ print $4 }'` > __md5hash.bin
else
echo -n `md5sum --binary $BUILD/application.bin | awk '{ print $1 }'` > __md5hash.bin
fi

# Concatenate it with the application binary
cat $BUILD/application.bin __md5hash.bin > $BUILD/mcuimg.bin
RET=$?

# Remove the tmp files
rm -f __md5hash.bin

# Remove the unsigned binary
rm -f $BUILD/application.bin

exit $RET
