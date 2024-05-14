import subprocess
import sys
import os
import time

device = sys.argv[1]

remote_directory_path = "/"

wdt = "psoc6/wdt.py"
wdt_reset_check = "psoc6/wdt_reset_check.py"

# out and exp file paths
wdt_op_fp = "./psoc6/test_scripts/wdt.py.out"
wdt_reset_check_op_fp = "./psoc6/test_scripts/wdt_reset_check.py.out"
exp_wdt = "./psoc6/test_scripts/wdt.py.exp"
exp_wdt_reset_check = "./psoc6/test_scripts/wdt_reset_check.py.exp"

# List of mpremote commands
mpr_run_wdt = f"../tools/mpremote/mpremote.py connect {device} run {wdt}"
mpr_run_resume = f"../tools/mpremote/mpremote.py resume run {wdt_reset_check} "


def exec(cmd, output_file):
    try:
        with open(output_file, "w") as f:
            process = subprocess.Popen(cmd, shell=True, stdout=f, stderr=subprocess.PIPE)
            output, error = process.communicate()
        if process.returncode != 0:
            print(f"Command execution failed with error: {error.decode('utf-8')}")
            sys.exit(1)
        else:
            print(f"Command executed successfully: {cmd}")
    except Exception as e:
        print(f"An error occurred while executing the command: {cmd}\nError: {e}")


def validate_test(op, exp_op):
    with open(op, "r") as output_file:
        output = [line.strip() for line in output_file]

    with open(exp_op, "r") as exp_output_file:
        exp_output = [line.strip() for line in exp_output_file]

    if output != exp_output:
        print("Test Failed!")
        for line in output:
            print(line)
        sys.exit(1)
    else:
        print("Test successful!")


def wdt_test():
    print("Running wdt test")
    exec(mpr_run_wdt, wdt_op_fp)
    validate_test(wdt_op_fp, exp_wdt)
    os.remove(wdt_op_fp)


def wdt_reset_check():
    print("Running wdt reset test")
    exec(mpr_run_resume, wdt_reset_check_op_fp)
    validate_test(wdt_reset_check_op_fp, exp_wdt_reset_check)
    os.remove(wdt_reset_check_op_fp)


wdt_test()
time.sleep(10)
wdt_reset_check()
