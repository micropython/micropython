import subprocess
import sys
import os
import logging

logger = logging.getLogger("fs")
logging.basicConfig(format="%(levelname)s: %(message)s", encoding="utf-8", level=logging.WARNING)

device = sys.argv[1]
test_type = sys.argv[2]
mem_type = sys.argv[3]

# tests inputs and script paths
test_input_dir = "./ports/psoc6/inputs"
test_script_dir = "./ports/psoc6/mp_custom"

# List of mpremote commands
mpr_connect = f"../tools/mpremote/mpremote.py connect {device}"
mpr_run_script = ""

# Remote directory path
remote_directory_path = ""


def set_mpr_run_script(mem_type):
    # Required to mount the sd card if the test is for the sd card
    global mpr_run_script
    if mem_type == "sd":
        mpr_run_script = f"run {test_script_dir}/fs_mount_sd.py"

    logger.debug(f'Set setup script command to: "{mpr_run_script}"')


def set_remote_dir_path(mem_type):
    # Set test directory path based on the memory type
    global remote_directory_path
    if mem_type == "sd":
        remote_directory_path = "/sd/"
    else:
        remote_directory_path = "/"

    logger.debug(f'Set remote path to : "{mpr_run_script}"')


def get_test_input_files(test_type):
    # The "basic" test uses only the small file
    # While the "adv" (advanced) tests uses all the files
    cp_input_files = [
        "test_fs_small_file.txt",
        "test_fs_medium_file.txt",
        "test_fs_large_file.txt",
    ]
    input_files_sizes = ["10240", "511876", "1047584"]

    if test_type == "basic":
        return [cp_input_files[0]], [input_files_sizes[0]]
    elif test_type == "adv":
        return cp_input_files, input_files_sizes


def get_test_name(test_type, mem_type):
    # Specify variant tests in test printed name
    if mem_type == "sd":
        mem_type_suffix = "_sd"
    elif mem_type == "flash":
        mem_type_suffix = ""

    logger.debug(f"Get tests name : fs_{test_type}{mem_type_suffix}.py")

    return f"fs_{test_type}{mem_type_suffix}.py"


def ls_files(files):
    logger.debug(f"ls_files input: {files}")
    # It will return an array with the file size found in the remote directory
    # If -1, the file was not found
    mpr_ls = f"{mpr_connect} {mpr_run_script} fs ls {remote_directory_path}"
    logger.debug(f"ls_files command: {mpr_ls}")
    output = subprocess.run(f"{mpr_ls}", shell=True, capture_output=True)

    files_result = []
    lines = output.stdout.decode().split("\n")
    logger.debug(f"ls_files output lines: {lines}")

    for file in files:
        file_size = -1
        for line in lines:
            line = line.split()
            logger.debug(f"ls_files current processed line: {line}")
            if file in line:
                file_size = line[0]
                logger.debug(f"ls_files found file: {file} with size {file_size}")

        files_result.append(file_size)

    logger.debug(f"ls_files result: {files_result}")
    return files_result


def rm_files(files):
    # It will remove the files in the remote directory
    # The command will be concatenated with the files to remove. Example:
    # ../tools/mpremote/mpremote.py connect /dev/ttyACM0 fs rm /test_fs_medium_file.txt + fs rm /test_fs_medium_file.txt
    mpr_rm = f"{mpr_connect} {mpr_run_script} fs rm "

    logger.debug(f"rm_files command: {mpr_rm}")

    rm_sub_cmd = ""
    last_file = files[-1]
    for file in files:
        append_cmd_operand = ""
        if last_file != file:
            append_cmd_operand = " + "

        rm_sub_cmd += f"fs rm {remote_directory_path}{file}{append_cmd_operand}"

    mpr_rm_cmd = f"{mpr_connect} {mpr_run_script} {rm_sub_cmd}"

    logger.debug(f"rm_files command: {mpr_rm_cmd}")

    subprocess.run(f"{mpr_rm_cmd}", shell=True, capture_output=True)


def rm_files_if_exist(files):
    matches = ls_files(files)
    logger.debug(f"Files (to be removed) found: {matches}")

    # Take only the files that which sizes are not -1 (the existing files in the remote directory)
    existing_files = []
    for i in range(len(matches)):
        if matches[i] != -1:
            existing_files.append(files[i])

    logger.debug(f"Existing (to be removed) files: {existing_files}")

    # Remove any found input files in the remote directory
    if existing_files != []:
        print(f"Removing existing files...")
        rm_files(existing_files)
        matches = ls_files(files)
        if matches == [-1 for _ in range(len(files))]:
            print(f"Existing files removed.")


def copy_files(input_cp_files):
    ### This will create a command with concatenation of cp commands for each file in the list:
    # ../tools/mpremote/mpremote.py connect /dev/ttyACM0 fs cp ./ports/psoc6/test_inputs/test_fs_medium_file.txt :/ + fs cp ./ports/psoc6/test_inputs/test_fs_medium_file.txt :/
    cp_sub_cmd = ""
    last_file = input_cp_files[-1]
    for file in input_cp_files:
        append_cmd_operand = ""
        if last_file != file:
            append_cmd_operand = " + "
        cp_sub_cmd += f"cp {test_input_dir}/{file} :{remote_directory_path}{append_cmd_operand}"

    cp_cmd = f"{mpr_connect} {mpr_run_script} {cp_sub_cmd}"

    logger.debug(f"cp_files command: {cp_cmd}")

    subprocess.run(cp_cmd, shell=True, capture_output=True)


def validate_test(files, file_sizes):
    # This function will validate the test by comparing the file sizes found with ls
    # in the remote directory with the expected file sizes
    found_sizes = ls_files(files)

    logger.debug(f"Found sizes: {found_sizes}")

    if found_sizes != file_sizes:
        msg = "fail"
        exit_code = 1
    else:
        msg = "pass"
        exit_code = 0

    # Print the test result
    print(f"\n{msg}  {get_test_name(test_type, mem_type)}")

    # Exit with the exit code
    sys.exit(exit_code)


def cp_files_test(input_files, input_files_size):
    rm_files_if_exist(input_files)
    copy_files(input_files)
    validate_test(input_files, input_files_size)


def large_file_tests(device, test_type, mem_type):
    set_mpr_run_script(mem_type)
    set_remote_dir_path(mem_type)

    input_files, input_files_size = get_test_input_files(test_type)

    cp_files_test(input_files, input_files_size)


large_file_tests(device, test_type, mem_type)
