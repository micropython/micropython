import subprocess
import sys
import os

device = sys.argv[1]
test_type = sys.argv[2]

# local and remote(MPY device) paths
local_small_file_path = "./ports/psoc6/test_inputs/test_fs_small_file.txt"
local_medium_file_path = "./ports/psoc6/test_inputs/test_fs_medium_file.txt"
local_large_file_path = "./ports/psoc6/test_inputs/test_fs_large_file.txt"
remote_directory_path = "/"

# out and exp file paths
basic_test_op_fp = "./ports/psoc6/test_scripts/fs_basic.py.out"
adv_test_op_fp = "./ports/psoc6/test_scripts/fs_adv.py.out"
exp_basic_op_fp = "./ports/psoc6/test_scripts/fs_basic.py.exp"
exp_adv_op_fp = "./ports/psoc6/test_scripts/fs_adv.py.exp"

# List of mpremote commands
mpr_connect = f"../tools/mpremote/mpremote.py connect {device}"
mpr_small_file_cp = f"{mpr_connect} cp {local_small_file_path} :{remote_directory_path}"
mpr_medium_file_cp = f"{mpr_connect} cp {local_medium_file_path} :{remote_directory_path}"
mpr_large_file_cp = f"{mpr_connect} cp {local_large_file_path} :{remote_directory_path}"
mpr_ls = f"{mpr_connect} fs ls /"
mpr_rm = f"{mpr_connect} fs rm "


def exec(cmd, op_file_path="null"):
    if cmd == mpr_rm:
        # Check if file is present already
        output = subprocess.run(f"{mpr_ls} | grep {op_file_path}", shell=True, capture_output=True)
        # If the file is present, remove it
        if output.returncode == 0:
            subprocess.run(f"{cmd} {op_file_path}", shell=True, capture_output=False)
    else:
        if os.path.exists(op_file_path):
            os.remove(op_file_path)
        with open(op_file_path, "a") as file:
            subprocess.check_call(cmd, shell=True, stdout=file)


def validate_test(op, exp_op):
    with open(op, "r") as output_file:
        output = [line.strip() for line in output_file]

    with open(exp_op, "r") as exp_output_file:
        exp_output = [line.strip() for line in exp_output_file]

    if output != exp_output:
        print("Operation failed!")
        sys.exit(1)
    else:
        print("Operation successful!")


def fs_basic_test():
    print("Running basic test")
    print("Saving small file - 10KB")
    exec(mpr_rm, "test_fs_small_file.txt")
    exec(mpr_small_file_cp, basic_test_op_fp)
    validate_test(basic_test_op_fp, exp_basic_op_fp)
    os.remove(basic_test_op_fp)


def fs_adv_test():
    print("Running advance test")
    print("Saving small files - 10KB")
    exec(mpr_rm, "test_fs_small_file.txt")
    exec(mpr_small_file_cp, adv_test_op_fp)
    print("Saving medium files - 500KB")
    exec(mpr_rm, "test_fs_medium_file.txt")
    exec(mpr_medium_file_cp, adv_test_op_fp)
    print("Saving large files - 1MB")
    exec(mpr_rm, "test_fs_large_file.txt")
    exec(mpr_large_file_cp, adv_test_op_fp)
    validate_test(adv_test_op_fp, exp_adv_op_fp)
    os.remove(adv_test_op_fp)


if test_type == "0":
    fs_basic_test()
if test_type == "1":
    fs_adv_test()
