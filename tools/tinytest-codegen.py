#!/usr/bin/env python3

import os, sys
from glob import glob
from re import sub
import argparse


def escape(s):
    s = s.decode()
    lookup = {
        '\0': '\\0',
        '\t': '\\t',
        '\n': '\\n\"\n\"',
        '\r': '\\r',
        '\\': '\\\\',
        '\"': '\\\"',
    }
    return "\"\"\n\"{}\"".format(''.join([lookup[x] if x in lookup else x for x in s]))

def chew_filename(t):
    return { 'func': "test_{}_fn".format(sub(r'/|\.|-', '_', t)), 'desc': t }

def script_to_map(test_file):
    r = {"name": chew_filename(test_file)["func"]}
    with open(test_file, "rb") as f:
        r["script"] = escape(f.read())
    with open(test_file + ".exp", "rb") as f:
        r["output"] = escape(f.read())
    return r

test_function = (
    "void {name}(void* data) {{\n"
    "  static const char pystr[] = {script};\n"
    "  static const char exp[] = {output};\n"
    '  printf("\\n");\n'
    "  upytest_set_expected_output(exp, sizeof(exp) - 1);\n"
    "  upytest_execute_test(pystr);\n"
    '  printf("result: ");\n'
    "}}"
)

testcase_struct = (
    "struct testcase_t {name}_tests[] = {{\n{body}\n  END_OF_TESTCASES\n}};"
)
testcase_member = (
    "  {{ \"{desc}\", {func}, TT_ENABLED_, 0, 0 }},"
)

testgroup_struct = (
    "struct testgroup_t groups[] = {{\n{body}\n  END_OF_GROUPS\n}};"
)
testgroup_member = (
    "  {{ \"{name}\", {name}_tests }},"
)

## XXX: may be we could have `--without <groups>` argument...
# currently these tests are selected because they pass on qemu-arm
test_dirs = ('basics', 'micropython', 'misc', 'extmod', 'float', 'inlineasm', 'qemu-arm',) # 'import', 'io',)
exclude_tests = (
    # pattern matching in .exp
    'basics/bytes_compare3.py',
    'extmod/ticks_diff.py',
    'extmod/time_ms_us.py',
    'extmod/uheapq_timeq.py',
    # unicode char issue
    'extmod/ujson_loads.py',
    # doesn't output to python stdout
    'extmod/ure_debug.py',
    'extmod/vfs_basic.py',
    'extmod/vfs_fat_ramdisk.py', 'extmod/vfs_fat_fileio.py',
    'extmod/vfs_fat_fsusermount.py', 'extmod/vfs_fat_oldproto.py',
    # rounding issues
    'float/float_divmod.py',
    # requires double precision floating point to work
    'float/float2int_doubleprec_intbig.py',
    'float/float_parse_doubleprec.py',
    # inline asm FP tests (require Cortex-M4)
    'inlineasm/asmfpaddsub.py', 'inlineasm/asmfpcmp.py', 'inlineasm/asmfpldrstr.py',
    'inlineasm/asmfpmuldiv.py','inlineasm/asmfpsqrt.py',
    # different filename in output
    'micropython/emg_exc.py',
    'micropython/heapalloc_traceback.py',
    # pattern matching in .exp
    'micropython/meminfo.py',
    # needs sys stdfiles
    'misc/print_exception.py',
    # settrace .exp files are too large
    'misc/sys_settrace_loop.py',
    'misc/sys_settrace_generator.py',
    'misc/sys_settrace_features.py',
)

output = []
tests = []

argparser = argparse.ArgumentParser(description='Convert native MicroPython tests to tinytest/upytesthelper C code')
argparser.add_argument('--stdin', action="store_true", help='read list of tests from stdin')
args = argparser.parse_args()

if not args.stdin:
    for group in test_dirs:
        tests += [test for test in glob('{}/*.py'.format(group)) if test not in exclude_tests]
else:
    for l in sys.stdin:
        tests.append(l.rstrip())

output.extend([test_function.format(**script_to_map(test)) for test in tests])
testcase_members = [testcase_member.format(**chew_filename(test)) for test in tests]
output.append(testcase_struct.format(name="", body='\n'.join(testcase_members)))

testgroup_members = [testgroup_member.format(name=group) for group in [""]]

output.append(testgroup_struct.format(body='\n'.join(testgroup_members)))

## XXX: may be we could have `--output <filename>` argument...
# Don't depend on what system locale is set, use utf8 encoding.
sys.stdout.buffer.write('\n\n'.join(output).encode('utf8'))
