#!/bin/bash

if which nproc > /dev/null; then
    MAKEOPTS="-j$(nproc)"
else
    MAKEOPTS="-j$(sysctl -n hw.ncpu)"
fi

# Ensure known OPEN_MAX (NO_FILES) limit.
ulimit -n 1024

# Fail on some things which are warnings otherwise
export MICROPY_MAINTAINER_BUILD=1

########################################################################################
# general helper functions

function ci_gcc_arm_setup {
    sudo apt-get update
    sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
    arm-none-eabi-gcc --version
}

function ci_gcc_riscv_setup {
    sudo apt-get update
    sudo apt-get install gcc-riscv64-unknown-elf picolibc-riscv64-unknown-elf
    riscv64-unknown-elf-gcc --version
}

function ci_picotool_setup {
    # Manually installing picotool ensures we use a release version, and speeds up the build.
    git clone https://github.com/raspberrypi/pico-sdk.git
    (cd pico-sdk && git submodule update --init lib/mbedtls)
    git clone https://github.com/raspberrypi/picotool.git
    (cd picotool && mkdir build && cd build && cmake -DPICO_SDK_PATH=../../pico-sdk .. && make && sudo make install)
    picotool version
}

########################################################################################
# c code formatting

function ci_c_code_formatting_setup {
    sudo apt-get update
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
    # Default GitHub Actions checkout for a PR is a generated merge commit where
    # the parents are the head of base branch (i.e. master) and the head of the
    # PR branch, respectively. Use these parents to find the merge-base (i.e.
    # where the PR branch head was branched)

    # If the common ancestor commit hasn't been found, fetch more.
    git merge-base HEAD^1 HEAD^2 || git fetch --unshallow origin

    MERGE_BASE=$(git merge-base HEAD^1 HEAD^2)
    HEAD=$(git rev-parse HEAD^2)
    echo "Checking commits between merge base ${MERGE_BASE} and PR head ${HEAD}..."
    tools/verifygitlog.py -v "${MERGE_BASE}..${HEAD}"
}

########################################################################################
# code size

function ci_code_size_setup {
    sudo apt-get update
    sudo apt-get install gcc-multilib
    gcc --version
    ci_gcc_arm_setup
    ci_gcc_riscv_setup
    ci_picotool_setup
}

function _ci_is_git_merge {
    [[ $(git log -1 --format=%P "$1" | wc -w) > 1 ]]
}

function ci_code_size_build {
    # check the following ports for the change in their code size
    # Override the list by setting PORTS_TO_CHECK in the environment before invoking ci.
    : ${PORTS_TO_CHECK:=bmusxpdv}
    
    SUBMODULES="lib/asf4 lib/berkeley-db-1.xx lib/btstack lib/cyw43-driver lib/lwip lib/mbedtls lib/micropython-lib lib/nxp_driver lib/pico-sdk lib/stm32lib lib/tinyusb"

    # Default GitHub pull request sets HEAD to a generated merge commit
    # between PR branch (HEAD^2) and base branch (i.e. master) (HEAD^1).
    #
    # We want to compare this generated commit with the base branch, to see what
    # the code size impact would be if we merged this PR. During CI we are at a merge commit,
    # so this tests the merged PR against its merge base.
    # Override the refs by setting REFERENCE and/or COMPARISON in the environment before invoking ci.
    : ${COMPARISON:=$(git rev-parse --short HEAD)}
    : ${REFERENCE:=$(git rev-parse --short ${COMPARISON}^1)}

    echo "Comparing sizes of reference ${REFERENCE} to ${COMPARISON}..."
    git log --oneline $REFERENCE..$COMPARISON

    OLD_BRANCH="$(git rev-parse --abbrev-ref HEAD)"

    ( # Execute in a subshell so the trap & code_size_build_step doesn't leak
        function code_size_build_step {
            if [ ! -z "$OLD_BRANCH" ]; then
                trap 'git checkout "$OLD_BRANCH"' RETURN EXIT ERR
            fi

            COMMIT=$1
            OUTFILE=$2
            IGNORE_ERRORS=$3

            git checkout --detach $COMMIT
            git submodule update --init $SUBMODULES
            git show -s
            tools/metrics.py clean "$PORTS_TO_CHECK"
            # Allow errors from tools/metrics.py to propagate out of the pipe below.
            set -o pipefail
            tools/metrics.py build "$PORTS_TO_CHECK" | tee -a $OUTFILE || $IGNORE_ERRORS
            return $?
        }

        # build reference, save to size0
        # ignore any errors with this build, in case master is failing
        echo "BUILDING $(git log --format='%s [%h]' -1 ${REFERENCE})" > ~/size0
        code_size_build_step $REFERENCE ~/size0 true
        # build PR/branch, save to size1
        if _ci_is_git_merge "$COMPARISON"; then
            echo "BUILDING $(git log --oneline -1 --format='%s [merge of %h]' ${COMPARISON}^2)"
        else
            echo "BUILDING $(git log --oneline -1 --formta='%s [%h]' ${COMPARISON})"
        fi > ~/size1
        code_size_build_step $COMPARISON ~/size1 false
    )
}

function ci_code_size_report {
    # Allow errors from tools/metrics.py to propagate out of the pipe above.
    (set -o pipefail; tools/metrics.py diff ~/size0 ~/size1 | tee diff)
}

########################################################################################
# .mpy file format

function ci_mpy_format_setup {
    sudo apt-get update
    sudo pip3 install pyelftools
    python3 --version
}

function ci_mpy_format_test {
    # Test mpy-tool.py dump feature on bytecode
    python3 ./tools/mpy-tool.py -xd tests/frozen/frozentest.mpy

    # Build MicroPython
    ci_unix_standard_build
    micropython=./ports/unix/build-standard/micropython
    $micropython -m mip install --target . argparse __future__
    export MICROPYPATH=.

    # Test mpy-tool.py running under MicroPython
    $micropython ./tools/mpy-tool.py -x -d tests/frozen/frozentest.mpy

    # Test mpy-tool.py dump feature on native code
    make -C examples/natmod/features1
    ./tools/mpy-tool.py -xd examples/natmod/features1/features1.mpy
    $micropython ./tools/mpy-tool.py -x -d examples/natmod/features1/features1.mpy
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

# GitHub tag of ESP-IDF to use for CI, extracted from the esp32 dependency lockfile
# This should end up as a tag name like vX.Y.Z
# (note: This hacky parsing can be replaced with 'yq' once Ubuntu >=24.04 is in use)
IDF_VER=v$(grep -A10 "idf:" ports/esp32/lockfiles/dependencies.lock.esp32 | grep "version:" | head -n1 | sed -E 's/ +version: //')
PYTHON=$(command -v python3 2> /dev/null)
PYTHON_VER=$(${PYTHON:-python} --version | cut -d' ' -f2)

export IDF_CCACHE_ENABLE=1

function ci_esp32_idf_setup {
    echo "Using ESP-IDF version $IDF_VER"
    git clone --depth 1 --branch $IDF_VER https://github.com/espressif/esp-idf.git
    # doing a treeless clone isn't quite as good as --shallow-submodules, but it
    # is smaller than full clones and works when the submodule commit isn't a head.
    git -C esp-idf submodule update --init --recursive --filter=tree:0
    ./esp-idf/install.sh
    # Install additional packages for mpy_ld into the IDF env
    source esp-idf/export.sh
    pip3 install pyelftools
    pip3 install ar
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

function ci_esp32_build_c2_c5_c6 {
    ci_esp32_build_common

    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC_C2
    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC_C5
    make ${MAKEOPTS} -C ports/esp32 BOARD=ESP32_GENERIC_C6
}

########################################################################################
# ports/esp8266

function ci_esp8266_setup {
    sudo pip3 install pyserial esptool==3.3.1 pyelftools ar
    wget https://micropython.org/resources/xtensa-lx106-elf-standalone.tar.gz
    (set -o pipefail; zcat xtensa-lx106-elf-standalone.tar.gz | tar x)
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

    # Test building native .mpy with xtensa architecture.
    ci_native_mpy_modules_build xtensa
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
    make ${MAKEOPTS} -C ports/mimxrt BOARD=MIMXRT1060_EVK submodules
    make ${MAKEOPTS} -C ports/mimxrt BOARD=MIMXRT1060_EVK CFLAGS_EXTRA=-DMICROPY_HW_USB_MSC=1
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
# ports/qemu

function ci_qemu_setup_arm {
    ci_gcc_arm_setup
    sudo apt-get update
    sudo apt-get install qemu-system
    sudo pip3 install pyelftools
    sudo pip3 install ar
    qemu-system-arm --version
}

function ci_qemu_setup_rv32 {
    ci_gcc_riscv_setup
    sudo apt-get update
    sudo apt-get install qemu-system
    sudo pip3 install pyelftools
    sudo pip3 install ar
    qemu-system-riscv32 --version
}

function ci_qemu_setup_rv64 {
    ci_gcc_riscv_setup
    sudo apt-get update
    sudo apt-get install qemu-system
    qemu-system-riscv64 --version
}

function ci_qemu_build_arm_prepare {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/qemu submodules
}

function ci_qemu_build_arm_bigendian {
    ci_qemu_build_arm_prepare
    make ${MAKEOPTS} -C ports/qemu CFLAGS_EXTRA=-DMP_ENDIANNESS_BIG=1
}

function ci_qemu_build_arm_sabrelite {
    ci_qemu_build_arm_prepare
    make ${MAKEOPTS} -C ports/qemu BOARD=SABRELITE test_full
}

function ci_qemu_build_arm_thumb_softfp {
    ci_qemu_build_arm_prepare
    make BOARD=MPS2_AN385 ${MAKEOPTS} -C ports/qemu test_full

    # Test building native .mpy with ARM-M softfp architectures.
    ci_native_mpy_modules_build armv6m
    ci_native_mpy_modules_build armv7m

    # Test running native .mpy with all ARM-M architectures.
    make BOARD=MPS2_AN385 ${MAKEOPTS} -C ports/qemu test_natmod RUN_TESTS_EXTRA="--arch armv6m"
    make BOARD=MPS2_AN385 ${MAKEOPTS} -C ports/qemu test_natmod RUN_TESTS_EXTRA="--arch armv7m"
}

function ci_qemu_build_arm_thumb_hardfp {
    ci_qemu_build_arm_prepare
    make BOARD=MPS2_AN500 ${MAKEOPTS} -C ports/qemu test_full

    # Test building native .mpy with all ARM-M hardfp architectures.
    ci_native_mpy_modules_build armv7emsp
    ci_native_mpy_modules_build armv7emdp

    # Test running native .mpy with all ARM-M hardfp architectures.
    make BOARD=MPS2_AN500 ${MAKEOPTS} -C ports/qemu test_natmod RUN_TESTS_EXTRA="--arch armv7emsp"
    make BOARD=MPS2_AN500 ${MAKEOPTS} -C ports/qemu test_natmod RUN_TESTS_EXTRA="--arch armv7emdp"
}

function ci_qemu_build_rv32 {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/qemu BOARD=VIRT_RV32 submodules
    make ${MAKEOPTS} -C ports/qemu BOARD=VIRT_RV32 test_full

    # Test building and running native .mpy with rv32imc architecture.
    ci_native_mpy_modules_build rv32imc
    make ${MAKEOPTS} -C ports/qemu BOARD=VIRT_RV32 test_natmod
}

function ci_qemu_build_rv64 {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/qemu BOARD=VIRT_RV64 submodules
    make ${MAKEOPTS} -C ports/qemu BOARD=VIRT_RV64 test
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
    ci_picotool_setup
}

function ci_rp2_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/rp2 submodules
    make ${MAKEOPTS} -C ports/rp2
    make ${MAKEOPTS} -C ports/rp2 BOARD=RPI_PICO_W submodules
    make ${MAKEOPTS} -C ports/rp2 BOARD=RPI_PICO_W USER_C_MODULES=../../examples/usercmodule/micropython.cmake
    make ${MAKEOPTS} -C ports/rp2 BOARD=RPI_PICO2 submodules
    make ${MAKEOPTS} -C ports/rp2 BOARD=RPI_PICO2
    make ${MAKEOPTS} -C ports/rp2 BOARD=W5100S_EVB_PICO submodules
    # This build doubles as a build test for disabling threads in the config
    make ${MAKEOPTS} -C ports/rp2 BOARD=W5100S_EVB_PICO CFLAGS_EXTRA=-DMICROPY_PY_THREAD=0

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
    pip3 install ar
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
}

function ci_stm32_nucleo_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_H743ZI submodules
    git submodule update --init lib/mynewt-nimble

    # Test building various MCU families, some with additional options.
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_F091RC
    make ${MAKEOPTS} -C ports/stm32 BOARD=STM32H573I_DK CFLAGS_EXTRA='-DMICROPY_HW_TINYUSB_STACK=1'
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
)

CI_UNIX_OPTS_QEMU_ARM=(
    CROSS_COMPILE=arm-linux-gnueabi-
    VARIANT=coverage
    MICROPY_STANDALONE=1
)

CI_UNIX_OPTS_QEMU_RISCV64=(
    CROSS_COMPILE=riscv64-linux-gnu-
    VARIANT=coverage
    MICROPY_STANDALONE=1
)

CI_UNIX_OPTS_SANITIZE_ADDRESS=(
    # Macro MP_ASAN allows detecting ASan on gcc<=13
    CFLAGS_EXTRA="-fsanitize=address --param asan-use-after-return=0 -DMP_ASAN=1"
    LDFLAGS_EXTRA="-fsanitize=address --param asan-use-after-return=0"
)

CI_UNIX_OPTS_SANITIZE_UNDEFINED=(
    # Macro MP_UBSAN allows detecting UBSan on gcc<=13
    CFLAGS_EXTRA="-fsanitize=undefined -fno-sanitize=nonnull-attribute -DMP_UBSAN=1"
    LDFLAGS_EXTRA="-fsanitize=undefined -fno-sanitize=nonnull-attribute"
)

CI_UNIX_OPTS_REPR_B=(
    VARIANT=standard
    CFLAGS_EXTRA="-DMICROPY_OBJ_REPR=MICROPY_OBJ_REPR_B -DMICROPY_PY_UCTYPES=0 -Dmp_int_t=int32_t -Dmp_uint_t=uint32_t"
    MICROPY_FORCE_32BIT=1
    RUN_TESTS_MPY_CROSS_FLAGS="--mpy-cross-flags=\"-march=x86 -msmall-int-bits=30\""

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

function ci_unix_run_tests_full_extra {
    micropython=$1
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=$micropython ./run-multitests.py multi_net/*.py)
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=$micropython ./run-perfbench.py --average 1 1000 1000)
}

function ci_unix_run_tests_full_no_native_helper {
    variant=$1
    shift
    micropython=../ports/unix/build-$variant/micropython
    make -C ports/unix VARIANT=$variant "$@" test_full_no_native
    ci_unix_run_tests_full_extra $micropython
}

function ci_unix_run_tests_full_helper {
    variant=$1
    shift
    micropython=../ports/unix/build-$variant/micropython
    make -C ports/unix VARIANT=$variant "$@" test_full
    ci_unix_run_tests_full_extra $micropython
}

function ci_native_mpy_modules_build {
    if [ "$1" = "" ]; then
        arch=x64
    else
        arch=$1
    fi
    for natmod in btree deflate features1 features3 features4 framebuf heapq random re
    do
        make -C examples/natmod/$natmod ARCH=$arch clean
        make -C examples/natmod/$natmod ARCH=$arch
    done

    # features2 requires soft-float on rv32imc and xtensa.
    make -C examples/natmod/features2 ARCH=$arch clean
    if [ $arch = "rv32imc" ] || [ $arch = "xtensa" ]; then
        make -C examples/natmod/features2 ARCH=$arch MICROPY_FLOAT_IMPL=float
    else
        make -C examples/natmod/features2 ARCH=$arch
    fi
}

function ci_native_mpy_modules_32bit_build {
    ci_native_mpy_modules_build x86
}

function ci_unix_minimal_build {
    make ${MAKEOPTS} -C ports/unix VARIANT=minimal
}

function ci_unix_minimal_run_tests {
    make -C ports/unix VARIANT=minimal test
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
    pip3 install setuptools
    pip3 install pyelftools
    pip3 install ar
    gcc --version
    python3 --version
}

function ci_unix_coverage_build {
    ci_unix_build_helper VARIANT=coverage
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_coverage_run_tests {
    MICROPY_TEST_TIMEOUT=60 ci_unix_run_tests_full_helper coverage
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
    sudo pip3 install ar
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
    ci_unix_build_helper VARIANT=nanbox CFLAGS_EXTRA="-DMICROPY_PY_MATH_CONSTANTS=1"
    ci_unix_build_ffi_lib_helper gcc -m32
}

function ci_unix_nanbox_run_tests {
    ci_unix_run_tests_full_no_native_helper nanbox
}

function ci_unix_longlong_build {
    ci_unix_build_helper VARIANT=longlong "${CI_UNIX_OPTS_SANITIZE_UNDEFINED[@]}"
}

function ci_unix_longlong_run_tests {
    ci_unix_run_tests_full_helper longlong
}

function ci_unix_float_build {
    ci_unix_build_helper VARIANT=standard CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_float_run_tests {
    # TODO get this working: ci_unix_run_tests_full_helper standard CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
    ci_unix_run_tests_helper CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
}

function ci_unix_gil_enabled_build {
    ci_unix_build_helper VARIANT=standard MICROPY_PY_THREAD_GIL=1
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_gil_enabled_run_tests {
    ci_unix_run_tests_full_helper standard MICROPY_PY_THREAD_GIL=1
}

function ci_unix_clang_setup {
    sudo apt-get update
    sudo apt-get install clang
    clang --version
}

function ci_unix_stackless_clang_build {
    make ${MAKEOPTS} -C mpy-cross CC=clang
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix CC=clang CFLAGS_EXTRA="-DMICROPY_STACKLESS=1 -DMICROPY_STACKLESS_STRICT=1"
}

function ci_unix_stackless_clang_run_tests {
    # Timeout needs to be increased for thread/stress_aes.py test.
    MICROPY_TEST_TIMEOUT=90 ci_unix_run_tests_helper CC=clang
}

function ci_unix_float_clang_build {
    make ${MAKEOPTS} -C mpy-cross CC=clang
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix CC=clang CFLAGS_EXTRA="-DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT"
}

function ci_unix_float_clang_run_tests {
    ci_unix_run_tests_helper CC=clang
}

function ci_unix_settrace_stackless_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix "${CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS[@]}"
}

function ci_unix_settrace_stackless_run_tests {
    ci_unix_run_tests_full_helper standard "${CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS[@]}"
}

function ci_unix_sanitize_undefined_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix VARIANT=coverage "${CI_UNIX_OPTS_SANITIZE_UNDEFINED[@]}"
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_sanitize_undefined_run_tests {
    MICROPY_TEST_TIMEOUT=60 ci_unix_run_tests_full_helper coverage VARIANT=coverage "${CI_UNIX_OPTS_SANITIZE_UNDEFINED[@]}"
}

function ci_unix_sanitize_address_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    make ${MAKEOPTS} -C ports/unix VARIANT=coverage "${CI_UNIX_OPTS_SANITIZE_ADDRESS[@]}"
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_sanitize_address_run_tests {
    MICROPY_TEST_TIMEOUT=60 ci_unix_run_tests_full_helper coverage VARIANT=coverage "${CI_UNIX_OPTS_SANITIZE_ADDRESS[@]}"
}

function ci_unix_macos_build {
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
    # - float_parse and float_parse_doubleprec parse/print floats out by a few mantissa bits
    # - ffi_callback crashes for an unknown reason
    # - thread/stress_heap.py is flaky
    # - thread/thread_gc1.py is flaky
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-standard/micropython ./run-tests.py --exclude '(float_parse|float_parse_doubleprec|ffi_callback|thread/stress_heap|thread/thread_gc1).py')
}

function ci_unix_qemu_mips_setup {
    sudo apt-get update
    sudo apt-get install gcc-mips-linux-gnu g++-mips-linux-gnu libc6-mips-cross
    sudo apt-get install qemu-user
    qemu-mips --version
    sudo mkdir /etc/qemu-binfmt
    sudo ln -s /usr/mips-linux-gnu/ /etc/qemu-binfmt/mips
}

function ci_unix_qemu_mips_build {
    ci_unix_build_helper "${CI_UNIX_OPTS_QEMU_MIPS[@]}"
    ci_unix_build_ffi_lib_helper mips-linux-gnu-gcc
}

function ci_unix_qemu_mips_run_tests {
    # Issues with MIPS tests:
    # - thread/stress_aes.py takes around 50 seconds
    # - thread/stress_recurse.py is flaky
    # - thread/thread_gc1.py is flaky
    file ./ports/unix/build-coverage/micropython
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-coverage/micropython MICROPY_TEST_TIMEOUT=90 ./run-tests.py --exclude 'thread/stress_recurse.py|thread/thread_gc1.py')
}

function ci_unix_qemu_arm_setup {
    sudo apt-get update
    sudo apt-get install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
    sudo apt-get install qemu-user
    qemu-arm --version
    sudo mkdir /etc/qemu-binfmt
    sudo ln -s /usr/arm-linux-gnueabi/ /etc/qemu-binfmt/arm
}

function ci_unix_qemu_arm_build {
    ci_unix_build_helper "${CI_UNIX_OPTS_QEMU_ARM[@]}"
    ci_unix_build_ffi_lib_helper arm-linux-gnueabi-gcc
}

function ci_unix_qemu_arm_run_tests {
    # Issues with ARM tests:
    # - (i)listdir does not work, it always returns the empty list (it's an issue with the underlying C call)
    # - thread/stress_aes.py takes around 70 seconds
    # - thread/stress_recurse.py is flaky
    # - thread/thread_gc1.py is flaky
    file ./ports/unix/build-coverage/micropython
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-coverage/micropython MICROPY_TEST_TIMEOUT=90 ./run-tests.py --exclude 'vfs_posix.*\.py|thread/stress_recurse.py|thread/thread_gc1.py')
}

function ci_unix_qemu_riscv64_setup {
    sudo apt-get update
    sudo apt-get install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu
    sudo apt-get install qemu-user
    qemu-riscv64 --version
    sudo mkdir /etc/qemu-binfmt
    sudo ln -s /usr/riscv64-linux-gnu/ /etc/qemu-binfmt/riscv64
}

function ci_unix_qemu_riscv64_build {
    ci_unix_build_helper "${CI_UNIX_OPTS_QEMU_RISCV64[@]}"
    ci_unix_build_ffi_lib_helper riscv64-linux-gnu-gcc
}

function ci_unix_qemu_riscv64_run_tests {
    # Issues with RISCV-64 tests:
    # - thread/stress_aes.py takes around 140 seconds
    # - thread/stress_recurse.py is flaky
    # - thread/thread_gc1.py is flaky
    file ./ports/unix/build-coverage/micropython
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/build-coverage/micropython MICROPY_TEST_TIMEOUT=180 ./run-tests.py --exclude 'thread/stress_recurse.py|thread/thread_gc1.py')
}

function ci_unix_repr_b_build {
    ci_unix_build_helper "${CI_UNIX_OPTS_REPR_B[@]}"
    ci_unix_build_ffi_lib_helper gcc -m32
}

function ci_unix_repr_b_run_tests {
    # ci_unix_run_tests_full_no_native_helper is not used due to
    # https://github.com/micropython/micropython/issues/18105
    ci_unix_run_tests_helper "${CI_UNIX_OPTS_REPR_B[@]}"
}

########################################################################################
# ports/windows

function ci_windows_setup {
    sudo apt-get update
    sudo apt-get install gcc-mingw-w64
}

function ci_windows_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/windows submodules
    make ${MAKEOPTS} -C ports/windows CROSS_COMPILE=i686-w64-mingw32-
    make ${MAKEOPTS} -C ports/windows CROSS_COMPILE=x86_64-w64-mingw32- BUILD=build-standard-w64
}

########################################################################################
# ports/zephyr

ZEPHYR_DOCKER_VERSION=v0.28.1
ZEPHYR_SDK_VERSION=0.17.2
ZEPHYR_VERSION=v4.2.0

function ci_zephyr_setup {
    IMAGE=ghcr.io/zephyrproject-rtos/ci:${ZEPHYR_DOCKER_VERSION}

    docker pull ${IMAGE}

    # Directories cached by GitHub Actions, mounted
    # into the container
    ZEPHYRPROJECT_DIR="$(pwd)/zephyrproject"
    CCACHE_DIR="$(pwd)/.ccache"

    mkdir -p "${ZEPHYRPROJECT_DIR}"
    mkdir -p "${CCACHE_DIR}"

    docker run --name zephyr-ci -d -it \
      -v "$(pwd)":/micropython \
      -v "${ZEPHYRPROJECT_DIR}":/zephyrproject \
      -v "${CCACHE_DIR}":/root/.cache/ccache \
      -e ZEPHYR_SDK_INSTALL_DIR=/opt/toolchains/zephyr-sdk-${ZEPHYR_SDK_VERSION} \
      -e ZEPHYR_TOOLCHAIN_VARIANT=zephyr \
      -e ZEPHYR_BASE=/zephyrproject/zephyr \
      -w /micropython/ports/zephyr \
      ${IMAGE}
    docker ps -a

    # qemu-system-arm is needed to run the test suite.
    sudo apt-get update
    sudo apt-get install qemu-system-arm
    qemu-system-arm --version
}

function ci_zephyr_install {
    docker exec zephyr-ci west init --mr ${ZEPHYR_VERSION} /zephyrproject
    docker exec -w /zephyrproject zephyr-ci west update
    docker exec -w /zephyrproject zephyr-ci west zephyr-export
}

function ci_zephyr_build {
    git submodule update --init lib/micropython-lib
    docker exec zephyr-ci west build -p auto -b qemu_x86 -- -DCONF_FILE=prj_minimal.conf
    docker exec zephyr-ci west build -p auto -b frdm_k64f
    docker exec zephyr-ci west build -p auto -b mimxrt1050_evk
    docker exec zephyr-ci west build -p auto -b nucleo_wb55rg # for bluetooth
}

function ci_zephyr_run_tests {
    docker exec zephyr-ci west build -p auto -b qemu_cortex_m3 -- -DCONF_FILE=prj_minimal.conf
    (cd tests && ./run-tests.py -t execpty:"qemu-system-arm -cpu cortex-m3 -machine lm3s6965evb -nographic -monitor null -serial pty -kernel ../ports/zephyr/build/zephyr/zephyr.elf")
}

########################################################################################
# ports/alif

function ci_alif_setup {
    ci_gcc_arm_setup
}

function ci_alif_ae3_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/alif BOARD=OPENMV_AE3 MCU_CORE=M55_HP submodules
    make ${MAKEOPTS} -C ports/alif BOARD=OPENMV_AE3 MCU_CORE=M55_HE submodules
    make ${MAKEOPTS} -C ports/alif BOARD=OPENMV_AE3 MCU_CORE=M55_DUAL
    make ${MAKEOPTS} -C ports/alif BOARD=ALIF_ENSEMBLE MCU_CORE=M55_DUAL
}

function _ci_help {
    # Note: these lines must be indented with tab characters (required by bash <<-EOF)
    cat <<-EOF
	ci.sh: Script fragments used during CI

	When invoked as a script, runs a sequence of ci steps,
	stopping after the first error.

	Usage:
	    ${BASH_SOURCE} step1 step2...

	Steps:
	EOF
    if type -path column > /dev/null 2>&1; then
        grep '^function ci_' $0 | awk '{print $2}' | sed 's/^ci_//' | column
    else
        grep '^function ci_' $0 | awk '{print $2}' | sed 's/^ci_//'
    fi
    exit
}

function _ci_bash_completion {
    echo "alias ci=\"$(readlink -f "$0")\"; complete -W '$(grep '^function ci_' $0 | awk '{print $2}' | sed 's/^ci_//')' ci"
}

function _ci_main {
    case "$1" in
        (-h|-?|--help)
            _ci_help
        ;;
        (--bash-completion)
            _ci_bash_completion
        ;;
        (-*)
            echo "Unknown option: $1" 1>&2
            exit 1
        ;;
        (*)
            set -e
            cd $(dirname "$0")/..
            while [ $# -ne 0 ]; do
                ci_$1
                shift
            done
        ;;
    esac
}

# https://stackoverflow.com/questions/2683279/how-to-detect-if-a-script-is-being-sourced
sourced=0
if [ -n "$ZSH_VERSION" ]; then
  case $ZSH_EVAL_CONTEXT in *:file) sourced=1;; esac
elif [ -n "$KSH_VERSION" ]; then
  [ "$(cd -- "$(dirname -- "$0")" && pwd -P)/$(basename -- "$0")" != "$(cd -- "$(dirname -- "${.sh.file}")" && pwd -P)/$(basename -- "${.sh.file}")" ] && sourced=1
elif [ -n "$BASH_VERSION" ]; then
  (return 0 2>/dev/null) && sourced=1
else # All other shells: examine $0 for known shell binary filenames.
     # Detects `sh` and `dash`; add additional shell filenames as needed.
  case ${0##*/} in sh|-sh|dash|-dash) sourced=1;; esac
fi

if [ $sourced -eq 0 ]; then
    # invoked as a command
    if [ "$#" -eq 0 ]; then
        set -- --help
    fi
    _ci_main "$@"
fi
