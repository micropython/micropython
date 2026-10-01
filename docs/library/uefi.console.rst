:mod:`uefi.console` --- text console control
============================================

.. module:: uefi.console
    :synopsis: colour/cursor/mode control and raw keystroke input over ConOut/ConIn

The ``uefi.console`` submodule is the interactive half of the shell: colour,
cursor and mode control on the active text output (``ConOut``), and raw keystroke
input including scancodes (arrow keys, function keys) on the active text input
(``ConIn``).  This is what a boot-menu UI is built from.

``ConOut`` and ``ConIn`` are the *active* console interfaces named in the System
Table (not just any text handle), so :class:`Console` reads their pointers from
there.

:class:`Console` is re-exported at the top level as :class:`uefi.Console`.

.. note::

    This is for building a full-screen text UI (a boot menu, a viewer).  For
    ordinary line-oriented I/O just use ``print()`` and ``input()`` -- the REPL is
    already wired to the console (or serial).

.. class:: Console()

    Wrap the active text console: ``.out`` (a ``ProtocolWrapper`` for ``ConOut``)
    for output and ``.inp`` (for ``ConIn``) for keystrokes.

    Output:

    .. method:: write(text)

        Write *text* to the console.

    .. method:: clear()

        Clear the screen.

    .. method:: set_color(fg, bg=BLACK)

        Set the foreground/background text colour (see the colour constants).

    .. method:: set_cursor(column, row)

        Move the text cursor.

    .. method:: show_cursor(visible=True)

        Show or hide the cursor.

    .. method:: cursor()

        Return the ``(column, row)`` of the text cursor.

    .. method:: mode_count()

        The number of text modes the console supports.

    .. method:: current_mode()

        The current text-mode number.

    .. method:: set_mode(n)

        Switch to text mode *n*.

    .. method:: size(mode=None)

        Return the ``(columns, rows)`` of *mode* (default: the current mode).

    Input:

    .. method:: read_key()

        Non-blocking: return ``(scancode, char)`` or ``None`` if no key is pending.
        *char* is a one-character ``str`` (or ``''`` for a pure-scancode key such
        as an arrow).

    .. method:: wait_key()

        Block until a key is pressed; return ``(scancode, char)``.  This is a hard
        block at ``TPL_APPLICATION`` (it does **not** yield to :mod:`asyncio`),
        which is what a modal boot menu wants.

    .. method:: flush_input()

        Discard any pending keystrokes.

.. function:: attribute(fg, bg=BLACK)

    Pack a foreground (0..15) and background (0..7) into an EFI text attribute
    (``fg | (bg << 4)``).

Colour constants
----------------

Foreground colours are ``0..15``, background colours ``0..7``:

.. data:: BLACK
          BLUE
          GREEN
          CYAN
          RED
          MAGENTA
          BROWN
          LIGHTGRAY
          DARKGRAY
          LIGHTBLUE
          LIGHTGREEN
          LIGHTCYAN
          LIGHTRED
          LIGHTMAGENTA
          YELLOW
          WHITE

    EFI text colours.

Scancode constants
------------------

``EFI_INPUT_KEY.ScanCode`` values returned by :meth:`Console.read_key` /
:meth:`Console.wait_key` when *char* is ``''``:

.. data:: SCAN_NULL
          SCAN_UP
          SCAN_DOWN
          SCAN_RIGHT
          SCAN_LEFT
          SCAN_HOME
          SCAN_END
          SCAN_INSERT
          SCAN_DELETE
          SCAN_PAGE_UP
          SCAN_PAGE_DOWN
          SCAN_F1
          SCAN_F2
          SCAN_F3
          SCAN_F4
          SCAN_ESC

    Non-character key scancodes.

Example
-------

A minimal modal menu::

    from uefi.console import Console, SCAN_UP, SCAN_DOWN, WHITE, BLUE, YELLOW

    con = Console()
    items = ['Boot Linux', 'UEFI Shell', 'Reboot']
    sel = 0
    while True:
        con.clear()
        for i, label in enumerate(items):
            con.set_cursor(2, i + 1)
            con.set_color(YELLOW if i == sel else WHITE, BLUE if i == sel else 0)
            con.write(label)
        scan, char = con.wait_key()
        if scan == SCAN_UP:
            sel = (sel - 1) % len(items)
        elif scan == SCAN_DOWN:
            sel = (sel + 1) % len(items)
        elif char in ('\r', '\n'):
            break
    con.clear()
    print('chose', items[sel])
