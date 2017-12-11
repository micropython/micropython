#!/usr/bin/env python3

import os, sys
from glob import glob
from re import sub

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
    return { 'func': "test_{}_fn".format(sub(r'/|\.|-', '_', t)), 'desc': t.split('/')[1] }

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
    "  upytest_set_expected_output(exp, sizeof(exp) - 1);\n"
    "  upytest_execute_test(pystr);\n"
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
    "  {{ \"{name}/\", {name}_tests }},"
)

## XXX: may be we could have `--without <groups>` argument...
# currently these tests are selected because they pass on qemu-arm
test_dirs = ('basics', 'micropython', 'float', 'extmod', 'inlineasm') # 'import', 'io', 'misc')
exclude_tests = (
    'float/float2int_doubleprec_intbig.py', # requires double precision floating point to work
    'inlineasm/asmfpaddsub.py', 'inlineasm/asmfpcmp.py', 'inlineasm/asmfpldrstr.py', 'inlineasm/asmfpmuldiv.py', 'inlineasm/asmfpsqrt.py',
    'extmod/ticks_diff.py', 'extmod/time_ms_us.py', 'extmod/uheapq_timeq.py',
    'extmod/vfs_fat_ramdisk.py', 'extmod/vfs_fat_fileio.py', 'extmod/vfs_fat_fsusermount.py', 'extmod/vfs_fat_oldproto.py',
)

output = []

for group in test_dirs:
    tests = [test for test in glob('{}/*.py'.format(group)) if test not in exclude_tests]
    output.extend([test_function.format(**script_to_map(test)) for test in tests])
    testcase_members = [testcase_member.format(**chew_filename(test)) for test in tests]
    output.append(testcase_struct.format(name=group, body='\n'.join(testcase_members)))

testgroup_members = [testgroup_member.format(name=group) for group in test_dirs]

output.append(testgroup_struct.format(body='\n'.join(testgroup_members)))

## XXX: may be we could have `--output <filename>` argument...
print('\n\n'.join(output))
