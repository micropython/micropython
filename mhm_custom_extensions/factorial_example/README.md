Example of how to build an `.mpy` file, which would contaijn native machine code - allowing you to write code in C and load it up in micropython without rebuilding the firmware.

`make` to build it, note that it requires the `xtensa-esp32-elf-gcc` cross compiler, you can install the tools for this from the instructions [here](https://gitdemo.readthedocs.io/en/latest/linux-setup.html).

And then just flash it to the board using `ampy -p [device port i.e /dev/ttyUSB0] factorial.mpy`.

And then using a tool like `picocom` you can verify you can use it by doing `import factorial` and `factoria.factorial(num)` to test it.