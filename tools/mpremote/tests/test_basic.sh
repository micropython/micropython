#!/bin/bash
set -e

# SKIP FOR NOW
echo "SKIP"
exit 0


# retrieve connected port and device id
$MPREMOTE devs > "${TMP}/devices.txt"
dev_port=$(awk 'NR==1 {print $1}' "${TMP}/devices.txt")
dev_id=$(awk 'NR==1 {print $2}' "${TMP}/devices.txt")
echo -----
# connect to specific port:
$MPREMOTE connect "${dev_port}" exec "print('connect <port>')" 
echo -----
# connect to specific port:
$MPREMOTE connect "port:${dev_port}" exec "print('connect port:<port>')" 
echo -----
# specific device id:
# only if device id is not "None"
if [ "$dev_id" != "None" ]; then
    $MPREMOTE connect "id:${dev_id}" exec "print('connect id:')"
else
    echo "SKIP : connect id: as device id is None"
fi
echo -----
# mismatched  port:
$MPREMOTE connect "port:/dev/tty/ACM1234dead" exec "print('connect wrong port:')" || echo "expect error: $?"
echo -----
# mismatched  device id:
$MPREMOTE connect "id:1234deadbeef" exec "print('connect wrong id:')" || echo "expect error: $?"

echo -----
# cp multiple files 
# TODO Add ramdisk
mkdir -p "${TMP}/data"
dd if=/dev/zero of="${TMP}/data/file1.txt" bs=1 count=20 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file2.txt" bs=1 count=204 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file3.txt" bs=1 count=1096 > /dev/null 2>&1
$MPREMOTE cp "${TMP}/data/file1.txt" "${TMP}/data/file3.txt" :

echo -----
# set & get time
$MPREMOTE rtc --set && echo "set time success $?"
$MPREMOTE rtc > /dev/null && echo "get time success $?"

####################################################################################
# ROMFS 
####################################################################################
echo -----
# not sure if romfs is supported 
$MPREMOTE romfs query > /dev/null && echo "returns $?"

echo -----
# create a romfs with some data 
$MPREMOTE romfs -o "${TMP}/data.romfs" build "${TMP}/data" 

echo -----
# deploy the romfs to the device
$MPREMOTE romfs deploy "${TMP}/data.romfs"
$MPREMOTE ls :/rom

echo -----
# too much data 
mkdir -p "${TMP}/bigdata"
dd if=/dev/urandom of="${TMP}/bigdata/file21.bin" bs=1 count=500000 > /dev/null 2>&1
dd if=/dev/urandom of="${TMP}/bigdata/file22.bin" bs=1 count=600000 > /dev/null 2>&1
dd if=/dev/urandom of="${TMP}/bigdata/file23.bin" bs=1 count=700000 > /dev/null 2>&1
dd if=/dev/urandom of="${TMP}/bigdata/file24.bin" bs=1 count=800000 > /dev/null 2>&1
dd if=/dev/urandom of="${TMP}/bigdata/file25.bin" bs=1 count=900000 > /dev/null 2>&1

$MPREMOTE romfs -o "${TMP}/bigdata.romfs" build "${TMP}/bigdata" 
# deploy the romfs to the device
$MPREMOTE romfs deploy "${TMP}/bigdata.romfs" || echo "expect error: $?"
echo -----
# not a romfs
dd if=/dev/urandom of="${TMP}/notreally.romfs" bs=1 count=5000 > /dev/null 2>&1
$MPREMOTE romfs deploy "${TMP}/notreally.romfs" || echo "expect error: $?"


####################################################################################
#  mpremote mip
####################################################################################
# name, name@version, github:org/repo, github:org/repo@branch, gitlab:org/repo, gitlab:org/repo@branch

echo -----
$MPREMOTE mip install __future__
$MPREMOTE mip install __future__@0.1.0
$MPREMOTE mip install github:micropython/micropython-lib/python-stdlib/__future__/__future__.py
# TODO : Need smaller test package
$MPREMOTE mip install github:josverl/micropython-stubber
$MPREMOTE mip install github:josverl/micropython-stubber@main
$MPREMOTE mip install github:josverl/micropython-stubber/mip/minified.json

# TODO : 
# - gitlab
# - package with dependencies


###################################
