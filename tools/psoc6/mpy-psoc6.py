import argparse, os, sys, shlex, shutil, subprocess, requests, tarfile, zipfile

boards = [
    {"name": "CY8CPROTO-062-4343W", "ocd_cfg_file": "psoc6_2m.cfg"},
    {"name": "CY8CPROTO-063-BLE", "ocd_cfg_file": "psoc6.cfg"},
]

opsys = ""


def colour_str_success(msg):
    green_str_start = "\x1b[1;32;40m"
    str_color_end = "\x1b[0m"
    return green_str_start + msg + str_color_end


def colour_str_error(msg):
    green_str_start = "\x1b[1;31;40m"
    str_color_end = "\x1b[0m"
    return green_str_start + msg + str_color_end


def colour_str_highlight(msg):
    green_str_start = "\x1b[1;35;40m"
    str_color_end = "\x1b[0m"
    return green_str_start + msg + str_color_end


def set_environment():
    global opsys
    if sys.platform == "linux" or sys.platform == "linux2":
        opsys = "linux"
    elif sys.platform == "win32" or sys.platform == "cygwin":
        opsys = "win"
    elif sys.platform == "darwin":
        opsys = "mac"
        raise Exception(colour_str_error("OS unsupported"))


def mpy_get_fw_hex_file_name(file_name, board):
    file_extension = ".hex"
    return str(file_name) + "_" + str(board) + file_extension


def mpy_firmware_deploy(file_name, board, serial_adapter_sn=None):
    print("Deploying firmware...")
    hex_file = mpy_get_fw_hex_file_name(file_name, board)
    openocd_program(board, hex_file, serial_adapter_sn)
    print(colour_str_success("Firmware deployed successfully"))


def mpy_firmware_download(file_name, board, version):
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

    res = requests.get(file_url)
    open(file_name_for_board, "wb").write(res.content)


def mpy_firmware_remove(file_name, board):
    os.remove(mpy_get_fw_hex_file_name(file_name, board))


def fwloader_download_install():
    file_extension = ".zip"
    fwloader_compressed = "fwloader" + file_extension

    def get_fwloader_file_url_name():
        if opsys == "linux":
            file_os_suffix = "linux"
        elif opsys == "win":
            file_os_suffix = "windows"

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

    def download_fwloader(file_url, file_name):
        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)
        os.rename(file_name, fwloader_compressed)

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
                raise Exception(colour_str_error("bash error"))

            os.chmod(os.path.join("fw-loader", "bin", "fw-loader"), 0o755)

    print("Downloading fw-loader...")
    file_url, file_name = get_fwloader_file_url_name()
    download_fwloader(file_url, file_name)
    print("Extracting fw-loader...")
    extract_fwloader()
    fwloader_setup()


def fwloader_update_kitprog():
    print("Updating kitprog3 firmware...")
    fwloader_cmd = "fw-loader --update-kp3 all"
    print(fwloader_cmd)
    fwloader_args = shlex.split(fwloader_cmd)

    try:
        fwl_proc = subprocess.Popen(fwloader_args)
        fwl_proc.wait()
    except:
        raise Exception(colour_str_error("fwloader error"))

    print(colour_str_success("Debugger kitprog3 firmware updated successfully"))


def fwloader_remove():
    file_extension = ".zip"
    fwloader_compressed = "fwloader" + file_extension
    os.remove(fwloader_compressed)
    shutil.rmtree("fw-loader")
    shutil.rmtree("kp-firmware")


def openocd_download_install():
    if opsys == "linux":
        file_os_suffix = "linux"
        file_extension = ".tar.gz"
    elif opsys == "win":
        file_os_suffix = "windows"
        file_extension = ".zip"

    openocd_compressed = "openocd" + file_extension

    def get_openocd_file_url_name():
        filename_base = "openocd-4.4.0.2134-"
        url_base = "https://github.com/Infineon/micropython/releases/download/v0.3.0/"

        file_name = filename_base + file_os_suffix + file_extension
        file_url = url_base + file_name

        return file_url, file_name

    def download_openocd(file_url, file_name):
        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)
        os.rename(file_name, openocd_compressed)

    def extract_openocd():
        if opsys == "linux":
            compress_file = tarfile.open(openocd_compressed)
            compress_file.extractall(".")
            compress_file.close()
        elif opsys == "win":
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
                raise Exception(colour_str_error("bash error"))

    print("Downloading openocd...")
    file_url, file_name = get_openocd_file_url_name()
    download_openocd(file_url, file_name)
    print("Extracting openocd...")
    extract_openocd()
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
        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)

        # Rename config file
        os.rename(file_name, "qspi_config.cfg")

    # Move to parent dir
    os.chdir(parent_dir)


def openocd_program(board, hex_file, serial_adapter_sn=None):
    if opsys == "linux":
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

    try:
        ocd_proc = subprocess.Popen(openocd_args)
        ocd_proc.wait()
    except:
        raise Exception(colour_str_error("openocd error"))


def openocd_remove():
    if opsys == "linux":
        file_extension = ".tar.gz"
    elif opsys == "win":
        file_extension = ".zip"

    openocd_compressed = "openocd" + file_extension

    os.remove(openocd_compressed)
    shutil.rmtree("openocd")


def validate_board_name(board_name):
    board_supported = False
    for brd in boards:
        if board_name == brd["name"]:
            board_supported = True
            break

    if not board_supported:
        raise Exception(colour_str_error("error: board is not supported"))


def select_board():
    def validate_board_index(board_index):
        max_board_index = len(boards)
        if board_index < 0 or board_index > max_board_index:
            raise Exception(colour_str_error("error: board ID not valid"))

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

    board_index = int(input(colour_str_highlight("Please type the desired board ID: ")))
    validate_board_index(board_index)
    board = boards[board_index]["name"]

    return board


def wait_and_request_board_connect():
    input(
        colour_str_highlight(
            "Please CONNECT THE BOARD and PRESS ENTER to start the firmware deployment\n"
        )
    )


def wait_user_termination():
    input(colour_str_highlight("Press ENTER to continue...\n"))


def device_setup(board, version, update_dbg_fw=False, quiet=False):
    if board is None:
        board = select_board()
    else:
        validate_board_name(board)

    print("MicroPython PSoC6 Board  :: ", board)

    if version is None:
        version = "latest"

    print("MicroPython PSoC6 Version :: ", version)

    if update_dbg_fw:
        fwloader_download_install()
        fwloader_update_kitprog()
        fwloader_remove()

    openocd_download_install()
    openocd_board_conf_download(board)
    mpy_firmware_download("hello-world", board, "v0.3.0")
    mpy_firmware_download("mpy-psoc6", board, version)

    if not quiet:
        wait_and_request_board_connect()

    mpy_firmware_deploy("hello-world", board)
    mpy_firmware_deploy("mpy-psoc6", board)

    openocd_remove()
    mpy_firmware_remove("hello-world", board)
    mpy_firmware_remove("mpy-psoc6", board)

    print(colour_str_success("Device setup completed :)"))

    if not quiet:
        wait_user_termination()


def device_erase(board, quiet=False):
    if board != "CY8CPROTO-062-4343W":
        raise Exception(colour_str_error("error: board is not supported"))

    openocd_download_install()
    openocd_board_conf_download(board)
    mpy_firmware_download("device-erase", board, "v0.3.0")

    if not quiet:
        wait_and_request_board_connect()

    mpy_firmware_deploy("device-erase", board)

    openocd_remove()
    mpy_firmware_remove("device-erase", board)

    print(colour_str_success("Device erase completed :)"))

    if not quiet:
        wait_user_termination()


def arduino_lab_download_and_launch():
    def download_arduino_lab():
        print("Downloading Arduino Lab for Micropython...")

        if opsys == "linux":
            file_os_suffix = "linux_x64"
        elif opsys == "win":
            file_os_suffix = "win_x64"

        url_base = "https://github.com/arduino/lab-micropython-editor/releases/download/0.8.0/"
        file_name_extension = ".zip"
        file_name = "Arduino.Lab.for.Micropython-" + file_os_suffix + file_name_extension
        file_url = url_base + file_name

        res = requests.get(file_url)
        open(file_name, "wb").write(res.content)

        os.rename(file_name, "arduino-for-micropython.zip")

    def extract_arduino_lab():
        print("Extracting Arduino Lab for Micropython...")
        mpyinolab_dir = "arduino-lab-mpy"
        if not os.path.exists(mpyinolab_dir):
            os.mkdir(mpyinolab_dir)

        compress_file = zipfile.ZipFile("arduino-for-micropython.zip")
        compress_file.extractall(mpyinolab_dir)
        compress_file.close()

    def launch_arduino_lab():
        print("Launching Arduino Lab for Micropython...")

        mpyinolab_dir = "arduino-lab-mpy"

        if opsys == "linux":
            mpy_ide = ["./arduino-lab-micropython-ide"]
            os.chmod(os.path.join(mpyinolab_dir, "arduino-lab-micropython-ide"), 0o755)
        elif opsys == "win":
            mpy_ide = ["Arduino Lab for Micropython.exe"]

        parent_dir = os.path.abspath(os.curdir)
        os.chdir(mpyinolab_dir)

        try:
            ide_proc = subprocess.Popen(mpy_ide)
            ide_proc.wait()
        except:
            raise Exception(colour_str_error("error: Could not launch Arduino Lab IDE"))

        os.chdir(parent_dir)

    download_arduino_lab()
    extract_arduino_lab()
    launch_arduino_lab()


def arduino_lab_install_package_remove():
    print("Cleaning up Arduino Lab for Micropython installation package...")
    os.remove("arduino-for-micropython.zip")


def quick_start(board, version):
    def print_retro_banner():
        print("################################################")
        print("                Welcome to the                  ")
        print(" __  __ _            ___      _   _             ")
        print("|  \/  (_)__ _ _ ___| _ \_  _| |_| |_  ___ _ _")
        print("| |\/| | / _| '_/ _ \  _/ || |  _| ' \/ _ \ ' \\")
        print("|_|_ |_|_\__|_| \___/_|  \_, |\__|_||_\___/_||_|")
        print("| _ \/ __| ___ / __|/ /  |__/")
        print("|  _/\__ \/ _ \ (__/ _ \\")
        print("|_|  |___/\___/\___\___/")
        print("")
        print("                 Quick Start                    ")
        print("################################################")

    def print_exit_banner():
        print(colour_str_success("################################################"))
        print(colour_str_success("   The quick-start is completed. Have fun :)    "))
        print(colour_str_success("################################################"))

    print_retro_banner()
    device_setup(board, version, True)
    arduino_lab_download_and_launch()
    arduino_lab_install_package_remove()
    print_exit_banner()
    wait_user_termination()


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parser_quick_start(args):
        quick_start(args.board, args.version)

    def parser_device_setup(args):
        device_setup(args.board, args.version, args.u, args.q)

    def parser_firmware_deploy(args):
        openocd_program(args.board, args.hexfile)

    def parser_device_erase(args):
        device_erase(args.board, args.q)

    # Main parser
    main_parser_desc = """
    Micropython PSoC6 utility script

    Available commands:

    quick-start         Setup of MicroPython IDE and PSoC6 device
    device-setup        Setup of MicroPython PSoC6 device
    device-erase        Erase the external memory of the PSoC6 device
    firmware-deploy     Firmware deployment on PSoC6 device (user provided binary file)
                                                                  
    mpy-psoc6.py <command> --help for more information about each specific command.
    """
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description=main_parser_desc
    )
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # quick start
    parser_qs = subparser.add_parser(
        "quick-start",
        description="Setup of MicroPython IDE and PSoC6 device. \
                    Use this command for a guided installation and \
                    quick start using MicroPython PSoC6.",
    )
    parser_qs.add_argument(
        "-b", "--board", default=None, type=str, help="PSoC6 prototyping kit name"
    )
    parser_qs.add_argument(
        "-v", "--version", default=None, type=str, help="MicroPython PSoC6 firmware version"
    )
    parser_qs.set_defaults(func=parser_quick_start)

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
        "-u", action="store_true", help="Update board Kitprog3 debugger firmware"
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
                    on a PSoC6 board. \
                    Requires openocd available on the system path.",
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
    set_environment()
    parser()
