# description for the raspberry pi (1) build for PiOS
PLATFORM=arm-none-eabi
CPU=arm926ej-s

NEWLIB_OPTS=--target=arm-none-eabi --enable-newlib-hw-fp --with-float=hard --with-cpu=arm926ej-s --with-fpu=vfp --disable-multilib --disable-shared --enable-target-optspace --disable-newlib-supplied-syscalls

ASFILES=$(BUILD)qemu_start.o\
        $(BUILD)vc4.o
