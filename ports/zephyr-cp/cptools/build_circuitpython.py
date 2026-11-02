import asyncio
import logging
import os
import pathlib
import pickle
import sys

import board_tools
import colorlog
import cpbuild
import tomlkit
import tomllib
import yaml

logger = logging.getLogger(__name__)

# print("hello zephyr", sys.argv)

# print(os.environ)
cmake_args = {}
for var in sys.argv[1:]:
    key, value = var.split("=", 1)
    cmake_args[key] = value

# Path to ports/zephyr-cp
portdir = pathlib.Path(cmake_args["PORT_SRC_DIR"])

# Path to CP root
srcdir = portdir.parent.parent

# Path to where CMake wants to put our build output.
builddir = pathlib.Path.cwd()

zephyrdir = portdir / "zephyr"

# Path to where CMake puts Zephyr's build output.
zephyrbuilddir = builddir / ".." / ".." / ".." / "zephyr"

sys.path.append(str(portdir / "zephyr/scripts/dts/python-devicetree/src/"))
from zephyr2cp import zephyr_dts_to_cp_board

compiler = cpbuild.Compiler(srcdir, builddir, cmake_args)

ALWAYS_ON_MODULES = ["sys", "collections"]
# Keep in alphabetical order.
DEFAULT_MODULES = [
    "__future__",
    "adafruit_bus_device",
    "aesio",
    "array",
    "asyncio",
    "atexit",
    "binascii",
    "bitbangio",
    "codeop",
    "digitalio",
    "errno",
    "getpass",
    "hashlib",
    "io",
    "json",
    "keypad",
    "keypad_demux",
    "locale",
    "math",
    "microcontroller",
    "msgpack",
    "os",
    "rainbowio",
    "random",
    "re",
    "rotaryio",
    "select",
    "storage",
    "struct",
    "supervisor",
    "time",
    "touchio",
    "traceback",
    "warnings",
    "zlib",
]
# Flags that don't match with with a *bindings module. Some used by adafruit_requests
MPCONFIG_FLAGS = ["array", "errno", "io", "json", "math", "binascii"]

# extmod-based modules that should appear in the autogen list even though they
# don't have shared-bindings/ or bindings/ directories.
EXTMOD_MODULES = ["asyncio", "binascii", "json", "re", "select"]

# List of other modules (the value) that can be enabled when another one (the key) is.
REVERSE_DEPENDENCIES = {
    "audiobusio": ["audiocore"],
    "audiocore": [
        "audiodelays",
        "audiofilters",
        "audiofreeverb",
        "audiomixer",
        "audiomp3",
        "synthio",
    ],
    "busio": ["fourwire", "i2cdisplaybus", "sdcardio", "sharpdisplay"],
    "fourwire": ["displayio", "busdisplay", "epaperdisplay"],
    "i2cdisplaybus": ["displayio", "busdisplay", "epaperdisplay"],
    "neopixel_write": ["adafruit_pixelbuf", "_pixelmap"],
    # Zephyr display backends need displayio and, by extension, terminalio so
    # the REPL console appears on the display by default.
    "zephyr_display": ["displayio"],
    "displayio": [
        "vectorio",
        "bitmapfilter",
        "bitmaptools",
        "terminalio",
        "lvfontio",
        "tilepalettemapper",
        "fontio",
        "gifio",
        "jpegio",
    ],
    "sharpdisplay": ["framebufferio"],
    "framebufferio": ["displayio"],
}

# Other flags to set when a module is enabled
EXTRA_FLAGS = {
    "audiobusio": {"AUDIOBUSIO_I2SOUT": 1, "AUDIOBUSIO_PDMIN": 0},
    "busio": {"BUSIO_SPI": 1, "BUSIO_I2C": 1, "BUSIO_UART": 1},
    "keypad": {"KEYPAD_KEYMATRIX": 1, "KEYPAD_KEYS": 1, "KEYPAD_SHIFTREGISTERKEYS": 1},
    "rotaryio": {"ROTARYIO_SOFTENCODER": 1},
    "synthio": {"SYNTHIO_MAX_CHANNELS": 12},
    "touchio": {"TOUCHIO_USE_NATIVE": 0},
}

# Library sources. Will be globbed from the top level directory
# No QSTR processing or CIRCUITPY specific flags
LIBRARY_SOURCE = {
    "audiomp3": ["lib/mp3/src/*.c"],
    "zlib": [
        "lib/uzlib/tinflate.c",
        "lib/uzlib/tinfzlib.c",
        "lib/uzlib/tinfgzip.c",
        "lib/uzlib/adler32.c",
        "lib/uzlib/crc32.c",
    ],
    "jpegio": ["lib/tjpgd/src/tjpgd.c"],
    "gifio": ["lib/AnimatedGIF/gif.c"],
}

SHARED_MODULE_AND_COMMON_HAL = ["_bleio", "os", "rotaryio"]

# Mapping from module directory name to the flag name used in CIRCUITPY_<FLAG>
MODULE_FLAG_NAMES = {
    "__future__": "FUTURE",
    "_bleio": "BLEIO",
}


async def preprocess_and_split_defs(compiler, source_file, build_path, flags):
    build_file = source_file.with_suffix(".pp")
    build_file = build_path / (build_file.relative_to(srcdir))
    await compiler.preprocess(source_file, build_file, flags=flags)
    async with asyncio.TaskGroup() as tg:
        for mode in ("qstr", "module", "root_pointer"):
            split_file = build_file.relative_to(build_path).with_suffix(f".{mode}")
            split_file = build_path / "genhdr" / mode / split_file
            split_file.parent.mkdir(exist_ok=True, parents=True)
            tg.create_task(
                cpbuild.run_command(
                    [
                        "python",
                        srcdir / "py/makeqstrdefs.py",
                        "split",
                        mode,
                        build_file,
                        build_path / "genhdr" / mode,
                        split_file,
                    ],
                    srcdir,
                )
            )


def _concatenate_if_changed(inputs, output_file):
    """Write the concatenation of inputs to output_file, but only if it differs from what is
    already there, so an unchanged output keeps its modification time."""
    content = "".join(path.read_text() for path in inputs)
    if not output_file.exists() or output_file.read_text() != content:
        output_file.write_text(content)


async def collect_defs(mode, build_path):
    output_file = build_path / f"{mode}defs.collected"
    splitdir = build_path / "genhdr" / mode
    # Sorted so the output does not depend on directory order.
    to_collect = sorted(splitdir.glob(f"**/*.{mode}"))
    await cpbuild.run_function(
        _concatenate_if_changed,
        (to_collect, output_file),
        {},
        description=f"Collect {mode} definitions -> {output_file.relative_to(build_path)}",
    )
    return output_file


async def generate_qstr_headers(build_path, compiler, flags, translation):
    collected = await collect_defs("qstr", build_path)
    generated = build_path / "genhdr" / "qstrdefs.generated.h"

    # check_hash: the collected file is rewritten on every build, so this reruns every
    # build too. Keep the old modification time when the header comes out unchanged, or
    # every file that includes it would be recompiled.
    await cpbuild.run_command(
        ["python", srcdir / "py" / "makeqstrdata.py", collected, ">", generated],
        srcdir,
        check_hash=[generated],
    )

    compression_level = 9

    # TODO: Do this alongside qstr stuff above.
    await cpbuild.run_command(
        [
            "python",
            srcdir / "tools" / "msgfmt.py",
            "-o",
            build_path / f"{translation}.mo",
            srcdir / "locale" / f"{translation}.po",
        ],
        srcdir,
    )

    await cpbuild.run_command(
        [
            "python",
            srcdir / "py" / "maketranslationdata.py",
            "--compression_filename",
            build_path / "genhdr" / "compressed_translations.generated.h",
            "--translation",
            build_path / f"{translation}.mo",
            "--translation_filename",
            build_path / f"translations-{translation}.c",
            "--qstrdefs_filename",
            generated,
            "--compression_level",
            compression_level,
            generated,
        ],
        srcdir,
    )


async def generate_module_header(build_path):
    collected = await collect_defs("module", build_path)
    generated = build_path / "genhdr" / "moduledefs.h"
    await cpbuild.run_command(
        ["python", srcdir / "py" / "makemoduledefs.py", collected, ">", generated],
        srcdir,
        check_hash=[generated],
    )


async def generate_root_pointer_header(build_path):
    collected = await collect_defs("root_pointer", build_path)
    generated = build_path / "genhdr" / "root_pointers.h"
    await cpbuild.run_command(
        ["python", srcdir / "py" / "make_root_pointers.py", collected, ">", generated],
        srcdir,
        check_hash=[generated],
    )


async def generate_display_resources(output_path, translation, font, extra_characters):
    await cpbuild.run_command(
        [
            "python",
            srcdir / "tools" / "gen_display_resources.py",
            "--font",
            srcdir / font,
            "--sample_file",
            srcdir / "locale" / f"{translation}.po",
            "--extra_characters",
            repr(extra_characters),
            "--output_c_file",
            output_path,
        ],
        srcdir,
        check_hash=[output_path],
    )


async def generate_web_workflow_static(output_path, static_files):
    await cpbuild.run_command(
        [
            "python",
            srcdir / "tools" / "gen_web_workflow_static.py",
            "--output_c_file",
            output_path,
            *static_files,
        ],
        srcdir,
        check_hash=[output_path],
    )


def determine_enabled_modules(board_info, portdir, srcdir):
    """Determine which CircuitPython modules should be enabled based on board capabilities.

    Args:
        board_info: Dictionary containing board hardware capabilities
        portdir: Path to the port directory (ports/zephyr-cp)
        srcdir: Path to the CircuitPython source root

    Returns:
        tuple: (enabled_modules set, module_reasons dict)
    """
    enabled_modules = set(DEFAULT_MODULES)
    module_reasons = {}

    # The board module is always built: shared-bindings/board/__init__.c and the
    # generated pin/board.c are compiled unconditionally in build_circuitpython().
    enabled_modules.add("board")
    module_reasons["board"] = "Always enabled"

    if board_info["wifi"]:
        enabled_modules.add("wifi")
        module_reasons["wifi"] = "Zephyr board has wifi"

    if board_info.get("ethernet", False):
        enabled_modules.add("mii")
        module_reasons["mii"] = "Zephyr board has ethernet"

    if board_info["flash_count"] > 0:
        enabled_modules.add("storage")
        module_reasons["storage"] = "Zephyr board has flash"

    if board_info.get("nvm_size", 0) > 0:
        enabled_modules.add("nvm")
        module_reasons["nvm"] = "Board has NVM partition"

    network_enabled = (
        board_info.get("wifi", False)
        or board_info.get("ethernet", False)
        or board_info.get("hostnetwork", False)
    )

    if network_enabled:
        enabled_modules.add("ipaddress")
        module_reasons["ipaddress"] = "Zephyr networking enabled"
        enabled_modules.add("socketpool")
        module_reasons["socketpool"] = "Zephyr networking enabled"
        enabled_modules.add("hashlib")
        module_reasons["hashlib"] = "Zephyr networking enabled"

    if board_info.get("wifi", False):
        enabled_modules.add("ssl")
        module_reasons["ssl"] = "Zephyr wifi enabled"
    elif board_info.get("ethernet", False):
        enabled_modules.add("ssl")
        module_reasons["ssl"] = "Zephyr ethernet enabled"
    elif board_info.get("hostnetwork", False):
        enabled_modules.add("ssl")
        module_reasons["ssl"] = "Zephyr hostnetwork enabled"

    # Iterate the shared-bindings directory in a stable (sorted) order. Several
    # modules can enable the same reverse dependency, and the "reason" comment
    # recorded in autogen_board_info.toml belongs to whichever module enabled it
    # first. Directory iteration order is filesystem dependent (it differs
    # between machines and CI runners), so an unsorted walk made the generated
    # comments - and therefore the committed toml files - change between builds.
    for port_module in sorted((portdir / "bindings").iterdir(), key=lambda x: x.name):
        if not board_info.get(port_module.name, False):
            continue
        enabled_modules.add(port_module.name)
        module_reasons[port_module.name] = f"Zephyr board has {port_module.name}"

    for shared_module in sorted((srcdir / "shared-bindings").iterdir(), key=lambda x: x.name):
        if not board_info.get(shared_module.name, False) or not shared_module.glob("*.c"):
            continue
        enabled_modules.add(shared_module.name)
        module_reasons[shared_module.name] = f"Zephyr board has {shared_module.name}"

        more_modules = []
        more_modules.extend(REVERSE_DEPENDENCIES.get(shared_module.name, []))
        while more_modules:
            reverse_dependency = more_modules.pop(0)
            if reverse_dependency in enabled_modules:
                continue
            logger.debug(f"Enabling {reverse_dependency} because {shared_module.name} is enabled")
            enabled_modules.add(reverse_dependency)
            more_modules.extend(REVERSE_DEPENDENCIES.get(reverse_dependency, []))
            module_reasons[reverse_dependency] = f"Zephyr board has {shared_module.name}"

    return enabled_modules, module_reasons


async def build_circuitpython():  # noqa: C901
    circuitpython_flags = ["-DCIRCUITPY"]
    port_flags = []
    enable_mpy_native = False
    full_build = True
    usb_host = False
    zephyr_board = cmake_args["BOARD"]
    board = cmake_args["BOARD_ALIAS"]
    if not board:
        board = zephyr_board
    # The environment wins, so the Makefile can switch languages without a reconfigure.
    translation = os.environ.get("TRANSLATION") or cmake_args["TRANSLATION"] or "en_US"
    for module in ALWAYS_ON_MODULES:
        circuitpython_flags.append(f"-DCIRCUITPY_{module.upper()}=1")
    lto = cmake_args.get("LTO", "n") == "y"
    circuitpython_flags.append(f"-DCIRCUITPY_ENABLE_MPY_NATIVE={1 if enable_mpy_native else 0}")
    circuitpython_flags.append(f"-DCIRCUITPY_FULL_BUILD={1 if full_build else 0}")
    # Zephyr does not reset all pins in bulk on soft reset, so never_reset is
    # compiled out and hardware is released by GC finalizers instead.
    circuitpython_flags.append("-DCIRCUITPY_BULK_RESET=0")
    circuitpython_flags.append("-DCIRCUITPY_OPT_LOAD_ATTR_FAST_PATH=1")
    circuitpython_flags.append(f"-DCIRCUITPY_OPT_MAP_LOOKUP_CACHE={1 if full_build else 0}")
    circuitpython_flags.append(f"-DCIRCUITPY_SETTINGS_TOML={1 if full_build else 0}")
    circuitpython_flags.append(f"-DMICROPY_PY_ASYNC_AWAIT={1 if full_build else 0}")
    circuitpython_flags.append(f"-DMICROPY_PY_ASYNCIO={1 if full_build else 0}")
    circuitpython_flags.append(f"-DMICROPY_PY_SELECT={1 if full_build else 0}")
    circuitpython_flags.append(f"-DMICROPY_PY_SELECT_SELECT={1 if full_build else 0}")
    circuitpython_flags.append("-DCIRCUITPY_STATUS_BAR=1")
    circuitpython_flags.append(f"-DCIRCUITPY_USB_HOST={1 if usb_host else 0}")
    circuitpython_flags.append(f"-DCIRCUITPY_BOARD_ID='\"{board}\"'")
    circuitpython_flags.append(f"-DCIRCUITPY_TRANSLATE_OBJECT={1 if lto else 0}")
    circuitpython_flags.append("-DINTERNAL_FLASH_FILESYSTEM")
    circuitpython_flags.append("-DLONGINT_IMPL_MPZ")
    circuitpython_flags.append("-DCIRCUITPY_SSL_MBEDTLS")
    circuitpython_flags.append("-DFFCONF_H='\"lib/oofatfs/ffconf.h\"'")
    circuitpython_flags.append(
        "-D_DEFAULT_SOURCE"
    )  # To get more from picolibc to match newlib such as M_PI
    circuitpython_flags.extend(("-I", srcdir))
    circuitpython_flags.extend(("-I", builddir))
    circuitpython_flags.extend(("-I", portdir))

    genhdr = builddir / "genhdr"
    genhdr.mkdir(exist_ok=True, parents=True)
    version_header = genhdr / "mpversion.h"
    mpconfigboard_fn = board_tools.find_mpconfigboard(portdir, board)
    mpconfigboard = {"USB_VID": 0x1209, "USB_PID": 0x000C, "USB_INTERFACE_NAME": "CircuitPython"}
    if mpconfigboard_fn is not None and mpconfigboard_fn.exists():
        with mpconfigboard_fn.open("rb") as f:
            mpconfigboard.update(tomllib.load(f))
    async with asyncio.TaskGroup() as tg:
        tg.create_task(
            cpbuild.run_command(
                [
                    "python",
                    srcdir / "py" / "makeversionhdr.py",
                    version_header,
                    "&&",
                    "touch",
                    version_header,
                ],
                srcdir,
                check_hash=[version_header],
            )
        )

        board_autogen_task = tg.create_task(
            zephyr_dts_to_cp_board(zephyr_board, portdir, builddir, zephyrbuilddir, mpconfigboard)
        )
    board_info = board_autogen_task.result()
    if mpconfigboard_fn is None:
        mpconfigboard_fn = (
            portdir / "boards" / board_info["vendor_id"] / board / "circuitpython.toml"
        )
        logging.warning(
            f"Could not find board config at: boards/{board_info['vendor_id']}/{board}"
        )

    autogen_board_info_fn = mpconfigboard_fn.parent / "autogen_board_info.toml"

    # Filesystem type is a compile-time choice made by the partition layout:
    # zephyr2cp reports littlefs when the devicetree has a littlefs_partition
    # node, otherwise the supervisor mounts FAT.
    filesystem_littlefs = board_info.get("littlefs", False)
    circuitpython_flags.append(
        f"-DCIRCUITPY_FILESYSTEM_LITTLEFS={1 if filesystem_littlefs else 0}"
    )
    if filesystem_littlefs:
        circuitpython_flags.append("-DMICROPY_VFS_LFS2=1")

    creator_id = mpconfigboard.get("CIRCUITPY_CREATOR_ID", mpconfigboard.get("USB_VID", 0x1209))
    creation_id = mpconfigboard.get("CIRCUITPY_CREATION_ID", mpconfigboard.get("USB_PID", 0x000C))
    circuitpython_flags.append(f"-DCIRCUITPY_CREATOR_ID=0x{creator_id:08x}")
    circuitpython_flags.append(f"-DCIRCUITPY_CREATION_ID=0x{creation_id:08x}")

    vendor = mpconfigboard.get("VENDOR", board_info["vendor"])
    name = mpconfigboard.get("NAME", board_info["name"])

    enabled_modules, module_reasons = determine_enabled_modules(board_info, portdir, srcdir)
    for m in mpconfigboard.get("DISABLED_MODULES", []):
        enabled_modules.discard(m)

    web_workflow_enabled = (
        board_info.get("wifi", False)
        or board_info.get("hostnetwork", False)
        or "mii" in enabled_modules
    )

    circuitpython_flags.extend(board_info["cflags"])
    circuitpython_flags.append(f"-DCIRCUITPY_WEB_WORKFLOW={1 if web_workflow_enabled else 0}")
    supervisor_source = [
        "main.c",
        "extmod/modjson.c",
        "extmod/modbinascii.c",
        "extmod/modre.c",
        "extmod/modasyncio.c",
        "extmod/modselect.c",
        "extmod/vfs_fat.c",
        "lib/tlsf/tlsf.c",
        portdir / "background.c",
        portdir / "supervisor/adaboot_info.c",
        portdir / "common-hal/microcontroller/__init__.c",
        portdir / "common-hal/microcontroller/Pin.c",
        portdir / "common-hal/microcontroller/Processor.c",
        portdir / "common-hal/os/__init__.c",
        "shared/readline/readline.c",
        "shared/runtime/buffer_helper.c",
        "shared/runtime/context_manager_helpers.c",
        "shared/runtime/gchelper_generic.c",
        "shared/runtime/gchelper_native.c",
        "shared/runtime/pyexec.c",
        "shared/runtime/interrupt_char.c",
        "shared/runtime/stdout_helpers.c",
        "shared/runtime/sys_stdio_mphal.c",
        "shared-bindings/board/__init__.c",
        "shared-bindings/supervisor/Runtime.c",
        "shared-bindings/microcontroller/Pin.c",
        "shared-bindings/util.c",
        "shared-module/board/__init__.c",
        "extmod/vfs_reader.c",
        "extmod/vfs_blockdev.c",
        "extmod/vfs_fat_file.c",
    ]
    top = srcdir
    supervisor_source = [pathlib.Path(p) for p in supervisor_source]
    supervisor_source.extend(board_info["source_files"])
    supervisor_source.extend(top.glob("supervisor/shared/*.c"))
    ble_workflow_enabled = "_bleio" in enabled_modules
    if ble_workflow_enabled:
        supervisor_source.append(top / "supervisor/shared/bluetooth/bluetooth.c")
        # BLE workflow = file transfer + serial services, matching other ports.
        supervisor_source.append(top / "supervisor/shared/bluetooth/file_transfer.c")
        supervisor_source.append(top / "supervisor/shared/bluetooth/serial.c")
    circuitpython_flags.append(f"-DCIRCUITPY_BLE_FILE_SERVICE={1 if ble_workflow_enabled else 0}")
    circuitpython_flags.append(
        f"-DCIRCUITPY_BLE_SERIAL_SERVICE={1 if ble_workflow_enabled else 0}"
    )
    supervisor_source.append(top / "supervisor/shared/translate/translate.c")
    if web_workflow_enabled:
        supervisor_source.extend(top.glob("supervisor/shared/web_workflow/*.c"))

    usb_ok = board_info.get("usb_device", False)
    if usb_ok and filesystem_littlefs:
        # USB MSC can only expose a FAT block image; a littlefs CIRCUITPY has
        # no MSC-compatible layout and USB MSC of it would corrupt the drive.
        # Rename the filesystem partition to fatfs_partition in the board's
        # Adaboot layout dtsi, or remove the USB device from the devicetree.
        raise SystemExit(
            f"{board}: littlefs CIRCUITPY filesystem is incompatible with USB. "
            "Rename the littlefs_partition node to fatfs_partition in the board's "
            "layout dtsi, or disable the USB device controller in the devicetree."
        )
    circuitpython_flags.append(f"-DCIRCUITPY_USB_DEVICE={1 if usb_ok else 0}")

    if usb_ok:
        enabled_modules.add("usb_cdc")
        enabled_modules.add("usb_hid")

        usb_num_endpoint_pairs = board_info.get("usb_num_endpoint_pairs", 4)
        circuitpython_flags.append(f"-DUSB_NUM_ENDPOINT_PAIRS={usb_num_endpoint_pairs}")
        circuitpython_flags.append("-DCIRCUITPY_USB_HID_ENABLED_DEFAULT=1")

        for macro in ("USB_PID", "USB_VID"):
            print(f"Setting {macro} to {mpconfigboard.get(macro)}")
            circuitpython_flags.append(f"-D{macro}=0x{mpconfigboard.get(macro):04x}")
        circuitpython_flags.append(
            f"-DUSB_INTERFACE_NAME='\"{mpconfigboard['USB_INTERFACE_NAME']}\"'"
        )
        for macro, limit, value in (
            ("USB_PRODUCT", 16, name),
            ("USB_MANUFACTURER", 8, vendor),
        ):
            circuitpython_flags.append(f"-D{macro}='\"{value}\"'")
            circuitpython_flags.append(f"-D{macro}_{limit}='\"{value[:limit]}\"'")

        circuitpython_flags.append("-DCIRCUITPY_USB_CDC_CONSOLE_ENABLED_DEFAULT=1")
        circuitpython_flags.append("-DCIRCUITPY_USB_CDC_DATA_ENABLED_DEFAULT=0")

        supervisor_source.extend(
            (portdir / "supervisor/usb.c", srcdir / "supervisor/shared/usb.c")
        )

    creator_id = mpconfigboard.get("CIRCUITPY_CREATOR_ID", mpconfigboard.get("USB_VID"))
    creation_id = mpconfigboard.get("CIRCUITPY_CREATION_ID", mpconfigboard.get("USB_PID"))
    if creator_id is not None:
        circuitpython_flags.append(f"-DCIRCUITPY_CREATOR_ID=0x{creator_id:08x}")
    if creation_id is not None:
        circuitpython_flags.append(f"-DCIRCUITPY_CREATION_ID=0x{creation_id:08x}")

    # Always use port serial. It'll switch between USB and UART automatically.
    circuitpython_flags.append("-DCIRCUITPY_PORT_SERIAL=1")

    if "hashlib" in enabled_modules:
        circuitpython_flags.append("-DCIRCUITPY_HASHLIB_MBEDTLS=1")
        if "ssl" not in enabled_modules:
            circuitpython_flags.append("-DCIRCUITPY_HASHLIB_MBEDTLS_ONLY=1")

    if "ssl" in enabled_modules or "hashlib" in enabled_modules:
        # TODO: Figure out how to get these paths from zephyr
        circuitpython_flags.append("-DMBEDTLS_CONFIG_FILE='\"config-mbedtls.h\"'")
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "mbedtls" / "include")
        )
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "mbedtls" / "configs")
        )
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "tf-psa-crypto" / "include")
        )
        circuitpython_flags.extend(
            (
                "-isystem",
                portdir
                / "modules"
                / "crypto"
                / "tf-psa-crypto"
                / "drivers"
                / "builtin"
                / "include",
            )
        )
        circuitpython_flags.extend(("-isystem", zephyrdir / "modules" / "mbedtls" / "configs"))
        if "ssl" in enabled_modules:
            supervisor_source.append(top / "lib" / "mbedtls_config" / "crt_bundle.c")

    # Make sure all modules have a setting by filling in defaults.
    hal_source = []
    library_sources = []
    autogen_board_info = tomlkit.document()
    autogen_board_info.add(
        tomlkit.comment(
            "This file is autogenerated when a board is built. Do not edit. Do commit it to git. Other scripts use its info."
        )
    )
    autogen_board_info.add("name", vendor + " " + name)
    autogen_modules = tomlkit.table()
    autogen_board_info.add("modules", autogen_modules)
    for module in sorted(
        list(top.glob("shared-bindings/*")) + list(portdir.glob("bindings/*")),
        key=lambda x: x.name,
    ):
        # Skip files and directories without C source files (like artifacts from a docs build)
        if not module.is_dir() or len(list(module.glob("*.c"))) == 0:
            continue
        enabled = module.name in enabled_modules
        # print(f"Module {module.name} enabled: {enabled}")
        v = tomlkit.item(enabled)
        if module.name in module_reasons:
            v.comment(module_reasons[module.name])
        autogen_modules.add(module.name, v)
        flag_name = MODULE_FLAG_NAMES.get(module.name, module.name.upper())
        circuitpython_flags.append(f"-DCIRCUITPY_{flag_name}={1 if enabled else 0}")

        if enabled:
            if module.name in EXTRA_FLAGS:
                for flag, value in EXTRA_FLAGS[module.name].items():
                    circuitpython_flags.append(f"-DCIRCUITPY_{flag}={value}")

        if enabled:
            hal_source.extend(portdir.glob(f"bindings/{module.name}/*.c"))
            len_before = len(hal_source)
            hal_source.extend(top.glob(f"ports/zephyr-cp/common-hal/{module.name}/*.c"))
            # Only include shared-module/*.c if no common-hal/*.c files were found
            if len(hal_source) == len_before or module.name in SHARED_MODULE_AND_COMMON_HAL:
                hal_source.extend(top.glob(f"shared-module/{module.name}/**/*.c"))
            # The USB HID report descriptors are shared with the TinyUSB ports and
            # have no common-hal override, so always compile them.
            if module.name == "usb_hid":
                hal_source.append(top / "shared-module/usb_hid/report_descriptors.c")
            # busio.SPI is built on common-hal/async_spi, with or without the async_spi module.
            if module.name == "busio":
                hal_source.append(top / "shared-module/busio/SPI.c")
                hal_source.append(top / "ports/zephyr-cp/common-hal/async_spi/SPI.c")
            hal_source.extend(top.glob(f"shared-bindings/{module.name}/**/*.c"))
            if module.name in LIBRARY_SOURCE:
                for library_source in LIBRARY_SOURCE[module.name]:
                    library_sources.extend(top.glob(library_source))

    autogen_modules.add(tomlkit.comment("extmod modules shared with MicroPython"))
    for extmod_module in EXTMOD_MODULES:
        enabled = extmod_module in enabled_modules
        v = tomlkit.item(enabled)
        if extmod_module in module_reasons:
            v.comment(module_reasons[extmod_module])
        autogen_modules.add(extmod_module, v)
        flag_name = MODULE_FLAG_NAMES.get(extmod_module, extmod_module.upper())
        circuitpython_flags.append(f"-DCIRCUITPY_{flag_name}={1 if enabled else 0}")

    if os.environ.get("CI", "false") == "true":
        # Warn if it isn't up to date.
        if (
            not autogen_board_info_fn.exists()
            or autogen_board_info_fn.read_text() != tomlkit.dumps(autogen_board_info)
        ):
            logger.warning(
                f"autogen_board_info.toml is missing or out of date. Please run `make BOARD={board}` locally and commit {autogen_board_info_fn}."
            )
            # Also as an annotation, so it shows on the run and next to the file in the
            # pull request rather than in one board log out of 29. The board builds once
            # per language, so only the first build of it says anything.
            reported = builddir / "autogen_board_info.reported"
            if not reported.exists():
                reported.touch()
                print(
                    f"::warning file={autogen_board_info_fn.relative_to(srcdir)}::"
                    f"out of date, run `make BOARD={board}` and commit it",
                    flush=True,
                )

    if autogen_board_info_fn.parent.exists():
        autogen_board_info_fn.write_text(tomlkit.dumps(autogen_board_info))

    # Set NVM size from the board's NVM partition.
    nvm_size = board_info.get("nvm_size", 0)
    circuitpython_flags.append(f"-DCIRCUITPY_INTERNAL_NVM_SIZE={nvm_size}")
    if nvm_size > 0:
        circuitpython_flags.append("-DCIRCUITPY_INTERNAL_NVM_START_ADDR=0")

    for mpflag in MPCONFIG_FLAGS:
        enabled = mpflag in DEFAULT_MODULES
        circuitpython_flags.append(f"-DCIRCUITPY_{mpflag.upper()}={1 if enabled else 0}")

    # ulab is on by default and boards that cannot spare the flash set
    # CIRCUITPY_ULAB = false in their circuitpython.toml. Flags mirror py/py.mk.
    ulab_enabled = mpconfigboard.get("CIRCUITPY_ULAB", True)
    circuitpython_flags.append(f"-DCIRCUITPY_ULAB={1 if ulab_enabled else 0}")
    if ulab_enabled:
        circuitpython_flags.extend(
            (
                "-DMODULE_ULAB_ENABLED=1",
                "-DULAB_HAS_USER_MODULE=0",
                "-iquote",
                str(top / "extmod" / "ulab" / "code"),
            )
        )
    # radio.ping() builds on Zephyr's ICMP API. On by default; boards that cannot
    # spare the flash set CIRCUITPY_WIFI_PING = false in their circuitpython.toml
    # and radio.ping() then reports no reply, as it does for an unreachable host.
    circuitpython_flags.append(
        f"-DCIRCUITPY_WIFI_PING={1 if mpconfigboard.get('CIRCUITPY_WIFI_PING', True) else 0}"
    )

    # The busio and async_spi modules both name common-hal/async_spi/SPI.c.
    hal_source = list(dict.fromkeys(hal_source))
    source_files = supervisor_source + hal_source + ["extmod/vfs.c"]
    if filesystem_littlefs:
        source_files.extend(
            (
                "extmod/vfs_lfs.c",
                "lib/littlefs/lfs2.c",
                "lib/littlefs/lfs2_util.c",
            )
        )
        circuitpython_flags.extend(
            (
                "-DLFS2_NO_MALLOC",
                "-DLFS2_NO_DEBUG",
                "-DLFS2_NO_WARN",
                "-DLFS2_NO_ERROR",
                "-DLFS2_NO_ASSERT",
            )
        )
    if ulab_enabled:
        source_files.extend(sorted((top / "extmod" / "ulab" / "code").rglob("*.c")))
    assembly_files = []
    for file in top.glob("py/*.c"):
        source_files.append(file)
    qstr_flags = "-DNO_QSTR"
    async with asyncio.TaskGroup() as tg:
        for source_file in source_files:
            tg.create_task(
                preprocess_and_split_defs(
                    compiler,
                    top / source_file,
                    builddir,
                    [qstr_flags, *circuitpython_flags, *port_flags],
                )
            )

        if "ssl" in enabled_modules:
            crt_bundle = builddir / "x509_crt_bundle.S"
            roots_pem = srcdir / "lib/certificates/data/roots-full.pem"
            generator = srcdir / "tools/gen_crt_bundle.py"
            tg.create_task(
                cpbuild.run_command(
                    [
                        "python",
                        generator,
                        "-i",
                        roots_pem,
                        "-o",
                        crt_bundle,
                        "--asm",
                    ],
                    srcdir,
                )
            )
            assembly_files.append(crt_bundle)

    async with asyncio.TaskGroup() as tg:
        board_build = builddir
        tg.create_task(
            generate_qstr_headers(
                board_build, compiler, [qstr_flags, *circuitpython_flags, *port_flags], translation
            )
        )
        tg.create_task(generate_module_header(board_build))
        tg.create_task(generate_root_pointer_header(board_build))

        if "terminalio" in enabled_modules:
            output_path = board_build / f"autogen_display_resources-{translation}.c"
            font_path = srcdir / mpconfigboard.get(
                "CIRCUITPY_DISPLAY_FONT", "tools/fonts/ter-u12n.bdf"
            )
            extra_characters = mpconfigboard.get("CIRCUITPY_FONT_EXTRA_CHARACTERS", "")
            tg.create_task(
                generate_display_resources(output_path, translation, font_path, extra_characters)
            )
            source_files.append(output_path)

        if web_workflow_enabled:
            output_path = board_build / "autogen_web_workflow_static.c"
            static_files = sorted((srcdir / "supervisor/shared/web_workflow/static").glob("*"))
            tg.create_task(generate_web_workflow_static(output_path, static_files))
            source_files.append(output_path)

    # This file is generated by the QSTR/translation process.
    source_files.append(builddir / f"translations-{translation}.c")
    # These files don't include unique QSTRs. They just need to be compiled.
    source_files.append(portdir / "supervisor" / "flash.c")
    source_files.append(portdir / "supervisor" / "port.c")
    source_files.append(portdir / "supervisor" / "serial.c")
    source_files.append(srcdir / "lib" / "oofatfs" / "ff.c")
    source_files.append(srcdir / "lib" / "oofatfs" / "ffunicode.c")
    source_files.append(srcdir / "extmod" / "vfs_fat_diskio.c")
    source_files.append(srcdir / "shared/timeutils/timeutils.c")
    source_files.append(srcdir / "shared-module/time/__init__.c")
    source_files.append(srcdir / "shared-module/os/__init__.c")
    source_files.append(srcdir / "shared-module/supervisor/__init__.c")
    source_files.append(portdir / "bindings/zephyr_kernel/__init__.c")
    source_files.append(portdir / "common-hal/zephyr_kernel/__init__.c")
    # source_files.append(srcdir / "ports" / port / "peripherals" / "nrf" / "nrf52840" / "pins.c")

    source_files.extend(assembly_files)

    objects = []
    async with asyncio.TaskGroup() as tg:
        for source_file in library_sources:
            source_file = top / source_file
            build_file = source_file.with_suffix(".o")
            object_file = builddir / (build_file.relative_to(top))
            objects.append(object_file)
            tg.create_task(compiler.compile(source_file, object_file))
        for source_file in source_files:
            source_file = top / source_file
            build_file = source_file.with_suffix(".o")
            object_file = builddir / (build_file.relative_to(top))
            objects.append(object_file)
            tg.create_task(
                compiler.compile(source_file, object_file, [*circuitpython_flags, *port_flags])
            )

    await compiler.archive(objects, pathlib.Path(cmake_args["OUTPUT_FILE"]))


async def main():
    try:
        await build_circuitpython()
    except* RuntimeError as e:
        logger.error(e)
        sys.exit(len(e.exceptions))


handler = colorlog.StreamHandler()
handler.setFormatter(colorlog.ColoredFormatter("%(log_color)s%(levelname)s:%(name)s:%(message)s"))

logging.basicConfig(level=logging.INFO, handlers=[handler])

asyncio.run(main())
