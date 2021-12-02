#!/bin/bash
#
# The functions in this file can be run independently to build boards.
# For example:
#
#   $ source build-boards.sh
#   $ MICROPY_AUTOBUILD_MAKE=make build_rp2_boards -latest /tmp

function build_boards {
    # check/get parameters
    if [ $# -lt 4 ]; then
        echo "usage: $0 <fw-tag> <dest-dir> <check-file> <exts...>"
        return 1
    fi

    fw_tag=$1
    dest_dir=$2
    check_file=$3
    shift
    shift
    shift

    # check we are in the correct directory
    if [ ! -r $check_file ]; then
        echo "must be in directory containing $check_file"
        return 1
    fi

    for board_json in $(find boards/ -name board.json | sort); do
        board=$(echo $board_json | awk -F '/' '{ print $2 }')
        descr=$(cat $board_json | python3 -c "import json,sys; print(json.load(sys.stdin).get('id', '$board'))")
        build_dir=/tmp/micropython-build-$board

        echo "building $descr $board"
        $MICROPY_AUTOBUILD_MAKE BOARD=$board BUILD=$build_dir && (
            for ext in $@; do
                mv $build_dir/firmware.$ext $dest_dir/$descr$fw_tag.$ext
            done
        )
        rm -rf $build_dir
    done
}

function build_mimxrt_boards {
    build_boards $1 $2 modmimxrt.c bin hex
}

function build_rp2_boards {
    build_boards $1 $2 modrp2.c uf2
}

function build_samd_boards {
    build_boards $1 $2 samd_soc.c uf2
}

function build_stm32_boards {
    build_boards $1 $2 modpyb.c dfu hex
}
