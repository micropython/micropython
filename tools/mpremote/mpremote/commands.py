import os
import sys
import tempfile

import serial.tools.list_ports

from . import pyboardextended as pyboard


class CommandError(Exception):
    pass


def do_connect(state, args=None):
    dev = args.device[0] if args else "auto"
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
            # Auto-detect and auto-connect to the first available USB serial port.
            for p in sorted(serial.tools.list_ports.comports()):
                if p.vid is not None and p.pid is not None:
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


def do_filesystem(state, args):
    state.ensure_raw_repl()
    state.did_action()

    def _list_recursive(files, path):
        if os.path.isdir(path):
            for entry in os.listdir(path):
                _list_recursive(files, "/".join((path, entry)))
        else:
            files.append(os.path.split(path))

    command = args.command[0]
    paths = args.path

    if command == "cat":
        # Don't be verbose by default when using cat, so output can be
        # redirected to something.
        verbose = args.verbose == True
    else:
        verbose = args.verbose != False

    if command == "cp" and args.recursive:
        if paths[-1] != ":":
            raise CommandError("'cp -r' destination must be ':'")
        paths.pop()
        src_files = []
        for path in paths:
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
        if args.recursive:
            raise CommandError("'-r' only supported for 'cp'")
        try:
            pyboard.filesystem_command(
                state.pyb, [command] + paths, progress_callback=show_progress_bar, verbose=verbose
            )
        except OSError as er:
            raise CommandError(er)


def do_edit(state, args):
    state.ensure_raw_repl()
    state.did_action()

    if not os.getenv("EDITOR"):
        raise pyboard.PyboardError("edit: $EDITOR not set")
    for src in args.files:
        src = src.lstrip(":")
        dest_fd, dest = tempfile.mkstemp(suffix=os.path.basename(src))
        try:
            print("edit :%s" % (src,))
            os.close(dest_fd)
            state.pyb.fs_touch(src)
            state.pyb.fs_get(src, dest, progress_callback=show_progress_bar)
            if os.system('%s "%s"' % (os.getenv("EDITOR"), dest)) == 0:
                state.pyb.fs_put(dest, src, progress_callback=show_progress_bar)
        finally:
            os.unlink(dest)


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
    _do_execbuffer(state, args.expr[0], args.follow)


def do_eval(state, args):
    buf = "print(" + args.expr[0] + ")"
    _do_execbuffer(state, buf, True)


def do_run(state, args):
    filename = args.path[0]
    try:
        with open(filename, "rb") as f:
            buf = f.read()
    except OSError:
        raise CommandError(f"could not read file '{filename}'")
    _do_execbuffer(state, buf, args.follow)


def do_mount(state, args):
    state.ensure_raw_repl()
    path = args.path[0]
    state.pyb.mount_local(path, unsafe_links=args.unsafe_links)
    print(f"Local directory {path} is mounted at /remote")


def do_umount(state, path):
    state.ensure_raw_repl()
    state.pyb.umount_local()


def do_resume(state, _args=None):
    state._auto_soft_reset = False


def do_soft_reset(state, _args=None):
    state.ensure_raw_repl(soft_reset=True)
