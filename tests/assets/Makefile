MPY_DIR = ../..

PYTHON := $(command -v python3 2> /dev/null)
ifndef PYTHON
PYTHON = python
endif

# Files created with dd if=/dev/urandom ... - the file name is the CRC32
# of the data contained in the file itself.

ROMFS_FILES = romfs_source/0x30d83fe5.bin \
	      romfs_source/0x37bef0eb.bin \
	      romfs_source/0x442f3b5f.bin \
	      romfs_source/0x648793fb.bin \
	      romfs_source/0x913837b6.bin \
	      romfs_source/0xdb14aac7.bin \
	      romfs_source/romfs_sentinel.txt

.PHONY: romfs
romfs: all

.PHONY: clean
clean:
	rm -f random_romfs.bin

all: random_romfs.bin

random_romfs.bin: $(ROMFS_FILES)
	$(PYTHON) $(MPY_DIR)/tools/mpremote/mpremote.py romfs --partition 0 --no-mpy --output $@ build romfs_source
