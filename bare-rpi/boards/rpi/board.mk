# description for the raspberry pi (1) build for PiOS
PLATFORM=arm-none-eabi
CPU=arm1176jzf-s
LDSCRIPT=rpi.ld

NEWLIB_OPTS=--target=arm-none-eabi --enable-newlib-hw-fp --with-float=hard --with-cpu=arm1176jzf-s --with-fpu=vfp --disable-multilib --disable-shared --enable-target-optspace --disable-newlib-supplied-syscalls

ASFILES=$(BUILD)start.o\
        $(BUILD)vc4.o
