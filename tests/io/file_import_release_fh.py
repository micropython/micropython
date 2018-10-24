# Test demonstrating failures to release an open file handle in
# mp_parse() and mp_raw_code_load().
# See https://github.com/micropython/micropython/pull/3874

import os

def delete(f):
    try:
        os.remove(f)
    except Exception as e:
        # Failure addressed in issue #3874
        print("FAILED to delete %s: %s" % (f, str(e)))

with open("testmpy.mpy", "w") as testmodule:
    testmodule.write("not valid")

try:
    import testmpy
except ValueError:
    pass     # expecting ValueError: incompatible .mpy file

delete("testmpy.mpy")
    
with open("testpy.py", "w") as testmodule:
    testmodule.write("(((\r")

try:
    import testpy
except SyntaxError:
    pass     # expecting SyntaxError: invalid syntax

delete("testpy.py")

print("TEST COMPLETE")
