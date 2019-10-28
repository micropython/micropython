// Fake source file used only to capture #define values for use in ld template files.
#include "mpconfigport.h"

// For each value needed in the LD file, create a C-like line:
// NAME_OF_VALUE; ///DEFINE_VALUE NAME_OF_VALUE <optional Python lambda expression to transform value>
// The C preprocessor will replace NAME_OF_VALUE with the actual value.
// This will be post-processed by tools/gen_ld_files.py to extract the name and vlaue.

BOOTLOADER_SIZE; ///DEFINE_VALUE BOOTLOADER_SIZE
RAM_SIZE; ///DEFINE_VALUE RAM_SIZE def noUL(expr): import re; return re.sub(
FLASH_SIZE; ///DEFINE_VALUE FLASH_SIZE lambda expr: expr.rstrip("UL")
CIRCUITPY_FIRMWARE_SIZE; ///DEFINE_VALUE CIRCUITPY_FIRMWARE_SIZE
CIRCUITPY_DEFAULT_STACK_SIZE; ///DEFINE_VALUE CIRCUITPY_DEFAULT_STACK_SIZE
