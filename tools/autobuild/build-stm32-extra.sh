#!/bin/bash
# Build additional variants of pyboard firmware (base variants are built by build-boards.sh).

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    shift
    shift
    echo "building $descr $board"
    build_dir=/tmp/stm-build-$board
    $MICROPY_AUTOBUILD_MAKE $@ BOARD=$board BUILD=$build_dir || exit 1
    mv $build_dir/firmware.dfu $dest_dir/$descr$fw_tag.dfu
    mv $build_dir/firmware.hex $dest_dir/$descr$fw_tag.hex
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
if [ ! -r modpyb.c ]; then
    echo "must be in stm directory"
    exit 1
fi

# build the versions
do_build pybv3 PYBV3
do_build pybv3-network PYBV3 MICROPY_PY_WIZNET5K=5200 MICROPY_PY_CC3K=1
do_build pybv10-dp PYBV10 MICROPY_FLOAT_IMPL=double
do_build pybv10-thread PYBV10 CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
do_build pybv10-dp-thread PYBV10 MICROPY_FLOAT_IMPL=double CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
do_build pybv10-network PYBV10 MICROPY_PY_WIZNET5K=5200 MICROPY_PY_CC3K=1
do_build pybv11-dp PYBV11 MICROPY_FLOAT_IMPL=double
do_build pybv11-thread PYBV11 CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
do_build pybv11-dp-thread PYBV11 MICROPY_FLOAT_IMPL=double CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
do_build pybv11-network PYBV11 MICROPY_PY_WIZNET5K=5200 MICROPY_PY_CC3K=1
do_build pyblitev10-dp PYBLITEV10 MICROPY_FLOAT_IMPL=double
do_build pyblitev10-thread PYBLITEV10 CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
do_build pyblitev10-dp-thread PYBLITEV10 MICROPY_FLOAT_IMPL=double CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
do_build pyblitev10-network PYBLITEV10 MICROPY_PY_WIZNET5K=5200 MICROPY_PY_CC3K=1
