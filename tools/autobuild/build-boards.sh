#!/bin/bash
#
# The functions in this file can be run independently to build boards.
# For example:
#
#   $ source build-boards.sh
#   $ MICROPY_AUTOBUILD_MAKE=make build_rp2_boards -latest /tmp

function build_board {
    # check/get parameters
    if [ $# -lt 4 ]; then
        echo "usage: $0 <board-json-file> <fw-tag> <dest-dir> <exts...>"
        return 1
    fi

    board_json=$1
    fw_tag=$2
    dest_dir=$3
    shift
    shift
    shift

    board=$(echo $board_json | awk -F '/' '{ print $2 }')
    descr=$(cat $board_json | python3 -c "import json,sys; print(json.load(sys.stdin).get('id', '$board'))")
    build_dir=/tmp/micropython-build-$board

    echo "building $descr $board"
    $MICROPY_AUTOBUILD_MAKE BOARD=$board BUILD=$build_dir && (
        for ext in $@; do
            dest=$dest_dir/$descr$fw_tag.$ext
            if [ -r $build_dir/firmware.$ext ]; then
                mv $build_dir/firmware.$ext $dest
            elif [ -r $build_dir/micropython.$ext ]; then
                # esp32 has micropython.elf, etc
                mv $build_dir/micropython.$ext $dest
            fi
        done
    )
    rm -rf $build_dir
}

function build_boards {
    # check/get parameters
    if [ $# -lt 4 ]; then
        echo "usage: $0 <check-file> <fw-tag> <dest-dir> <exts...>"
        return 1
    fi

    check_file=$1
    shift

    # check we are in the correct directory
    if [ ! -r $check_file ]; then
        echo "must be in directory containing $check_file"
        return 1
    fi

    # build all boards that have a board.json file
    for board_json in $(find boards/ -name board.json | sort); do
        build_board $board_json $@
    done
}

function build_esp32_boards {
    build_boards modesp32.c $1 $2 bin elf map uf2
}

function build_mimxrt_boards {
    build_boards modmimxrt.c $1 $2 bin hex
}

function build_nrf_boards {
    build_boards nrfx_glue.h $1 $2 bin hex uf2
}

function build_renesas_ra_boards {
    build_boards ra_it.c $1 $2 hex
}

function build_rp2_boards {
    build_boards modrp2.c $1 $2 uf2
}

function build_samd_boards {
    build_boards samd_soc.c $1 $2 uf2
}

function build_stm32_boards {
    build_boards modpyb.c $1 $2 dfu hex
}
