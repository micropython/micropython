# Raspberry Pi Port

This is a Raspberry Pi (Model A) port of the Micropython Python interpreter. 

## Compiling

This port uses newlib as a libc, to deliver some essential functions like ```memset```, which are used by micropython but not defined anywhere in the code. 

For compilation do:
- init the submodule for newlib
- build newlib
- build the kernel
- transfer the kernel.img over to the Raspberry Pi (either via Raspbootin or by copying it to the flash card)
- start the Pi, see the output coming through the UART-interface (using a TTL-USB-adapter)

### Building newlib

After inititation of the git submodule do:

```
make newlib
```

### Building the Rapberry Pi Port

```
make
```
