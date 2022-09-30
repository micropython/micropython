import os
import sys
import tempfile

import serial.tools.list_ports

from . import pyboardextended as pyboard


class CommandError(Exception):
    pass

    do_disconnect(state)

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
            # Don't do implicit REPL command.
            state.did_action()
        elif dev == "auto":
            # Auto-detect and auto-connect to the first available device.
            for p in sorted(serial.tools.list_ports.comports()):
                try:
                    state.pyb = pyboard.PyboardExtended(p.device, baudrate=115200)
                    return
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
                    state.pyb = pyboard.PyboardExtended(p.device, baudrate=115200)
                    return
            raise pyboard.PyboardError("no device with serial number {}".format(serial_number))
        else:
            # Connect to the given device.
            if dev.startswith("port:"):
                dev = dev[len("port:") :]
            state.pyb = pyboard.PyboardExtended(dev, baudrate=115200)
            return
    except pyboard.PyboardError as er:
        msg = er.args[0]
        if msg.startswith("failed to access"):
            msg += " (it may be in use by another program)"
        print(msg)
        sys.exit(1)


def do_disconnect(state, _args=None):
    if not state.pyb:
        return

    try:
        if state.pyb.mounted:
            if not state.pyb.in_raw_repl:
                state.pyb.enter_raw_repl(soft_reset=False)
            state.pyb.umount_local()
        if state.pyb.in_raw_repl:
            state.pyb.exit_raw_repl()
    except OSError:
        # Ignore any OSError exceptions when shutting down, eg:
        # - pyboard.filesystem_command will close the connecton if it had an error
        # - umounting will fail if serial port disappeared
        pass
    state.pyb.close()
    state.pyb = None
    state._auto_soft_reset = True


def show_progress_bar(size, total_size, op="copying"):
    if not sys.stdout.isatty():
        return
    verbose_size = 2048
    bar_length = 20
    if total_size < verbose_size:
        return
    elif size >= total_size:
        # Clear progress bar when copy completes
        print("\r" + " " * (13 + len(op) + bar_length) + "\r", end="")
    else:
        bar = size * bar_length // total_size
        progress = size * 100 // total_size
        print(
            "\r ... {} {:3d}% [{}{}]".format(op, progress, "#" * bar, "-" * (bar_length - bar)),
            end="",
        )


# Get all args up to the terminator ("+").
# The passed args will be updated with these ones removed.
def _get_fs_args(args):
    n = 0
    for src in args:
        if src == "+":
            break
        n += 1
    fs_args = args[:n]
    args[:] = args[n + 1 :]
    return fs_args


def do_filesystem(state, args):
    state.ensure_raw_repl()
    state.did_action()

    def _list_recursive(files, path):
        if os.path.isdir(path):
            for entry in os.listdir(path):
                _list_recursive(files, "/".join((path, entry)))
        else:
            files.append(os.path.split(path))

    fs_args = _get_fs_args(args)

    # Don't be verbose when using cat, so output can be redirected to something.
    verbose = fs_args[0] != "cat"

    if fs_args[0] == "cp" and fs_args[1] == "-r":
        fs_args.pop(0)
        fs_args.pop(0)
        if fs_args[-1] != ":":
            print(f"{_PROG}: 'cp -r' destination must be ':'")
            sys.exit(1)
        fs_args.pop()
        src_files = []
        for path in fs_args:
            if path.startswith(":"):
                raise CommandError("'cp -r' source files must be local")
            _list_recursive(src_files, path)
        known_dirs = {""}
        state.pyb.exec_("import uos")
        for dir, file in src_files:
            dir_parts = dir.split("/")
            for i in range(len(dir_parts)):
                d = "/".join(dir_parts[: i + 1])
                if d not in known_dirs:
                    state.pyb.exec_("try:\n uos.mkdir('%s')\nexcept OSError as e:\n print(e)" % d)
                    known_dirs.add(d)
            pyboard.filesystem_command(
                state.pyb,
                ["cp", "/".join((dir, file)), ":" + dir + "/"],
                progress_callback=show_progress_bar,
                verbose=verbose,
            )
    else:
        try:
            pyboard.filesystem_command(
                state.pyb, fs_args, progress_callback=show_progress_bar, verbose=verbose
            )
        except OSError as er:
            raise CommandError(er)


def do_edit(state, args):
    state.ensure_raw_repl()
    state.did_action()

    if not os.getenv("EDITOR"):
        raise pyboard.PyboardError("edit: $EDITOR not set")
    for src in _get_fs_args(args):
        src = src.lstrip(":")
        dest_fd, dest = tempfile.mkstemp(suffix=os.path.basename(src))
        try:
            print("edit :%s" % (src,))
            os.close(dest_fd)
            state.pyb.fs_touch(src)
            state.pyb.fs_get(src, dest, progress_callback=show_progress_bar)
            if os.system("$EDITOR '%s'" % (dest,)) == 0:
                state.pyb.fs_put(dest, src, progress_callback=show_progress_bar)
        finally:
            os.unlink(dest)


def _get_follow_arg(args):
    if args[0] == "--no-follow":
        args.pop(0)
        return False
    else:
        return True


def _do_execbuffer(state, buf, follow):
    state.ensure_raw_repl()
    state.did_action()

    try:
        state.pyb.exec_raw_no_follow(buf)
        if follow:
            ret, ret_err = state.pyb.follow(timeout=None, data_consumer=pyboard.stdout_write_bytes)
            if ret_err:
                pyboard.stdout_write_bytes(ret_err)
                sys.exit(1)
    except pyboard.PyboardError as er:
        print(er)
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(1)


def do_exec(state, args):
    follow = _get_follow_arg(args)
    buf = args.pop(0)
    _do_execbuffer(state, buf, follow)


def do_eval(state, args):
    follow = _get_follow_arg(args)
    buf = "print(" + args.pop(0) + ")"
    _do_execbuffer(state, buf, follow)


def do_run(state, args):
    follow = _get_follow_arg(args)
    filename = args.pop(0)
    try:
        with open(filename, "rb") as f:
            buf = f.read()
    except OSError:
        raise CommandError(f"could not read file '{filename}'")
        sys.exit(1)
    _do_execbuffer(state, buf, follow)


def do_mount(state, args):
    state.ensure_raw_repl()

    unsafe_links = False
    if args[0] == "--unsafe-links" or args[0] == "-l":
        args.pop(0)
        unsafe_links = True
    path = args.pop(0)
    state.pyb.mount_local(path, unsafe_links=unsafe_links)
    print(f"Local directory {path} is mounted at /remote")


def do_umount(state, path):
    state.ensure_raw_repl()
    state.pyb.umount_local()


def do_resume(state, _args=None):
    state._auto_soft_reset = False


def do_soft_reset(state, _args=None):
    state.ensure_raw_repl(soft_reset=True)
