from .console import Console, ConsolePosix

from .transport import TransportError


def do_repl_main_loop(
    state, console_in, console_out_write, *, escape_non_printable, code_to_inject, file_to_inject
):
    while True:
        try:
            console_in.waitchar(state.transport.serial)
            c = console_in.readchar()
            if c:
                if c in (b"\x1d", b"\x18"):  # ctrl-] or ctrl-x, quit
                    break
                elif c == b"\x04":  # ctrl-D
                    # special handling needed for ctrl-D if filesystem is mounted
                    state.transport.write_ctrl_d(console_out_write)
                elif c == b"\x0a" and code_to_inject is not None:  # ctrl-j, inject code
                    state.transport.serial.write(code_to_inject)
                elif c == b"\x0b" and file_to_inject is not None:  # ctrl-k, inject script
                    console_out_write(bytes("Injecting %s\r\n" % file_to_inject, "utf8"))
                    state.transport.enter_raw_repl(soft_reset=False)
                    with open(file_to_inject, "rb") as f:
                        pyfile = f.read()
                    try:
                        state.transport.exec_raw_no_follow(pyfile)
                    except TransportError as er:
                        console_out_write(b"Error:\r\n")
                        console_out_write(er)
                    state.transport.exit_raw_repl()
                else:
                    state.transport.serial.write(c)

            n = state.transport.serial.inWaiting()
            if n > 0:
                dev_data_in = state.transport.serial.read(n)
                if dev_data_in is not None:
                    if escape_non_printable:
                        # Pass data through to the console, with escaping of non-printables.
                        console_data_out = bytearray()
                        for c in dev_data_in:
                            if c in (8, 9, 10, 13, 27) or 32 <= c <= 126:
                                console_data_out.append(c)
                            else:
                                console_data_out.extend(b"[%02x]" % c)
                    else:
                        console_data_out = dev_data_in
                    console_out_write(console_data_out)

        except OSError as er:
            if _is_disconnect_exception(er):
                return True
            else:
                raise
    return False


def do_repl(state, args):
    state.ensure_friendly_repl()
    state.did_action()

    escape_non_printable = args.escape_non_printable
    capture_file = args.capture
    code_to_inject = args.inject_code
    file_to_inject = args.inject_file

    print("Connected to MicroPython at %s" % state.transport.device_name)
    print("Use Ctrl-] or Ctrl-x to exit this shell")
    if escape_non_printable:
        print("Escaping non-printable bytes/characters by printing their hex code")
    if capture_file is not None:
        print('Capturing session to file "%s"' % capture_file)
        capture_file = open(capture_file, "wb")
    if code_to_inject is not None:
        code_to_inject = bytes(code_to_inject.replace("\\n", "\r\n"), "utf8")
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
        return do_repl_main_loop(
            state,
            console,
            console_out_write,
            escape_non_printable=escape_non_printable,
            code_to_inject=code_to_inject,
            file_to_inject=file_to_inject,
        )
    finally:
        console.exit()
        if capture_file is not None:
            capture_file.close()


def _is_disconnect_exception(exception):
    """
    Check if an exception indicates device disconnect.

    Returns True if the exception indicates the device has disconnected,
    False otherwise.
    """
    if isinstance(exception, OSError):
        if hasattr(exception, 'args') and len(exception.args) > 0:
            # IO error, device disappeared
            if exception.args[0] == 5:
                return True
        # Check for common disconnect messages in the exception string
        exception_str = str(exception)
        disconnect_indicators = ["Write timeout", "Device disconnected", "ClearCommError failed"]
        return any(indicator in exception_str for indicator in disconnect_indicators)
    return False
