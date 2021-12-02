#!/bin/bash

if which nproc > /dev/null; then
    MAKEOPTS="-j$(nproc)"
else
    MAKEOPTS="-j$(sysctl -n hw.ncpu)"
fi

########################################################################################
# general helper functions

function ci_gcc_arm_setup {
    sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
    arm-none-eabi-gcc --version
}

########################################################################################
# code formatting

function ci_code_formatting_setup {
    sudo apt-add-repository --yes --update ppa:pybricks/ppa
    sudo apt-get install uncrustify
    pip3 install black
    uncrustify --version
    black --version
}

function ci_code_formatting_run {
    tools/codeformat.py -v
}

########################################################################################
# commit formatting

function ci_commit_formatting_run {
    git remote add upstream https://github.com/micropython/micropython.git
    git fetch --depth=100 upstream  master
    # For a PR, upstream/master..HEAD ends with a merge commit into master, exlude that one.
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
    # starts off at either the ref/pull/N/merge FETCH_HEAD, or the current branch HEAD
    git checkout -b pull_request # save the current location
    git remote add upstream https://github.com/micropython/micropython.git
    git fetch --depth=100 upstream master
    # build reference, save to size0
    # ignore any errors with this build, in case master is failing
    git checkout `git merge-base --fork-point upstream/master pull_request`
    git show -s
    tools/metrics.py clean bm
    tools/metrics.py build bm | tee ~/size0 || true
    # build PR/branch, save to size1
    git checkout pull_request
    git log upstream/master..HEAD
    tools/metrics.py clean bm
    tools/metrics.py build bm | tee ~/size1
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

function ci_esp32_setup_helper {
    git clone https://github.com/espressif/esp-idf.git
    git -C esp-idf checkout $1
    git -C esp-idf submodule update --init \
        components/bt/host/nimble/nimble \
        components/esp_wifi \
        components/esptool_py/esptool \
        components/lwip/lwip \
        components/mbedtls/mbedtls
    if [ -d esp-idf/components/bt/controller/esp32 ]; then
        git -C esp-idf submodule update --init \
            components/bt/controller/lib_esp32 \
            components/bt/controller/lib_esp32c3_family
    else
        git -C esp-idf submodule update --init \
            components/bt/controller/lib
    fi
    ./esp-idf/install.sh
}

function ci_esp32_idf402_setup {
    ci_esp32_setup_helper v4.0.2
}

function ci_esp32_idf44_setup {
    # This commit is just before v5.0-dev
    ci_esp32_setup_helper 142bb32c50fa9875b8b69fa539a2d59559460d72
}

function ci_esp32_build {
    source esp-idf/export.sh
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/esp32 submodules
    make ${MAKEOPTS} -C ports/esp32 USER_C_MODULES=../../../examples/usercmodule/micropython.cmake FROZEN_MANIFEST=$(pwd)/ports/esp32/boards/manifest.py
    if [ -d $IDF_PATH/components/esp32c3 ]; then
        make ${MAKEOPTS} -C ports/esp32 BOARD=GENERIC_C3
    fi
    if [ -d $IDF_PATH/components/esp32s2 ]; then
        make ${MAKEOPTS} -C ports/esp32 BOARD=GENERIC_S2
    fi
    if [ -d $IDF_PATH/components/esp32s3 ]; then
        make ${MAKEOPTS} -C ports/esp32 BOARD=GENERIC_S3
    fi
}

########################################################################################
# ports/esp8266

function ci_esp8266_setup {
    sudo pip install pyserial esptool
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
    make ${MAKEOPTS} -C ports/esp8266
    make ${MAKEOPTS} -C ports/esp8266 BOARD=GENERIC_512K
    make ${MAKEOPTS} -C ports/esp8266 BOARD=GENERIC_1M
}

########################################################################################
# ports/javascript

function ci_javascript_setup {
    git clone https://github.com/emscripten-core/emsdk.git
    (cd emsdk && ./emsdk install latest && ./emsdk activate latest)
}

function ci_javascript_build {
    source emsdk/emsdk_env.sh
    make ${MAKEOPTS} -C ports/javascript
}

function ci_javascript_run_tests {
    # This port is very slow at running, so only run a few of the tests.
    (cd tests && MICROPY_MICROPYTHON=../ports/javascript/node_run.sh ./run-tests.py -j1 basics/builtin_*.py)
}

########################################################################################
# ports/mimxrt

function ci_mimxrt_setup {
    ci_gcc_arm_setup
}

function ci_mimxrt_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/mimxrt submodules
    make ${MAKEOPTS} -C ports/mimxrt BOARD=MIMXRT1020_EVK
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
    make ${MAKEOPTS} -C ports/nrf BOARD=pca10040
    make ${MAKEOPTS} -C ports/nrf BOARD=microbit
    make ${MAKEOPTS} -C ports/nrf BOARD=pca10056 SD=s140
    make ${MAKEOPTS} -C ports/nrf BOARD=pca10090
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
    make ${MAKEOPTS} -C ports/qemu-arm CFLAGS_EXTRA=-DMP_ENDIANNESS_BIG=1
    make ${MAKEOPTS} -C ports/qemu-arm clean
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test test
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test clean
    make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test BOARD=sabrelite test
}

########################################################################################
# ports/rp2

function ci_rp2_setup {
    ci_gcc_arm_setup
}

function ci_rp2_build {
    make ${MAKEOPTS} -C mpy-cross
    git submodule update --init lib/pico-sdk lib/tinyusb
    make ${MAKEOPTS} -C ports/rp2
    make ${MAKEOPTS} -C ports/rp2 clean
    make ${MAKEOPTS} -C ports/rp2 USER_C_MODULES=../../examples/usercmodule/micropython.cmake
}

########################################################################################
# ports/samd

function ci_samd_setup {
    ci_gcc_arm_setup
}

function ci_samd_build {
    make ${MAKEOPTS} -C ports/samd submodules
    make ${MAKEOPTS} -C ports/samd
}

########################################################################################
# ports/stm32

function ci_stm32_setup {
    ci_gcc_arm_setup
    pip3 install pyhy
}

function ci_stm32_pyb_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/stm32 submodules
    git submodule update --init lib/btstack
    git submodule update --init lib/mynewt-nimble
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBV11 MICROPY_PY_WIZNET5K=5200 MICROPY_PY_CC3K=1 USER_C_MODULES=../../examples/usercmodule
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBD_SF2
    make ${MAKEOPTS} -C ports/stm32 BOARD=PYBD_SF6 NANBOX=1 MICROPY_BLUETOOTH_NIMBLE=0 MICROPY_BLUETOOTH_BTSTACK=1
    make ${MAKEOPTS} -C ports/stm32/mboot BOARD=PYBV10 CFLAGS_EXTRA='-DMBOOT_FSLOAD=1 -DMBOOT_VFS_LFS2=1'
    make ${MAKEOPTS} -C ports/stm32/mboot BOARD=PYBD_SF6
}

function ci_stm32_nucleo_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/stm32 submodules
    git submodule update --init lib/mynewt-nimble

    # Test building various MCU families, some with additional options.
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_F091RC
    make ${MAKEOPTS} -C ports/stm32 BOARD=NUCLEO_H743ZI CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
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

########################################################################################
# ports/teensy

function ci_teensy_setup {
    ci_gcc_arm_setup
}

function ci_teensy_build {
    make ${MAKEOPTS} -C ports/teensy
}

########################################################################################
# ports/unix

CI_UNIX_OPTS_SYS_SETTRACE=(
    MICROPY_PY_BTREE=0
    MICROPY_PY_FFI=0
    MICROPY_PY_USSL=0
    CFLAGS_EXTRA="-DMICROPY_PY_SYS_SETTRACE=1"
)

CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS=(
    MICROPY_PY_BTREE=0
    MICROPY_PY_FFI=0
    MICROPY_PY_USSL=0
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
    $1 $2 -shared -o tests/unix/ffi_lib.so tests/unix/ffi_lib.c
}

function ci_unix_run_tests_helper {
    make -C ports/unix "$@" test
}

function ci_unix_run_tests_full_helper {
    variant=$1
    shift
    if [ $variant = standard ]; then
        micropython=micropython
    else
        micropython=micropython-$variant
    fi
    make -C ports/unix VARIANT=$variant "$@" test_full
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/$micropython ./run-multitests.py multi_net/*.py)
}

function ci_native_mpy_modules_build {
    if [ "$1" = "" ]; then
        arch=x64
    else
        arch=$1
    fi
    make -C examples/natmod/features1 ARCH=$arch
    make -C examples/natmod/features2 ARCH=$arch
    make -C examples/natmod/btree ARCH=$arch
    make -C examples/natmod/framebuf ARCH=$arch
    make -C examples/natmod/uheapq ARCH=$arch
    make -C examples/natmod/urandom ARCH=$arch
    make -C examples/natmod/ure ARCH=$arch
    make -C examples/natmod/uzlib ARCH=$arch
}

function ci_native_mpy_modules_32bit_build {
    ci_native_mpy_modules_build x86
}

function ci_unix_minimal_build {
    make ${MAKEOPTS} -C ports/unix VARIANT=minimal
}

function ci_unix_minimal_run_tests {
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython-minimal ./run-tests.py -e exception_chain -e self_type_check -e subclass_native_init -d basics)
}

function ci_unix_standard_build {
    ci_unix_build_helper VARIANT=standard
    ci_unix_build_ffi_lib_helper gcc
}

function ci_unix_standard_run_tests {
    ci_unix_run_tests_full_helper standard
}

function ci_unix_standard_run_perfbench {
    (cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython ./run-perfbench.py 1000 1000)
}

function ci_unix_dev_build {
    ci_unix_build_helper VARIANT=dev
}

function ci_unix_dev_run_tests {
    ci_unix_run_tests_helper VARIANT=dev
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

function ci_unix_coverage_run_native_mpy_tests {
    MICROPYPATH=examples/natmod/features2 ./ports/unix/micropython-coverage -m features2
    (cd tests && ./run-natmodtests.py "$@" extmod/{btree*,framebuf*,uheapq*,ure*,uzlib*}.py)
}

function ci_unix_32bit_setup {
    sudo dpkg --add-architecture i386
    sudo apt-get update
    sudo apt-get install gcc-multilib g++-multilib libffi-dev:i386
    sudo pip3 install setuptools
    sudo pip3 install pyelftools
    gcc --version
    python2 --version
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
    ci_unix_build_helper PYTHON=python2 VARIANT=nanbox
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
    make ${MAKEOPTS} -C ports/unix "${CI_UNIX_OPTS_SYS_SETTRACE[@]}"
}

function ci_unix_settrace_run_tests {
    ci_unix_run_tests_helper "${CI_UNIX_OPTS_SYS_SETTRACE[@]}"
}

function ci_unix_settrace_stackless_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix "${CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS[@]}"
}

function ci_unix_settrace_stackless_run_tests {
    ci_unix_run_tests_helper "${CI_UNIX_OPTS_SYS_SETTRACE_STACKLESS[@]}"
}

function ci_unix_macos_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/unix submodules
    #make ${MAKEOPTS} -C ports/unix deplibs
    make ${MAKEOPTS} -C ports/unix
    # check for additional compiler errors/warnings
    make ${MAKEOPTS} -C ports/unix VARIANT=dev submodules
    make ${MAKEOPTS} -C ports/unix VARIANT=dev
    make ${MAKEOPTS} -C ports/unix VARIANT=coverage submodules
    make ${MAKEOPTS} -C ports/unix VARIANT=coverage
}

function ci_unix_macos_run_tests {
    # Issues with macOS tests:
    # - OSX has poor time resolution and these uasyncio tests do not have correct output
    # - import_pkg7 has a problem with relative imports
    # - urandom_basic has a problem with getrandbits(0)
    (cd tests && ./run-tests.py --exclude 'uasyncio_(basic|heaplock|lock|wait_task)' --exclude 'import_pkg7.py' --exclude 'urandom_basic.py')
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
    file ./ports/unix/micropython-coverage
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/micropython-coverage ./run-tests.py --exclude 'vfs_posix.py' --exclude 'ffi_(callback|float|float2).py')
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
    file ./ports/unix/micropython-coverage
    (cd tests && MICROPY_MICROPYTHON=../ports/unix/micropython-coverage ./run-tests.py --exclude 'vfs_posix.py')
}

########################################################################################
# ports/windows

function ci_windows_setup {
    sudo apt-get install gcc-mingw-w64
}

function ci_windows_build {
    make ${MAKEOPTS} -C mpy-cross
    make ${MAKEOPTS} -C ports/windows CROSS_COMPILE=i686-w64-mingw32-
}

########################################################################################
# ports/zephyr

function ci_zephyr_setup {
    docker pull zephyrprojectrtos/ci:v0.17.3
    docker run --name zephyr-ci -d -it \
      -v "$(pwd)":/micropython \
      -e ZEPHYR_SDK_INSTALL_DIR=/opt/toolchains/zephyr-sdk-0.12.4 \
      -e ZEPHYR_TOOLCHAIN_VARIANT=zephyr \
      -e ZEPHYR_BASE=/zephyrproject/zephyr \
      -w /micropython/ports/zephyr \
      zephyrprojectrtos/ci:v0.17.3
    docker ps -a
}

function ci_zephyr_install {
    docker exec zephyr-ci west init --mr v2.6.0 /zephyrproject
    docker exec -w /zephyrproject zephyr-ci west update
    docker exec -w /zephyrproject zephyr-ci west zephyr-export
}

function ci_zephyr_build {
    docker exec zephyr-ci west build -p auto -b qemu_x86 -- -DCONF_FILE=prj_minimal.conf
    docker exec zephyr-ci west build -p auto -b frdm_k64f -- -DCONF_FILE=prj_minimal.conf
    docker exec zephyr-ci west build -p auto -b qemu_x86
    docker exec zephyr-ci west build -p auto -b frdm_k64f
    docker exec zephyr-ci west build -p auto -b mimxrt1050_evk
    docker exec zephyr-ci west build -p auto -b reel_board
}
