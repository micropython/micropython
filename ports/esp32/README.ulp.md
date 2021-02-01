# ULP

To compile binarys for the ulp you need the ulp toolkit. Download it from https://github.com/espressif/binutils-esp32ulp/wiki#downloads
Then extract it, then add ```esp32ulp-elf-binutils/bin``` to your PATH

## Example Makefile

```make
ULP_S_SOURCES := main.S
ULP_APP_NAME := test
ULP_LD_SCRIPT := esp32.ulp.ld

SRC_PATH := src
BUILD_PATH := build

include $(ESPIDF)/components/ulp/Makefile.projbuild 

ULP_ELF := $(ULP_APP_NAME).elf
ULP_MAP := $(ULP_ELF:.elf=.map)
ULP_SYM := $(ULP_ELF:.elf=.sym)
ULP_BIN := $(ULP_ELF:.elf=.bin)
ULP_EXPORTS_LD := $(ULP_ELF:.elf=.ld)
ULP_EXPORTS_HEADER := $(ULP_ELF:.elf=.h)

ULP_OBJECTS := $(notdir $(ULP_S_SOURCES:.S=.ulp.o))
ULP_DEP := $(notdir $(ULP_S_SOURCES:.S=.ulp.d)) $(ULP_LD_SCRIPT:.ld=.d)
ULP_PREPROCESSED := $(notdir $(ULP_S_SOURCES:.S=.ulp.pS))
ULP_LISTINGS := $(notdir $(ULP_S_SOURCES:.S=.ulp.lst))

.PHONY: all clean

all: $(BUILD_PATH) $(BUILD_PATH)/$(ULP_BIN) 

clean:
	rm -rf $(BUILD_PATH)

$(BUILD_PATH):
	mkdir $@

# Generate preprocessed linker file.
$(BUILD_PATH)/$(ULP_APP_NAME).ld: $(SRC_PATH)/$(ULP_LD_SCRIPT)
	cpp -P $< -o $@

# Generate preprocessed assembly files.
# To inspect these preprocessed files, add a ".PRECIOUS: %.ulp.pS" rule. 
$(BUILD_PATH)/%.ulp.pS: $(SRC_PATH)/%.S
	cpp $< -o $@

# Compiled preprocessed files into object files.
$(BUILD_PATH)/%.ulp.o: $(BUILD_PATH)/%.ulp.pS
	$(ULP_AS) -al=$(patsubst %.ulp.o,%.ulp.lst,$@) -o $@ $<

# Link object files and generate map file
$(BUILD_PATH)/$(ULP_ELF): $(BUILD_PATH)/$(ULP_OBJECTS) $(BUILD_PATH)/$(ULP_APP_NAME).ld
	$(ULP_LD) -o $@ -A elf32-esp32ulp -Map=$(BUILD_PATH)/$(ULP_MAP) -T $(BUILD_PATH)/$(ULP_APP_NAME).ld $<

# Dump the list of global symbols in a convenient format.
$(ULP_SYM): $(ULP_ELF)
	$(ULP_NM) -g -f posix $< > $@

# Dump the binary for inclusion into the project 
$(BUILD_PATH)/$(ULP_BIN): $(BUILD_PATH)/$(ULP_ELF)
	$(ULP_OBJCOPY) -O binary $< $@
```

## Example linker script for the ulp
```
#define ULP_BIN_MAGIC		0x00706c75
#define HEADER_SIZE			12
#define CONFIG_ULP_COPROC_RESERVE_MEM	4096

MEMORY
{
    ram(RW) : ORIGIN = 0, LENGTH = CONFIG_ULP_COPROC_RESERVE_MEM
}

SECTIONS
{
    .text : AT(HEADER_SIZE)
    {
        *(.text)
    } >ram
    .data :
    {
        . = ALIGN(4);
        *(.data)
    } >ram
    .bss :
    {
        . = ALIGN(4);
        *(.bss)
    } >ram
    
    .header : AT(0)
    {
        LONG(ULP_BIN_MAGIC)
        SHORT(LOADADDR(.text)) 
        SHORT(SIZEOF(.text))
        SHORT(SIZEOF(.data))
        SHORT(SIZEOF(.bss))
    }
}
```

## Example ulp code
```asm
move R3, 99
move R0, 10

# mem[R0+0] = R3
st R3, R0, 0

HALT
```

## Example python code using the ulp
```python
import esp32
import time

u = esp32.ULP()
with open('test.bin', 'rb') as f:
    b = f.read()
u.load_binary(0,b)
u.run(0)
```
