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
    # check/get parameters
    if [ $# != 2 ]; then
        echo "usage: $0 <fw-tag> <dest-dir>"
        return 1
    fi

    fw_tag=$1
    dest_dir=$2

    # check we are in the correct directory
    if [ ! -r modesp32.c ]; then
        echo "must be in esp32 directory"
        return 1
    fi

    # build the boards, based on the IDF version
    for board_json in $(find boards/ -name board.json | sort); do
        mcu=$(cat $board_json | python3 -c "import json,sys; print(json.load(sys.stdin).get('mcu', 'unknown'))")
        if idf.py --version | grep -q v4.2; then
            if [ $mcu = esp32 ]; then
                # build standard esp32-based boards with IDF v4.2
                if echo $board_json | grep -q GENERIC; then
                    # traditionally, GENERIC and GENERIC_SPIRAM boards used manifest_release.py
                    MICROPY_AUTOBUILD_MAKE="$MICROPY_AUTOBUILD_MAKE FROZEN_MANIFEST=$(pwd)/boards/manifest_release.py" build_board $board_json $fw_tag $dest_dir bin elf map
                else
                    build_board $board_json $fw_tag $dest_dir bin elf map
                fi
            fi
        else
            if [ $mcu != esp32 ]; then
                # build esp32-s2/s3/c3 based boards with IDF v4.4+
                build_board $board_json $fw_tag $dest_dir bin elf map uf2
            fi
        fi
    done
}

function build_mimxrt_boards {
    build_boards modmimxrt.c $1 $2 bin hex
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
