#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: bootgen.sh *build dir*"
    exit 1
fi

BUILD=$1

# Re-locator Path
RELOCATOR=bootmgr/relocator

# Build location
BOOTMGR=${BUILD}

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
