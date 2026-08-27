# XIAO SAMD21 与 XIAO SAMD21 Plus 引脚差异分析

本文用于评估是否可以用一个 MicroPython 板级定义同时支持普通版 XIAO SAMD21 和 XIAO SAMD21 Plus。

## 结论摘要

- 大部分 MCU 引脚和基础外设映射相同，可以复用同一套基础 pin 定义。
- PA17 和 PA18 的“板级功能语义”不同，不能只把它们当作普通别名差异处理。
- `pins.csv` 允许同一个 MCU 引脚拥有多个名称。例如 `PA18` 可以同时列为 `RX_LED` 和 `D27`。但是别名只解决名称解析，不会解决硬件连接或功能冲突。
- 如果接受用户根据实际硬件选择引脚名称，可以使用一个合并固件；底层驱动无需为两块板分别实现，板级差异主要由 `pins.csv` 别名表达。

## 官方原理图依据

- 普通版：[Seeeduino XIAO v1.0 Schematic](https://files.seeedstudio.com/wiki/Seeeduino-XIAO/res/Seeeduino-XIAO-v1.0-SCH-191112.pdf)
- Plus：[XIAO SAMD21 Plus Schematic](https://files.seeedstudio.com/wiki/Seeeduino-XIAO/res/202004620_XIAO-SAMD21Plus_260422.pdf)

### PA17

| 型号 | 原理图标注 | 板级含义 |
|---|---|---|
| 普通版 | `PA17_W13` | 接到普通版的 W13 网络/焊盘标号；不是 Plus 的第二路 I2C 外部引脚定义 |
| Plus | `PA17/SCL1/D13` | 作为第二路 I2C 的 SCL1，同时是 Plus 外部引脚 D13 |

SAMD21 芯片本身允许 PA17 复用为 SERCOM1 PAD1（可用于 I2C SCL），但“芯片支持该复用”不等于两块板的外部电路功能相同。普通版原理图没有按 Plus 的方式把 PA17 定义为 SCL1 接口。

### PA18

| 型号 | 原理图标注 | 板级含义 |
|---|---|---|
| 普通版 | `PA18_RX_LED` | 连接普通版接收指示灯网络，属于板载 LED 功能 |
| Plus | `PA18/D27` | 外部普通 GPIO，Plus 引出为 D27；板载 RGB LED 不在 PA18 |

因此，普通版程序使用 `RX_LED` 控制 PA18 时，预期是控制板载指示灯；在 Plus 上同样写 PA18/D27 只会改变外部 GPIO 电平，不会产生同样的 LED 效果。

## 当前 MicroPython 定义对比

普通版原始定义中包含：

```text
USER_LED,PA17
RX_LED,PA18
TX_LED,PA19
```

Plus 定义包含：

```text
SCL1,PA17
SDA1,PA16
D12,PA28
RGB_LED,PA27
BUTTON,PB22
```

注意：官方 Plus 原理图（2026-04-22 版本）把 PA18 标为 `D27`，PA28 标为 `D12`。如果当前分支把 `D12` 绑定到 PA18，应以实际硬件版本和官方原理图为准重新校正，不能仅依据旧 PR 中的名称。

## 引脚映射总表

下表按官方原理图上的 MCU 网络名整理；`D/A` 是 Plus 原理图的扩展板编号，不代表普通版也有同名接口。

| MCU 引脚 | 普通版原理图/用途 | Plus 原理图/用途 | 是否可视为同一功能 |
|---|---|---|---|
| PA02 | A0/D0 | A0/D0 | 是 |
| PA04 | A1/D1 | A1/D1 | 是 |
| PA10 | A2/D2 | A2/D2 | 是 |
| PA11 | A3/D3 | A3/D3 | 是 |
| PA08 | A4/D4/SDA | A4/D4/SDA0 | 是 |
| PA09 | A5/D5/SCL | A5/D5/SCL0 | 是 |
| PB08 | A6/D6/TX | A6/D6/TX | 是 |
| PB09 | A7/D7/RX | A7/D7/RX | 是 |
| PA07 | A8/D8/SCK | A8/D8/SCK | 是 |
| PA05 | A9/D9/MISO | A9/D9/MISO | 是 |
| PA06 | A10/D10/MOSI | A10/D10/MOSI | 是 |
| PA16 | MCU 可复用 I2C/Sercom，但未作为普通版 XIAO 第二路 I2C 引脚引出 | SDA1/D14 | 硬件语义不同 |
| PA17 | W13；普通版板级网络/引脚名 | SCL1/D13 | 否 |
| PA18 | RX_LED，普通版板载接收指示灯网络 | D27，Plus 外部 GPIO | 否 |
| PA19 | TX_LED，普通版板载发送指示灯网络 | I2S_SD/D19（按 Plus 原理图） | 否 |
| PA27 | 普通版未作为板载 RGB LED | RGB_LED，连接 WS2812 RGB LED | 否 |
| PA28 | 普通版未作为 Plus 扩展编号 | D12 | 否/新增 |
| PB22 | 普通版未作为用户按键 | User_Button | 否/新增 |

普通版和 Plus 的基础 `A0~A10`、UART、SPI、I2C0 映射基本相同；差异集中在 Plus 扩展排针、第二路 I2C、RGB LED、按键，以及 PA17/PA18/PA19 的板级用途。

## 一个引脚可以有两个别名吗？

可以。`ports/samd/boards/make-pins.py` 使用通用 `tools/boardgen.py` 解析 `pins.csv`，同一 CPU pin 可以出现多行。例如：

```csv
RX_LED,PA18
D27,PA18
```

生成后，Python 侧可以通过不同名称取得同一个 Pin 对象：

```python
from machine import Pin

Pin("RX_LED")  # 普通版语义名称
Pin("D27")     # Plus 语义名称
```

这两个对象指向同一个物理 MCU 引脚 PA18。别名不会：

1. 改变引脚的电气连接；
2. 自动检测当前插入的是普通版还是 Plus；
3. 让普通版的 LED 在 Plus 上出现；
4. 防止两个驱动同时配置同一个引脚。

同理，PA17 可以同时写成 `PA17_W13`、`SCL1` 或 `D13`，但这些名称表达的是不同的板级语义，是否应该公开必须谨慎。

## 合并方案评估

### 方案 A：单一固件（当前目标）

继续使用 `SEEED_XIAO_SAMD21` 作为唯一 board target，在同一个 `pins.csv` 中同时提供普通版和 Plus 的名称：

```csv
SCL1,PA17
SDA1,PA16
W13,PA17
PA17_W13,PA17
RX_LED,PA18
D27,PA18
D12,PA28
RGB_LED,PA27
BUTTON,PB22
```

但建议同时采取以下限制：

- `USER_LED/RX_LED/TX_LED` 继续保留，保证普通版旧代码可解析；但这些名称在 Plus 上没有对应的板载 LED 语义。
- Plus 程序使用 `SDA1/SCL1`、`D12~D27`、`RGB_LED` 和 `BUTTON`；普通版程序使用原有 `A0_D0~A10_D10`、`USER_LED` 等名称。
- 不需要修改 SAMD21 底层 GPIO、I2C、SPI 或 UART 驱动；这些名称最终都指向同一个 `Pin` 对象。
- 必须在两块实机上验证 PA17、PA18、I2C1、D12/D27、RGB_LED 和 BUTTON。

### 不采用：运行时自动识别

SAMD21 没有一个可直接用于可靠识别两种 PCB 的标准 ID 引脚。仅靠读取 PA17/PA18 电平无法区分板型，因为这些脚可能被外部电路、LED 或用户负载影响。除非硬件提供明确的识别电阻/ID 网络，否则不要用运行时猜测代替独立 board 定义。

## 建议的代码组织

当前单固件方案不需要增加第二个 board 目录，继续维护现有目录即可：

```text
ports/samd/boards/
  SEEED_XIAO_SAMD21/
    pins.csv       # 普通版和 Plus 的全部公共/扩展别名
    mpconfigboard.h
```

`mpconfigboard.h` 和底层外设驱动保持不变；公共 UART、SPI、I2C0、USB 以及普通版/Plus 的 GPIO 名称都由 `pins.csv` 提供。这样适配工作集中在板级映射，不引入运行时板型识别。

## 最终建议

从硬件兼容性和用户程序兼容性看，PA17/PA18 不是简单的“一个引脚增加两个别名”问题：

- PA17：普通版是 W13 网络，Plus 是 SCL1/D13；
- PA18：普通版是 RX_LED，Plus 是外部 GPIO D27；
- 别名技术上可以同时存在，但语义和电路功能不会合并。

因此，按当前目标可以合并为单一 board 定义：保留普通版名称，补充 Plus 的完整 GPIO 别名；底层驱动保持不变。需要接受的事实是，同名的 `USER_LED/RX_LED` 在 Plus 上只是指向相同 MCU pad 的兼容名称，并不意味着 Plus 存在普通版对应的 LED 电路。
