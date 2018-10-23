# pyboardCN V2

pyboardCN V2学习板，是MicroPython中文社区在V1基础上改进而来，它是专门为学习和研究MicroPython而设计，方便大家快速学习和掌握目前开源硬件/智能硬件界的热点 MicroPython。它在功能上完全兼容官方的pybv10，可以直接运行官方pybv10的固件。它可以作为micropython学习板，也可以作为STM32学习板使用。

![](pyboardCN_V2.jpg)


## 升级固件

PYBCN_V2可以直接使用PYBV10的固件，可以通过DFU或者SWD方式升级固件。使用SWD升级时会破坏磁盘中的内容，所以需要先备份好磁盘内容。

## 编译

PYBCN_V2带有CN_V2板级定义文件，添加了外部flash的支持和线程功能。编译固件时，先将board_define.ZIP中的内容解压到micropython的ports/stm32/boards目录下，然后在micropython目录输入下面命令进行编译：

* make -C ports/stm32 BOARD=PYBCNV2

如果需要使用双精度浮点功能，请使用下面命令：

* make -C ports/stm32 BOARD=PYBCNV2 MICROPY_FLOAT_IMPL=double

编译前，最好先使用clean清除临时文件：

* make -C ports/stm32 BOARD=PYBCNV2 clean

[micropython 中文社区](http://www.micropython.org.cn)
