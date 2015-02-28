#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: bootgen.sh *board type* *build type*"
    exit 1
fi

BOARD=$1
BTYPE=$2

# Re-locator Path
RELOCATOR=bootmgr/relocator

# Boot Manager Path
# First parameter passed is the board type
BOOTMGR=bootmgr/build/${BOARD}/${BTYPE}

# Check for re-locator binary
if [ ! -f $RELOCATOR/relocator.bin ]; then

	echo "Error : Relocator Not found!"
	exit 1
else
	echo "Relocator found..."
fi

# Check for boot manager binary
if [ ! -f $BOOTMGR/bootmgr.bin ]; then

	echo "Error : Boot Manager Not found!"
	exit 1
else
	echo "Boot Manager found..."
fi

# echo
echo "Generating bootloader..."

# Generate an all 0 bin file
dd if=/dev/zero of=__tmp.bin ibs=1 count=256 conv=notrunc >/dev/null 2>&1

# Generate a 0 padded version of the relocator 
dd if=$RELOCATOR/relocator.bin of=__tmp.bin ibs=1 conv=notrunc >/dev/null 2>&1

# Concatenate the re-locator and the boot-manager
cat __tmp.bin $BOOTMGR/bootmgr.bin > $BOOTMGR/bootloader.bin

# Remove the tmp files
rm -f __tmp.bin

# Remove bootmgr.bin
rm -f $BOOTMGR/bootmgr.bin
