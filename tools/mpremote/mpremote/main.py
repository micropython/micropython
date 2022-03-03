"""
MicroPython Remote - Interaction and automation tool for MicroPython
MIT license; Copyright (c) 2019-2022 Damien P. George

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

import argparse
import os, sys, time
from collections.abc import Mapping
from textwrap import dedent

from .commands import (
    CommandError,
    do_connect,
    do_disconnect,
    do_edit,
    do_filesystem,
    do_mount,
    do_umount,
    do_exec,
    do_eval,
    do_run,
    do_resume,
    do_rtc,
    do_soft_reset,
    do_romfs,
)
from .mip import do_mip
from .repl import do_repl

_PROG = "mpremote"


def do_sleep(state, args):
    time.sleep(args.ms[0])


def do_help(state, _args=None):
    def print_commands_help(cmds, help_key):
        max_command_len = max(len(cmd) for cmd in cmds.keys())
        for cmd in sorted(cmds.keys()):
            help_message_lines = dedent(help_key(cmds[cmd])).split("\n")
            help_message = help_message_lines[0]
            for line in help_message_lines[1:]:
                help_message = "{}\n{}{}".format(help_message, " " * (max_command_len + 4), line)
            print("  ", cmd, " " * (max_command_len - len(cmd) + 2), help_message, sep="")

    print(_PROG, "-- MicroPython remote control")
    print("See https://docs.micropython.org/en/latest/reference/mpremote.html")

    print("\nList of commands:")
    print_commands_help(
        _COMMANDS, lambda x: x[1]().description
    )  # extract description from argparse

    print("\nList of shortcuts:")
    print_commands_help(_command_expansions, lambda x: x[2])  # (args, sub, help_message)

    sys.exit(0)


def do_version(state, _args=None):
    from . import __version__

    print(f"{_PROG} {__version__}")
    sys.exit(0)


def _bool_flag(cmd_parser, name, short_name, default, description):
    # In Python 3.9+ this can be replaced with argparse.BooleanOptionalAction.
    group = cmd_parser.add_mutually_exclusive_group()
    group.add_argument(
        "--" + name,
        "-" + short_name,
        action="store_true",
        default=default,
        help=description,
    )
    group.add_argument(
        "--no-" + name,
        action="store_false",
        dest=name,
    )


def argparse_connect():
    cmd_parser = argparse.ArgumentParser(description="connect to given device")
    cmd_parser.add_argument(
        "device", nargs=1, help="Either list, auto, id:x, port:x, or any valid device name/path"
    )
    return cmd_parser


def argparse_sleep():
    cmd_parser = argparse.ArgumentParser(description="sleep before executing next command")
    cmd_parser.add_argument("ms", nargs=1, type=float, help="milliseconds to sleep for")
    return cmd_parser


def argparse_edit():
    cmd_parser = argparse.ArgumentParser(description="edit files on the device")
    cmd_parser.add_argument("files", nargs="+", help="list of remote paths")
    return cmd_parser


def argparse_mount():
    cmd_parser = argparse.ArgumentParser(description="mount local directory on device")
    _bool_flag(
        cmd_parser,
        "unsafe-links",
        "l",
        False,
        "follow symbolic links pointing outside of local directory",
    )
    cmd_parser.add_argument("path", nargs=1, help="local path to mount")
    return cmd_parser


def argparse_repl():
    cmd_parser = argparse.ArgumentParser(description="connect to given device")
    _bool_flag(cmd_parser, "escape-non-printable", "e", False, "escape non-printable characters")
    cmd_parser.add_argument(
        "--capture",
        type=str,
        required=False,
        help="saves a copy of the REPL session to the specified path",
    )
    cmd_parser.add_argument(
        "--inject-code", type=str, required=False, help="code to be run when Ctrl-J is pressed"
    )
    cmd_parser.add_argument(
        "--inject-file",
        type=str,
        required=False,
        help="path to file to be run when Ctrl-K is pressed",
    )
    return cmd_parser


def argparse_eval():
    cmd_parser = argparse.ArgumentParser(description="evaluate and print the string")
    cmd_parser.add_argument("expr", nargs=1, help="expression to execute")
    return cmd_parser


def argparse_exec():
    cmd_parser = argparse.ArgumentParser(description="execute the string")
    _bool_flag(
        cmd_parser, "follow", "f", True, "follow output until the expression completes (default)"
    )
    cmd_parser.add_argument("expr", nargs=1, help="expression to execute")
    return cmd_parser


def argparse_run():
    cmd_parser = argparse.ArgumentParser(description="run the given local script")
    _bool_flag(
        cmd_parser, "follow", "f", True, "follow output until the script completes (default)"
    )
    cmd_parser.add_argument("path", nargs=1, help="path to script to execute")
    return cmd_parser


def argparse_rtc():
    cmd_parser = argparse.ArgumentParser(description="get (default) or set the device RTC")
    _bool_flag(cmd_parser, "set", "s", False, "set the RTC to the current local time")
    return cmd_parser


def argparse_filesystem():
    cmd_parser = argparse.ArgumentParser(description="execute filesystem commands on the device")
    _bool_flag(cmd_parser, "recursive", "r", False, "recursive copy (for cp command only)")
    _bool_flag(
        cmd_parser,
        "force",
        "f",
        False,
        "force copy even if file is unchanged (for cp command only)",
    )
    _bool_flag(
        cmd_parser,
        "verbose",
        "v",
        None,
        "enable verbose output (defaults to True for all commands except cat)",
    )
    cmd_parser.add_argument(
        "command",
        nargs=1,
        help="filesystem command (e.g. cat, cp, sha256sum, ls, rm, rmdir, touch)",
    )
    cmd_parser.add_argument("path", nargs="+", help="local and remote paths")
    return cmd_parser


def argparse_mip():
    cmd_parser = argparse.ArgumentParser(
        description="install packages from micropython-lib or third-party sources"
    )
    _bool_flag(cmd_parser, "mpy", "m", True, "download as compiled .mpy files (default)")
    cmd_parser.add_argument(
        "--target", type=str, required=False, help="destination direction on the device"
    )
    cmd_parser.add_argument(
        "--index",
        type=str,
        required=False,
        help="package index to use (defaults to micropython-lib)",
    )
    cmd_parser.add_argument("command", nargs=1, help="mip command (e.g. install)")
    cmd_parser.add_argument(
        "packages",
        nargs="+",
        help="list package specifications, e.g. name, name@version, github:org/repo, github:org/repo@branch, gitlab:org/repo, gitlab:org/repo@branch",
    )
    return cmd_parser


def argparse_romfs():
    cmd_parser = argparse.ArgumentParser(description="manage ROM partitions")
    _bool_flag(
        cmd_parser,
        "mpy",
        "m",
        True,
        "automatically compile .py files to .mpy when building the ROMFS image (default)",
    )
    cmd_parser.add_argument(
        "--partition",
        "-p",
        type=int,
        default=0,
        help="ROMFS partition to use",
    )
    cmd_parser.add_argument(
        "--output",
        "-o",
        help="output file",
    )
    cmd_parser.add_argument("command", nargs=1, help="romfs command, one of: query, build, deploy")
    cmd_parser.add_argument("path", nargs="?", help="path to directory to deploy")
    return cmd_parser


def argparse_none(description):
    return lambda: argparse.ArgumentParser(description=description)


# Map of "command" to tuple of (handler_func, argparse_func).
_COMMANDS = {
    "connect": (
        do_connect,
        argparse_connect,
    ),
    "sleep": (
        do_sleep,
        argparse_sleep,
    ),
    "disconnect": (
        do_disconnect,
        argparse_none("disconnect current device"),
    ),
    "edit": (
        do_edit,
        argparse_edit,
    ),
    "resume": (
        do_resume,
        argparse_none("resume a previous mpremote session (will not auto soft-reset)"),
    ),
    "soft-reset": (
        do_soft_reset,
        argparse_none("perform a soft-reset of the device"),
    ),
    "mount": (
        do_mount,
        argparse_mount,
    ),
    "umount": (
        do_umount,
        argparse_none("unmount the local directory"),
    ),
    "repl": (
        do_repl,
        argparse_repl,
    ),
    "eval": (
        do_eval,
        argparse_eval,
    ),
    "exec": (
        do_exec,
        argparse_exec,
    ),
    "run": (
        do_run,
        argparse_run,
    ),
    "rtc": (
        do_rtc,
        argparse_rtc,
    ),
    "fs": (
        do_filesystem,
        argparse_filesystem,
    ),
    "mip": (
        do_mip,
        argparse_mip,
    ),
    "help": (
        do_help,
        argparse_none("print help and exit"),
    ),
    "version": (
        do_version,
        argparse_none("print version and exit"),
    ),
    "romfs": (
        do_romfs,
        argparse_romfs,
    ),
}

# Additional commands aliases.
# The value can either be:
#   - A command string.
#   - A list of command strings, each command will be executed sequentially.
#   - A dict of command: { [], help: ""}
_BUILTIN_COMMAND_EXPANSIONS = {
    # Device connection shortcuts.
    "devs": {
        "command": "connect list",
        "help": "list available serial ports",
    },
    # Filesystem shortcuts (use `cp` instead of `fs cp`).
    "cat": "fs cat",
    "cp": "fs cp",
    "ls": "fs ls",
    "mkdir": "fs mkdir",
    "rm": "fs rm",
    "rmdir": "fs rmdir",
    "sha256sum": "fs sha256sum",
    "touch": "fs touch",
    # Disk used/free.
    "df": [
        "exec",
        "import os\nprint('mount \\tsize \\tused \\tavail \\tuse%')\nfor _m in [''] + os.listdir('/'):\n _s = os.stat('/' + _m)\n if not _s[0] & 1 << 14: continue\n _s = os.statvfs(_m)\n if _s[0]:\n  _size = _s[0] * _s[2]; _free = _s[0] * _s[3]; print(_m, _size, _size - _free, _free, int(100 * (_size - _free) / _size), sep='\\t')",
    ],
    # Other shortcuts.
    "reset": {
        "command": [
            "exec",
            "--no-follow",
            "import time, machine; time.sleep_ms(100); machine.reset()",
        ],
        "help": "hard reset the device",
    },
    "bootloader": {
        "command": [
            "exec",
            "--no-follow",
            "import time, machine; time.sleep_ms(100); machine.bootloader()",
        ],
        "help": "make the device enter its bootloader",
    },
    # Simple aliases.
    "--help": "help",
    "--version": "version",
}

# Add "a0", "a1", ..., "u0", "u1", ..., "c0", "c1", ... as aliases
# for "connect /dev/ttyACMn" (and /dev/ttyUSBn, COMn) etc.
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
            if args and args[0] == "+":
                break
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

    # Insert expansion with optional setting of arguments.
    if pre:
        args[0:0] = ["exec", ";".join(pre)]


class State:
    def __init__(self):
        self.transport = None
        self._did_action = False
        self._auto_soft_reset = True

    def did_action(self):
        self._did_action = True

    def run_repl_on_completion(self):
        return not self._did_action

    def ensure_connected(self):
        if self.transport is None:
            do_connect(self)

    def ensure_raw_repl(self, soft_reset=None):
        self.ensure_connected()
        soft_reset = self._auto_soft_reset if soft_reset is None else soft_reset
        if soft_reset or not self.transport.in_raw_repl:
            self.transport.enter_raw_repl(soft_reset=soft_reset)
            self._auto_soft_reset = False

    def ensure_friendly_repl(self):
        self.ensure_connected()
        if self.transport.in_raw_repl:
            self.transport.exit_raw_repl()


def main():
    config = load_user_config()
    prepare_command_expansions(config)

    remaining_args = sys.argv[1:]
    state = State()

    try:
        while remaining_args:
            # Skip the terminator.
            if remaining_args[0] == "+":
                remaining_args.pop(0)
                continue

            # Rewrite the front of the list with any matching expansion.
            do_command_expansion(remaining_args)

            # The (potentially rewritten) command must now be a base command.
            cmd = remaining_args.pop(0)
            try:
                handler_func, parser_func = _COMMANDS[cmd]
            except KeyError:
                raise CommandError(f"'{cmd}' is not a command")

            # If this command (or any down the chain) has a terminator, then
            # limit the arguments passed for this command. They will be added
            # back after processing this command.
            try:
                terminator = remaining_args.index("+")
                command_args = remaining_args[:terminator]
                extra_args = remaining_args[terminator:]
            except ValueError:
                command_args = remaining_args
                extra_args = []

            # Special case: "fs ls" allowed have no path specified.
            if cmd == "fs" and len(command_args) == 1 and command_args[0] == "ls":
                command_args.append("")

            # Use the command-specific argument parser.
            cmd_parser = parser_func()
            cmd_parser.prog = cmd
            # Catch all for unhandled positional arguments (this is the next command).
            cmd_parser.add_argument(
                "next_command", nargs=argparse.REMAINDER, help=f"Next {_PROG} command"
            )
            args = cmd_parser.parse_args(command_args)

            # Execute command.
            handler_func(state, args)

            # Get any leftover unprocessed args.
            remaining_args = args.next_command + extra_args

        # If no commands were "actions" then implicitly finish with the REPL
        # using default args.
        if state.run_repl_on_completion():
            do_repl(state, argparse_repl().parse_args([]))

        return 0
    except CommandError as e:
        # Make sure existing stdout appears before the error message on stderr.
        sys.stdout.flush()
        print(f"{_PROG}: {e}", file=sys.stderr)
        sys.stderr.flush()
        return 1
    finally:
        do_disconnect(state)
