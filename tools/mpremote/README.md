# mpremote -- MicroPython remote control

This CLI tool provides an integrated set of utilities to remotely interact with
and automate a MicroPython device over a serial connection.

The simplest way to use this tool is:

    mpremote

This will automatically connect to a USB serial port and provide an interactive REPL.

The full list of supported commands are:

    mpremote connect <device>         -- connect to given device
                                         device may be: list, auto, id:x, port:x
                                         or any valid device name/path
                                         options:
                                             --dtr [0|1|on|off]
                                             --rts [0|1|on|off]
    mpremote disconnect               -- disconnect current device
    mpremote mount <local-dir>        -- mount local directory on device
    mpremote eval <string>            -- evaluate and print the string
    mpremote exec <string>            -- execute the string
    mpremote run <file>               -- run the given local script
    mpremote fs <command> <args...>   -- execute filesystem commands on the device
                                         command may be: cat, ls, cp, rm, mkdir, rmdir, sha256sum
                                         use ":" as a prefix to specify a file on the device
    mpremote repl                     -- enter REPL
                                         options:
                                             --capture <file>
                                             --inject-code <string>
                                             --inject-file <file>
    mpremote mip install <package...> -- Install packages (from micropython-lib or third-party sources)
                                         options:
                                             --target <path>
                                             --index <url>
                                             --no-mpy
    mpremote help                     -- print list of commands and exit

Multiple commands can be specified and they will be run sequentially.  Connection
and disconnection will be done automatically at the start and end of the execution
of the tool, if such commands are not explicitly given.  Automatic connection will
search for the first available serial device.  If no action is specified then the
REPL will be entered.

Shortcuts can be defined using the macro system.  Built-in shortcuts are:

- a0, a1, a2, a3: connect to `/dev/ttyACM?`
- u0, u1, u2, u3: connect to `/dev/ttyUSB?`
- c0, c1, c2, c3: connect to `COM?`
- cat, ls, cp, rm, mkdir, rmdir, df: filesystem commands
- reset: reset the device
- bootloader: make the device enter its bootloader

Any user configuration, including user-defined shortcuts, can be placed in
.config/mpremote/config.py.  For example:

    # Custom macro commands
    commands = {
        "c33": "connect id:334D335C3138",
        "bl": "bootloader",
        "double x=4": {
            "command": "eval x*2",
            "help": "multiply by two"
        }
    }

Examples:

    mpremote
    mpremote a1
    mpremote connect /dev/ttyUSB0 repl
    mpremote ls
    mpremote a1 ls
    mpremote exec "import micropython; micropython.mem_info()"
    mpremote eval 1/2 eval 3/4
    mpremote mount .
    mpremote mount . exec "import local_script"
    mpremote ls
    mpremote cat boot.py
    mpremote cp :main.py .
    mpremote cp main.py :
    mpremote cp -r dir/ :
    mpremote sha256sum :main.py
    mpremote mip install aioble
    mpremote mip install github:org/repo@branch
    mpremote mip install gitlab:org/repo@branch

Note for ESP-32 CAM with MB

When using mpremote with the ESP-32 CAM attached to the ESP-32 CAM MB you might
get into the situation that the communication is not working (i.e. you might
get a timeout). To avoid this try using the following options:

    mpremote connect --dtr 0 --rts 0 auto
    mpremote connect --dtr 0 --rts 0 auto fs ls

The commands may take some additional 1-2s to be executed in case the ESP32 is
performing a hard reset.
