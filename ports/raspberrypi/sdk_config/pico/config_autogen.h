#pragma once

#include "pico-sdk-configboard.h"

// alphabetized
#define LIB_CMSIS_CORE      (1)
#define LIB_PICO_BINARY_INFO (0)
#define LIB_PICO_PRINTF_NONE (0)
#define LIB_PICO_PRINTF_PICO (0)
#define LIB_PICO_STDIO_SEMIHOSTING (0)
#define LIB_PICO_STDIO_UART (0)
#define LIB_PICO_STDIO_USB (0)
#define PICO_DIVIDER_DISABLE_INTERRUPTS (0)
#define PICO_DOUBLE_SUPPORT_ROM_V1 (1)
#define PICO_ENTER_USB_BOOT_ON_EXIT (0)
#define PICO_FLOAT_SUPPORT_ROM_V1 (1)
#define PICO_IE_26_29_UNCHANGED_ON_RESET (0)
#define PICO_INT64_OPS_IN_RAM (0)
#define PICO_NO_HARDWARE (0)
#define PICO_ON_DEVICE (1)
#define PICO_PRINTF_ALWAYS_INCLUDED (1)
#define PICO_RP2040_USB_DEVICE_ENUMERATION_FIX (1)
#define PICO_RP2040_USB_DEVICE_UFRAME_FIX (1)
#define PICO_STDIO_IGNORE_NESTED_STDOUT (0)
#define PICO_USE_CRT_PRINTF (0)
#define PICO_USE_OPTIMISTIC_SBRK (0)
// Stack guards cause a hard fault when 32 bytes around the stack bottom are
// accessed. These backtraces aren't always helpful and this conflicts with our
// own stack checking.
#define PICO_USE_STACK_GUARDS (0)

#include "include/cmsis/rename_exceptions.h"
