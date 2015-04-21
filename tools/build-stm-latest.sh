#!/bin/bash

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/stm-build-$board
    make -B $@ BOARD=$board BUILD=$build_dir || exit 1
    mv $build_dir/firmware.dfu $dest_dir/$descr-$date-$git_tag.dfu
    rm -rf $build_dir
}

# check/get parameters
if [ $# != 1 ]; then
    echo "usage: $0 <dest-dir>"
    exit 1
fi

dest_dir=$1

# check we are in the correct directory
if [ ! -r modpyb.c ]; then
    echo "must be in stm directory"
    exit 1
fi

# get the date
date=$(date '+%Y-%m-%d')

# get the git tag
git_tag="$(git describe --dirty || echo unknown)"

# build the versions
do_build pybv3 PYBV3
do_build pybv3-network PYBV3 MICROPY_PY_WIZNET5K=1 MICROPY_PY_CC3K=1
do_build pybv10 PYBV10
do_build pybv10-network PYBV10 MICROPY_PY_WIZNET5K=1 MICROPY_PY_CC3K=1
do_build stm32f4disc STM32F4DISC
do_build espruino-pico ESPRUINO_PICO
