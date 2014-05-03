#!/bin/bash

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

# get the git hash
git_hash="$(git rev-parse --short HEAD 2> /dev/null || echo unknown)"

# build the versions
for board in PYBV3 PYBV10; do
    echo $board
    lower_board=$(echo $board | tr A-Z a-z)
    build_dir=/tmp/stm-build-$board
    make -B BOARD=$board BUILD=$build_dir || exit 1
    mv $build_dir/firmware.dfu $dest_dir/$lower_board-$date-$git_hash.dfu
    rm -rf $build_dir
done
