import hashlib
import os
import sys
import tempfile

import serial.tools.list_ports

from .transport import TransportError, stdout_write_bytes
from .transport_serial import SerialTransport


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
                        state.transport = SerialTransport(p.device, baudrate=115200)
                        return
                    except TransportError as er:
                        if not er.args[0].startswith("failed to access"):
                            raise er
            raise TransportError("no device found")
        elif dev.startswith("id:"):
            # Search for a device with the given serial number.
            serial_number = dev[len("id:") :]
            dev = None
            for p in serial.tools.list_ports.comports():
                if p.serial_number == serial_number:
                    state.transport = SerialTransport(p.device, baudrate=115200)
                    return
            raise TransportError("no device with serial number {}".format(serial_number))
        else:
            # Connect to the given device.
            if dev.startswith("port:"):
                dev = dev[len("port:") :]
            state.transport = SerialTransport(dev, baudrate=115200)
            return
    except TransportError as er:
        msg = er.args[0]
        if msg.startswith("failed to access"):
            msg += " (it may be in use by another program)"
        raise CommandError(msg)


def do_disconnect(state, _args=None):
    if not state.transport:
        return

    try:
        if state.transport.mounted:
            if not state.transport.in_raw_repl:
                state.transport.enter_raw_repl(soft_reset=False)
            state.transport.umount_local()
        if state.transport.in_raw_repl:
            state.transport.exit_raw_repl()
    except OSError:
        # Ignore any OSError exceptions when shutting down, eg:
        # - filesystem_command will close the connection if it had an error
        # - umounting will fail if serial port disappeared
        pass
    state.transport.close()
    state.transport = None
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


def _remote_path_join(a, *b):
    if not a:
        a = "./"
    result = a.rstrip("/")
    for x in b:
        result += "/" + x.strip("/")
    return result


def _remote_path_dirname(a):
    a = a.rsplit("/", 1)
    if len(a) == 1:
        return ""
    else:
        return a[0]


def _remote_path_basename(a):
    return a.rsplit("/", 1)[-1]


def do_filesystem_cp(state, src, dest, multiple, check_hash=False):
    if dest.startswith(":"):
        dest_no_slash = dest.rstrip("/" + os.path.sep + (os.path.altsep or ""))
        dest_exists = state.transport.fs_exists(dest_no_slash[1:])
        dest_isdir = dest_exists and state.transport.fs_isdir(dest_no_slash[1:])

        # A trailing / on dest forces it to be a directory.
        if dest != dest_no_slash:
            if not dest_isdir:
                raise CommandError("cp: destination is not a directory")
            dest = dest_no_slash
    else:
        dest_exists = os.path.exists(dest)
        dest_isdir = dest_exists and os.path.isdir(dest)

    if multiple:
        if not dest_exists:
            raise CommandError("cp: destination does not exist")
        if not dest_isdir:
            raise CommandError("cp: destination is not a directory")

    # Download the contents of source.
    try:
        if src.startswith(":"):
            data = state.transport.fs_readfile(src[1:], progress_callback=show_progress_bar)
            filename = _remote_path_basename(src[1:])
        else:
            with open(src, "rb") as f:
                data = f.read()
            filename = os.path.basename(src)
    except IsADirectoryError:
        raise CommandError("cp: -r not specified; omitting directory")

    # Write back to dest.
    if dest.startswith(":"):
        # If the destination path is just the directory, then add the source filename.
        if dest_isdir:
            dest = ":" + _remote_path_join(dest[1:], filename)

        # Skip copy if the destination file is identical.
        if check_hash:
            try:
                remote_hash = state.transport.fs_hashfile(dest[1:], "sha256")
                source_hash = hashlib.sha256(data).digest()
                # remote_hash will be None if the device doesn't support
                # hashlib.sha256 (and therefore won't match).
                if remote_hash == source_hash:
                    print("Up to date:", dest[1:])
                    return
            except OSError:
                pass

        # Write to remote.
        state.transport.fs_writefile(dest[1:], data, progress_callback=show_progress_bar)
    else:
        # If the destination path is just the directory, then add the source filename.
        if dest_isdir:
            dest = os.path.join(dest, filename)

        # Write to local file.
        with open(dest, "wb") as f:
            f.write(data)


def do_filesystem_recursive_cp(state, src, dest, multiple, check_hash):
    # Ignore trailing / on both src and dest. (Unix cp ignores them too)
    src = src.rstrip("/" + os.path.sep + (os.path.altsep if os.path.altsep else ""))
    dest = dest.rstrip("/" + os.path.sep + (os.path.altsep if os.path.altsep else ""))

    # If the destination directory exists, then we copy into it. Otherwise we
    # use the destination as the target.
    if dest.startswith(":"):
        dest_exists = state.transport.fs_exists(dest[1:])
    else:
        dest_exists = os.path.exists(dest)

    # Recursively find all files to copy from a directory.
    # `dirs` will be a list of dest split paths.
    # `files` will be a list of `(dest split path, src joined path)`.
    dirs = []
    files = []

    # For example, if src=/tmp/foo, with /tmp/foo/x.py and /tmp/foo/a/b/c.py,
    # and if the destination directory exists, then we will have:
    #   dirs = [['foo'], ['foo', 'a'], ['foo', 'a', 'b']]
    #   files = [(['foo', 'x.py'], '/tmp/foo/x.py'), (['foo', 'a', 'b', 'c.py'], '/tmp/foo/a/b/c.py')]
    # If the destination doesn't exist, then we will have:
    #   dirs = [['a'], ['a', 'b']]
    #   files = [(['x.py'], '/tmp/foo/x.py'), (['a', 'b', 'c.py'], '/tmp/foo/a/b/c.py')]

    def _list_recursive(base, src_path, dest_path, src_join_fun, src_isdir_fun, src_listdir_fun):
        src_path_joined = src_join_fun(base, *src_path)
        if src_isdir_fun(src_path_joined):
            if dest_path:
                dirs.append(dest_path)
            for entry in src_listdir_fun(src_path_joined):
                _list_recursive(
                    base,
                    src_path + [entry],
                    dest_path + [entry],
                    src_join_fun,
                    src_isdir_fun,
                    src_listdir_fun,
                )
        else:
            files.append(
                (
                    dest_path,
                    src_path_joined,
                )
            )

    if src.startswith(":"):
        src_dirname = [_remote_path_basename(src[1:])]
        dest_dirname = src_dirname if dest_exists else []
        _list_recursive(
            _remote_path_dirname(src[1:]),
            src_dirname,
            dest_dirname,
            src_join_fun=_remote_path_join,
            src_isdir_fun=state.transport.fs_isdir,
            src_listdir_fun=lambda p: [x.name for x in state.transport.fs_listdir(p)],
        )
    else:
        src_dirname = [os.path.basename(src)]
        dest_dirname = src_dirname if dest_exists else []
        _list_recursive(
            os.path.dirname(src),
            src_dirname,
            dest_dirname,
            src_join_fun=os.path.join,
            src_isdir_fun=os.path.isdir,
            src_listdir_fun=os.listdir,
        )

    # If no directories were encountered then we must have just had a file.
    if not dirs:
        return do_filesystem_cp(state, src, dest, multiple, check_hash)

    def _mkdir(a, *b):
        try:
            if a.startswith(":"):
                state.transport.fs_mkdir(_remote_path_join(a[1:], *b))
            else:
                os.mkdir(os.path.join(a, *b))
        except FileExistsError:
            pass

    # Create the destination if necessary.
    if not dest_exists:
        _mkdir(dest)

    # Create all sub-directories relative to the destination.
    for d in dirs:
        _mkdir(dest, *d)

    # Copy all files, in sorted order to help it be deterministic.
    files.sort()
    for dest_path_split, src_path_joined in files:
        if src.startswith(":"):
            src_path_joined = ":" + src_path_joined

        if dest.startswith(":"):
            dest_path_joined = ":" + _remote_path_join(dest[1:], *dest_path_split)
        else:
            dest_path_joined = os.path.join(dest, *dest_path_split)

        do_filesystem_cp(state, src_path_joined, dest_path_joined, False, check_hash)


def do_filesystem(state, args):
    state.ensure_raw_repl()
    state.did_action()

    command = args.command[0]
    paths = args.path

    if command == "cat":
        # Don't do verbose output for `cat` unless explicitly requested.
        verbose = args.verbose is True
    else:
        verbose = args.verbose is not False

    if command == "cp":
        # Note: cp requires the user to specify local/remote explicitly via
        # leading ':'.

        # The last argument must be the destination.
        if len(paths) <= 1:
            raise CommandError("cp: missing destination path")
        cp_dest = paths[-1]
        paths = paths[:-1]
    else:
        # All other commands implicitly use remote paths. Strip the
        # leading ':' if the user included them.
        paths = [path[1:] if path.startswith(":") else path for path in paths]

    # ls implicitly lists the cwd.
    if command == "ls" and not paths:
        paths = [""]

    try:
        # Handle each path sequentially.
        for path in paths:
            if verbose:
                if command == "cp":
                    print("{} {} {}".format(command, path, cp_dest))
                else:
                    print("{} :{}".format(command, path))

            if command == "cat":
                state.transport.fs_printfile(path)
            elif command == "ls":
                for result in state.transport.fs_listdir(path):
                    print(
                        "{:12} {}{}".format(
                            result.st_size, result.name, "/" if result.st_mode & 0x4000 else ""
                        )
                    )
            elif command == "mkdir":
                state.transport.fs_mkdir(path)
            elif command == "rm":
                state.transport.fs_rmfile(path)
            elif command == "rmdir":
                state.transport.fs_rmdir(path)
            elif command == "touch":
                state.transport.fs_touchfile(path)
            elif command.endswith("sum") and command[-4].isdigit():
                digest = state.transport.fs_hashfile(path, command[:-3])
                print(digest.hex())
            elif command == "cp":
                if args.recursive:
                    do_filesystem_recursive_cp(
                        state, path, cp_dest, len(paths) > 1, not args.force
                    )
                else:
                    do_filesystem_cp(state, path, cp_dest, len(paths) > 1, not args.force)
    except FileNotFoundError as er:
        raise CommandError("{}: {}: No such file or directory.".format(command, er.args[0]))
    except IsADirectoryError as er:
        raise CommandError("{}: {}: Is a directory.".format(command, er.args[0]))
    except FileExistsError as er:
        raise CommandError("{}: {}: File exists.".format(command, er.args[0]))
    except TransportError as er:
        raise CommandError("Error with transport:\n{}".format(er.args[0]))


def do_edit(state, args):
    state.ensure_raw_repl()
    state.did_action()

    if not os.getenv("EDITOR"):
        raise CommandError("edit: $EDITOR not set")
    for src in args.files:
        src = src.lstrip(":")
        dest_fd, dest = tempfile.mkstemp(suffix=os.path.basename(src))
        try:
            print("edit :%s" % (src,))
            state.transport.fs_touchfile(src)
            data = state.transport.fs_readfile(src, progress_callback=show_progress_bar)
            with open(dest_fd, "wb") as f:
                f.write(data)
            if os.system('%s "%s"' % (os.getenv("EDITOR"), dest)) == 0:
                with open(dest, "rb") as f:
                    state.transport.fs_writefile(
                        src, f.read(), progress_callback=show_progress_bar
                    )
        finally:
            os.unlink(dest)


def _do_execbuffer(state, buf, follow):
    state.ensure_raw_repl()
    state.did_action()

    try:
        state.transport.exec_raw_no_follow(buf)
        if follow:
            ret, ret_err = state.transport.follow(timeout=None, data_consumer=stdout_write_bytes)
            if ret_err:
                stdout_write_bytes(ret_err)
                sys.exit(1)
    except TransportError as er:
        raise CommandError(er.args[0])
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
    state.transport.mount_local(path, unsafe_links=args.unsafe_links)
    print(f"Local directory {path} is mounted at /remote")


def do_umount(state, path):
    state.ensure_raw_repl()
    state.transport.umount_local()


def do_resume(state, _args=None):
    state._auto_soft_reset = False


def do_soft_reset(state, _args=None):
    state.ensure_raw_repl(soft_reset=True)
    state.did_action()


def do_rtc(state, args):
    state.ensure_raw_repl()
    state.did_action()

    state.transport.exec("import machine")

    if args.set:
        import datetime

        now = datetime.datetime.now()
        timetuple = "({}, {}, {}, {}, {}, {}, {}, {})".format(
            now.year,
            now.month,
            now.day,
            now.weekday(),
            now.hour,
            now.minute,
            now.second,
            now.microsecond,
        )
        state.transport.exec("machine.RTC().datetime({})".format(timetuple))
    else:
        print(state.transport.eval("machine.RTC().datetime()"))
