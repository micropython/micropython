#! /bin/sh

# prerequisites
#
# https://github.com/WebAssembly/wasi-libc/pull/483 (wasi-sdk 22.0)
# https://github.com/WebAssembly/wasi-libc/pull/473 (wasi-sdk 22.0)
# https://github.com/WebAssembly/binaryen/pull/6294 (version_117)
# https://github.com/WebAssembly/binaryen/pull/6259 (version_117)
# https://github.com/llvm/llvm-project/pull/84137 (LLVM 19?)
#
# WASI_SDK: wasi-sdk 22.0 or later
# WASM_OPT: binaryen wasm-opt version_117 or later
# WASI_SYSROOT: sysroot from wasi-sdk 22.0 or later
# CLANG: clang from LLVM 19 or later, which contains the above mentioned PR

# REVISIT:
# we specify the target "--target=wasm32-wasi" explicitly below for the case
# where $CLANG is built with a configuration different from wasi-sdk.
# it can be simplified once LLVM 19 becomes a part of wasi-sdk.
# ditto for "-B ${WASI_SDK}/bin/".

WASI_SDK=${WASI_SDK:-/opt/wasi-sdk-22.0}
WASI_SYSROOT=${WASI_SYSROOT:-${WASI_SDK}/share/wasi-sysroot}
BINARYEN_BIN=${BINARYEN_BIN:-${HOME}/git/wasm/binaryen/b/bin}
WASM_OPT=${WASM_OPT:-${BINARYEN_BIN}/wasm-opt}
RESOURCE_DIR=$(${WASI_SDK}/bin/clang --print-resource-dir)
LLVM_BUILD_DIR=${LLVM_BUILD_DIR:-/Volumes/PortableSSD/llvm/build}
CLANG=${CLANG:-${LLVM_BUILD_DIR}/bin/clang}
CC="${CLANG} --sysroot ${WASI_SYSROOT} -resource-dir ${RESOURCE_DIR}"

CFLAGS="--target=wasm32-wasi -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL -D_WASI_EMULATED_MMAN -mllvm -wasm-enable-sjlj" \
LDFLAGS="--target=wasm32-wasi -lwasi-emulated-process-clocks -lwasi-emulated-signal -lwasi-emulated-mman -lsetjmp -B ${WASI_SDK}/bin/" \
make \
CC="${CC}" \
STRIP="${WASI_SDK}/bin/strip" \
SIZE="${WASI_SDK}/bin/size" \
UNAME_S=wasi \
VARIANT=wasi \
"$@"

PROG=build-wasi/micropython

# Unlike emscripten, WASI doesn't provide a way to scan GC roots
# like WASM locals. Hopefully --spill-pointers can workaround it.
${WASM_OPT} \
--spill-pointers \
-o ${PROG}.spilled ${PROG}

# LLVM still uses the older version of EH proposal. ("phase 3")
# Convert to the latest version of EH proposal with exnref.
${WASM_OPT} \
--translate-to-exnref --enable-exception-handling \
-o ${PROG}.spilled.exnref ${PROG}.spilled

# now you can run it with EH-enabled runtimes.
# eg.  (using the latest EH)
# toywasm --wasi build-wasi/micropython.spilled.exnref
# eg.  (using the old EH)
# iwasm build-wasi/micropython.spilled
