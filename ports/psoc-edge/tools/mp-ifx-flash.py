import argparse, os, sys, shlex, shutil, subprocess, requests, tempfile, zipfile, tarfile

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


#######################################################################
# OpenOCD download, install, flash and cleanup functions


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
            # Add openocd to path
            os.environ["PATH"] = os.path.join("openocd", "bin") + os.pathsep + os.environ["PATH"]
            valid_openocd_version = is_openocd_already_in_sys_path()
            if not valid_openocd_version:
                clean_openocd()

            return valid_openocd_version

        return False

    def is_openocd_already_in_sys_path():
        try:
            ocd_proc = subprocess.Popen(
                [openocd_exe, "--version"], stderr=subprocess.PIPE, stdout=subprocess.PIPE
            )
            out, err = ocd_proc.communicate(timeout=10)

            if err:
                return False
            out_str = out.decode()
            supported_versions = [
                "0.12.0+dev-5.8.0.3960",
                "0.12.0+dev-5.11.0.4042",
                "0.12.0+dev-5.12.0.4170",
            ]
            for ver in supported_versions:
                if ver in out_str:
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
        os.environ["PATH"] = os.path.join("openocd", "bin") + os.pathsep + os.environ["PATH"]

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


def openocd_program(board, hex_file, cfg_dir, flm_file, serial_adapter_sn=None):
    # Find tool path cross-platform
    openocd = shutil.which("openocd")
    if not openocd:
        sys.exit(colour_str_error("error: openocd not found in PATH"))

    # Convert to forward slashes for cross-platform compatibility
    openocd = openocd.replace("\\", "/")

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
        + " -s "
        + str(cfg_dir)
        + ' -c "set QSPI_FLASHLOADER '
        + str(flm_file)
        + '"'
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


####################################################################################################


def validate_board_name(board_name):
    board_supported = False
    for brd in boards:
        if board_name == brd["name"]:
            board_supported = True
            break

    if not board_supported:
        sys.exit(colour_str_error("error: board is not supported"))


def flash_from_files(board, hex_file, cfg_file, flm_file, serial_adapter_sn):
    cfg_dir = os.path.dirname(cfg_file)
    print(f"Flashing board '{board}' with hex file '{os.path.basename(hex_file)}'...")
    openocd_program(board, hex_file, cfg_dir, flm_file, serial_adapter_sn)
    print(colour_str_success("Device setup completed :)"))


def flash_from_package(board, zip_package, serial_adapter_sn):
    def get_files_from_package(zip_package):
        hex_file = None
        cfg_file = None
        flm_file = None

        if not os.path.exists(zip_package):
            sys.exit(colour_str_error("error: zip package not found"))

        tmpdirname = tempfile.mkdtemp()
        with zipfile.ZipFile(zip_package, "r") as zip_ref:
            zip_ref.extractall(tmpdirname)
        for root, dirs, files in os.walk(tmpdirname):
            for file in files:
                if file.endswith(".hex"):
                    hex_file = os.path.join(root, file)
                elif file.endswith(".cfg"):
                    cfg_file = os.path.join(root, file)
                elif file.endswith(".FLM"):
                    flm_file = os.path.join(root, file)

        # If any of the files is missing, raise an error
        if not hex_file or not cfg_file or not flm_file:
            shutil.rmtree(tmpdirname)
            sys.exit(colour_str_error("error: zip package must contain .hex, .cfg and .FLM files"))

        return hex_file, cfg_file, flm_file, tmpdirname

    hex_file, cfg_file, flm_file, tmpdirname = get_files_from_package(zip_package)
    try:
        flash_from_files(board, hex_file, cfg_file, flm_file, serial_adapter_sn)
    finally:
        shutil.rmtree(tmpdirname)


def flash_command_args(args):
    # TODO: if no command (None) implement interactive mode to gather the values.
    validate_board_name(args.board)

    if args.command == "from-package":
        flash_from_package(args.board, args.zip_package, args.serial_num)


def parser():
    class ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            return super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string, **kwargs):
            print_f("mp-ifx-flash version: " + version)
            parser.exit()

    p = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        description="MicroPython Infineon Flash Utility",
    )
    p.add_argument("-v", "--version", action=ver_action, help="mp-ifx-flash version")

    subparsers = p.add_subparsers(dest="command")

    # Shared parent parser for common arguments (board, serial-num)
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument("-b", "--board", required=True, type=str, help="Infineon board name")
    common.add_argument(
        "-n",
        "--serial-num",
        default=None,
        type=str,
        help="Serial number of the board serial adapter",
    )

    # Subcommand: from-package
    sp_zip = subparsers.add_parser(
        "from-package",
        parents=[common],
        formatter_class=argparse.RawTextHelpFormatter,
        help="Flash from a compressed package",
        description="Flash from a compressed package containing .hex, .cfg and .FLM files.",
    )
    sp_zip.add_argument(
        "-z",
        "--zip-package",
        required=True,
        type=str,
        help="Firmware zip package containing .hex, .cfg and .FLM files",
    )

    # TODO: Subcommand: from-files (pass directly the .hex, .cfg and .FLM files)
    # TODO: Subcommand: from-remote (just pass the board and optionally the version, and the tool will fetch the latest firmware from Infineon fork and flash it)

    args = p.parse_args()

    # Print help if no command is provided
    # TODO: Future interactive mode.
    if args.command is None:
        p.print_help(sys.stderr)
        sys.exit(1)

    return args


if __name__ == "__main__":
    try:
        set_environment()
        args = parser()
        flash_command_args(args)
    except KeyboardInterrupt:
        print_f(colour_str_error("error: keyboard interrupt"))
        openocd_remove()
