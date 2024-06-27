#!/bin/bash

if which nproc > /dev/null; then
    MAKEOPTS="-j$(nproc)"
else
    MAKEOPTS="-j$(sysctl -n hw.ncpu)"
fi

# Ensure known OPEN_MAX (NO_FILES) limit.
ulimit -n 1024

########################################################################################
# general helper functions

function ci_gcc_arm_setup {
    sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
    arm-none-eabi-gcc --version
}

function ci_gcc_riscv_setup {
    sudo apt-get install gcc-riscv64-unknown-elf picolibc-riscv64-unknown-elf
    riscv64-unknown-elf-gcc --version
}

########################################################################################
# c code formatting

function ci_c_code_formatting_setup {
    sudo apt-get install uncrustify
    uncrustify --version
}

function ci_c_code_formatting_run {
    # Only run on C files. The ruff rule runs separately on Python.
    tools/codeformat.py -v -c
}

########################################################################################
# commit formatting

function ci_commit_formatting_run {
    git remote add upstream https://github.com/micropython/micropython.git
    git fetch --depth=100 upstream master
    # If the common ancestor commit hasn't been found, fetch more.
    git merge-base upstream/master HEAD || git fetch --unshallow upstream master
    # For a PR, upstream/master..HEAD ends with a merge commit into master, exclude that one.
    tools/verifygitlog.py -v upstream/master..HEAD --no-merges
}

########################################################################################
# code size

function ci_code_size_setup {
    sudo apt-get update
    sudo apt-get install gcc-multilib
    gcc --version
    ci_gcc_arm_setup
}

function ci_code_size_build {
    # check the following ports for the change in their code size
    PORTS_TO_CHECK=bmusxpd
    SUBMODULES="lib/asf4 lib/berkeley-db-1.xx lib/btstack lib/cyw43-driver lib/lwip lib/mbedtls lib/micropython-lib lib/nxp_driver lib/pico-sdk lib/stm32lib lib/tinyusb"

    # starts off at either the ref/pull/N/merge FETCH_HEAD, or the current branch HEAD
    git checkout -b pull_request # save the current location
    git remote add upstream https://github.com/micropython/micropython.git
    git fetch --depth=100 upstream master
    # If the common ancestor commit hasn't been found, fetch more.
    git merge-base upstream/master HEAD || git fetch --unshallow upstream master
    # build reference, save to size0
    # ignore any errors with this build, in case master is failing
    git checkout `git merge-base --fork-point upstream/master pull_request`
    git submodule update --init $SUBMODULES
    git show -s
    tools/metrics.py clean $PORTS_TO_CHECK
    tools/metrics.py build $PORTS_TO_CHECK | tee ~/size0 || true
    # build PR/branch, save to size1
    git checkout pull_request
    git submodule update --init $SUBMODULES
    git log upstream/master..HEAD
    tools/metrics.py clean $PORTS_TO_CHECK
    tools/metrics.py build $PORTS_TO_CHECK | tee ~/size1
}

########################################################################################
# .mpy file format

function ci_mpy_format_setup {
    sudo pip3 install pyelftools
}

function ci_mpy_format_test {
    # Test mpy-tool.py dump feature on bytecode
    python2 ./tools/mpy-tool.py -xd tests/frozen/frozentest.mpy
    python3 ./tools/mpy-tool.py -xd tests/frozen/frozentest.mpy

    # Test mpy-tool.py dump feature on native code
    make -C examples/natmod/features1
    ./tools/mpy-tool.py -xd examples/natmod/features1/features1.mpy
}

########################################################################################
# ports/cc3200

function ci_cc3200_setup {
    ci_gcc_arm_setup
}

function ci_cc3200_build {
    make ${MAKEOPTS} -C ports/cc3200 BTARGET=application BTYPE=release
    make ${MAKEOPTS} -C ports/cc3200 BTARGET=bootloader  BTYPE=release
}

########################################################################################
# ports/esp32

# GitHub tag of ESP-IDF to use for CI (note: must be a tag or a branch)
IDF_VER=v5.0.4

export IDF_CCACHE_ENABLE=1

function ci_esp32_idf_setup {
    pip3 install pyelftools
    git clone --depth 1 --branch $IDF_VER https://github.com/espressif/esp-idf.git
    # doing a treeless clone isn't quite as good as --shallow-submodules, but it
    # is smaller than full clones and works when the submodule commit isn't a head.
    git -C esp-idf submodule update --init --recursive --filter=tree:0
    ./esp-idf/install.sh
}

function ci_esp32_build_common {
    source esp-idf/export.sh
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/esp32 submodules
}

function ci_esp32_build_cmod_spiram_s2 {
    ci_esp32_build_common

    make ${MAKEOPTS} -C ports/esp32 \
        USER_C_MODULES=../../../examples/usercmodule/micropython.cmake \
        FROZEN_MANIFEST=$(pwd)/ports/esp32/boards/manifest_test.py

    # Test building native .mpy with xtensawin architecture.
    ci_native_mpy_modules_build xtensawin

    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC BOARD_VARIANT=SPIRAM
    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC_S2
}

function ci_esp32_build_s3_c3 {
    ci_esp32_build_common

    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC_S3
    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC_C3
}

########################################################################################
# ports/esp8266

function ci_esp8266_setup {
    sudo pip install pyserial esptool==3.3.1
    wget https://github.com/jepler/esp-open-sdk/releases/download/2018-06-10/xtensa-lx106-elf-standalone.tar.gz
    zcat xtensa-lx106-elf-standalone.tar.gz | tar x
    # Remove this esptool.py so pip version is used instead
    rm xtensa-lx106-elf/bin/esptool.py
}

function ci_esp8266_path {
    echo $(pwd)/xtensa-lx106-elf/bin
}

function ci_esp8266_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/esp8266 submodules
    make ${MAKEOPTS} -C ports/esp8266 BOARD=ESP8266_GENERIC
    make ${MAKEOPTS} -C ports/esp8266 BOARD=ESP8266_GENERIC BOARD_VARIANT=FLASH_512K
    make ${MAKEOPTS} -C ports/esp8266 BOARD=ESP8266_GENERIC BOARD_VARIANT=FLASH_1M
}

########################################################################################
# ports/webassembly

function ci_webassembly_setup {
    npm install terser
    git clone https://github.com/emscripten-core/emsdk.git
    (cd emsdk && ./emsdk install latest && ./emsdk activate latest)
}

function ci_webassembly_build {
    source emsdk/emsdk_env.sh
    make ${MAKEOPTS} -C ports/webassembly VARIANT=pyscript submodules
    make ${MAKEOPTS} -C ports/webassembly VARIANT=pyscript
}

function ci_webassembly_run_tests {
    make -C ports/webassembly VARIANT=pyscript test_min
}

########################################################################################
# ports/mimxrt

function ci_mimxrt_setup {
    ci_gcc_arm_setup
}

function ci_mimxrt_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/mimxrt BOARD=MIMXRT1020_EVK submodules
    make ${MAKEOPTS} -C ports/mimxrt BOARD=MIMXRT1020_EVK
    make ${MAKEOPTS} -C ports/mimxrt BOARD=TEENSY40 submodules
    make ${MAKEOPTS} -C ports/mimxrt BOARD=TEENSY40
}

########################################################################################
# ports/nrf

function ci_nrf_setup {
    ci_gcc_arm_setup
}

function ci_nrf_build {
    ports/nrf/drivers/bluetooth/download_ble_stack.sh s140_nrf52_6_1_1
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/nrf submodules
    make ${MAKEOPTS} -C ports/nrf BOARD=PCA10040
    make ${MAKEOPTS} -C ports/nrf BOARD=MICROBIT
    make ${MAKEOPTS} -C ports/nrf BOARD=PCA10056 SD=s140
    make ${MAKEOPTS} -C ports/nrf BOARD=PCA10090
}

########################################################################################
# ports/powerpc

function ci_powerpc_setup {
    sudo apt-get update
    sudo apt-get install gcc-powerpc64le-linux-gnu libc6-dev-ppc64el-cross
}

function ci_powerpc_build {
    make ${MAKEOPTS} -C ports/powerpc UART=potato
    make ${MAKEOPTS} -C ports/powerpc UART=lpc_serial
}

########################################################################################
# ports/qemu-arm

function ci_qemu_arm_setup {
    ci_gcc_arm_setup
    sudo apt-get update
    sudo apt-get install qemu-system
    qemu-system-arm --version
}

function ci_qemu_arm_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/qemu-arm submodules
    make ${MAKEOPTS} -C ports/qemu-arm CFLAGS_EXTRA=-DMP_ENDIANNESS_BIG=1
    make ${MAKEOPTS} -C ports/qemu-arm clean
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test submodules
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test test
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test clean
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test BOARD=sabrelite test
}

########################################################################################
# ports/qemu-riscv

function ci_qemu_riscv_setup {
    ci_gcc_riscv_setup
    sudo apt-get update
    sudo apt-get install qemu-system
    qemu-system-riscv32 --version
}

function ci_qemu_riscv_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/qemu-riscv submodules
    make ${MAKEOPTS} -C ports/qemu-riscv
    make ${MAKEOPTS} -C ports/qemu-riscv clean
    make ${MAKEOPTS} -C ports/qemu-riscv -f Makefile.test submodules
    make ${MAKEOPTS} -C ports/qemu-riscv -f Makefile.test test
}

########################################################################################
# ports/renesas-ra

function ci_renesas_ra_setup {
    ci_gcc_arm_setup
    sudo apt-get install protobuf-c-compiler
}

function ci_renesas_ra_board_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/renesas-ra submodules
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=RA4M1_CLICKER
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=EK_RA6M2
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=EK_RA6M1
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=EK_RA4M1
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=EK_RA4W1
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=ARDUINO_PORTENTA_C33 submodules
    make ${MAKEOPTS} -C ports/renesas-ra BOARD=ARDUINO_PORTENTA_C33
}

########################################################################################
# ports/rp2

function ci_rp2_setup {
    ci_gcc_arm_setup
}

function ci_rp2_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/rp2 submodules
    make ${MAKEOPTS} -C ports/rp2
    make ${MAKEOPTS} -C ports/rp2 BOARD=RPI_PICO_W submodules
    make ${MAKEOPTS} -C ports/rp2 BOARD=RPI_PICO_W USER_C_MODULES=../../examples/usercmodule/micropython.cmake
    make ${MAKEOPTS} -C ports/rp2 BOARD=W5100S_EVB_PICO submodules
    make ${MAKEOPTS} -C ports/rp2 BOARD=W5100S_EVB_PICO

    # Test building ninaw10 driver and NIC interface.
    make ${MAKEOPTS} -C ports/rp2 BOARD=ARDUINO_NANO_RP2040_CONNECT submodules
    make ${MAKEOPTS} -C ports/rp2 BOARD=ARDUINO_NANO_RP2040_CONNECT
}

########################################################################################
# ports/samd

function ci_samd_setup {
    ci_gcc_arm_setup
}

function ci_samd_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/samd submodules
    make ${MAKEOPTS} -C ports/samd BOARD=ADAFRUIT_ITSYBITSY_M0_EXPRESS
    make ${MAKEOPTS} -C ports/samd BOARD=ADAFRUIT_ITSYBITSY_M4_EXPRESS
}

########################################################################################
# ports/stm32

function ci_stm32_setup {
    ci_gcc_arm_setup
    pip3 install pyelftools
    pip3 install pyhy
}

function ci_stm32_pyb_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/stm32 MICROPY_PY_NETWORK_WIZNET5K=5200 submodules
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBD_SF2 submodules
    git submodule update --init lib/btstack
    git submodule update --init lib/mynewt-nimble
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBV11 MICROPY_PY_NETWORK_WIZNET5K=5200 USER_C_MODULES=../../examples/usercmodule
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBD_SF2
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBD_SF6 COPT=-O2 NANBOX=1 MICROPY_BLUETOOTH_NIMBLE=0 MICROPY_BLUETOOTH_BTSTACK=1
    make ${MAKEOPTS} -C ports/stm32/mboot BOARD=PYBV10 CFLAGS_EXTRA='-DMBOOT_FSLOAD=1 -DMBOOT_VFS_LFS2=1'
    make ${MAKEOPTS} -C ports/stm32/mboot BOARD=PYBD_SF6
    make ${MAKEOPTS} -C ports/stm32/mboot BOARD=STM32F769DISC CFLAGS_EXTRA='-DMBOOT_ADDRESS_SPACE_64BIT=1 -DMBOOT_SDCARD_ADDR=0x100000000ULL -DMBOOT_SDCARD_BYTE_SIZE=0x400000000ULL -DMBOOT_FSLOAD=1 -DMBOOT_VFS_FAT=1'

    # Test building native .mpy with armv7emsp architecture.
    git submodule update --init lib/berkeley-db-1.xx
    ci_native_mpy_modules_build armv7emsp
}

function ci_stm32_nucleo_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_H743ZI submodules
    git submodule update --init lib/mynewt-nimble

    # Test building various MCU families, some with additional options.
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_F091RC
    make ${MAKEOPTS} -C ports/stm32 BOARD=STM32H573I_DK
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_H743ZI COPT=-O2 CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_L073RZ
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_L476RG DEBUG=1

    # Test building a board with mboot packing enabled (encryption, signing, compression).
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_WB55 USE_MBOOT=1 MBOOT_ENABLE_PACKING=1
    make ${MAKEOPTS} -C ports/stm32/mboot BOARD=NUCLEO_WB55 USE_MBOOT=1 MBOOT_ENABLE_PACKING=1
    # Test mboot_pack_dfu.py created a valid file, and that its unpack-dfu command works.
    BOARD_WB55=ports/stm32/boards/NUCLEO_WB55
    BUILD_WB55=ports/stm32/build-NUCLEO_WB55
    python3 ports/stm32/mboot/mboot_pack_dfu.py -k $BOARD_WB55/mboot_keys.h unpack-dfu $BUILD_WB55/firmware.pack.dfu $BUILD_WB55/firmware.unpack.dfu
    diff $BUILD_WB55/firmware.unpack.dfu $BUILD_WB55/firmware.dfu
    # Test unpack-dfu command works without a secret key
    tail -n +2 $BOARD_WB55/mboot_keys.h > $BOARD_WB55/mboot_keys_no_sk.h
    python3 ports/stm32/mboot/mboot_pack_dfu.py -k $BOARD_WB55/mboot_keys_no_sk.h unpack-dfu $BUILD_WB55/firmware.pack.dfu $BUILD_WB55/firmware.unpack_no_sk.dfu
    diff $BUILD_WB55/firmware.unpack.dfu $BUILD_WB55/firmware.unpack_no_sk.dfu
}

function ci_stm32_misc_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/stm32 BOARD=ARDUINO_GIGA submodules
    make ${MAKEOPTS} -C ports/stm32 BOARD=ARDUINO_GIGA
}

########################################################################################
# ports/unix

CI_UNIX_OPTS_SYS_SETTRACE=(
    MICROPY_PY_BTREE=0
    MICROPY_PY_FFI=0
    MICROPY_PY_SSL=0
    CFLAGS_EXTRA="-DMICROPY_PY_SYS_SETTRACE=1"
)

CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS=(
    MICROPY_PY_BTREE=0
    MICROPY_PY_FFI=0
    MICROPY_PY_SSL=0
    CFLAGS_EXTRA="-DMICROPY_STACKLESS=1 -DMICROPY_STACKLESS_STRICT=1 -DMICROPY_PY_SYS_SETTRACE=1"
)

CI_UNIX_OPTS_QEMU_MIPS=(
    CROSS_COMPILE=mips-linux-gnu-
    VARIANT=coverage
    MICROPY_STANDALONE=1
    LDFLAGS_EXTRA="-static"
)

CI_UNIX_OPTS_QEMU_ARM=(
    CROSS_COMPILE=arm-linux-gnueabi-
    VARIANT=coverage
    MICROPY_STANDALONE=1
)

function ci_unix_build_helper {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix "$@" submodules
    make ${MAKEOPTS} -C ports/unix "$@" deplibs
    make ${MAKEOPTS} -C ports/unix "$@"
}

function ci_unix_build_ffi_lib_helper {
    $1 $2 -shared -o tests/ports/unix/ffi_lib.so tests/ports/unix/ffi_lib.c
}

function ci_unix_run_tests_helper {
    make -C ports/unix "$@" test
}

function ci_unix_run_tests_full_helper {
    variant=$1
    shift
    micropython=../ports/unix/build-$variant/micropython
    make -C ports/unix VARIANT=$variant "$@" test_full
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=$micropython ./run-multitests.py multi_net/*.py)
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=$micropython ./run-perfbench.py 1000 1000)
}

function ci_native_mpy_modules_build {
    if [ "$1" = "" ]; then
        arch=x64
    else
        arch=$1
    fi
    make -C examples/natmod/features1 ARCH=$arch
    make -C examples/natmod/features2 ARCH=$arch
    make -C examples/natmod/features3 ARCH=$arch
    make -C examples/natmod/features4 ARCH=$arch
    make -C examples/natmod/btree ARCH=$arch
    make -C examples/natmod/deflate ARCH=$arch
    make -C examples/natmod/framebuf ARCH=$arch
    make -C examples/natmod/heapq ARCH=$arch
    make -C examples/natmod/random ARCH=$arch
    make -C examples/natmod/re ARCH=$arch
}

function ci_native_mpy_modules_32bit_build {
    ci_native_mpy_modules_build x86
}

function ci_unix_minimal_build {
    make ${MAKEOPTS} -C ports/unix VARIANT=minimal
}

function ci_unix_minimal_run_tests {
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/build-minimal/micropython ./run-tests.py -e exception_chain -e self_type_check -e subclass_native_init -d basics)
}

function ci_unix_standard_build {
    ci_unix_build_helper VARIANT=standard
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_standard_run_tests {
    ci_unix_run_tests_full_helper standard
}

function ci_unix_standard_v2_build {
    ci_unix_build_helper VARIANT=standard MICROPY_PREVIEW_VERSION_2=1
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_standard_v2_run_tests {
    ci_unix_run_tests_full_helper standard
}

function ci_unix_coverage_setup {
    sudo pip3 install setuptools
    sudo pip3 install pyelftools
    gcc --version
    python3 --version
}

function ci_unix_coverage_build {
    ci_unix_build_helper VARIANT=coverage
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_coverage_run_tests {
    ci_unix_run_tests_full_helper coverage
}

function ci_unix_coverage_run_mpy_merge_tests {
    mptop=$(pwd)
    outdir=$(mktemp -d)
    allmpy=()

    # Compile a selection of tests to .mpy and execute them, collecting the output.
    # None of the tests should SKIP.
    for inpy in $mptop/tests/basics/[acdel]*.py; do
        test=$(basename $inpy .py)
        echo $test
        outmpy=$outdir/$test.mpy
        $mptop/mpy-cross/build/mpy-cross -o $outmpy $inpy
        (cd $outdir && $mptop/ports/unix/build-coverage/micropython -m $test >> out-individual)
        allmpy+=($outmpy)
    done

    # Merge all the tests into one .mpy file, and then execute it.
    python3 $mptop/tools/mpy-tool.py --merge -o $outdir/merged.mpy ${allmpy[@]}
    (cd $outdir && $mptop/ports/unix/build-coverage/micropython -m merged > out-merged)

    # Make sure the outputs match.
    diff $outdir/out-individual $outdir/out-merged && /bin/rm -rf $outdir
}

function ci_unix_coverage_run_native_mpy_tests {
    MICROPYPATH=examples/natmod/features2 ./ports/unix/build-coverage/micropython -m features2
    (cd tests && ./run-natmodtests.py "$@" extmod/*.py)
}

function ci_unix_32bit_setup {
    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install gcc-multilib g++-multilib libffi-dev:i386
    sudo pip3 install setuptools
    sudo pip3 install pyelftools
    gcc --version
    python3 --version
}

function ci_unix_coverage_32bit_build {
    ci_unix_build_helper VARIANT=coverage MICROPY_FORCE_32BIT=1
    ci_unix_build_ffi_lib_helper gcc -m32
}

function ci_unix_coverage_32bit_run_tests {
    ci_unix_run_tests_full_helper coverage MICROPY_FORCE_32BIT=1
}

function ci_unix_coverage_32bit_run_native_mpy_tests {
    ci_unix_coverage_run_native_mpy_tests --arch x86
}

function ci_unix_nanbox_build {
    # Use Python 2 to check that it can run the build scripts
    ci_unix_build_helper PYTHON=python2 VARIANT=nanbox CFLAGS_EXTRA="-DMICROPY_PY_MATH_CONSTANTS=1"
    ci_unix_build_ffi_lib_helper gcc -m32
}

function ci_unix_nanbox_run_tests {
    ci_unix_run_tests_full_helper nanbox PYTHON=python2
}

function ci_unix_float_build {
    ci_unix_build_helper VARIANT=standard CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_float_run_tests {
    # TODO get this working: ci_unix_run_tests_full_helper standard CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
    ci_unix_run_tests_helper CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
}

function ci_unix_clang_setup {
    sudo apt-get install clang
    clang --version
}

function ci_unix_stackless_clang_build {
    make ${MAKEOPTS} -C mpy-cross CC=clang
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix CC=clang CFLAGS_EXTRA="-DMICROPY_STACKLESS=1 -DMICROPY_STACKLESS_STRICT=1"
}

function ci_unix_stackless_clang_run_tests {
    ci_unix_run_tests_helper CC=clang
}

function ci_unix_float_clang_build {
    make ${MAKEOPTS} -C mpy-cross CC=clang
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix CC=clang CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
}

function ci_unix_float_clang_run_tests {
    ci_unix_run_tests_helper CC=clang
}

function ci_unix_settrace_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix "${CI_UNIX_OPTS_SYS_SETTRACE[@]}"
}

function ci_unix_settrace_run_tests {
    ci_unix_run_tests_full_helper standard "${CI_UNIX_OPTS_SYS_SETTRACE[@]}"
}

function ci_unix_settrace_stackless_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix "${CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS[@]}"
}

function ci_unix_settrace_stackless_run_tests {
    ci_unix_run_tests_full_helper standard "${CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS[@]}"
}

function ci_unix_macos_build {
    # Install pkg-config to configure libffi paths.
    brew install pkg-config

    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    #make ${MAKEOPTS} -C ports/unix deplibs
    make ${MAKEOPTS} -C ports/unix
    # check for additional compiler errors/warnings
    make ${MAKEOPTS} -C ports/unix VARIANT=coverage submodules
    make ${MAKEOPTS} -C ports/unix VARIANT=coverage
}

function ci_unix_macos_run_tests {
    # Issues with macOS tests:
    # - import_pkg7 has a problem with relative imports
    # - random_basic has a problem with getrandbits(0)
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-standard/micropython ./run-tests.py --exclude 'import_pkg7.py' --exclude 'random_basic.py')
}

function ci_unix_qemu_mips_setup {
    sudo apt-get update
    sudo apt-get install gcc-mips-linux-gnu g++-mips-linux-gnu
    sudo apt-get install qemu-user
    qemu-mips --version
}

function ci_unix_qemu_mips_build {
    # qemu-mips on GitHub Actions will seg-fault if not linked statically
    ci_unix_build_helper "${CI_UNIX_OPTS_QEMU_MIPS[@]}"
}

function ci_unix_qemu_mips_run_tests {
    # Issues with MIPS tests:
    # - (i)listdir does not work, it always returns the empty list (it's an issue with the underlying C call)
    # - ffi tests do not work
    file ./ports/unix/build-coverage/micropython
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-coverage/micropython ./run-tests.py --exclude 'vfs_posix.*\.py' --exclude 'ffi_(callback|float|float2).py')
}

function ci_unix_qemu_arm_setup {
    sudo apt-get update
    sudo apt-get install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
    sudo apt-get install qemu-user
    qemu-arm --version
}

function ci_unix_qemu_arm_build {
    ci_unix_build_helper "${CI_UNIX_OPTS_QEMU_ARM[@]}"
    ci_unix_build_ffi_lib_helper arm-linux-gnueabi-gcc
}

function ci_unix_qemu_arm_run_tests {
    # Issues with ARM tests:
    # - (i)listdir does not work, it always returns the empty list (it's an issue with the underlying C call)
    export QEMU_LD_PREFIX=/usr/arm-linux-gnueabi
    file ./ports/unix/build-coverage/micropython
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-coverage/micropython ./run-tests.py --exclude 'vfs_posix.*\.py')
}

########################################################################################
# ports/windows

function ci_windows_setup {
    sudo apt-get install gcc-mingw-w64
}

function ci_windows_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/windows submodules
    make ${MAKEOPTS} -C ports/windows CROSS_COMPILE=i686-w64-mingw32-
}

########################################################################################
# ports/zephyr

ZEPHYR_DOCKER_VERSION=v0.21.0
ZEPHYR_SDK_VERSION=0.13.2
ZEPHYR_VERSION=v3.1.0

function ci_zephyr_setup {
    docker pull zephyrprojectrtos/ci:${ZEPHYR_DOCKER_VERSION}
    docker run --name zephyr-ci -d -it \
      -v "$(pwd)":/micropython \
      -e ZEPHYR_SDK_INSTALL_DIR=/opt/toolchains/zephyr-sdk-${ZEPHYR_SDK_VERSION} \
      -e ZEPHYR_TOOLCHAIN_VARIANT=zephyr \
      -e ZEPHYR_BASE=/zephyrproject/zephyr \
      -w /micropython/ports/zephyr \
      zephyrprojectrtos/ci:${ZEPHYR_DOCKER_VERSION}
    docker ps -a
}

function ci_zephyr_install {
    docker exec zephyr-ci west init --mr ${ZEPHYR_VERSION} /zephyrproject
    docker exec -w /zephyrproject zephyr-ci west update
    docker exec -w /zephyrproject zephyr-ci west zephyr-export
}

function ci_zephyr_build {
    docker exec zephyr-ci west build -p auto -b qemu_x86 -- -DCONF_FILE=prj_minimal.conf
    docker exec zephyr-ci west build -p auto -b frdm_k64f
    docker exec zephyr-ci west build -p auto -b mimxrt1050_evk
    docker exec zephyr-ci west build -p auto -b nucleo_wb55rg # for bluetooth
}
