# Unix Fuzzing Variant

This variant is for use with the [AFLplusplus](https://github.com/AFLplusplus/AFLplusplus)
fuzzer.

 1. Install AFLplusplus so that the program `afl-cc` is on $PATH
 1. `cd ports/unix && make VARIANT=aflplusplus -j$(nproc)`
 1. Gather your inputs (e.g., from test cases in `tests`)
 1. Optionally, minimize them e.g., with `afl-cmin` (see AFLplusplus docs)
 1. Run the fuzzer. The simplest single process way is: `afl-fuzz -i inputs -o findings -- ports/unix/build-aflfuzz/micropython @@`

Eventually, if crashing test cases are found, the crashing program(s) are placed in
`findings/default/crashes` and invocations that were determined to hang go to
`findings/default/hangs`.

There are many more advanced ways to run the fuzzer; see the AFLplusplus documentation for info.

# Safety

Functionality that is known to be unsafe (host filesystem write access via vfs_posix)
or is accepted as causing crashes when used improperly (ctypes, certain struct
& array typecodes) is build-time disabled. However, it's always possible that
the fuzzer will find a condition that can cause an unexpected modification to
the runtime environment.
