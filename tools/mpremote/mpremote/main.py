"""
MicroPython Remote - Interaction and automation tool for MicroPython
MIT license; Copyright (c) 2019-2021 Damien P. George

This program provides a set of utilities to interact with and automate a
MicroPython device over a serial connection.  Commands supported are:

    mpremote                         -- auto-detect, connect and enter REPL
    mpremote <device-shortcut>       -- connect to given device
    mpremote connect <device>        -- connect to given device
    mpremote disconnect              -- disconnect current device
    mpremote mount <local-dir>       -- mount local directory on device
    mpremote eval <string>           -- evaluate and print the string
    mpremote exec <string>           -- execute the string
    mpremote run <script>            -- run the given local script
    mpremote fs <command> <args...>  -- execute filesystem commands on the device
    mpremote repl                    -- enter REPL
"""

import os, sys
from collections.abc import Mapping
from textwrap import dedent

import serial.tools.list_ports

from . import pyboardextended as pyboard
from .console import Console, ConsolePosix

_PROG = "mpremote"

_COMMANDS = {
    "connect": (
        False,
        False,
        1,
        """\
        connect to given device
        device may be: list, auto, id:x, port:x
        or any valid device name/path""",
    ),
    "disconnect": (False, False, 0, "disconnect current device"),
    "resume": (False, False, 0, "resume a previous mpremote session (will not auto soft-reset)"),
    "soft-reset": (False, True, 0, "perform a soft-reset of the device"),
    "mount": (
        True,
        False,
        1,
        """\
        mount local directory on device
        options:
            --unsafe-links, -l
                follow symbolic links pointing outside of local directory""",
    ),
    "umount": (True, False, 0, "unmount the local directory"),
    "repl": (
        False,
        True,
        0,
        """\
        enter REPL
        options:
            --capture <file>
            --inject-code <string>
            --inject-file <file>""",
    ),
    "eval": (True, True, 1, "evaluate and print the string"),
    "exec": (True, True, 1, "execute the string"),
    "run": (True, True, 1, "run the given local script"),
    "fs": (True, True, 1, "execute filesystem commands on the device"),
    "help": (False, False, 0, "print help and exit"),
}

_BUILTIN_COMMAND_EXPANSIONS = {
    # Device connection shortcuts.
    "devs": {
        "command": "connect list",
        "help": "list available serial ports",
    },
    # Filesystem shortcuts.
    "cat": "fs cat",
    "ls": "fs ls",
    "cp": "fs cp",
    "rm": "fs rm",
    "mkdir": "fs mkdir",
    "rmdir": "fs rmdir",
    "df": [
        "exec",
        "import uos\nprint('mount \\tsize \\tused \\tavail \\tuse%')\nfor _m in [''] + uos.listdir('/'):\n _s = uos.stat('/' + _m)\n if not _s[0] & 1 << 14: continue\n _s = uos.statvfs(_m)\n if _s[0]:\n  _size = _s[0] * _s[2]; _free = _s[0] * _s[3]; print(_m, _size, _size - _free, _free, int(100 * (_size - _free) / _size), sep='\\t')",
    ],
    # Other shortcuts.
    "reset t_ms=100": {
        "command": [
            "exec",
            "--no-follow",
            "import utime, machine; utime.sleep_ms(t_ms); machine.reset()",
        ],
        "help": "reset the device after delay",
    },
    "bootloader t_ms=100": {
        "command": [
            "exec",
            "--no-follow",
            "import utime, machine; utime.sleep_ms(t_ms); machine.bootloader()",
        ],
        "help": "make the device enter its bootloader",
    },
    "setrtc": [
        "exec",
        "import machine; machine.RTC().datetime((2020, 1, 1, 0, 10, 0, 0, 0))",
    ],
    "--help": "help",
}

for port_num in range(4):
    for prefix, port in [("a", "/dev/ttyACM"), ("u", "/dev/ttyUSB"), ("c", "COM")]:
        _BUILTIN_COMMAND_EXPANSIONS["{}{}".format(prefix, port_num)] = {
            "command": "connect {}{}".format(port, port_num),
            "help": 'connect to serial port "{}{}"'.format(port, port_num),
        }


def load_user_config():
    # Create empty config object.
    config = __build_class__(lambda: None, "Config")()
    config.commands = {}

    # Get config file name.
    path = os.getenv("XDG_CONFIG_HOME")
    if path is None:
        path = os.getenv("HOME")
        if path is None:
            return config
        path = os.path.join(path, ".config")
    path = os.path.join(path, _PROG)
    config_file = os.path.join(path, "config.py")

    # Check if config file exists.
    if not os.path.exists(config_file):
        return config

    # Exec the config file in its directory.
    with open(config_file) as f:
        config_data = f.read()
    prev_cwd = os.getcwd()
    os.chdir(path)
    exec(config_data, config.__dict__)
    os.chdir(prev_cwd)

    return config


def prepare_command_expansions(config):
    global _command_expansions

    _command_expansions = {}

    for command_set in (_BUILTIN_COMMAND_EXPANSIONS, config.commands):
        for cmd, sub in command_set.items():
            cmd = cmd.split()
            if len(cmd) == 1:
                args = ()
            else:
                args = tuple(c.split("=") for c in cmd[1:])

            help_message = ""
            if isinstance(sub, Mapping):
                help_message = sub.get("help", "")
                sub = sub["command"]
            if isinstance(sub, str):
                sub = sub.split()
            _command_expansions[cmd[0]] = (args, sub, help_message)


def do_command_expansion(args):
    def usage_error(cmd, exp_args, msg):
        print(f"Command {cmd} {msg}; signature is:")
        print("   ", cmd, " ".join("=".join(a) for a in exp_args))
        sys.exit(1)

    last_arg_idx = len(args)
    pre = []
    while args and args[0] in _command_expansions:
        cmd = args.pop(0)
        exp_args, exp_sub, _ = _command_expansions[cmd]
        for exp_arg in exp_args:
            exp_arg_name = exp_arg[0]
            if args and "=" not in args[0]:
                # Argument given without a name.
                value = args.pop(0)
            elif args and args[0].startswith(exp_arg_name + "="):
                # Argument given with correct name.
                value = args.pop(0).split("=", 1)[1]
            else:
                # No argument given, or argument given with a different name.
                if len(exp_arg) == 1:
                    # Required argument (it has no default).
                    usage_error(cmd, exp_args, f"missing argument {exp_arg_name}")
                else:
                    # Optional argument with a default.
                    value = exp_arg[1]
            pre.append(f"{exp_arg_name}={value}")

        args[0:0] = exp_sub
        last_arg_idx = len(exp_sub)

    if last_arg_idx < len(args) and "=" in args[last_arg_idx]:
        # Extra unknown arguments given.
        arg = args[last_arg_idx].split("=", 1)[0]
        usage_error(cmd, exp_args, f"given unexpected argument {arg}")
        sys.exit(1)

    # Insert expansion with optional setting of arguments.
    if pre:
        args[0:0] = ["exec", ";".join(pre)]


def do_connect(args):
    dev = args.pop(0)
    try:
        if dev == "list":
            # List attached devices.
            for p in sorted(serial.tools.list_ports.comports()):
                print(
                    "{} {} {:04x}:{:04x} {} {}".format(
                        p.device,
                        p.serial_number,
                        p.vid if isinstance(p.vid, int) else 0,
                        p.pid if isinstance(p.pid, int) else 0,
                        p.manufacturer,
                        p.product,
                    )
                )
            return None
        elif dev == "auto":
            # Auto-detect and auto-connect to the first available device.
            for p in sorted(serial.tools.list_ports.comports()):
                try:
                    return pyboard.PyboardExtended(p.device, baudrate=115200)
                except pyboard.PyboardError as er:
                    if not er.args[0].startswith("failed to access"):
                        raise er
            raise pyboard.PyboardError("no device found")
        elif dev.startswith("id:"):
            # Search for a device with the given serial number.
            serial_number = dev[len("id:") :]
            dev = None
            for p in serial.tools.list_ports.comports():
                if p.serial_number == serial_number:
                    return pyboard.PyboardExtended(p.device, baudrate=115200)
            raise pyboard.PyboardError("no device with serial number {}".format(serial_number))
        else:
            # Connect to the given device.
            if dev.startswith("port:"):
                dev = dev[len("port:") :]
            return pyboard.PyboardExtended(dev, baudrate=115200)
    except pyboard.PyboardError as er:
        msg = er.args[0]
        if msg.startswith("failed to access"):
            msg += " (it may be in use by another program)"
        print(msg)
        sys.exit(1)


def do_disconnect(pyb):
    try:
        if pyb.mounted:
            if not pyb.in_raw_repl:
                pyb.enter_raw_repl(soft_reset=False)
            pyb.umount_local()
        if pyb.in_raw_repl:
            pyb.exit_raw_repl()
    except OSError:
        # Ignore any OSError exceptions when shutting down, eg:
        # - pyboard.filesystem_command will close the connecton if it had an error
        # - umounting will fail if serial port disappeared
        pass
    pyb.close()


def show_progress_bar(size, total_size):
    if not sys.stdout.isatty():
        return
    verbose_size = 2048
    bar_length = 20
    if total_size < verbose_size:
        return
    elif size >= total_size:
        # Clear progress bar when copy completes
        print("\r" + " " * (20 + bar_length) + "\r", end="")
    else:
        progress = size / total_size
        bar = round(progress * bar_length)
        print(
            "\r ... copying {:3.0f}% [{}{}]".format(
                progress * 100, "#" * bar, "-" * (bar_length - bar)
            ),
            end="",
        )


def do_filesystem(pyb, args):
    def _list_recursive(files, path):
        if os.path.isdir(path):
            for entry in os.listdir(path):
                _list_recursive(files, "/".join((path, entry)))
        else:
            files.append(os.path.split(path))

    if args[0] == "cp" and args[1] == "-r":
        args.pop(0)
        args.pop(0)
        assert args[-1] == ":"
        args.pop()
        src_files = []
        for path in args:
            _list_recursive(src_files, path)
        known_dirs = {""}
        pyb.exec_("import uos")
        for dir, file in src_files:
            dir_parts = dir.split("/")
            for i in range(len(dir_parts)):
                d = "/".join(dir_parts[: i + 1])
                if d not in known_dirs:
                    pyb.exec_("try:\n uos.mkdir('%s')\nexcept OSError as e:\n print(e)" % d)
                    known_dirs.add(d)
            pyboard.filesystem_command(
                pyb,
                ["cp", "/".join((dir, file)), ":" + dir + "/"],
                progress_callback=show_progress_bar,
            )
    else:
        pyboard.filesystem_command(pyb, args, progress_callback=show_progress_bar)
    args.clear()


def do_repl_main_loop(pyb, console_in, console_out_write, *, code_to_inject, file_to_inject):
    while True:
        console_in.waitchar(pyb.serial)
        c = console_in.readchar()
        if c:
            if c == b"\x1d":  # ctrl-], quit
                break
            elif c == b"\x04":  # ctrl-D
                # special handling needed for ctrl-D if filesystem is mounted
                pyb.write_ctrl_d(console_out_write)
            elif c == b"\x0a" and code_to_inject is not None:  # ctrl-j, inject code
                pyb.serial.write(code_to_inject)
            elif c == b"\x0b" and file_to_inject is not None:  # ctrl-k, inject script
                console_out_write(bytes("Injecting %s\r\n" % file_to_inject, "utf8"))
                pyb.enter_raw_repl(soft_reset=False)
                with open(file_to_inject, "rb") as f:
                    pyfile = f.read()
                try:
                    pyb.exec_raw_no_follow(pyfile)
                except pyboard.PyboardError as er:
                    console_out_write(b"Error:\r\n")
                    console_out_write(er)
                pyb.exit_raw_repl()
            else:
                pyb.serial.write(c)

        try:
            n = pyb.serial.inWaiting()
        except OSError as er:
            if er.args[0] == 5:  # IO error, device disappeared
                print("device disconnected")
                break

        if n > 0:
            c = pyb.serial.read(1)
            if c is not None:
                # pass character through to the console
                oc = ord(c)
                if oc in (8, 9, 10, 13, 27) or 32 <= oc <= 126:
                    console_out_write(c)
                else:
                    console_out_write(b"[%02x]" % ord(c))


def do_repl(pyb, args):
    capture_file = None
    code_to_inject = None
    file_to_inject = None

    while len(args):
        if args[0] == "--capture":
            args.pop(0)
            capture_file = args.pop(0)
        elif args[0] == "--inject-code":
            args.pop(0)
            code_to_inject = bytes(args.pop(0).replace("\\n", "\r\n"), "utf8")
        elif args[0] == "--inject-file":
            args.pop(0)
            file_to_inject = args.pop(0)
        else:
            break

    print("Connected to MicroPython at %s" % pyb.device_name)
    print("Use Ctrl-] to exit this shell")
    if capture_file is not None:
        print('Capturing session to file "%s"' % capture_file)
        capture_file = open(capture_file, "wb")
    if code_to_inject is not None:
        print("Use Ctrl-J to inject", code_to_inject)
    if file_to_inject is not None:
        print('Use Ctrl-K to inject file "%s"' % file_to_inject)

    console = Console()
    console.enter()

    def console_out_write(b):
        console.write(b)
        if capture_file is not None:
            capture_file.write(b)
            capture_file.flush()

    try:
        do_repl_main_loop(
            pyb,
            console,
            console_out_write,
            code_to_inject=code_to_inject,
            file_to_inject=file_to_inject,
        )
    finally:
        console.exit()
        if capture_file is not None:
            capture_file.close()


def execbuffer(pyb, buf, follow):
    ret_val = 0
    try:
        pyb.exec_raw_no_follow(buf)
        if follow:
            ret, ret_err = pyb.follow(timeout=None, data_consumer=pyboard.stdout_write_bytes)
            if ret_err:
                pyboard.stdout_write_bytes(ret_err)
                ret_val = 1
    except pyboard.PyboardError as er:
        print(er)
        ret_val = 1
    except KeyboardInterrupt:
        ret_val = 1
    return ret_val


def print_help():
    def print_commands_help(cmds, help_idx):
        max_command_len = max(len(cmd) for cmd in cmds.keys())
        for cmd in sorted(cmds.keys()):
            help_message_lines = dedent(cmds[cmd][help_idx]).split("\n")
            help_message = help_message_lines[0]
            for line in help_message_lines[1:]:
                help_message = "{}\n{}{}".format(help_message, " " * (max_command_len + 4), line)
            print("  ", cmd, " " * (max_command_len - len(cmd) + 2), help_message, sep="")

    print(_PROG, "-- MicroPython remote control")
    print("See https://docs.micropython.org/en/latest/reference/mpremote.html")

    print("\nList of commands:")
    print_commands_help(_COMMANDS, 3)

    print("\nList of shortcuts:")
    print_commands_help(_command_expansions, 2)


def main():
    config = load_user_config()
    prepare_command_expansions(config)

    args = sys.argv[1:]
    pyb = None
    auto_soft_reset = True
    did_action = False

    try:
        while args:
            do_command_expansion(args)
            cmd = args.pop(0)
            try:
                need_raw_repl, is_action, num_args_min, _ = _COMMANDS[cmd]
            except KeyError:
                print(f"{_PROG}: '{cmd}' is not a command")
                return 1

            if len(args) < num_args_min:
                print(f"{_PROG}: '{cmd}' neads at least {num_args_min} argument(s)")
                return 1

            if cmd == "connect":
                if pyb is not None:
                    do_disconnect(pyb)
                pyb = do_connect(args)
                if pyb is None:
                    did_action = True
                continue
            elif cmd == "help":
                print_help()
                sys.exit(0)
            elif cmd == "resume":
                auto_soft_reset = False
                continue

            # The following commands need a connection, and either a raw or friendly REPL.

            if pyb is None:
                pyb = do_connect(["auto"])

            if need_raw_repl:
                if not pyb.in_raw_repl:
                    pyb.enter_raw_repl(soft_reset=auto_soft_reset)
                    auto_soft_reset = False
            else:
                if pyb.in_raw_repl:
                    pyb.exit_raw_repl()
            if is_action:
                did_action = True

            if cmd == "disconnect":
                do_disconnect(pyb)
                pyb = None
                auto_soft_reset = True
            elif cmd == "soft-reset":
                pyb.enter_raw_repl(soft_reset=True)
                auto_soft_reset = False
            elif cmd == "mount":
                unsafe_links = False
                if args[0] == "--unsafe-links" or args[0] == "-l":
                    args.pop(0)
                    unsafe_links = True
                path = args.pop(0)
                pyb.mount_local(path, unsafe_links=unsafe_links)
                print(f"Local directory {path} is mounted at /remote")
            elif cmd == "umount":
                pyb.umount_local()
            elif cmd in ("exec", "eval", "run"):
                follow = True
                if args[0] == "--no-follow":
                    args.pop(0)
                    follow = False
                if cmd == "exec":
                    buf = args.pop(0)
                elif cmd == "eval":
                    buf = "print(" + args.pop(0) + ")"
                else:
                    filename = args.pop(0)
                    try:
                        with open(filename, "rb") as f:
                            buf = f.read()
                    except OSError:
                        print(f"{_PROG}: could not read file '{filename}'")
                        return 1
                ret = execbuffer(pyb, buf, follow)
                if ret:
                    return ret
            elif cmd == "fs":
                do_filesystem(pyb, args)
            elif cmd == "repl":
                do_repl(pyb, args)

        if not did_action:
            if pyb is None:
                pyb = do_connect(["auto"])
            if pyb.in_raw_repl:
                pyb.exit_raw_repl()
            do_repl(pyb, args)
    finally:
        if pyb is not None:
            do_disconnect(pyb)
