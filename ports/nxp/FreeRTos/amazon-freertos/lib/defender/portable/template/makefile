# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

group?=
CFLAGS?=

#We try to detect the OS we are running on, and adjust commands as needed
ifeq ($(OSTYPE),cygwin)
	CLEANUP          = rm -f
	MKDIR            = mkdir -p
	TARGET_EXTENSION =.out
elseifeq ($(OSTYPE),msys)
	CLEANUP          = rm -f
	MKDIR            = mkdir -p
	TARGET_EXTENSION =.exe
elseifeq ($(OS),Windows_NT)
	CLEANUP          = del /F /Q
	MKDIR            = mkdir
	TARGET_EXTENSION =.exe
else
	CLEANUP          = rm -f
	MKDIR            = mkdir -p
	TARGET_EXTENSION =.out
endif

dir_guard=@mkdir -p $(@D)

TOP_DIR    = ../../../
PATH_SRC   = $(TOP_DIR)src/
PATH_LIB   = $(TOP_DIR)lib/
PATH_BUILD = $(TOP_DIR)build/

PATH_CBOR  = $(PATH_LIB)Ts_cbor_lib/src/
INC_DIRS  += -I $(PATH_CBOR)
HDR_ALL   += $(wildcard $(PATH_CBOR)*.h)
SRC_CBOR   = $(wildcard $(PATH_CBOR)*.c)
SRC_ALL   += $(SRC_CBOR)
OBJ_CBOR   = $(patsubst $(PATH_CBOR)%.c,$(PATH_BUILD)%.o,$(SRC_CBOR))
OBJ_ALL   += $(OBJ_CBOR)

PATH_UNITY  = $(PATH_LIB)unity/src/
INC_DIRS   += -I $(PATH_UNITY)
HDR_ALL    += $(wildcard $(PATH_UNITY)*.h)
SRC_UNITY   = $(wildcard $(PATH_UNITY)*.c)
SRC_ALL    += $(SRC_UNITY)
OBJ_UNITY   = $(patsubst $(PATH_UNITY)%.c,$(PATH_BUILD)%.o,$(SRC_UNITY))
OBJ_ALL    += $(OBJ_UNITY)

PATH_UNITY_FIXT  = $(PATH_LIB)unity/extras/fixture/src/
INC_DIRS        += -I $(PATH_UNITY_FIXT)
HDR_ALL         += $(wildcard $(PATH_UNITY_FIXT)*.h)
SRC_UNITY_FIXT   = $(wildcard $(PATH_UNITY_FIXT)*.c)
SRC_ALL         += $(SRC_UNITY_FIXT)
OBJ_UNITY_FIXT   = $(patsubst $(PATH_UNITY_FIXT)%.c,$(PATH_BUILD)%.o,$(SRC_UNITY_FIXT))
OBJ_ALL         += $(OBJ_UNITY_FIXT)

PATH_REPORT  = $(PATH_SRC)report/
INC_DIRS    += -I $(PATH_REPORT)
HDR_ALL     += $(wildcard $(PATH_REPORT)*.h)
SRC_REPORT   = $(wildcard $(PATH_REPORT)*.c)
SRC_ALL     += $(SRC_REPORT)
OBJ_REPORT   = $(patsubst $(PATH_REPORT)%.c,$(PATH_BUILD)%.o,$(SRC_REPORT))
OBJ_ALL     += $(OBJ_REPORT)

PATH_TEST  = $(TOP_DIR)test/
INC_DIRS  += -I $(PATH_TEST)
SRC_TEST   = $(wildcard $(PATH_TEST)*.c)
SRC_ALL   += $(SRC_TEST)
OBJ_TEST   = $(patsubst $(PATH_TEST)%.c,$(PATH_BUILD)%.o,$(SRC_TEST))
OBJ_ALL   += $(OBJ_TEST)

PATH_PORT_TEST  = $(PATH_TEST)port_tests/
INC_DIRS       += -I $(PATH_PORT_TEST)
SRC_PORT_TEST   = $(wildcard $(PATH_PORT_TEST)*.c)
SRC_ALL        += $(SRC_PORT_TEST)
OBJ_PORT_TEST   = $(patsubst $(PATH_PORT_TEST)%.c,$(PATH_BUILD)%.o,$(SRC_PORT_TEST))
OBJ_ALL        += $(OBJ_PORT_TEST)

PATH_METRIC_HDRS  = $(PATH_SRC)metrics/
INC_DIRS         += -I $(PATH_METRIC_HDRS)
HDR_ALL          += $(wildcard $(PATH_METRIC_HDRS)*.h)

PATH_METRIC_SRC ?= $(PATH_SRC)portable/template/
INC_DIRS        += -I $(PATH_METRIC_SRC)
HDR_ALL         += $(wildcard $(PATH_METRIC_SRC)*.h)
SRC_METRICS      = $(wildcard $(PATH_METRIC_SRC)*.c)
SRC_ALL         += $(SRC_METRICS)
OBJ_METRICS      = $(patsubst $(PATH_METRIC_SRC)%.c,$(PATH_BUILD)%.o,$(SRC_METRICS))
OBJ_ALL         += $(OBJ_METRICS)

CHECK_SRC += $(filter src% test%,$(SRC_ALL))
CHECK_SRC += $(filter src% test%,$(HDR_ALL))

TGT     = $(PATH_BUILD)test$(TARGET_EXTENSION)
RESULTS = $(PATH_BUILD)results.txt

#Tool Definitions
C_COMPILER = @$(CC)
CFLAGS    += -std=c99
CFLAGS    += -D PORT_TEST
CFLAGS    += -g
CFLAGS    += -O0
CFLAGS    += -Wall
CFLAGS    += -include "unity_fixture_malloc_overrides.h"

RUN_FLAGS=-g "$(group)"
ifeq ($(group),)
	COV_REP_FLAGS=$(TGT)
else
	COV_REP_FLAGS=$(PATH_BUILD)*$(group)*.o
endif

COMPILE = $(C_COMPILER) -c $(CFLAGS) $(INC_DIRS) $< -o $@
LINK    = $(C_COMPILER) $(COV_FLAGS) -o $@ $^

#Result formatting
NO_COLOR = sgr0
GREEN    = setaf 2
RED      = setaf 1
YELLOW   = setaf 3

PASSED_TESTS = echo; \
	echo ----Passed--------------------------; \
	grep -s -E '.*PASS' ./$(RESULTS); true

IGNORED_TESTS = echo; \
	echo ----Ignored-------------------------; \
	grep -s -E '.*IGNORE' ./$(RESULTS); true

FAILED_TESTS = echo; \
	echo ----Failed--------------------------; \
	grep -s -E '.*FAIL' ./$(RESULTS); true

RESULT_SUMMARY = echo; \
	echo -----------------------------------; \
	grep -s Ignored ./$(RESULTS)

FINAL_RESULT = if grep -q FAIL ./$(RESULTS); \
	then tput $(RED);   echo ==============FAIL==============; tput $(NO_COLOR); \
	else tput $(GREEN); echo ==============PASS==============; tput $(NO_COLOR); \
	fi

default: test
	@$(IGNORED_TESTS)
	@$(FAILED_TESTS)
	@$(RESULT_SUMMARY)
	@$(FINAL_RESULT)

debug: $(PATH_BUILD) $(TGT)
	@gdb -ex 'b assert_override.c:13' ./$(TGT)

test: $(TGT)
	@./$(TGT) -v $(RUN_FLAGS) > $(RESULTS)

lint:
	@echo ----CPPCHECK-----------------------------
	@cppcheck --enable=all --check-config --suppress=missingIncludeSystem      \
		$(INC_DIRS) $(CHECK_SRC) >/dev/null
	@echo ----CLANG-TIDY---------------------------
	@clang-tidy $(CHECK_SRC) -- $(INC_DIRS)                    \
		2>/dev/null

clean:
	@$(CLEANUP) $(PATH_BUILD)*.o
	@$(CLEANUP) $(TGT)

clean-lib:
	@$(CLEANUP) -r $(PATH_LIB)

clean-all:
	@$(CLEANUP) -r $(PATH_LIB)
	@$(CLEANUP) -r $(PATH_BUILD)

lib:
	@git clone https://github.com/ThrowTheSwitch/Unity.git lib/unity
	@git --git-dir=$(PATH_LIB)unity/.git --work-tree=$(PATH_LIB)unity/ \
		checkout "v2.4.3"

	@git clone ssh://git.amazon.com/pkg/Ts_cbor_lib $(PATH_LIB)Ts_cbor_lib
	@git --git-dir=$(PATH_LIB)Ts_cbor_$(PATH_LIB).git \
		--work-tree=lib/Ts_cbor_lib/ checkout   \
		"develop"

docs:
	$(MKDIR) $(PATH_BUILD)
	doxygen

list_src:
	echo $(SRC_ALL)
	echo $(CHECK_SRC)

all: clean-all lib test docs coverage lint

$(PATH_BUILD)%.o:: $(PATH_REPORT)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(PATH_BUILD)%.o:: $(PATH_METRIC_SRC)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(PATH_BUILD)%.o:: $(PATH_TEST)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(PATH_BUILD)%.o:: $(PATH_PORT_TEST)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(PATH_BUILD)%.o:: $(PATH_CBOR)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(PATH_BUILD)%.o:: $(PATH_UNITY)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(PATH_BUILD)%.o:: $(PATH_UNITY_FIXT)%.c $(HDR_ALL)
	$(dir_guard)
	$(COMPILE)

$(TGT): $(OBJ_ALL)
	$(dir_guard)
	$(LINK)
