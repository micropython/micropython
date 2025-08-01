# The m68k Mac port

This port runs on m68k macs. The author tests it in a modified umac emulating a
4MB "mac plus" with System 7, though it may also run on System 6.

## Building and running

Two methods of building are supported. First, entirely within docker, using the steps in
`.github/workflows/port_m68k.yaml`. Second, building on a standard Linux host machine
(e.g., debian stable) with docker installed:

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
