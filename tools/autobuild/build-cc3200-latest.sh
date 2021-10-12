#!/bin/bash

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/cc3200-build-$board
    $MICROPY_AUTOBUILD_MAKE $@ BTARGET=application BOARD=$board BUILD=$build_dir || exit 1
    zip $dest_dir/$descr$fw_tag.zip $build_dir/mcuimg.bin
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
if [ ! -r application.mk ]; then
    echo "must be in cc3200 directory"
    exit 1
fi

# build the versions
do_build wipy WIPY
