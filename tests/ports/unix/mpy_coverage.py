try:
    import mpycross
except ImportError:
    print("SKIP")
    raise SystemExit

import os
import sys

TESTS = None
# When running from run-tests.py, "extmod" is on sys.path. Use it to find the tests directory.
for p in sys.path:
    if p.endswith("/extmod"):
        TESTS = p.rsplit("/", 1)[0] + "/tests"
        break

# When running manually from the commandline, use the location of this file to find the tests directory.
if TESTS is None:
    TESTS = (__file__.rsplit("/", 1)[0] or ".") + "/../.."

# A subset of things in run-tests.py:emitter_tests_to_skip: ones that actually cause errors while compiling
blacklist = {
    "exception_chain.py",
    "try_reraise2.py",
    "try_finally_return2.py",
    "gen_yield_from_close.py",
    "try_reraise.py",
}


def run_dir(testdir, pfx="", emit_arch="host", emit_opt="EMIT_OPT_BYTECODE"):
    testdir = f"{TESTS}/{testdir}"
    for leaf in sorted(os.listdir(testdir)):
        if leaf in blacklist:
            continue
        if not leaf.endswith(".py"):
            continue
        if not leaf.startswith(pfx):
            continue
        with open(f"{testdir}/{leaf}", "r") as f:
            content = f.read()
            print(f"Compiling {leaf} with {emit_arch} {emit_opt}")
            try:
                compiled = mpycross.compile(
                    content, leaf, emit_arch=emit_arch, emit_opt=getattr(mpycross, emit_opt)
                )
            except SyntaxError as e:
                print(e)
            # This is likely to be fragile
            # print(f"got {len(compiled)} bytes of mpy file")


def arch_support_check(arch):
    try:
        mpycross.compile("3+3", "<arch-support-test>", emit_arch=arch)
    except ValueError as e:
        if "emit_arch" in e.value:
            # Might be 'unable to determine host architecture for emit_arch="host"' or 'Invalid emit_arch'
            print("SKIP")
            raise SystemExit
        else:
            raise e


def run_one_arch(arch, inlineasm=None):
    arch_support_check(arch)

    run_dir("micropython", "viper_", emit_arch=arch)
    run_dir("basics", emit_arch=arch, emit_opt="EMIT_OPT_NATIVE_PYTHON")
    if inlineasm:
        run_dir(f"inlineasm/{inlineasm}", emit_arch=arch, emit_opt="EMIT_OPT_NATIVE_PYTHON")


def run_extra_cases(emit_arch, cases):
    arch_support_check(emit_arch)

    for content in cases:
        try:
            compiled = mpycross.compile(content, "<expect-error>", emit_arch=emit_arch)
            print(f"OK")
        except SyntaxError as e:
            print(e)


if __name__ == "__main__":
    arch_support_check("host")
    run_dir(f"basics", emit_opt="EMIT_OPT_BYTECODE")
    mpycross.compile(
        "3+3",
        "<debug>",
        emit_arch="debug",
        emit_opt=mpycross.EMIT_OPT_NATIVE_PYTHON,
        nlr_buf_num_regs=32,
    )

    try:
        mpycross.compile("3+3", "<debug>", emit_arch="invalid")
    except ValueError as e:
        print(e)

    try:
        mpycross.compile("3+3", "<debug>", emit_opt=-1)
    except ValueError as e:
        print(e)

    try:
        mpycross.compile("3+3", "<debug>", emit_opt=mpycross.EMIT_OPT_NATIVE_PYTHON)
    except ValueError as e:
        print(e)
