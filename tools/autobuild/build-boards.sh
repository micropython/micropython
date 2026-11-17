#!/bin/bash
#
# The functions in this file can be run independently to build boards.
# For example (using bash):
#
#   $ source tools/autobuild/build-boards.sh
#   $ cd ports/rp2
#   $ MICROPY_AUTOBUILD_MAKE="make -j8" build_rp2_boards -latest /tmp
#
# Or to build a single board:
#
#   $ source tools/autobuild/build-boards.sh
#   $ cd ports/rp2
#   $ MICROPY_AUTOBUILD_MAKE="make -j8" build_board boards/RPI_PICO/board.json -latest /tmp uf2

function copy_artefacts {
    local dest_dir=$1
    local board=$2
    local fw_tag=$3
    local build_dir=$4
    shift 4

    for ext in $@; do
        dest=$dest_dir/$board$fw_tag.$ext
        if [ -r $build_dir/firmware.$ext ]; then
            mv $build_dir/firmware.$ext $dest
        elif [ -r $build_dir/micropython.$ext ]; then
            # esp32 has micropython.elf, etc
            mv $build_dir/micropython.$ext $dest
        elif [ $ext = app-bin -a -r $build_dir/micropython.bin ]; then
            # esp32 has micropython.bin which is just the application
            mv $build_dir/micropython.bin $dest
        fi
    done
}

function build_board {
    # check/get parameters
    if [ $# -lt 4 ]; then
        echo "usage: $0 <board-json-file> <fw-tag> <dest-dir> <exts...>"
        return 1
    fi

    local board_json=$1
    local fw_tag=$2
    local dest_dir=$3
    shift 3

    local board=$(echo $board_json | awk -F '/' '{ print $2 }')

    # Build the "default" variant. For most boards this is the only thing we build.
    echo "$0: Autobuild started for $board"
    local build_dir=/tmp/micropython-build-$board
    if $MICROPY_AUTOBUILD_MAKE BOARD=$board BUILD=$build_dir; then
        copy_artefacts $dest_dir $board $fw_tag $build_dir $@
        echo "$0: Autobuild succeeded for $board"
    else
        echo "$0: Autobuild failed for $board"
    fi
    rm -rf $build_dir

    # Query variants from board.json and build them.
    for variant in `cat $board_json | python3 -c "import json,sys; print(' '.join(json.load(sys.stdin).get('variants', {}).keys()))"`; do
        local variant_build_dir=$build_dir-$variant
        echo "$0: Autobuild started for $board-$variant"
        if $MICROPY_AUTOBUILD_MAKE BOARD=$board BOARD_VARIANT=$variant BUILD=$variant_build_dir; then
            copy_artefacts $dest_dir $board-$variant $fw_tag $variant_build_dir $@
            echo "$0: Autobuild succeeded for $board-$variant"
        else
            echo "$0: Autobuild failed for $board-$variant"
        fi
        rm -rf $variant_build_dir
    done
}

function build_boards {
    # check/get parameters
    if [ $# -lt 4 ]; then
        echo "usage: $0 <check-file> <fw-tag> <dest-dir> <exts...>"
        return 1
    fi

    local check_file=$1
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

function build_alif_boards {
    build_boards modalif.c $1 $2 zip
}

function build_cc3200_boards {
    build_boards hal/cc3200_hal.c $1 $2 zip
}

function build_esp32_boards {
    build_boards modesp32.c $1 $2 bin elf map uf2 app-bin
}

function build_esp8266_boards {
    build_boards modesp.c $1 $2 bin elf map
}

function build_mimxrt_boards {
    build_boards modmimxrt.c $1 $2 bin hex uf2
}

function build_nrf_boards {
    build_boards nrfx_glue.h $1 $2 bin hex uf2
}

function build_psoc_edge_boards {
    build_boards modpsocedge.c $1 $2 zip
}

function build_renesas_ra_boards {
    build_boards ra_it.c $1 $2 bin hex
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
