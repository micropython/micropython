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

import os, sys
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
    do_soft_reset,
)
from .repl import do_repl

_PROG = "mpremote"


def do_help(state, _args=None):
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
    print_commands_help(_COMMANDS, 1)

    print("\nList of shortcuts:")
    print_commands_help(_command_expansions, 2)

    sys.exit(0)


def do_version(state, _args=None):
    from . import __version__

    print(f"{_PROG} {__version__}")
    sys.exit(0)


# Map of "command" to tuple of (num_args_min, help_text, handler).
_COMMANDS = {
    "connect": (
        1,
        """\
        connect to given device
        device may be: list, auto, id:x, port:x
        or any valid device name/path""",
        do_connect,
    ),
    "disconnect": (
        0,
        "disconnect current device",
        do_disconnect,
    ),
    "edit": (
        1,
        "edit files on the device",
        do_edit,
    ),
    "resume": (
        0,
        "resume a previous mpremote session (will not auto soft-reset)",
        do_resume,
    ),
    "soft-reset": (
        0,
        "perform a soft-reset of the device",
        do_soft_reset,
    ),
    "mount": (
        1,
        """\
        mount local directory on device
        options:
            --unsafe-links, -l
                follow symbolic links pointing outside of local directory""",
        do_mount,
    ),
    "umount": (
        0,
        "unmount the local directory",
        do_umount,
    ),
    "repl": (
        0,
        """\
        enter REPL
        options:
            --capture <file>
            --inject-code <string>
            --inject-file <file>""",
        do_repl,
    ),
    "eval": (
        1,
        "evaluate and print the string",
        do_eval,
    ),
    "exec": (
        1,
        "execute the string",
        do_exec,
    ),
    "run": (
        1,
        "run the given local script",
        do_run,
    ),
    "fs": (
        1,
        "execute filesystem commands on the device",
        do_filesystem,
    ),
    "help": (
        0,
        "print help and exit",
        do_help,
    ),
    "version": (
        0,
        "print version and exit",
        do_version,
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
    # Filesystem shortcuts.
    "cat": "fs cat",
    "ls": "fs ls",
    "cp": "fs cp",
    "rm": "fs rm",
    "touch": "fs touch",
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


class State:
    def __init__(self):
        self.pyb = None
        self._did_action = False
        self._auto_soft_reset = True

    def did_action(self):
        self._did_action = True

    def run_repl_on_completion(self):
        return not self._did_action

    def ensure_connected(self):
        if self.pyb is None:
            do_connect(self, ["auto"])

    def ensure_raw_repl(self, soft_reset=None):
        self.ensure_connected()
        soft_reset = self._auto_soft_reset if soft_reset is None else soft_reset
        if soft_reset or not self.pyb.in_raw_repl:
            self.pyb.enter_raw_repl(soft_reset=soft_reset)
            self._auto_soft_reset = False

    def ensure_friendly_repl(self):
        self.ensure_connected()
        if self.pyb.in_raw_repl:
            self.pyb.exit_raw_repl()


def main():
    config = load_user_config()
    prepare_command_expansions(config)

    args = sys.argv[1:]
    state = State()

    try:
        while args:
            do_command_expansion(args)
            cmd = args.pop(0)
            try:
                num_args_min, _help, handler = _COMMANDS[cmd]
            except KeyError:
                raise CommandError(f"'{cmd}' is not a command")

            if len(args) < num_args_min:
                print(f"{_PROG}: '{cmd}' neads at least {num_args_min} argument(s)")
                return 1

            handler(state, args)


        # If no commands were "actions" then implicitly finish with the REPL.
        if state.run_repl_on_completion():
            do_repl(state, args)

        return 0
    except CommandError as e:
        print(f"{_PROG}: {e}", file=sys.stderr)
        return 1
    finally:
        do_disconnect(state)
