# MPY ST103 STM32F103ZGT

MPY ST103 (STM32F103ZGT)的 MicroPython 开发板定义文件

### 构建与烧写固件:

* 克隆MicroPython stmf103 (https://github.com/lonphy/micropython)

```bash
git clone https://github.com/lonphy/micropython
cd micropython
make -C mpy-cross
cd micropython/ports/stm32f1
make BOARD=MPY_ST103
```


TODO:
> 自行安装arm-none-eabi-gcc工具 与 Python环境

* 断开开发板的供电
* Boot0(J6-1脚) 接 3v3(J5)
* Boot1(J6-2脚) 接 gnd(J7)
* 开发板供电

```
cd micropython/ports/stm32f1
make BOARD=YeHuo_F103VE deploy
```

* 断开开发板的供电
* Boot0(J6-1脚) 接 gnd(J7)
* 开发板供电

使用开发板的USB Mini口(J11)连接电脑，打开串口工具开始你的repl之旅