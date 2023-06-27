# test sys.path

import sys
# check that this script was executed from a file of the same name
if "__file__" not in globals() or "sys_path.py" not in __file__:
    print("SKIP")
    raise SystemExit

# test that sys.path[0] is the directory containing this script
with open(sys.path[0] + "/sys_path.py") as f:
    for _ in range(4):
        print(f.readline())
