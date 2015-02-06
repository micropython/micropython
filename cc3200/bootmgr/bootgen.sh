#!/bin/bash

# Re-locator Path
RELOCATOR=bootmgr/relocator

# Boot Manager Path
# First parameter passed is the board type
BOOTMGR=bootmgr/build/$1

# Check for re-locator binary
if [ ! -f $RELOCATOR/relocator.bin ]; then

	echo "Error : Relocator Not found!"
	exit
else
	echo "Relocator found..."
fi

# Check for boot manager binary
if [ ! -f $BOOTMGR/bootmgr.bin ]; then

	echo "Error : Boot Manager Not found!"
	exit
else
	echo "Boot Manager found..."
fi

# echo
echo "Generating bootloader..."

# Generate an all 0 bin file
dd if=/dev/zero of=__tmp.bin ibs=1 count=256 conv=notrunc >/dev/null 2>&1

# Generate 0 a padded version of relocator 
dd if=$RELOCATOR/relocator.bin of=__tmp.bin ibs=1 conv=notrunc >/dev/null 2>&1

# Concatenate re-locator and boot-manager
cat __tmp.bin $BOOTMGR/bootmgr.bin > $BOOTMGR/bootloader.bin

# Remove the tmp files
rm -f __tmp.bin

# Remove bootmgr.bin
rm -f $BOOTMGR/bootmgr.bin
