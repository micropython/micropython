#!/bin/bash
#
# Build esp32 port

# for debugging
#exec &> /tmp/esp-log-$$.txt

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/esp32-build-$board
    rm -rf $build_dir # be sure we don't have anything leftover from a previous build
    make $@ BOARD=$board BUILD=$build_dir || exit 1
    mv $build_dir/firmware.bin $dest_dir/$descr$fw_tag.bin
    mv $build_dir/micropython.elf $dest_dir/$descr$fw_tag.elf
    mv $build_dir/micropython.map $dest_dir/$descr$fw_tag.map
    rm -rf $build_dir
}

# check/get parameters
if [ $# != 3 ]; then
    echo "usage: $0 <idf-path> <fw-tag> <dest-dir>"
    exit 1
fi

idf_path=$1
fw_tag=$2
dest_dir=$3

# check we are in the correct directory
if [ ! -r modesp32.c ]; then
    echo "must be in esp32 directory"
    exit 1
fi

source $idf_path/export.sh

# build the boards, based on the IDF version
if idf.py --version | grep -q v4.2; then
    do_build esp32 GENERIC FROZEN_MANIFEST=$(pwd)/boards/manifest_release.py
    do_build esp32spiram GENERIC_SPIRAM FROZEN_MANIFEST=$(pwd)/boards/manifest_release.py
    do_build tinypico UM_TINYPICO
    do_build wesp32 SIL_WESP32
else
    do_build esp32c3 GENERIC_C3
    do_build tinys2 UM_TINYS2
    do_build featherS2 UM_FEATHERS2
fi
