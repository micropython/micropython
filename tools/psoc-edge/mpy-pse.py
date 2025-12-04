import argparse, os, sys, shlex, shutil, subprocess, time, requests, tarfile, zipfile

boards = [
    {"name": "KIT_PSE84_AI", "ocd_cfg_file": "pse84xgxs2.cfg"},
]

opsys = ""
version = "0.1.0"


# Decorator to flush every print
def flush_print(func):
    printer = func

    def wrapped(*args):
        printer(*args, flush=True)

    return wrapped


print_f = flush_print(print)


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


def colour_str_warn(msg):
    yellow_str_start = "\x1b[1;33;40m"
    str_color_end = "\x1b[0m"
    return yellow_str_start + msg + str_color_end


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


def mpy_firmware_deploy(file_name, board, serial_adapter_sn=None, silent=False):
    if not silent:
        print_f(f"Deploying firmware {file_name} ...")

    hex_file = mpy_get_fw_hex_file_name(file_name, board)
    openocd_program(board, hex_file, serial_adapter_sn)
    if not silent:
        print_f(colour_str_success(f"Firmware {file_name} deployed successfully"))


def mpy_firmware_download(file_name, board, version, silent=False):
    def mpy_firmware_clean(file_name, board):
        file_name_for_board = mpy_get_fw_hex_file_name(file_name, board)

        if os.path.exists(file_name_for_board):
            os.remove(file_name_for_board)

    if not silent:
        print_f(
            "Downloading "
            + str(file_name)
            + " "
            + str(version)
            + " for "
            + str(board)
            + " board..."
        )

    if version == "latest":
        sub_url = "latest/download"
    else:
        sub_url = "download/" + str(version)

    file_name_for_board = mpy_get_fw_hex_file_name(file_name, board)
    file_url = (
        "https://github.com/infineon/micropython-psoc-edge/releases/"
        + sub_url
        + "/"
        + file_name_for_board
    )

    # Clean if existing from previous run
    # It is not sure if it is the same version
    mpy_firmware_clean(file_name, board)

    res = requests.get(file_url)
    open(file_name_for_board, "wb").write(res.content)


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

    def is_openocd_already_downloaded():
        if os.path.exists(os.path.join("openocd", "bin", openocd_exe)):
            return True

        return False

    def is_openocd_already_in_sys_path():
        try:
            ocd_proc = subprocess.Popen(
                ["openocd", "--version"], stderr=subprocess.PIPE, stdout=subprocess.PIPE
            )
            out, err = ocd_proc.communicate(timeout=10)

            if err:
                return False
            out_str = out.decode()
            if "0.12.0+dev-5.11.0.4042" or "0.12.0+dev-5.8.0.3960" in out_str:
                print_f(f"openocd found in system path : {shutil.which('openocd')}")
                return True
        except:
            return False

        return False

    def get_openocd_file_url_name():
        filename_base = "openocd-5.11.0.4042-"
        url_base = "https://github.com/Infineon/openocd/releases/download/release-v5.11.0/"

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

    if not is_openocd_already_in_sys_path():
        if not is_openocd_already_downloaded():
            print_f("Downloading openocd...")
            file_url, file_name = get_openocd_file_url_name()
            clean_openocd(file_name)
            download_openocd(file_url, file_name)
            print_f("Extracting openocd...")
            extract_openocd()
        else:
            print_f("openocd already available. Download skipped")

        openocd_setup()


def openocd_board_conf_download(board):
    print_f("Downloading openocd " + str(board) + " configuration...")

    # Create config folder to store the downloaded config files
    config_dir = "config"
    if not os.path.exists(config_dir):
        os.mkdir(config_dir)

    # Download config file
    if board == "KIT_PSE84_AI":
        file_names = ["qspi_config_" + str(board) + ".cfg", "PSE84_SMIF.FLM"]

        for file_name in file_names:
            file_url = (
                "https://github.com/infineon/micropython-psoc-edge/releases/download/v0.0.0/"
                + file_name
            )

            # Clean file if exists from previous run
            file_w_path = os.path.join(config_dir, file_name)
            if os.path.exists(file_w_path):
                os.remove(file_w_path)

            res = requests.get(file_url)
            open(file_name, "wb").write(res.content)

            if "qspi_config" in file_name:
                # Rename qspi config file to generic name
                file_name_generic = "qspi_config.cfg"
                file_w_path = os.path.join(config_dir, file_name_generic)

            # Move file to config dir
            os.replace(file_name, file_w_path)


def openocd_program(board, hex_file, serial_adapter_sn=None):
    # Find tool path cross-platform
    openocd = shutil.which("openocd")
    if not openocd:
        sys.exit(colour_str_error("error: openocd not found in PATH"))

    # Get the root openocd path.
    # It is two level up from the openocd executable
    openocd_root_dir = os.path.dirname(os.path.dirname(openocd))

    for brd in boards:
        if board == brd["name"]:
            ocd_cfg_file = brd["ocd_cfg_file"]

    serial_adapter_opt = ""
    if serial_adapter_sn is not None:
        serial_adapter_opt = "adapter serial " + str(serial_adapter_sn)

    openocd_cmd = (
        openocd
        + " -s "
        + str(openocd_root_dir)
        + "/scripts"
        + " -s config"
        + ' -c "set QSPI_FLASHLOADER config/PSE84_SMIF.FLM"'
        + ' -c "source [find interface/kitprog3.cfg]; '
        + str(serial_adapter_opt)
        + ";"
        + " transport select swd;"
        + " source [find target/infineon/"
        + str(ocd_cfg_file)
        + "];"
        + " init; reset init; adapter speed 12000;"
        + " flash write_image erase "
        + str(hex_file)
        + ";"
        + " verify_image "
        + str(hex_file)
        + ";"
        + ' reset run; shutdown;"'
    )

    openocd_args = shlex.split(openocd_cmd)

    ocd_proc = subprocess.Popen(openocd_args, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    try:
        out, err = ocd_proc.communicate(timeout=30)
    except:
        ocd_proc.kill()
        sys.exit(colour_str_error("openocd error."))
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

    print_f("")
    print_f("     Supported MicroPython PSOC EDGE boards    ")
    print_f("+---------+-----------------------------------+")
    print_f("|   ID    |              Board                |")
    print_f("+---------+-----------------------------------+")
    print_f("|   0     |  KIT_PSE84_AI                     |")
    print_f("+---------+-----------------------------------+")
    print_f("")
    print_f("")

    """
    Currently only one board is supported, so no need to select
    from user. The code is left here for future reference.

    board_index = validate_user_input(
        input(colour_str_highlight("Please type the desired board ID: "))
    )
    board = boards[board_index]["name"]
    """
    board = "KIT_PSE84_AI"
    print_f(colour_str_highlight("Auto-selecting the only supported board: KIT_PSE84_AI\n"))

    return board


def wait_and_request_board_connect():
    input(
        colour_str_highlight(
            "Please CONNECT THE BOARD and PRESS ENTER to start the firmware deployment\n"
        )
    )


def device_setup(board, version, serial_adapter_sn=None, quiet=False):
    if board is None:
        board = select_board()
    else:
        validate_board_name(board)

    print_f("MicroPython PSOC Edge Board   :: ", board)

    if version is None:
        version = "latest"

    print_f("MicroPython PSOC Edge Version :: ", version)

    if not quiet:
        wait_and_request_board_connect()

    openocd_download_install()
    openocd_board_conf_download(board)

    mpy_firmware_download("mpy-psoc-edge", board, version, silent=quiet)
    mpy_firmware_deploy("mpy-psoc-edge", board, serial_adapter_sn)

    print_f(colour_str_success("Device setup completed :)"))


def firmware_deploy(board, hex_file, serial_adapter_sn=None):
    openocd_download_install()
    openocd_board_conf_download(board)
    print(f"Deploying hex file {hex_file} ...")
    openocd_program(board, hex_file, serial_adapter_sn)
    print(colour_str_success(f"Firmware {hex_file} deployed successfully"))


def clean_tool_downloads():
    openocd_remove()


def parser():
    def main_parser_func(args):
        parser.print_help()

    def parser_device_setup(args):
        device_setup(args.board, args.version, args.serial_num, args.q)

    def parser_firmware_deploy(args):
        firmware_deploy(args.board, args.hexfile, args.serial_num)

    # Main parser
    class ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            return super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string, **kwargs):
            print_f("mpy-pse version: " + version)
            parser.exit()

    main_parser_desc = """
    Micropython PSOC Edge utility script

    Available commands:

    device-setup        Setup of MicroPython PSOC Edge device
    firmware-deploy     Firmware deployment on PSOC Edge device (user provided binary file)
                                                                  
    mpy-pse.py <command> --help for more information about each specific command.
    """
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description=main_parser_desc
    )
    parser.add_argument("-v", "--version", action=ver_action, help="mpy-pse version")
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # device setup
    parser_ds = subparser.add_parser(
        "device-setup",
        description="Setup of MicroPython PSOC Edge device. \
                    Use this command to install the deployment tools \
                    and MicroPython firmware binary, and deploy the \
                    firmware on the PSOC Edge device.",
    )
    parser_ds.add_argument(
        "-b", "--board", default=None, type=str, help="PSOC Edge prototyping kit name"
    )
    parser_ds.add_argument(
        "-n",
        "--serial-num",
        default=None,
        type=str,
        help="Serial number of the board serial adapter",
    )
    parser_ds.add_argument(
        "-v", "--version", default=None, type=str, help="MicroPython PSOC Edge firmware version"
    )
    parser_ds.add_argument(
        "-q", action="store_true", help="Quiet. Do not prompt any user confirmation request"
    )
    parser_ds.set_defaults(func=parser_device_setup)

    # firmware deploy
    parser_fd = subparser.add_parser(
        "firmware-deploy",
        description="Firmware deployment on MicroPython device. \
                    Use this command to deploy an existing .hex file \
                    on a PSOC Edge board.",
    )
    parser_fd.add_argument(
        "-b",
        "--board",
        default=None,
        type=str,
        required=True,
        help="PSOC Edge prototyping kit name",
    )
    parser_fd.add_argument(
        "-n",
        "--serial-num",
        default=None,
        type=str,
        help="Serial number of the board serial adapter",
    )
    parser_fd.add_argument(
        "-f", "--hexfile", type=str, required=True, help="MicroPython PSOC Edge firmware .hex file"
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
        print_f(colour_str_error("error: keyboard interrupt"))
        clean_tool_downloads()
