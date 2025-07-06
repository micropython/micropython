# The m68k Mac port

This port runs on m68k macs. The author tests it in a modified umac emulating a
4MB "mac plus" with System 7, though it may also run on System 6.

## Building and running

Two methods of building are supported. First, entirely within docker, using the steps in
`.github/workflows/port_m68k.yaml`. Second, building on a standard Linux host machine
(e.g., debian stable) with docker installed:

    $ pip install pyyaml
    $ make submodules
    $ make docker-build

There's a `make run` target to launch the emulator but it is very specific to
@jepler's development environment. Once you do have your target environment running,
double click micropython application icon. It will open up with a repl window
that supports minimal ANSI-style escape codes.

## Built in editor

There's a built in editor:
```py
>>> import editor
>>> editor.edit()              ## OR
>>> editor.edit("filename.py")
```
umac doesn't support arrow keys. Emacs-style ctrl+p/n f/b work to move cursor instead.
Probably doesn't work right now on a keyboard with no control key.

## Key TODOs

 * Finish `VfsMac`
   * a few more methods
   * chdir may affect a lot
   * handling `.` and `..` in paths
 * Add sys.stdin.readable & polling of sys.stdin for editor
 * auto-launch `code.py` (with bypass via shift key or something?)
 * OR double-clickable ".py" files
 * "Mac Roman" encoding support in terminal, text I/O & filenames
 * Mac API support (e.g., quickdraw, maybe arbitrary traps)
 * Support larger heap (via split heap? how does heap allocation work?)
 * Decide whether RetroConsole needs to be replaced (is GPL a problem for upstream??)
 * ctrl-c interrupt char handling
 * Any other issues that might ease upstream inclusion.
