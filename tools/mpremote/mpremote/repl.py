from .console import Console, ConsolePosix

from . import pyboardextended as pyboard


def do_repl_main_loop(state, console_in, console_out_write, *, code_to_inject, file_to_inject):
    while True:
        console_in.waitchar(state.pyb.serial)
        c = console_in.readchar()
        if c:
            if c in (b"\x1d", b"\x18"):  # ctrl-] or ctrl-x, quit
                break
            elif c == b"\x04":  # ctrl-D
                # special handling needed for ctrl-D if filesystem is mounted
                state.pyb.write_ctrl_d(console_out_write)
            elif c == b"\x0a" and code_to_inject is not None:  # ctrl-j, inject code
                state.pyb.serial.write(code_to_inject)
            elif c == b"\x0b" and file_to_inject is not None:  # ctrl-k, inject script
                console_out_write(bytes("Injecting %s\r\n" % file_to_inject, "utf8"))
                state.pyb.enter_raw_repl(soft_reset=False)
                with open(file_to_inject, "rb") as f:
                    pyfile = f.read()
                try:
                    state.pyb.exec_raw_no_follow(pyfile)
                except pyboard.PyboardError as er:
                    console_out_write(b"Error:\r\n")
                    console_out_write(er)
                state.pyb.exit_raw_repl()
            else:
                state.pyb.serial.write(c)

        try:
            n = state.pyb.serial.inWaiting()
        except OSError as er:
            if er.args[0] == 5:  # IO error, device disappeared
                print("device disconnected")
                break

        if n > 0:
            c = state.pyb.serial.read(1)
            if c is not None:
                # pass character through to the console
                oc = ord(c)
                if oc in (8, 9, 10, 13, 27) or 32 <= oc <= 126:
                    console_out_write(c)
                else:
                    console_out_write(b"[%02x]" % ord(c))


def do_repl(state, args):
    state.ensure_friendly_repl()
    state.did_action()

    capture_file = args.capture
    code_to_inject = args.inject_code
    file_to_inject = args.inject_file

    print("Connected to MicroPython at %s" % state.pyb.device_name)
    print("Use Ctrl-] or Ctrl-x to exit this shell")
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
        do_repl_main_loop(
            state,
            console,
            console_out_write,
            code_to_inject=code_to_inject,
            file_to_inject=file_to_inject,
        )
    finally:
        console.exit()
        if capture_file is not None:
            capture_file.close()
