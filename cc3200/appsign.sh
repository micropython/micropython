#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: appsign.sh *build dir*"
    exit 1
fi

# Build location
BUILD=$1

# Generate the MD5 hash
# md5 on Darwin, md5sum on Unix
if [ `uname -s` = "Darwin" ]; then
echo -n `md5 -q $BUILD/application.bin` > __md5hash.bin
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
