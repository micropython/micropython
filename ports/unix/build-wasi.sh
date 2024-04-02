#! /bin/sh

# prerequisites
#
# https://github.com/WebAssembly/wasi-libc/pull/483 (wasi-sdk 22.0?)
# https://github.com/WebAssembly/wasi-libc/pull/473 (wasi-sdk 22.0?)
# https://github.com/WebAssembly/binaryen/pull/6294 (version_117)
# https://github.com/WebAssembly/binaryen/pull/6259 (version_117)
# https://github.com/llvm/llvm-project/pull/84137 (LLVM 19?)
#
# WASI_SDK: wasi-sdk 21.0
# WASM_OPT: binaryen wasm-opt built with the above patches
# WASI_SYSROOT: wasi-libc built with the above patches
# CLANG: clang built with the above patches

WASI_SDK=${WASI_SDK:-/opt/wasi-sdk-21.0}
WASI_SYSROOT=${WASI_SYSROOT:-${HOME}/git/wasi-libc/sysroot}
BINARYEN_BIN=${BINARYEN_BIN:-${HOME}/git/wasm/binaryen/b/bin}
WASM_OPT=${WASM_OPT:-${BINARYEN_BIN}/wasm-opt}
RESOURCE_DIR=$(${WASI_SDK}/bin/clang --print-resource-dir)
LLVM_BUILD_DIR=${LLVM_BUILD_DIR:-/Volumes/PortableSSD/llvm/build}
CLANG=${CLANG:-${LLVM_BUILD_DIR}/bin/clang}
CC="${CLANG} --sysroot ${WASI_SYSROOT} -resource-dir ${RESOURCE_DIR}"

CFLAGS="-D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -mllvm -wasm-enable-sjlj" \
LDFLAGS="-lwasi-emulated-process-clocks -lwasi-emulated-signal -lwasi-emulated-mman -lsetjmp" \
make \
CC="${CC}" \
LD="${WASI_SDK}/bin/wasm-ld" \
STRIP="${WASI_SDK}/bin/strip" \
SIZE="${WASI_SDK}/bin/size" \
UNAME_S=wasi \
VARIANT=wasi \
"$@"

PROG=build-wasi/micropython

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
# toywasm --wasi build-wasi/micropython.spilled.neweh
# eg.  (using the old EH)
# iwasm build-wasi/micropython.spilled
