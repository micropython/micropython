import sys
import time
import os
import subprocess

device = sys.argv[1]
file_location = "./psoc6/test_inputs/main.py"
remote_directory_path = "/"
mpr_connect = f"../tools/mpremote/mpremote.py connect {device}"
mpr_file_cp = f"{mpr_connect} cp {file_location} :{remote_directory_path}"
mpr_ls = f"{mpr_connect} fs ls /"
mpr_rm = f"{mpr_connect} fs rm "
mpr_reset = f"../tools/mpremote/mpremote.py reset"


def exec(cmd, op_file_path="null"):
    if cmd == mpr_rm:
        # Check if file is present already
        output = subprocess.run(f"{mpr_ls} | grep {op_file_path}", shell=True, capture_output=True)
        # If the file is present, remove it
        if output.returncode == 0:
            subprocess.run(f"{cmd} {op_file_path}", shell=True, capture_output=False)
    else:
        with open(op_file_path, "a") as file:
            subprocess.check_call(cmd, shell=True, stdout=file)


# bitsream generation

print("copying file to remote device")
exec(mpr_rm, "main.py")
exec(mpr_file_cp)
print("Resest board")
subprocess.run(mpr_reset, shell=True)
