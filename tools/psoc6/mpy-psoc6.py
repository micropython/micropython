import argparse, os, sys, shlex, shutil, subprocess, time, requests, tarfile, zipfile

boards = [
    {"name": "CY8CPROTO-062-4343W", "ocd_cfg_file": "psoc6_2m.cfg"},
    {"name": "CY8CPROTO-063-BLE", "ocd_cfg_file": "psoc6.cfg"},
]

opsys = ""
version = "0.2.0"


def colour_str_success(msg):
    green_str_start = "\x1b[1;32;40m"
    str_color_end = "\x1b[0m"
    return green_str_start + msg + str_color_end


def colour_str_error(msg):
    red_str_start = "\x1b[1;31;40m"
    str_color_end = "\x1b[0m"
    return red_str_start + msg + str_color_end


def colour_str_highlight(msg):
    purple_str_start = "\x1b[1;35;40m"
    str_color_end = "\x1b[0m"
    return purple_str_start + msg + str_color_end


def set_environment():
    global opsys
    if sys.platform == "linux" or sys.platform == "linux2":
        opsys = "linux"
    elif sys.platform == "win32" or sys.platform == "cygwin":
        opsys = "win"
        os.system("color")  # Enable colouring in cmd and powershell
    elif sys.platform == "darwin":
        opsys = "mac"


def mpy_get_fw_hex_file_name(file_name, board):
    file_extension = ".hex"
    return str(file_name) + "_" + str(board) + file_extension


def mpy_firmware_deploy(file_name, board, serial_adapter_sn=None):
    print(f"Deploying firmware {file_name} ...")
    hex_file = mpy_get_fw_hex_file_name(file_name, board)
    openocd_program(board, hex_file, serial_adapter_sn)
    print(colour_str_success(f"Firmware {file_name} deployed successfully"))


def mpy_firmware_download(file_name, board, version):
    def mpy_firmware_clean(file_name, board):
        file_name_for_board = mpy_get_fw_hex_file_name(file_name, board)
        if os.path.exists(file_name_for_board):
            os.remove(file_name_for_board)

    print(
        "Downloading " + str(file_name) + " " + str(version) + " for " + str(board) + " board..."
    )

    if version == "latest":
        sub_url = "latest/download"
    else:
        sub_url = "download/" + str(version)

    file_name_for_board = mpy_get_fw_hex_file_name(file_name, board)
    file_url = (
        "https://github.com/infineon/micropython/releases/" + sub_url + "/" + file_name_for_board
    )

    # Clean if existing from previous run
    # It is not sure if it is the same version
    mpy_firmware_clean(file_name, board)

    res = requests.get(file_url)
    open(file_name_for_board, "wb").write(res.content)


def fwloader_download_install():
    file_extension = ".zip"
    fwloader_compressed = "fwloader" + file_extension

    def is_fwloader_already_installed():
        if opsys == "linux":
            fwloader = "fw-loader"
        elif opsys == "win":
            fwloader = "fw-loader.exe"
        elif opsys == "mac":
            fwloader = "fw-loader"

        if os.path.exists(os.path.join("fw-loader", "bin", fwloader)):
            return True

        return False

    def get_fwloader_file_url_name():
        if opsys == "linux":
            file_os_suffix = "linux"
        elif opsys == "win":
            file_os_suffix = "windows"
        elif opsys == "mac":
            file_os_suffix = "macos"

        version = "3.5.0.2114"
        package_version = "2.50.0.1383"
        file_name = (
            "fw-loader-"
            + version
            + "-kitprog3-package-"
            + package_version
            + "-"
            + file_os_suffix
            + file_extension
        )

        base_url = "https://github.com/Infineon/Firmware-loader/releases/download/3.5.0/"
        file_url = base_url + file_name

        return file_url, file_name

    def clean_fwloader(file_name):
        if os.path.exists(file_name):
            os.remove(file_name)

    def download_fwloader(file_url, file_name):
        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)
        os.replace(file_name, fwloader_compressed)

    def extract_fwloader():
        compress_file = zipfile.ZipFile(fwloader_compressed)
        compress_file.extractall(".")
        compress_file.close()

    def fwloader_setup():
        # Add fw-loader to path
        os.environ["PATH"] += os.pathsep + os.path.join("fw-loader", "bin")

        if opsys == "linux":
            # Install udev rules
            sh_args = ["sh", "fw-loader/udev_rules/install_rules.sh"]
            try:
                sh_proc = subprocess.Popen(sh_args)
                sh_proc.wait()
            except:
                sys.exit(colour_str_error("bash error"))

        if opsys == "linux" or opsys == "mac":
            os.chmod(os.path.join("fw-loader", "bin", "fw-loader"), 0o755)

    if not is_fwloader_already_installed():
        print("Downloading fw-loader...")
        file_url, file_name = get_fwloader_file_url_name()
        clean_fwloader(file_name)
        download_fwloader(file_url, file_name)
        print("Extracting fw-loader...")
        extract_fwloader()
    else:
        print("fw-loader installation skipped. Already installed")

    fwloader_setup()


def fwloader_update_kitprog():
    def parse_output_for_error(fwloader_stdout):
        fwloader_out_lines = fwloader_stdout.decode().split("\n")
        for line in fwloader_out_lines:
            if "Error" in line:
                print("fw-loader output: \n" + fwloader_stdout.decode())

    print("Updating kitprog3 firmware...")
    fwloader_cmd = "fw-loader --update-kp3 all"
    fwloader_args = shlex.split(fwloader_cmd)

    fwl_proc = subprocess.Popen(fwloader_args, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    try:
        out, err = fwl_proc.communicate(timeout=100)
    except:
        fwl_proc.kill()
        sys.exit(colour_str_error("fwloader error"))

    if err:
        sys.exit(colour_str_error(err.decode()))

    parse_output_for_error(out)

    print(colour_str_success("Debugger kitprog3 firmware updated successfully"))


def fwloader_remove():
    file_extension = ".zip"
    fwloader_compressed = "fwloader" + file_extension
    if os.path.exists(fwloader_compressed):
        os.remove(fwloader_compressed)
    if os.path.exists("fw-loader"):
        shutil.rmtree("fw-loader")
    if os.path.exists("kp-firmware"):
        shutil.rmtree("kp-firmware")


def openocd_download_install():
    if opsys == "linux":
        file_os_suffix = "linux"
        file_extension = ".tar.gz"
        openocd_exe = "openocd"
    elif opsys == "win":
        file_os_suffix = "windows"
        file_extension = ".zip"
        openocd_exe = "openocd.exe"
    elif opsys == "mac":
        file_os_suffix = "macos"
        file_extension = ".zip"
        openocd_exe = "openocd"

    openocd_compressed = "openocd" + file_extension

    def is_openocd_already_installed():
        if os.path.exists(os.path.join("openocd", "bin", openocd_exe)):
            return True

        return False

    def get_openocd_file_url_name():
        filename_base = "openocd-5.1.0.3099-"
        url_base = "https://github.com/Infineon/openocd/releases/download/release-v5.1.0/"

        file_name = filename_base + file_os_suffix + file_extension
        file_url = url_base + file_name

        return file_url, file_name

    def clean_openocd(file_name):
        if os.path.exists(file_name):
            os.remove(file_name)

    def download_openocd(file_url, file_name):
        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)
        os.replace(file_name, openocd_compressed)

    def extract_openocd():
        if opsys == "linux":
            compress_file = tarfile.open(openocd_compressed)
            compress_file.extractall(".")
            compress_file.close()
        elif opsys == "win" or opsys == "mac":
            compress_file = zipfile.ZipFile(openocd_compressed)
            compress_file.extractall(".")
            compress_file.close()

    def openocd_setup():
        # Add openocd to path
        os.environ["PATH"] += os.pathsep + os.path.join("openocd", "bin")

        if opsys == "linux":
            # Install udev rules
            sh_args = ["sh", "openocd/udev_rules/install_rules.sh"]
            try:
                sh_proc = subprocess.Popen(sh_args)
                sh_proc.wait()
            except:
                sys.exit(colour_str_error("bash error"))

        if opsys == "mac":
            os.chmod(os.path.join("openocd", "bin", "openocd"), 0o755)

    if not is_openocd_already_installed():
        print("Downloading openocd...")
        file_url, file_name = get_openocd_file_url_name()
        clean_openocd(file_name)
        download_openocd(file_url, file_name)
        print("Extracting openocd...")
        extract_openocd()
    else:
        print("openocd already available. Installation skipped")

    openocd_setup()


def openocd_board_conf_download(board):
    print("Downloading openocd " + str(board) + " configuration...")

    # Create and move to board dir in openocd folder
    parent_dir = os.path.abspath(os.curdir)
    os.chdir("openocd")
    if not os.path.exists("board"):
        os.mkdir("board")
    os.chdir("board")

    # Download config file
    if board == "CY8CPROTO-062-4343W":
        file_name = "qspi_config_" + str(board) + ".cfg"
        file_url = "https://github.com/infineon/micropython/releases/download/v0.3.0/" + file_name

        # Clean file if exists from previous run
        if os.path.exists(file_name):
            os.remove(file_name)

        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)

        # Rename config file
        os.replace(file_name, "qspi_config.cfg")

    # Move to parent dir
    os.chdir(parent_dir)


def openocd_program(board, hex_file, serial_adapter_sn=None):
    if opsys == "linux" or opsys == "mac":
        openocd = "openocd/bin/openocd"
    elif opsys == "win":
        openocd = "openocd.exe"

    for brd in boards:
        if board == brd["name"]:
            ocd_cfg_file = brd["ocd_cfg_file"]

    serial_adapter_opt = ""
    if serial_adapter_sn is not None:
        serial_adapter_opt = "adapter serial " + str(serial_adapter_sn)

    openocd_cmd = (
        openocd
        + ' -s openocd/scripts -s openocd/board -c "source [find interface/kitprog3.cfg];'
        + str(serial_adapter_opt)
        + " ; source [find target/"
        + str(ocd_cfg_file)
        + "]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program "
        + str(hex_file)
        + ' verify reset exit;"'
    )
    openocd_args = shlex.split(openocd_cmd)

    ocd_proc = subprocess.Popen(openocd_args, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    try:
        out, err = ocd_proc.communicate(timeout=20)
    except:
        ocd_proc.kill()
        sys.exit(colour_str_error("openocd error"))

    if err:
        sys.exit(colour_str_error(err.decode() + "Are you sure the board is connected?"))


def openocd_remove():
    if opsys == "linux":
        file_extension = ".tar.gz"
    elif opsys == "win" or opsys == "mac":
        file_extension = ".zip"

    openocd_compressed = "openocd" + file_extension
    if os.path.exists(openocd_compressed):
        os.remove(openocd_compressed)

    if os.path.exists("openocd"):
        shutil.rmtree("openocd")


def validate_board_name(board_name):
    board_supported = False
    for brd in boards:
        if board_name == brd["name"]:
            board_supported = True
            break

    if not board_supported:
        sys.exit(colour_str_error("error: board is not supported"))


def select_board():
    def validate_user_input(user_input):
        def invalid_exit():
            sys.exit(colour_str_error("error: board ID is not valid"))

        try:
            board_index = int(user_input)
        except:
            invalid_exit()

        max_board_index = len(boards)
        if board_index < 0 or board_index > max_board_index:
            invalid_exit()

        return board_index

    print("")
    print("      Supported MicroPython PSoC6 boards       ")
    print("+---------+-----------------------------------+")
    print("|   ID    |              Board                |")
    print("+---------+-----------------------------------+")
    print("|   0     |  CY8CPROTO-062-4343W              |")
    print("+---------+-----------------------------------+")
    print("|   1     |  CY8CPROTO-063-BLE                |")
    print("+---------+-----------------------------------+")
    print("")
    print("")

    board_index = validate_user_input(
        input(colour_str_highlight("Please type the desired board ID: "))
    )
    board = boards[board_index]["name"]

    return board


def wait_and_request_board_connect():
    input(
        colour_str_highlight(
            "Please CONNECT THE BOARD and PRESS ENTER to start the firmware deployment\n"
        )
    )


def device_setup(board, version, skip_update_dbg_fw=True, quiet=False):
    if board is None:
        board = select_board()
    else:
        validate_board_name(board)

    print("MicroPython PSoC6 Board  :: ", board)

    if version is None:
        version = "latest"

    print("MicroPython PSoC6 Version :: ", version)

    if not quiet:
        wait_and_request_board_connect()

    if not skip_update_dbg_fw:

        def wait_for_dev_restart():
            print("Waiting for device restart ", end="", flush=True)
            for j in range(3):
                for i in range(3):
                    print(".", end="", flush=True)
                    time.sleep(1)
                sys.stdout.write("\b\b\b\b")
                sys.stdout.write("    ")
                sys.stdout.write("\b\b\b")
                sys.stdout.flush()

            print("\nDevice restarted")

        fwloader_download_install()
        fwloader_update_kitprog()
        wait_for_dev_restart()

    openocd_download_install()
    openocd_board_conf_download(board)

    mpy_firmware_download("hello-world", board, "v0.3.0")
    mpy_firmware_download("mpy-psoc6", board, version)

    mpy_firmware_deploy("hello-world", board)
    mpy_firmware_deploy("mpy-psoc6", board)

    print(colour_str_success("Device setup completed :)"))


def device_erase(board, quiet=False):
    if board != "CY8CPROTO-062-4343W":
        sys.exit(colour_str_error("error: board is not supported"))

    if not quiet:
        wait_and_request_board_connect()

    openocd_download_install()
    openocd_board_conf_download(board)

    mpy_firmware_download("device-erase", board, "v0.3.0")

    mpy_firmware_deploy("device-erase", board)

    print(colour_str_success("Device erase completed :)"))


def firmware_deploy(board, hex_file):
    openocd_download_install()
    openocd_board_conf_download(board)
    print(f"Deploying hex file {hex_file} ...")
    openocd_program(board, hex_file)
    print(colour_str_success(f"Firmware {hex_file} deployed successfully"))


def clean_tool_downloads():
    fwloader_remove()
    openocd_remove()


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parser_device_setup(args):
        device_setup(args.board, args.version, args.skip_fw_update, args.q)

    def parser_firmware_deploy(args):
        firmware_deploy(args.board, args.hexfile)

    def parser_device_erase(args):
        device_erase(args.board, args.q)

    # Main parser
    class ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            return super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string, **kwargs):
            print("mpy-psoc6 version: " + version)
            parser.exit()

    main_parser_desc = """
    Micropython PSoC6 utility script

    Available commands:

    device-setup        Setup of MicroPython PSoC6 device
    device-erase        Erase the external memory of the PSoC6 device
    firmware-deploy     Firmware deployment on PSoC6 device (user provided binary file)
                                                                  
    mpy-psoc6.py <command> --help for more information about each specific command.
    """
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description=main_parser_desc
    )
    parser.add_argument("-v", "--version", action=ver_action, help="mpy-psoc6 version")
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # device setup
    parser_ds = subparser.add_parser(
        "device-setup",
        description="Setup of MicroPython PSoC6 device. \
                    Use this command to install the deployment tools \
                    and MicroPython firmware binary, and deploy the \
                    firmware on the PSoC6 device.",
    )
    parser_ds.add_argument(
        "-b", "--board", default=None, type=str, help="PSoC6 prototyping kit name"
    )
    parser_ds.add_argument(
        "-v", "--version", default=None, type=str, help="MicroPython PSoC6 firmware version"
    )
    parser_ds.add_argument(
        "-q", action="store_true", help="Quiet. Do not prompt any user confirmation request"
    )
    parser_ds.add_argument(
        "-s",
        "--skip-fw-update",
        action="store_true",
        help="Skip board Kitprog3 debugger firmware update",
    )
    parser_ds.set_defaults(func=parser_device_setup)

    # device erase
    parser_de = subparser.add_parser(
        "device-erase",
        description="Erase the external memory of the device. \
                    Use this command to erase the external memory if available \
                    for the selected board. \
                    Running device-setup after this command \
                    is required to re-enable MicroPython.",
    )
    parser_de.add_argument(
        "-b", "--board", default=None, type=str, required=True, help="PSoC6 prototyping kit name"
    )
    parser_de.add_argument(
        "-q", action="store_true", help="Quiet. Do not prompt any user confirmation request"
    )
    parser_de.set_defaults(func=parser_device_erase)

    # firmware deploy
    parser_fd = subparser.add_parser(
        "firmware-deploy",
        description="Firmware deployment on MicroPython device. \
                    Use this command to deploy an existing .hex file \
                    on a PSoC6 board.",
    )
    parser_fd.add_argument(
        "-b", "--board", default=None, type=str, required=True, help="PSoC6 prototyping kit name"
    )
    parser_fd.add_argument(
        "-f", "--hexfile", type=str, required=True, help="MicroPython PSoC6 firmware .hex file"
    )
    parser_fd.set_defaults(func=parser_firmware_deploy)

    # Parser call
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    try:
        set_environment()
        parser()
    except KeyboardInterrupt:
        print(colour_str_error("error: keyboard interrupt"))
        clean_tool_downloads()
