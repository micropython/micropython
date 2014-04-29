This is a windows port of MicroPython which builds with Visual Studio 2013.

- genhdr.targets: msbuild script for generating the headers in build/genhdr,
  fullfills the purpose of py/py-version.sh and the qstr target in py/py.mk.
  Call before a build.
- *.props: build options for the vcxproj
- unistd.h: not available in Windows SDK but includes similar io.h
- init.c: called as first thing in main, setup Windows SDK specifics
