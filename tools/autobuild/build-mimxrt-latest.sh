#!/bin/bash

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    ext=$3
    shift
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/mimxrt-build-$board
    $MICROPY_AUTOBUILD_MAKE $@ BOARD=$board BUILD=$build_dir || exit 1
    mv $build_dir/firmware.$ext $dest_dir/$descr$fw_tag.$ext
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
if [ ! -r modmimxrt.c ]; then
    echo "must be in mimxrt directory"
    exit 1
fi

# build the boards
do_build TEENSY40 TEENSY40 hex
do_build TEENSY41 TEENSY41 hex
do_build MIMXRT1020_EVK MIMXRT1020_EVK bin
do_build MIMXRT1050_EVKB MIMXRT1050_EVKB bin
