#!/bin/bash
# Build additional variants of pyboard firmware (base variants are built by build-boards.sh).

# function for building firmware
function do_build() {
    descr=$1
    board=$2
    shift
    shift
    for variant in `$MICROPY_AUTOBUILD_MAKE BOARD=$board query-variants | grep VARIANTS: | cut -d' ' -f2-`; do
        target=$descr-$variant
        echo "building $target $board"
        build_dir=/tmp/stm-build-$board-$variant
        $MICROPY_AUTOBUILD_MAKE $@ BOARD=$board BOARD_VARIANT=$variant BUILD=$build_dir || exit 1
        mv $build_dir/firmware.dfu $dest_dir/$target$fw_tag.dfu
        mv $build_dir/firmware.hex $dest_dir/$target$fw_tag.hex
        rm -rf $build_dir
    done
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

# build the variants for each board
do_build pybv10 PYBV10
do_build pybv11 PYBV11
do_build pyblitev10 PYBLITEV10
