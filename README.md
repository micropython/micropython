 Subway Map
=======================
Prerequisites
```
sudo apt update
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
make -C mpy-cross
cd ports/rp2
make BOARD=PICO submodules
```
Build Project
```
cd ports/rp2
make
```