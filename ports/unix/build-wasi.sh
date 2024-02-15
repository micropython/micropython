#! /bin/sh

# prerequisites
#
# https://github.com/WebAssembly/wasi-libc/pull/467
# https://github.com/WebAssembly/wasi-libc/pull/473
# https://github.com/WebAssembly/binaryen/pull/6294
# https://github.com/WebAssembly/binaryen/pull/6259
#
# WASI_SDK: wasi-sdk 21.0
# WASM_OPT: binaryen wasm-opt built with the above patches
# WASI_SYSROOT: wasi-libc built with the above patches
# ../../../micropython-ulab: check out https://github.com/v923z/micropython-ulab

WASI_SDK=${WASI_SDK:-/opt/wasi-sdk-21.0}
WASI_SYSROOT=${WASI_SYSROOT:-${HOME}/git/wasi-libc/sysroot}
BINARYEN_BIN=${BINARYEN_BIN:-${HOME}/git/wasm/binaryen/b/bin}
WASM_OPT=${WASM_OPT:-${BINARYEN_BIN}/wasm-opt}

# MICROPY_PY_THREAD: GC uses signals, which is not available on wasi.

# the following things seem building well. i disabled them just because
# i was not intererested in them.
# MICROPY_VFS_FAT
# MICROPY_VFS_LFS1
# MICROPY_VFS_LFS2

# micropython-ulab was added just as an example of user C module.
# you can remove it if you don't use it.

CFLAGS="-D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -mllvm -wasm-enable-sjlj" \
LDFLAGS="-lwasi-emulated-process-clocks -lwasi-emulated-signal -lwasi-emulated-mman" \
make \
CC="${WASI_SDK}/bin/clang --sysroot=${WASI_SYSROOT}" \
LD="${WASI_SDK}/bin/wasm-ld" \
STRIP="${WASI_SDK}/bin/strip" \
SIZE="${WASI_SDK}/bin/size" \
UNAME_S=wasi \
MICROPY_PY_FFI=0 \
MICROPY_PY_THREAD=0 \
MICROPY_PY_SOCKET=0 \
MICROPY_PY_SSL=0 \
MICROPY_PY_BTREE=0 \
MICROPY_PY_TERMIOS=0 \
MICROPY_USE_READLINE=0 \
MICROPY_VFS_FAT=0 \
MICROPY_VFS_LFS1=0 \
MICROPY_VFS_LFS2=0 \
USER_C_MODULES=../../../micropython-ulab \
"$@"

PROG=build-standard/micropython

# We doesn't have a way to scan GC roots like WASM locals.
# Hopefully --spill-pointers can workaround it.
${WASM_OPT} \
--spill-pointers \
-o ${PROG}.spilled ${PROG}

# LLVM still uses the older version of EH proposal.
# Convert to the latest version of EH proposal.
${WASM_OPT} \
--translate-to-new-eh --enable-exception-handling \
-o ${PROG}.spilled.neweh ${PROG}.spilled

# now you can run it with EH-enabled runtimes.
# eg.  (using the latest EH)
# toywasm --wasi build-standard/micropython.spilled.neweh
# eg.  (using the old EH)
# iwasm build-standard/micropython.spilled
