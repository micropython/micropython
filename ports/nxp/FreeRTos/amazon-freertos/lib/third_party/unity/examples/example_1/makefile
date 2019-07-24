# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=.out
endif

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

UNITY_ROOT=../..

CFLAGS=-std=c89
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition

TARGET_BASE1=test1
TARGET_BASE2=test2
TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
TARGET2 = $(TARGET_BASE2)$(TARGET_EXTENSION)
SRC_FILES1=$(UNITY_ROOT)/src/unity.c src/ProductionCode.c  test/TestProductionCode.c  test/test_runners/TestProductionCode_Runner.c
SRC_FILES2=$(UNITY_ROOT)/src/unity.c src/ProductionCode2.c test/TestProductionCode2.c test/test_runners/TestProductionCode2_Runner.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src
SYMBOLS=

all: clean default

default: $(SRC_FILES1) $(SRC_FILES2)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES1) -o $(TARGET1)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES2) -o $(TARGET2)
	- ./$(TARGET1)
	./$(TARGET2)

test/test_runners/TestProductionCode_Runner.c: test/TestProductionCode.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test/TestProductionCode.c  test/test_runners/TestProductionCode_Runner.c
test/test_runners/TestProductionCode2_Runner.c: test/TestProductionCode2.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test/TestProductionCode2.c test/test_runners/TestProductionCode2_Runner.c

clean:
	$(CLEANUP) $(TARGET1) $(TARGET2)

ci: CFLAGS += -Werror
ci: default
