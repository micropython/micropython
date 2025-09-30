# Native module makefile for building .mpy file

# Location of top-level MicroPython directory
MPY_DIR = ../..

# Name of module
MOD = dualmode_example

# Source files
SRC = dualmode_example.c

# Architecture to build for
ARCH = x64

# Add include path for mp_dualmode.h
CFLAGS += -I.

# Include to get the rules for compiling and linking the module
include $(MPY_DIR)/py/dynruntime.mk