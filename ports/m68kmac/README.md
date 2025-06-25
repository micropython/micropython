# The m68k Mac port

This port runs on m68k macs. The author tests it in a modified umac emulating a
4MB "mac plus" with System 7, though it may also run on System 6.

## Building and running

The build assumes it will occur inside the Retro68 docker image:

    $ docker run --rm --mount type=bind,source=.,destination=/work -it ghcr.io/autc04/retro68 make -C /work/ports/m68kmac

A modified version of umac with multi disc image support is required.
(It hopefully works in other emulators but this is what I use.)
To run the executable and get a basic working REPL do:

    $ /path/to/umac/main -r rom.bin -d HyperCardBootSystem7.img -d build/micropython.dsk

.. then when the micropython disk is mounted, double click it and then the
micropython application icon. It will open up with a repl window that supports
minimal ANSI-style escape codes.

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
