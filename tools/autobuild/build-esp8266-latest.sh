#!/bin/bash

PYTHON3=python3
yaota8266=$HOME/yaota8266

# for debugging
#exec &> /tmp/esp-log-$$.txt

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/esp8266-build-$board
    $MICROPY_AUTOBUILD_MAKE $@ BOARD=$board BUILD=$build_dir || exit 1
    mv $build_dir/firmware-combined.bin $dest_dir/$descr$fw_tag.bin
    mv $build_dir/firmware.elf $dest_dir/$descr$fw_tag.elf
    mv $build_dir/firmware.map $dest_dir/$descr$fw_tag.map
    rm -rf $build_dir
}

function do_build_ota() {
    descr=$1
    board=$2
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/esp8266-build-$board
    $MICROPY_AUTOBUILD_MAKE $@ BOARD=$board BUILD=$build_dir || exit 1
    cat $yaota8266/yaota8266.bin $build_dir/firmware-ota.bin > $dest_dir/$descr$fw_tag.bin
    pushd $yaota8266/ota-client
    $PYTHON3 ota_client.py sign $build_dir/firmware-ota.bin
    popd
    mv $build_dir/firmware-ota.bin.ota $dest_dir/$descr$fw_tag.ota
    mv $build_dir/firmware.elf $dest_dir/$descr$fw_tag.elf
    mv $build_dir/firmware.map $dest_dir/$descr$fw_tag.map
    rm -rf $build_dir
}

# check/get parameters
if [ $# != 2 ]; then
    echo "usage: $0 <fw-tag> <dest-dir>"
    exit 1
fi

fw_tag=$1
dest_dir=$2

# check we are in the correct directory
if [ ! -r boards/esp8266_common.ld ]; then
    echo "must be in esp8266 directory"
    exit 1
fi

# build the versions
do_build esp8266 GENERIC
do_build esp8266-512k GENERIC_512K
do_build esp8266-1m GENERIC_1M
do_build_ota esp8266-ota GENERIC_1M ota
