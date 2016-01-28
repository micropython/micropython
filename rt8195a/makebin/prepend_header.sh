##!/bin/bash 

################
# Library
################
Usage() {
    echo "Usage: $0 [Image Name] [Start Symbol Name] [Symbols List File]"
}

# Parameter:
#     value, width, dest
function MakeFixedWidthHeaderString() {
    local __value=$1
    local __width=$2
    local __dest=$3
    local __header_raw
    local __header_raw_reorder
    local __header_array

    if [[ "$__dest" ]]; then

        __header_raw=$(printf "%0""$__width""x" $__value)
        # echo $__header_raw

        # 20000680 to 80060020
        for (( i=$__width; i > 0; i-=2 ))
        do
            __header_raw_reorder+=$(echo $__header_raw | cut -b $((i-1)))
            __header_raw_reorder+=$(echo $__header_raw | cut -b $i)
        done
        # echo $__header_raw_reorder

        __header_array=($(echo $__header_raw_reorder | sed 's/\(.\)/\1 /g'))

        for (( i=0; i < $__width; i+=2))
        do
            eval $__dest+='\\x'"${__header_array[$i]}${__header_array[$i+1]}"
        done
    fi
}

################
# Main
################
if [ "$#" -lt 3 ]; then
    Usage
    exit 1
fi

# Get Parameters
IMAGE_FILENAME=$1
IMAGE_SECTION_START_NAME=$2
SYMBOL_LIST=$3
IMG2_OFFSET=$4

# Constant Variables
PATTERN_1=0x96969999
PATTERN_2=0xFC66CC3F
PATTERN_3=0x03CC33C0
PATTERN_4=0x6231DCE5
RSVD=0xFFFFFFFFFFFFFFFF
IMAGE_LEN=$(du -b $IMAGE_FILENAME | cut -f 1)
IMAGE_ADDR="0x$(grep $IMAGE_SECTION_START_NAME $SYMBOL_LIST | awk '{print $1}')"
IMAGE_FILENAME_PREPEND="${IMAGE_FILENAME%.*}"'_prepend.'"${IMAGE_FILENAME##*.}"

IMAGE_FILENAME_NEW=$(basename $IMAGE_FILENAME)

HEADER_FINAL=''
if [ "$IMAGE_FILENAME_NEW" == "ram_1.bin" ]; then
    MakeFixedWidthHeaderString $PATTERN_1   8  HEADER_FINAL
    MakeFixedWidthHeaderString $PATTERN_2   8  HEADER_FINAL
    MakeFixedWidthHeaderString $PATTERN_3   8  HEADER_FINAL
    MakeFixedWidthHeaderString $PATTERN_4   8  HEADER_FINAL
fi

MakeFixedWidthHeaderString $IMAGE_LEN   8  HEADER_FINAL
MakeFixedWidthHeaderString $IMAGE_ADDR  8  HEADER_FINAL
if [ "$IMAGE_FILENAME_NEW" == "ram_1.bin" ]; then
    MakeFixedWidthHeaderString $IMG2_OFFSET  4  HEADER_FINAL
    MakeFixedWidthHeaderString $RSVD        12  HEADER_FINAL
else
    MakeFixedWidthHeaderString $RSVD        16  HEADER_FINAL
fi

# echo $HEADER_FINAL

echo -n -e $HEADER_FINAL | cat - $IMAGE_FILENAME > $IMAGE_FILENAME_PREPEND

