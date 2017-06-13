# Raspberry Pi Port

This is a Raspberry Pi (Model A) port of the Micropython Python interpreter. 

## Compiling

This port uses newlib as a libc, to deliver some essential functions like ```memset```, which are used by micropython but not defined anywhere in the code. 

For compilation do:
- init the submodule for newlib
- decide whether to build for QEMU or real Raspberry Pi hardware
- build newlib and build libpios
- build the kernel
- transfer the kernel.img over to the Raspberry Pi (either via Raspbootin or by copying it to the flash card)
- start the Pi, see the output coming through the UART-interface (using a TTL-USB-adapter)

### QEMU Builds?

For testing reasons it may be good to run on QEMU-based virtual hardware instead of the real Raspberry Pi hardware. The problems of real hardware are many, with a working QEMU-demo it might be possible for others to work without a stable and working Raspberry Pi port.

If you want to build for QEMU go into the Makefile in the bare-rpi folder and change the variable ```PIOS_BOARD``` to ```qemu```. Otherwise see lib/PiOS/boards for reference on possible values.

To run the kernel in QEMU after building do:

```
qemu-system-arm -M versatilepb -m 128M -nographic -kernel build/kernel.img
```

For arm-none-eabi-gdb support you may add ```-s -S``` then connect with gdb to port 1234 in localhost for debugging session.

### Building newlib and libpios

After inititation of the git submodule do (to speed things up you may do want to add ```-j 4```). Note: PiOS is built automatically with ```make```. 

```
make newlib
```

To build libpios you may run:

```
make pios
```

### Building the Rapberry Pi Port

```
make
```
