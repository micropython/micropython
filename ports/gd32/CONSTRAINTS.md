# GD32 端口：资源约束与设计取舍

> 本文记录 GD32F4xx 端口的硬性约束、已经做出的取舍，以及升级/部署时必须注意的事项。
> 所有数字均为实测，不是估算。

## 1. Flash 预算

### 1.1 分区布局

512 KB Flash 被切成三段（`ld/*.ld` 中定义，`flashbdev.c` 通过链接符号读取）：

| 区域 | 扇区 | 大小 | 地址范围 |
|------|------|------|----------|
| `.vectors` | 0 | 16 KB | `0x08000000`–`0x08003FFF` |
| littlefs | 1–3 | 48 KB | `0x08004000`–`0x0800FFFF` |
| 固件 | 4–7 | **448 KB** | `0x08010000`–`0x0807FFFF` |

向量表必须位于 `0x08000000`，所以单独占一个区域。文件系统紧跟其后、代码最后，
这样代码区不受文件系统大小拖累——这个布局思路与 `ports/stm32` 的
`FLASH_START` / `FLASH_FS` / `FLASH_TEXT` 相同。

> 早期版本是"固件在下、文件系统在 S7"（384 KB 上限）。改成现在这样是为了给浮点腾空间，
> 代价是文件系统从 128 KB 缩到 48 KB。

### 1.1.1 逻辑块大小与物理扇区解耦（2026-11-08 改动）

**分区边界没变（仍是 48 KB），但 littlefs 看到的块数从 3 变成 12。**

旧方案把逻辑块大小设为等于物理扇区（16 KB），于是 48 KB 只有 3 个块。
其中 2 个被 littlefs 的根目录元数据对占用，只剩 1 个空闲块，导致：

- 任何 >128 B 的文件（超过 littlefs 内联阈值）独占一整块
- 该块被占后，**同一个文件的第二次保存必然失败**：`LFS2_ERR_NOSPC`（`OSError: 28`）
- 表现就是 Thonny 能打开文件但保存不了 `/main.py`

现在 `flashbdev.c` 引入 **16 KB 的 RAM 扇区缓存**，逻辑块改为 **4 KB**：

| 项目 | 旧 | 新 |
|------|-----|-----|
| 逻辑块大小 | 16 KB | **4 KB** |
| 块数 | 3 | **12** |
| 内联阈值 | 128 B | 128 B（不变） |
| RAM 占用 | — | +16 KB（`.bss`） |
| Flash 占用 | — | +约 280 B |

**机制**（借鉴 `ports/stm32/flashbdev.c`，但它的内部 flashbdev 用 FAT、不实现
`BLOCK_ERASE`，而 littlefs 必须有擦除语义，故做了适配）：

1. 缓存持有**一个完整物理扇区**的镜像（16 KB）
2. `writeblocks` / `BLOCK_ERASE` 只改缓存，标记 dirty，**不碰 flash**
3. 切换到另一个扇区、或 `SYNC` / `DEINIT` 时才冲刷：先整扇区擦除，再把缓存整体写回

**关键正确性约束（踩过坑，务必保留）**：

> **缓存必须无条件预读旧内容，不能因为是写操作就跳过预读。**
>
> 最初为省时间让写路径跳过预读，结果：写逻辑块 A 时，同扇区的逻辑块 B/C/D
> 的内容在缓存里是垃圾；冲刷时整扇区被覆盖，B/C/D 全部损坏。
> 实测症状极具迷惑性——文件长度正确（280 B），但内容是
> `\x02\x00\x00\x00\xf0\x0f\xff\xf7littlefs`（原始 superblock 字节）。
>
> 规则：`flash_bdev_cache_load()` 一律先从 flash 拷贝整扇区，再允许修改。

另外两点：

- `machine.Flash.mkfs()` 前必须**作废缓存**（`cache_sector = -1`），
  否则旧缓存会在格式化后被冲刷回去，静默破坏新文件系统。
- 读操作也必须走缓存，dirty 扇区尚未落盘，直接读物理地址会读到陈旧数据。

**`readsize` 为什么不调大**：littlefs 的内联阈值是
`cache_size = MIN(block_size, 4 * MAX(read_size, prog_size))`，默认 32 字节 I/O
把内联上限压在 128 B。调到 256 可把上限提到 1 KB，小文件就不占块了。实测过，
但 **`cache_size` 是按打开的文件分配的**（1024 B/文件），本板只剩约 15 KB RAM，
多开几个文件就 OOM，因此回退为默认值。块数从 3→12 已经解决了主要问题。

**升级注意**：块几何变了（3×16K → 12×4K），littlefs 的 superblock 记录
`block_size`/`block_count`，不匹配会拒绝挂载（见 §4.1）。刷入此固件后
`/` 会是空的，需要重新 `machine.Flash.mkfs()`。


### 1.2 当前占用

| 板型 | 已用 | 上限 | 余量 |
|------|------|------|------|
| GD32F407 | 452176 B | 458752 B | **6576 B (1.4%)** |
| GD32F470 | 450504 B | 458752 B | 8020 B |

**余量已不足 8 KB。** 任何新功能都必须先腾出空间。

### 1.3 空间构成（链接后实测，非 .o 大小）

按功能域统计真正进入固件的字节数：

| 模块 | 占用 | 占比 |
|------|------|------|
| Python 核心 | 172306 B | 41.0% |
| **mbedTLS** | **91181 B** | **21.7%** |
| extmod | 62088 B | 14.8% |
| lwIP | 24448 B | 5.8% |
| 端口自身 | 20796 B | 5.0% |
| littlefs | 15018 B | 3.6% |
| GD32 外设库 | 11539 B | 2.7% |
| TinyUSB | 10226 B | 2.4% |
| FatFs | 6435 B | 1.5% |
| shared | 5951 B | 1.4% |

最大单文件：`py/qstr.o` 41248 B（qstr 字符串池）、`lfs2.o` 14846 B、
`compile.o` 12674 B、`emitnthumb.o` 10763 B。

### 1.4 ⚠️ 判断精简空间的正确方法

**不要读 `.o` 文件大小**——`--gc-sections` 会丢弃从未被引用的代码，`.o` 总和
（483897 B）比实际固件（452176 B）多 31 KB。

必须解析 `build-<BOARD>/firmware.elf.map` 或用 `nm --print-size` 看链接后的符号。

实例：`.o` 显示 mDNS 占 11 KB、IPv4 分片占 2.2 KB，但实测关闭它们**节省 0 字节**
——这些代码从未被链接（mDNS 需要应用层调用 `mdns_resp_init` 才生效，本端口没调用）。

### 1.5 实测可行的精简项

| 方案 | 实测节省 | 代价 |
|------|----------|------|
| 关闭 DTLS | **−5944 B** | 失去 UDP-TLS；TCP-TLS 不受影响 |
| FatFs 关闭长文件名 | **−3024 B** | FAT 只支持 8.3 短名 |
| 关闭 mDNS | 0 B | 无效（已被回收） |
| 关闭 IPv4 分片 | 0 B | 无效（已被回收） |
| 关闭 lwIP DNS | 编译失败 | `modlwip.c` 依赖 `LWIP_DNS_ADDRTYPE_*` |
| ROM level 降到 CORE | 编译失败 | 会关掉 finaliser，VFS 依赖它 |
| mbedTLS 去服务端 | 编译失败 | `modtls_mbedtls.c` 需要 DTLS cookie/PSK 符号 |

两项合计最多约 **9 KB**。

### 1.6 若要大幅释放空间（结构性取舍）

| 方案 | 可释放 | 影响 |
|------|--------|------|
| 关闭 TLS 全部 | ~91 KB | 失去 HTTPS/MQTT-TLS |
| 关闭 lwIP + 网络 | ~24 KB + extmod 网络部分 | 失去 `network`/`socket` |
| littlefs 48 KB → 16 KB | 32 KB 代码空间 | 文件系统仅剩 16 KB（3×16K 是 littlefs 下限） |

---

## 2. RAM 预算

| 板型 | 静态占用 (.data+.bss) | 总量 | 余量 |
|------|----------------------|------|------|
| GD32F407 | 116048 B | 131072 B | 15024 B (11%) |
| GD32F470 | 91480 B | 196608 B | 105128 B (54%) |

主要占用项：

| 项 | 大小 |
|----|------|
| MicroPython 堆（`MICROPY_HEAP_SIZE`） | 48 KB |
| lwIP `ram_heap`（`MEM_SIZE`） | 16 KB |
| littlefs / FatFs 缓冲 | ~8 KB |
| `machine_uart_obj`（6 路 × 256B RX 环形缓冲） | 1872 B |
| mDNS 表、lwIP PCB 池等 | 若干 |

**RAM 不是当前瓶颈**，Flash 才是。

---

## 3. 已做出的取舍

### 3.1 浮点：启用（硬件 FPU）

`MICROPY_FLOAT_IMPL = FLOAT`。GD32F4 是 Cortex-M4F，`MCU_ARCH` 已带
`-mfloat-abi=hard -mfpu=fpv4-sp-d16`，所以是真硬件浮点
（固件中有 736 条 `vadd.f32`/`vmul.f32` 指令），不是软件模拟。

代价约 32.7 KB。早期 384 KB 布局下放不下，改成 448 KB 布局后才启用。

### 3.2 `_thread`：不启用（已评估并放弃）

**评估结论：不引入。**

理由：

1. **官方标注"highly experimental"**，API 未稳定
2. **最成熟的 STM32 端口自己设 `MICROPY_PY_THREAD=0`**（尽管它有完整的
   `mpthreadport.c` 和 `pybthread.c`）。只有 esp32/rp2/zephyr 启用——而它们
   依赖 RTOS 或双核
3. **成本超标**：框架 4–6 KB（余量仅 8 KB），RAM 6.6 KB/线程
   （4 KB 栈 + 2 KB pystack + 状态）
4. **与现有功能冲突且无法验证**：
   - `machine.bitstream` 靠 DWT 周期计数器做**不可抢占**时序
     （NeoPixel 容限 ±150 ns），抢占会毁掉波形
   - `UART.irq`、`time_pulse_us`、DHT 同样依赖中断时序
   - 没有硬件可验证这些交互，引入抢占式调度等于埋伏隐蔽 bug

**替代方案（均已启用）**：`asyncio`（MicroPython 官方推荐的 MCU 并发模型）、
`machine.Timer` 回调、`Pin.irq`/`UART.irq`、`collections.deque`、
`machine.disable_irq()`。

### 3.3 `os.VfsRom`：不启用

ROMFS 需要一个只读镜像数据源才能挂载，本端口没有。打开开关只会得到一个
无法挂载的空文件系统。

### 3.4 `pyb` / `stm` 模块：不实现

STM32 专有 API（`pyb` 是 ST 板级封装，`stm` 暴露 STM32 寄存器名），
在 GD32 上没有对应语义。

### 3.5 蓝牙：不实现

GD32F4 无片上 BLE 射频，需要外挂模块。

### 3.6 网络：仅 LAN，无 PPP

lwIP 已接入（`network.LAN`），但 PPP 拨号未实现且空间不足。

---

## 4. ⚠️ 升级与部署注意事项

### 4.1 分区变更导致旧文件系统数据丢失

**这是最重要的一条。**

旧固件：文件系统在 Sector 7（`0x08060000`，128 KB × 1 块）
新固件：文件系统在 Sectors 1–3（`0x08004000`，16 KB × 3 块）

两个问题叠加：

1. **物理地址重叠**：新固件的 `.rodata` 从 `0x080674f0` 开始，落在旧 FS 区域内
   （`0x08060000`–`0x0807FFFF`）。烧写新固件时，Sector 6/7 会被覆盖，
   **旧数据必然损毁，无法保留**。

2. **几何参数不匹配**：即使数据侥幸留存，littlefs 挂载时会比对 superblock
   与配置参数（`lfs2.c` 的 `lfs2_mount_`）：

   ```c
   if (lfs2->cfg->block_count && superblock.block_count != lfs2->cfg->block_count) {
       LFS2_ERROR("Invalid block count ...");
       return LFS2_ERR_INVAL;      // 挂载失败
   }
   if (superblock.block_size != lfs2->cfg->block_size) {
       LFS2_ERROR("Invalid block size ...");
       return LFS2_ERR_INVAL;      // 挂载失败
   }
   ```

**行为**：挂载失败 → `main.c` 的 `mount_internal_fs()` 吞掉异常 → `/` 为空目录
→ `_boot.py` 打印 `littlefs: root not mounted`。REPL 仍可用。

**恢复**：需要重新格式化文件系统（见 4.3）。

### 4.2 全新板子的首次启动

Flash 全 `0xFF` → 扫描不到 superblock → `lfs2_mount` 返回 `LFS2_ERR_NOENT`
→ 挂载失败 → `/` 为空 → `_boot.py` 打印：

```
littlefs: root is empty (run machine.Flash.mkfs() to format)
```

REPL 正常可用，但文件操作会失败，直到格式化。

### 4.3 格式化文件系统

```python
import machine
machine.Flash().mkfs()
```

**但是——`mkfs` 在文件系统已挂载时是不安全的。** 见下面 4.4。

安全的格式化流程：

```python
import machine, os, vfs
os.umount('/')                    # 先卸载
machine.Flash().mkfs()            # 再格式化
os.mount(machine.Flash(), '/')    # 重新挂载（或直接软复位）
```

或者格式化后直接 `machine.reset()`。

### 4.4 🐛 已知问题：已挂载时调用 mkfs 会损坏文件系统

**现象**：在 `/` 正常挂载的情况下直接 `machine.Flash().mkfs()`，格式化会
成功执行，但已挂载的 VFS 实例仍持有旧的 littlefs 状态（root 目录指针、
block 位图等）。之后任何文件操作都会作用于已被擦除并重写的块。

**根因**（`extmod/vfs_lfsx.c:149`）：

```c
static mp_obj_t MP_VFS_LFSx(mkfs)(...) {
    MP_OBJ_VFS_LFSx self;                         // 栈上的临时对象
    MP_VFS_LFSx(init_config)(&self, ...);
    int ret = LFSx_API(format)(&self.lfs, &self.config);   // 直接格式化
    ...
}
```

它**不检查块设备是否已被挂载**，也不通知已挂载的实例。这是上游
MicroPython 的通用行为，不是本端口引入的。

**后果**：轻则随后的文件操作报 `OSError`，重则 littlefs 断言失败或静默数据损坏。
掉电或复位后重新挂载通常能恢复（因为磁盘上是刚格式化好的干净文件系统）。

**规避**：务必先 `os.umount('/')` 再 `mkfs()`，或格式化后立即 `machine.reset()`。

### 4.5 烧写产物说明

因为向量表和代码不再连续，`firmware.bin` 不覆盖整个镜像：

| 文件 | 烧写到 | 内容 |
|------|--------|------|
| `firmware-isr.bin` | `0x08000000` | 仅向量表（约 392 B） |
| `firmware.bin` | `0x08010000` | 代码 + rodata + 初始数据 |
| `firmware.hex` | — | 完整镜像，地址内嵌（**推荐**） |

用 `.hex` 最简单：

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
        -c "program dist/GD32F407/firmware.hex verify reset exit"
```

手工烧 `.bin` 时两半都要写，先写 `-isr`：

```bash
st-flash write dist/GD32F407/firmware-isr.bin 0x08000000
st-flash write dist/GD32F407/firmware.bin     0x08010000
```

> **注意**：直接对 `firmware.bin` 做 `objcopy -O binary` 而不加 `-j` 过滤，
> 会把 48 KB 文件系统空洞填成 `0xFF`，产出一个 453 KB 的错误镜像。
> 本端口的 Makefile 已用 `-j` 过滤，无需手工处理。

### 4.6 littlefs 磨损提示

逻辑块是 4 KB，但底层每 4 块共享一个 16 KB 物理扇区（有 RAM 缓存聚合）。
冲刷一个扇区时会把该扇区内所有逻辑块一起重写，因此**实际擦除粒度仍是 16 KB**，
不能当作 4 KB 粒度的磨损均衡。
内部 Flash 擦写寿命约 10 万次，**不要把它当高频写的暂存区**。
高频数据应放 RAM 或外挂存储。

---

## 5. 关键设计决策记录

### 5.1 分区参数只在链接脚本中定义

`0x08004000` / `0x08010000` 这些边界只在 `ld/*.ld` 里写一次，导出为
`_flash_fs_start` / `_flash_fs_end` / `_flash_code_start`，`flashbdev.c` 用
`extern` 读取。改分区只需改一处。

### 5.2 扇区表显式枚举 + 运行时自检

`flashbdev.c` 用显式表 `{扇区号, 地址, 大小}` 而不是 `sector = FIRST + block`
算术。原因：GD32 的 `FMC_CTL.SN` 编码不是数据手册扇区号的一一映射
（库里 `CTL_SECTOR_NUMBER_24` 实际是 `CTL_SN(12)`），算术在 1 MB / 2 MB
器件上会擦错扇区——而擦错扇区会**直接变砖**。

块设备 `INIT` 时会用 `flash_bdev_layout_ok()` 校验扇区表与链接符号一致，
不一致就拒绝提供设备。

### 5.3 qstr 必须显式注册

本端口手工列源文件而不是 `include extmod/extmod.mk` 的完整源列表，所以要显式
`SRC_QSTR += $(SRC_C)`。否则 qstr 扫描器看不到端口和 extmod 源文件，
新增模块时报一堆 `MP_QSTR_xxx undeclared`。

### 5.4 `mpconfigport.mk` 的存在理由

`extmod.mk` 用 **make 变量**（不是 C 宏）判断 `MICROPY_VFS_FAT`，所以需要
`mpconfigport.mk` 提供同名变量，否则 `lib/oofatfs/ff.c` 根本不会被编译。

### 5.5 已知的坑：不存在的宏

配置里曾有一条 `MICROPY_PY_UHASHLIB (0)`。**MicroPython 没有这个宏**——
真正控制 `extmod/modhashlib.c` 的是 `MICROPY_PY_HASHLIB`（ROM level 已启用）。
这条死配置曾误导排查，已删除。

判断某个功能是否真的启用，应该看编译期宏的实际值：

```bash
arm-none-eabi-gcc -dM -E -include py/mpconfig.h -x c /dev/null \
  | grep MICROPY_PY_XXX
```

或者直接查固件符号，而不是相信配置文件里写了什么。

---

## 6. 构建与验证

```bash
cd ports/gd32
./build.sh all          # 编译两个板型
./build.sh clean        # 清理
```

产物在 `dist/<BOARD>/`。

当前状态验证结果（每次改动都应重跑）：

- 两板从零干净编译通过，`error=0` / `undefined reference=0`
- 重复构建（干净 ×2、`build.sh all`、并发）哈希完全一致
- 无板型交叉污染（407 固件不含 `GD32F470` 字符串，反之亦然）
- 布局符号两板一致：`fs=0x08004000-0x08010000`、`code=0x08010000`

---

## 7. 未完成项与后续方向

| 项 | 状态 | 前置条件 |
|----|------|----------|
| `_thread` | 评估后放弃 | 需先腾出 ≥20 KB，且有硬件验证时序交互 |
| `os.VfsRom` | 未实现 | 需 ROMFS 镜像数据源 |
| `pyb`/`stm`/蓝牙/PPP | 不适用或不可行 | — |
| 小幅精简（DTLS + 短文件名） | 未实施 | 可释放 ~9 KB |
| 运行时硬件验证 | **部分已做** | Pin/SPI/I2C/ADC/DAC/Timer/WDT/RTC/CAN/UART/文件系统/
SDCard 均已上板实测；TLS、lwIP、TinyUSB、FAT 写入仍未实测 |

### 7.1 microSD 写入：已修复（2026-11-08）

曾经只读，现已可读写。根因来自 GD32F4xx 用户手册 24.4.2 节给出的一小段写流程。

#### 决定性的一段（手册原文）

> Program SDIO_CMDCTL: CMDIDX with 24, CMDRESP with 1, CSMEN with 1.
> **Other fields are their reset value.**
> **When the CMDRECV flag is set**, program SDIO_DATACTL: DATAEN with 1,
> DATADIR with 0, TRANSMOD with 0, DMAEN with 1, BLKSZ with 0x9.
> Wait for **DTBLKEND** flag is set.

两个要点：

1. **`DATAEN` 在发命令时必须为 0**，等 `CMDRECV`（R1 收到）之后再置位。
   提前使能会让 DSM 进入 `DS_WaitS`，FIFO 一有数据就往总线上送，而卡片还没在听
   —— 于是每个块到卡上都是错位字节流，卡片回数据 CRC 失败。
   读方向提前使能无害：DSM 停在 `DS_WaitR` 等起始位，这也解释了为什么读一直正常。
2. **完成判据是 `DTBLKEND`**，不是 `DTEND`。`DTEND` 只表示主机把字节搬完了；
   `DTBLKEND` 才是卡片的 CRC 状态令牌确认。用 `DTEND` + "无错误标志"判断，
   会让"卡片压根没确认"的写看起来成功（这正是之前误报成功的原因）。

#### 三个数据标志的确切含义

| 标志 | 位 | 手册定义 | 含义 |
|------|----|----------|------|
| `DTEND` | 8 | "data end (data counter, SDIO_DATACNT, is zero)" | 主机搬完字节 |
| `DTBLKEND` | 10 | "data block sent/received (CRC check passed)" | 卡片确认，CRC 通过 |
| `DTCRCERR` | 1 | "data block sent/received (CRC check failed)" | 卡片确认，CRC 失败 |

#### 最终实现

- 发送走 DMA1 CH6（`DMA_REQ_SDIO_2`，peripheral select 4），手册要求的
  外设侧 4 拍增量突发；接收仍用 FIFO 轮询（已验证可靠，不引入额外风险）。
- 顺序严格按手册：配 `DATALEN`/`BLKSZ` → 配 DMA → 发 CMD24（`DATAEN=0`）
  → 等 `CMDRECV` → 置 `DATAEN/DATADIR/TRANSMOD/DMAEN` → 等 `DTBLKEND`
  → 等 DMA 完成 → CMD13 轮询 `READY_FOR_DATA`。
- 端口层按块读写，部分块写先读出整块再覆盖，避免用填充值覆盖同块其他数据。

#### 验证结果（真机 16 GB 卡）

| 测试 | 结果 |
|------|------|
| 原始块写后读回逐字节比对 | 3/3 一致 |
| FAT 文件写入 + 读回 | 一致 |
| 12 KiB 分 256 字节块写入、逐块读回 | 全部一致 |
| 小文件（走部分块写路径） | 一致 |
| 复位后重挂载读回 | 内容完好 |
| 内部 littlefs 无回归 | 12 块、`boot.py` + `main.py` 均在 |
| GD32F470 编译 | 通过 |

#### 走过的弯路（均已排除，勿重复）

CPU 轮询 `TFH` 喂 FIFO（四种时序）、DMA 但 DPSM 先于命令、1 位 vs 4 位总线、
24 MHz vs 4 MHz 卡时钟 —— 全部失败，且**根因都不在这里**，唯一要害是
`DATAEN` 的使能时机。当时若先拿到用户手册这一段，可以省掉绝大部分时间。

#### 注意

测试过程中曾把花样写进块 1、2、1000（属于 FAT 元数据区），会破坏文件系统。
验证写入请只用分区之外的高 LBA，或只用文件 API。

---

**最后一项最重要**：本仓库的所有验证都是编译期和静态分析。
浮点、viper、TLS、lwIP、TinyUSB、文件系统、FAT、DHT、UART 中断等在
**真实硬件上的行为均未经实测**。上板验证是必要的下一步。
