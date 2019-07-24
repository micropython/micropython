CC ?= gcc
BUILD_DIR ?= ./build
SRC_DIR ?= ./src
TEST_DIR ?= ./test
TEST_BUILD_DIR ?= ${BUILD_DIR}/test
TEST_MAKEFILE = ${TEST_BUILD_DIR}/MakefileTestSupport
OBJ ?= ${BUILD_DIR}/obj
OBJ_DIR = ${OBJ}

default: all

all: setup test ${BUILD_DIR}/main run

setup:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${OBJ}
	ruby ../../scripts/create_makefile.rb --silent

clean:
	rm -rf ${BUILD_DIR}

${BUILD_DIR}/main: ${SRC_DIR}/main.c ${SRC_DIR}/foo.c
	${CC} $< -o $@

run:
	./build/main || true

test: setup

-include ${TEST_MAKEFILE}
