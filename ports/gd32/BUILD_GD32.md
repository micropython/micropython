# GD32F407 / GD32F470 手动编译指南

本文档描述如何从零开始手动编译本 port 的固件。所有命令与耗时数据均为实际验证结果。

- 仓库：`micropython`（GD32F4xx port）
- 版本：MicroPython v1.30.0
- 分支：`gd32-port`
- 文档对应提交：`518303050`

---

## 目录

1. [环境要求](#1-环境要求)
2. [获取代码](#2-获取代码)
3. [安装交叉编译工具链](#3-安装交叉编译工具链)
4. [首次编译](#4-首次编译)
5. [编译产物说明](#5-编译产物说明)
6. [常用编译命令](#6-常用编译命令)
7. [配置开关](#7-配置开关)
8. [烧录固件](#8-烧录固件)
9. [验证固件](#9-验证固件)
10. [故障排查](#10-故障排查)
11. [附录：完整变量参考](#11-附录完整变量参考)

---

## 1. 环境要求

### 1.1 硬件

| 项目 | 最低要求 | 说明 |
|------|---------|------|
| CPU 架构 | x86_64 或 aarch64 | 工具链需与主机架构匹配，见下方警告 |
| 内存 | 2 GB | 1.9 GB 实测可用，**编译必须用 `-j1`** |
| 磁盘 | 5 GB 可用 | 完整编译约需 1.2 GB（含 mbedtls） |

> **架构警告**：如果你从 ARM 官网下载工具链，必须选择与主机一致的版本。
> 在 aarch64（ARM64）主机上下载 `x86_64` 版本会导致二进制无法执行。

### 1.2 软件

实测通过的组合：

| 软件 | 版本 | 备注 |
|------|------|------|
| 操作系统 | Ubuntu 20.04 LTS | 其他 Linux 发行版同理 |
| arm-none-eabi-gcc | **9.2.1** (15:9-2019-q4-0ubuntu1) | apt 版本，够用 |
| binutils-arm-none-eabi | 2.34-4ubuntu1+13ubuntu1 | |
| libnewlib-arm-none-eabi | 3.3.0-0ubuntu1 | |
| libnewlib-dev | 3.3.0-0ubuntu1 | 提供 newlib 头文件 |
| Python | 3.8.2 | 用于 qstr 生成、manifest 处理 |
| GNU Make | 4.2.1 | |
| 主机 gcc | 9.3.0 | 编译 mpy-cross 需要 |

---

## 2. 获取代码

### 2.1 克隆仓库

```bash
git clone https://github.com/GEEK174/micropython.git
cd micropython
git checkout gd32-port
```

### 2.2 初始化子模块

**这一步不能跳过**，否则编译时报缺文件。本 port 只需要以下子模块：

```bash
git submodule update --init \
    lib/lwip \
    lib/mbedtls \
    lib/micropython-lib \
    lib/tinyusb \
    lib/berkeley-db-1.xx \
    lib/libhydrogen \
    lib/CMSIS_6 \
    lib/stm32lib
```

验证：

```bash
git submodule status | grep -v '^-'
```

应至少看到这 8 个子模块没有 `-` 前缀。**`lib/mbedtls` 是 TLS 功能必需的，`lib/lwip` 是网络功能必需的**。

### 2.3 应用 TinyUSB 补丁（必需）

GD32F4 的 USB 支持需要给 TinyUSB 打补丁，补丁已随仓库提供：

```bash
cd lib/tinyusb
git apply ../../ports/gd32/0001-tinyusb-add-GD32F4-USBFS.patch
cd ../..
```

> 补丁针对 TinyUSB 提交 `b549ac1d` 验证过可干净应用。
> 若上游更新导致锚点移动，用 `git apply -3` 尝试三方合并。

如需让子模块 `git status` 保持干净，可额外提交：

```bash
cd lib/tinyusb
git add -A && git commit -m "dwc2: add GD32F4 (Cortex-M4) USBFS support"
cd ../..
```

---

## 3. 安装交叉编译工具链

### 3.1 安装

```bash
sudo apt-get update
sudo apt-get install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libnewlib-dev \
    build-essential \
    python3
```

### 3.2 验证

```bash
arm-none-eabi-gcc --version
# 应输出: arm-none-eabi-gcc (15:9-2019-q4-0ubuntu1) 9.2.1 ...

arm-none-eabi-objcopy --version | head -1
arm-none-eabi-size --version | head -1
```

### 3.3 修复：newlib 头文件不可见（常见坑）

如果在编译时报 `fatal error: string.h: No such file or directory`，说明 `dpkg` 处于未配置完成状态：

```bash
sudo dpkg --configure -a
```

验证修复：

```bash
echo '#include <string.h>' | arm-none-eabi-gcc -E -x c - >/dev/null && echo OK
```

### 3.4 编译 mpy-cross（一次性）

`mpy-cross` 是 MicroPython 的字节码编译器，冻结模块需要它：

```bash
cd mpy-cross
make -j1
cd ..
```

产物：`mpy-cross/build/mpy-cross`

验证：

```bash
./mpy-cross/build/mpy-cross --version
# MicroPython v1.30.0 on ...
```

---

## 4. 首次编译

### 4.1 进入 port 目录

```bash
cd ports/gd32
```

### 4.2 编译 GD32F407

```bash
make BOARD=GD32F407 -j1
```

**实测耗时：166 秒**（干净编译，单线程）

### 4.3 编译 GD32F470

```bash
make BOARD=GD32F470 -j1
```

### 4.4 成功输出示例

```
LINK build-GD32F407/firmware.elf
Memory region         Used Size  Region Size  %age Used
           FLASH:      375208 B       512 KB     71.57%
             RAM:       90888 B       128 KB     69.34%
          TCMRAM:          0 GB        64 KB      0.00%
   text	   data	    bss	    dec	    hex	filename
 375208	    240	  90888	 466336	  71da0	build-GD32F407/firmware.elf
BIN build-GD32F407/firmware.bin
HEX build-GD32F407/firmware.hex
```

### 4.5 一键编译全部板子

```bash
./build.sh all
```

或指定单块：

```bash
./build.sh GD32F407
```

`build.sh` 内部调用 `make BOARD=<board> -j1 dist`，产物会自动导出到 `dist/<board>/`。

### 4.6 编译耗时参考

| 场景 | 耗时 |
|------|------|
| 首次干净编译（含 mbedtls） | ~166 秒 |
| 仅改一个 .c 文件后重编 | 3-10 秒 |
| mpy-cross 首次编译 | ~60 秒 |

---

## 5. 编译产物说明

编译成功后，产物位于 `build-<BOARD>/`：

| 文件 | 大小（实测） | 用途 |
|------|-------------|------|
| `firmware.elf` | 6.4 MB | 含调试符号，用于调试和烧录 |
| `firmware.bin` | 367 KB | 裸二进制，烧录到 `0x08000000` |
| `firmware.hex` | 1.1 MB | Intel HEX 格式 |
| `firmware.elf.map` | ~1 MB | 内存映射文件，排查空间问题用 |

导出到 `dist/`：

```bash
make BOARD=GD32F407 dist
# 产物复制到 dist/GD32F407/
```

---

## 6. 常用编译命令

```bash
cd ports/gd32

# 编译指定板子（必须指定 BOARD）
make BOARD=GD32F407 -j1

# 编译并导出产物
make BOARD=GD32F407 -j1 dist

# 干净重编（删掉 build-<BOARD> 目录）
make BOARD=GD32F407 clean
make BOARD=GD32F407 -j1

# 一口气清理全部板子的构建目录
rm -rf build-GD32F407 build-GD32F470 dist

# 查看固件大小
arm-none-eabi-size build-GD32F407/firmware.elf

# 查看各段详细布局
arm-none-eabi-objdump -h build-GD32F407/firmware.elf | head -20

# 反汇编查看某个函数
arm-none-eabi-objdump -d build-GD32F407/firmware.elf | sed -n '/<SystemInit>:/,/^$/p'

# 列出固件中的符号（排查功能是否编入）
arm-none-eabi-nm build-GD32F407/firmware.elf | grep machine_
```

### 关于 `-j1`

**必须使用单线程编译。** 实测在 1.9 GB 内存的机器上：
- `make -j4` → 编译过程触发 OOM，SSH 连接被重置
- `make -j1` → 稳定，166 秒完成

如果你在内存更大的机器上编译，可以尝试 `-j2` 或 `-j4`，但需自行确认稳定性。

---

## 7. 配置开关

Makefile 中以下变量可用 `?=` 覆盖，直接在命令行指定即可，无需改文件。

### 7.1 关闭 TLS（已实测可用）

```bash
make BOARD=GD32F407 -j1 MICROPY_PY_SSL=0
```

**实测收益：Flash 从 375,208 B 降到 253,596 B，节省 118.8 KB。**

关闭后 `import tls` 不可用，但网络（`socket`、`network`）不受影响。

### 7.2 关闭网络 lwIP —— ⚠️ 当前版本不支持

```bash
# ❌ 这条命令会编译失败，不要使用
make BOARD=GD32F407 -j1 MICROPY_PY_LWIP=0
```

**原因**：以下 4 个文件的 `#include "lwip/..."` 是无条件引入的，且 `eth.c` / `network_lan.c` 被无条件加入 `SRC_C`：

| 文件 | lwIP 依赖处数 |
|------|--------------|
| `eth.c` | 3 |
| `network_lan.c` | 2 |
| `eth.h` | 1 |
| `mphalport.c` | 1 |

关闭 lwIP 后会在预处理阶段报 `fatal error: lwip/netif.h: No such file or directory`。

**如果确实需要无网络版本**，需要先修改代码：给上述 include 加 `#if MICROPY_PY_LWIP` 包裹，并在 Makefile 中条件加入 `eth.c` / `network_lan.c`。这属于代码改动，不是单纯改编译参数。

### 7.3 冻结模块

```bash
# 默认：冻结 manifest.py 中列出的模块
make BOARD=GD32F407 -j1

# 使用自定义清单
make BOARD=GD32F407 -j1 FROZEN_MANIFEST=/path/to/my_manifest.py
```

**关于关闭冻结**：`FROZEN_MANIFEST=` 传空值会链接失败：

```
undefined reference to `mp_qstr_frozen_const_pool'
```

原因是 `mpconfigport.h` 里定义了 `MICROPY_QSTR_EXTRA_POOL mp_qstr_frozen_const_pool`，而该符号由 `frozen_content.c` 提供。若不需要冻结，应清空 `manifest.py` 的内容而不是传空路径——保留空的 manifest 仍会生成一个（几乎为空的）`frozen_content.c`，符号依然存在。

最简单可靠的做法：直接编辑 `manifest.py`，把 `module(...)` / `package(...)` 行注释掉。

### 7.3 自定义输出目录

```bash
make BOARD=GD32F407 BUILD=build-custom -j1
```

### 7.4 指定工具链前缀

```bash
make BOARD=GD32F407 CROSS_COMPILE=arm-none-eabi- -j1
```

---

## 8. 烧录固件

### 8.1 使用 ST-Link / J-Link / DAPLink（OpenOCD）

GD32F4 的调试接口与 STM32F4 兼容，可直接用 `stm32f4x` 目标：

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
        -c "program build-GD32F407/firmware.elf verify reset exit"
```

### 8.2 使用 DFU（USB）

GD32F4 支持系统存储区 DFU 引导（BOOT0 拉高进入）：

```bash
dfu-util -a 0 -s 0x08000000:leave -D build-GD32F407/firmware.bin
```

### 8.3 使用 pyocd

```bash
pyocd flash -t gd32f407 build-GD32F407/firmware.hex
```

---

## 9. 验证固件

### 9.1 接线

REPL 走 USART0：

| USB 串口模块 | MCU 引脚 | 功能 |
|-------------|---------|------|
| RX | PA9 | USART0 TX |
| TX | PA10 | USART0 RX |
| GND | GND | 共地 |

串口参数：**115200 8N1**

### 9.2 上电后应看到

```
GD32F407 with GD32F407VE
MicroPython v1.30.0 on 2026-11-06; GD32F407 with GD32F407VE
>>>
```

### 9.3 冒烟测试

```python
import sys
print(sys.platform)          # gd32

import machine
print(machine.freq())        # 168000000 (F407) / 240000000 (F470)
print(machine.unique_id().hex())

from machine import Pin
p = Pin("PA0", Pin.OUT)
p.toggle()

import os
print(os.listdir("/"))       # 文件系统根目录

import network                 # 若启用了 lwIP
nic = network.LAN()
print(nic.status())

import tls                     # 若启用了 TLS
print(tls.SSLContext)

import onewire
print(onewire.OneWire)
```

### 9.4 检查 Flash/RAM 余量

```bash
arm-none-eabi-size build-GD32F407/firmware.elf
```

对照约束：

| 资源 | 上限 | 说明 |
|------|------|------|
| Flash | `0x08060000` 之前（384 KB） | 超出会被链接脚本断言拦下 |
| RAM | 128 KB（F407）/ 192 KB（F470） | |

---

## 10. 故障排查

### 10.1 `Makefile:5: *** BOARD not specified`

必须显式指定 `BOARD`：

```bash
make BOARD=GD32F407 -j1
```

### 10.2 `fatal error: gd32f4xx.h: No such file or directory`

子模块或 GD32 库未就绪。检查：

```bash
ls ports/gd32/gd32lib/CMSIS_include/gd32f4xx.h
```

若缺失，说明仓库克隆不完整，重新执行 2.2 的步骤。

### 10.3 `fatal error: string.h: No such file or directory`（ARM 编译时）

newlib 头文件未配置：

```bash
sudo dpkg --configure -a
```

### 10.4 `error: "MICROPY_HW_FLASH_WS" redefined`

Makefile 与 board 头文件重复定义。检查 `boards/<BOARD>/mpconfigboard.h` 中**不应**有该宏——它由 Makefile 按 `BOARD` 注入。

### 10.5 `FIRMWARE TOO LARGE: image overruns the internal littlefs partition`

固件超过 384 KB，会覆盖文件系统分区。实测可用的解决办法：

```bash
# 关闭 TLS —— 实测节省 118.8 KB，固件回到 253,596 B
make BOARD=GD32F407 -j1 MICROPY_PY_SSL=0
```

> 注意：`MICROPY_PY_LWIP=0` 当前版本**不可用**（见 7.2），无法用这种方式省空间。

如需其他方式腾空间，考虑减少 `manifest.py` 中冻结的模块，或修改分区布局（需同时改 `flashbdev.c` 和链接脚本）。

### 10.6 编译时 SSH 断开 / OOM

内存不足。确认使用 `-j1`：

```bash
make BOARD=GD32F407 -j1
```

### 10.7 `undefined reference to 'mp_module_xxx'`

某个模块的源文件没进构建。检查 `extmod.mk` 中的条件，或确认 `mpconfigport.h` 里对应 `MICROPY_PY_*` 宏已开启。

排查方法：

```bash
# 看某符号是否在固件里
arm-none-eabi-nm build-GD32F407/firmware.elf | grep mp_module_tls
```

### 10.8 USB 不枚举

检查 TinyUSB 补丁是否应用：

```bash
cd lib/tinyusb
git status --short src/portable/synopsys/dwc2/
# 应看到 dwc2_gd32f4.h
```

---

## 11. 附录：完整变量参考

### 11.1 Makefile 可覆盖变量

| 变量 | 默认值 | 说明 |
|------|--------|------|
| `BOARD` | **必须指定** | `GD32F407` 或 `GD32F470` |
| `BUILD` | `build-$(BOARD)` | 构建目录 |
| `FROZEN_MANIFEST` | `manifest.py` | 冻结清单，设为空可关闭 |
| `CROSS_COMPILE` | `arm-none-eabi-` | 工具链前缀 |
| `MICROPY_PY_LWIP` | `1` | 网络支持。**当前不可设为 0**，见 7.2 |
| `MICROPY_PY_SSL` | `1` | TLS 支持，设为 0 可省 118.8 KB Flash（实测） |
| `MICROPY_SSL_MBEDTLS` | `1` | TLS 后端（仅 mbedTLS 可用） |
| `MICROPY_PY_LWIP_REPEAT_ARP` | `1` | 重复 ARP 请求 |

### 11.2 编译参数

```makefile
MCU_ARCH = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
```

- Cortex-M4F，硬浮点，单精度 FPU（`fpv4-sp-d16`）
- 优化级别 `-Os`，开启 `-ffunction-sections -fdata-sections` 便于 `--gc-sections` 裁剪

### 11.3 板级配置对照

| 项目 | GD32F407 | GD32F470 |
|------|----------|----------|
| MCU | GD32F407VE | GD32F470VE |
| 频率 | 168 MHz | 240 MHz |
| Flash | 512 KB | 512 KB |
| RAM | 128 KB | 192 KB |
| 启动文件 | `startup_gd32f407_427.S` | `startup_gd32f450_470.S` |
| 链接脚本 | `gd32f405_407_xE_flash.ld` | `gd32f470xE_flash.ld` |
| Flash 等待周期 | 5 | 7 |
| Python heap | 48 KB | 48 KB |
| 编译宏 | `-DGD32F407` | `-DGD32F470` |

### 11.4 内存分区布局

```
Flash 总量 512 KB:
  0x08000000 - 0x0805FFFF   固件（384 KB，链接脚本断言保护此处）
  0x08060000 - 0x0807FFFF   littlefs 文件系统（128 KB，Sector 7）

RAM:
  0x20000000 - ...          数据 + bss + heap + stack
  0x10000000 - 0x1000FFFF   TCMRAM 64 KB（当前未使用）
```

### 11.5 当前固件占用（完整功能）

```
Flash: 375,208 B / 384 KB  (95.5%, 剩余 17.3 KB)
RAM  :  90,888 B / 128 KB  (69.3%, 剩余 39.2 KB)   ← F407
        90,888 B / 192 KB  (46.2%, 剩余 103 KB)    ← F470
```

> 注：`arm-none-eabi-size` 输出的 text+data 为 375,208 B，而链接器 Memory region 报告为 375,504 B，
> 两者差值是段对齐填充。判断是否超限以链接器报告的 FLASH 值为准（上限 393,216 B = 0x08060000）。

---

## 快速上手（TL;DR）

```bash
# 1. 安装工具链
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
                        libnewlib-arm-none-eabi libnewlib-dev build-essential
sudo dpkg --configure -a

# 2. 获取代码 + 子模块
git clone https://github.com/GEEK174/micropython.git && cd micropython
git checkout gd32-port
git submodule update --init lib/lwip lib/mbedtls lib/micropython-lib \
    lib/tinyusb lib/berkeley-db-1.xx lib/libhydrogen lib/CMSIS_6 lib/stm32lib

# 3. TinyUSB 补丁
cd lib/tinyusb && git apply ../../ports/gd32/0001-tinyusb-add-GD32F4-USBFS.patch && cd ../..

# 4. mpy-cross
make -C mpy-cross -j1

# 5. 编译
cd ports/gd32
make BOARD=GD32F407 -j1 dist

# 产物在 dist/GD32F407/
```
