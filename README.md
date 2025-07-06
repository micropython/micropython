m68k-micropython
================

This is a fork of the MicroPython project, which aims to put an implementation
of Python 3.x on m68k Macintosh computers.

You can find the official micropython website at [micropython.org](http://www.micropython.org).

WARNING: this project is in alpha stage and is subject to changes of the
code-base, including "git rebasing", project-wide name changes and API changes.

MicroPython implements the entire Python 3.4 syntax (including exceptions,
`with`, `yield from`, etc., and additionally `async`/`await` keywords from
Python 3.5 and some select features from later versions). The following core
datatypes are provided: `str`(including basic Unicode support), `bytes`,
`bytearray`, `tuple`, `list`, `dict`, `set`, `frozenset`, `array.array`,
`collections.namedtuple`, classes and instances. Builtin modules include
`os`, `sys`, `time`, `re`, and `struct`, etc.

Mac specific functionality is limited:
 * `os.VfsMac` for interacting with filesystems
 * `qd`, a dirty wrapper around QuickDraw

MicroPython can execute scripts in textual source form (.py files) or from
precompiled bytecode (.mpy files), in both cases either from an on-device
filesystem or "frozen" into the MicroPython executable.

Getting started
---------------

Grab a release image and use it with your favorite mac emulator, including the
website [Infinite Mac](https://infinitemac.org/).

See micropython's [online documentation](https://docs.micropython.org/) for the API
reference and information about using MicroPython and information about how
it is implemented. TODO: Document modules specific to m68k-micropython.

We use [GitHub Discussions](https://github.com/orgs/m68k-micropython/discussions)
as our forum, and Mastodon (#m68kMicroPython) for chat. These
are great places to ask questions and advice from the community or to discuss your
MicroPython-based projects.

For bugs and feature requests, please [raise an issue](https://github.com/m68k-micropython/micropython/issues/new/choose)
and follow the templates there.

Full-screen Editor
------------------

A full-screen editor is frozen in: `import editor; editor.edit("code.py")`

This editor does not use the mouse, instead it uses arrow keys or emacs-like
ctrl+n/p, ctrl-f/b, ctrl-a/e for cursor motion.

Launching Programs
------------------

Double click a file of type "mupy" and it is run as a Python program. I've found it
convenient to create a file "edit\_code.py" that invokes the full-screen editor on "code.py",
which gives a quick-ish turnaround for development as long as you're not loading the
Micropython program from a real floppy.

Contributing
------------

MicroPython is an open-source project and welcomes contributions. To be
productive, please be sure to follow the
[Contributors' Guidelines](https://github.com/micropython/micropython/wiki/ContributorGuidelines)
and the [Code Conventions](https://github.com/micropython/micropython/blob/master/CODECONVENTIONS.md).
Note that MicroPython is licenced under the MIT license, and all contributions
should follow this license.

About this repository
---------------------

This repository contains the following components:
- [py/](py/) -- the core Python implementation, including compiler, runtime, and
  core library.
- [mpy-cross/](mpy-cross/) -- the MicroPython cross-compiler which is used to turn scripts
  into precompiled bytecode.
- [ports/m68kmac](ports/m68kmac) -- platform-specific code for the Macintosh port
- [lib/](lib/) -- submodules for external dependencies.
- [tests/](tests/) -- test framework and test scripts.
- [docs/](docs/) -- user documentation in Sphinx reStructuredText format. This is used to generate the [online documentation](http://docs.micropython.org).
- [extmod/](extmod/) -- additional (non-core) modules implemented in C.
- [tools/](tools/) -- various tools, including the pyboard.py module.
- [examples/](examples/) -- a few example Python scripts.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need [Retro68](https://github.com/autc04/Retro68),
bash, gcc, and Python 3.3+ available as the command `python3`.
`yamllib` must be installed via pip3.

The easiest way to get Retro68 is by using docker and invoking the `make
docker-build` target.

A goal of this project is to keep differences from micropython to a minimum, and to
rebase onto it from time to time. However, at present there are a couple of
nasty hacks in `py/`.


Supported platforms & architectures
-----------------------------------

The main targets are 4MB Mac with System 6 or System 7.

The MicroPython cross-compiler, mpy-cross
-----------------------------------------

Most ports including m68kmac require the [MicroPython cross-compiler](mpy-cross) to be built
first.  This program, called mpy-cross, is used to pre-compile Python scripts
to .mpy files which can then be included (frozen) into the
firmware/executable for a port.  To build mpy-cross use:

    $ cd mpy-cross
    $ make

External dependencies
---------------------

The core MicroPython VM and runtime has no external dependencies, but a given
port might depend on third-party drivers or vendor HALs. This repository
includes [several submodules](lib/) linking to these external dependencies.
Before compiling a given port, use

    $ cd ports/name
    $ make submodules

to ensure that all required submodules are initialised.

Development status
------------------

The project owner ([@jepler](https://github.com/jepler)) occasionally develops and maintains this project for fun.
Pull requests are appreciated.
There are many bugs. Noting them with issues may be helpful, but do not expect rapid response.
Feature requests are not likely to be acted on.
The project is seeking co-maintainer(s).

