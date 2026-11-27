# Serial ports: driver layer, REPL attachment and `machine.UART`

Status: **implemented** (`uefi_serial.c`, `machine_uart.c`, REPL glue in `uefi_event.c`,
launch options in `main.c`). Tested under QEMU on both arches; hardware testing on the
FTDI rig is pending. Hot-plug has not been tested yet.

## 1. Goals and scope

In scope:

1. **Port, speed and flow-control settings**: choose which `SerialIo` handle the REPL
   uses, set its baud rate, and turn RTS/CTS on or off, at launch and at run time.
2. **RX performance**: read in bulk instead of one byte per `Read`.
3. **`machine.UART`**: every `SerialIo` port is a `machine.UART`, in both console and
   serial-REPL mode, with the standard API, pollable through the stream `ioctl` so
   `select.poll` and our asyncio streams work. This is also the "direct raw access" to
   the REPL's port; there is no separate raw-console object.
4. **Interrupt-character handling moves to the REPL glue**, as on `stm32`/`rp2`: only
   bytes heading for the REPL's input are checked for Ctrl-C.

Parked (not in this work): statistics counters and `serial_stats()`, the soak-test
script, `sendbreak`, `UART.irq`, parity/bits/stop-bits other than 8N1 (accepted only if
the driver accepts them, see section 6), the asustest console-binding crash, and the
Part B PCI/memory items.

## 2. Facts this design rests on

- **Spec `Timeout` is per character**, in microseconds, for both RX and TX. `0` passed
  to `SetAttributes` means "use the device default", not "don't wait". The ftdi-uefi
  driver we run implements it per character.
- **`Read(n)` for more than is buffered** waits one `Timeout` after the last byte that
  arrives, then returns `EFI_TIMEOUT` with a partial count. With `Timeout` = one
  character time, a bulk read returns almost immediately with whatever is there.
- **`INPUT_BUFFER_EMPTY` in `GetControl`** reflects the *driver's* buffer. A driver that
  lists it in `Mode->ControlMask` (ftdi-uefi does: mask `0x43f3`) moves data off the
  chip on its own timer. A driver that doesn't list it may only fetch on `Read`.
- **`SetControl` accepts only DTR, RTS and `HARDWARE_FLOW_CONTROL_ENABLE`.** Other bits
  can make it fail.
- **TerminalDxe owns a serial port** whenever the firmware console is on it. Anyone
  else reading the port races it for bytes, and Ctrl-C bytes it receives reach the REPL
  through `ConIn`. Taking the port means `DisconnectController`; giving it back means
  restoring settings and `ConnectController`.
- **`stm32`/`rp2` model**: one RX buffer per UART; the REPL's UART is flagged
  `attached_to_repl`, and only that UART's RX path compares bytes with
  `mp_interrupt_char`. `pyexec` already sets the interrupt character to -1 while it reads
  raw-REPL and raw-paste input, so Ctrl-C passes through there.

## 3. Layers

```
  machine.UART (Python)           REPL (pyexec, readline)
         |                                |
         |                   REPL glue: attach one port, Ctrl-C check,
         |                   stdin/stdout in serial mode (uefi_event.c)
         \______________   _______________/
                        \ /
             serial driver layer (new: uefi_serial.c)
             one instance per SerialIo handle: open/close, settings,
             RX buffer + background fill, TX loop, readiness + wake
                         |
               EFI_SERIAL_IO_PROTOCOL (firmware / ftdi-uefi)
```

The driver layer knows nothing about the REPL or the interrupt character.

## 4. Driver layer (`uefi_serial.c`)

### 4.1 Ports, identity and hot-plug

USB serial ports can be unplugged and re-plugged at any time, so a port is identified
by its device path, not its handle, and a handle is never used without checking it.

- **Slot table.** Each port's identity is its device-path bytes, kept in an
  append-only table of slots for the session. At start-up slots 0..N-1 are assigned in
  `LocateHandleBuffer(SerialIo)` order. A port that is unplugged keeps its slot, marked
  absent, so later indices never shift. A port that reappears with the same device
  path (same USB socket) gets its old slot back; a new path gets the next new slot.
  The slot number is the `machine.UART` id and the `--serial=N` index. Across reboots,
  numbering is enumeration order, stable for unchanged hardware.
- **String ids.** `UART("<text>")` and `--serial=<text>` pick the first present port
  whose device-path text contains `<text>` (e.g. `"USB(0x0,0x0)"`), by the same code.
- **Unplug detection.** Before every call into a port, from Python or the timer, check
  that its handle still carries `SerialIo` with the same interface pointer
  (`OpenProtocol(..., GET_PROTOCOL)`; the firmware validates handles against its own
  list, so this is safe after the handle is freed). On failure the slot is marked
  absent and no driver call is made: calling through a stale interface pointer would be
  a use-after-free in the firmware.
- **Arrival detection.** `RegisterProtocolNotify` on `SerialIo`; the notify only sets a
  "rescan" flag, and the next timer tick re-enumerates and matches paths to slots.
- **Instances.** One per slot: the `SerialIo` pointer, handle, saved settings (`Mode`
  values and settable control bits), current settings, an RX ring, "present", "owned"
  and "REPL-attached" flags. `machine.UART` objects point at a slot; two `UART(n)`
  objects for the same id share it.
- **While absent**: an open `UART` raises `OSError(ENODEV)` from `read`/`write`/etc.,
  and polls as ready-with-error so asyncio waiters wake instead of hanging. The error
  is sticky: the object stays dead even if the port comes back, until the program
  calls `uart.init(...)` or creates a new `UART(...)`, which looks the id up again,
  takes the port from TerminalDxe and applies the settings explicitly. This means a
  program always learns about a gap (bytes in flight are lost), and decides itself
  whether the device in that socket is still the one it expects. A re-plugged port
  has a new handle and interface pointer, so the per-call check catches a gap even
  if nothing called the port while it was away.
- **The REPL's port is the exception**: no program is in a position to re-open it, so
  it re-attaches automatically. While it is gone, REPL output goes to `ConOut`; when a
  port with the same device path reappears, the REPL glue (in foreground code, not the
  timer) re-opens it with the REPL's settings and resumes.

### 4.2 Open, configure, close

- **Open** (first user of the instance): save `Mode` and the settable control bits,
  `DisconnectController` (evicts TerminalDxe), apply settings.
- **Configure**: `SetAttributes(baud, ReceiveFifoDepth=0, Timeout=one character time,
  parity, data bits, stop bits)`, then `SetControl(saved DTR|RTS | flow bit)`. Read
  `Mode` and `GetControl` back. If the driver refuses, raise `OSError(EINVAL)` and leave
  the previous settings in place. One character time =
  `ceil(10 * 1e6 / baud)` µs (start + 8 data + stop; more if parity or 2 stop bits).
- **Close** (last user gone, or interpreter exit): wait for TX to drain (bounded, as
  now), restore saved `Mode` values and control bits, `ConnectController` so TerminalDxe
  and the console mirror come back. The REPL's port closes last, after the exit marker.

### 4.3 RX strategy

Your question was whether we need our own timer polling when the driver already polls
in the background. We don't need it to move data, but we do still need it for two
things: catching Ctrl-C on the REPL's port while a program runs and isn't reading
input, and waking a blocked wait (`mp_uefi_wfe`, `select.poll`, asyncio) when data
arrives. So:

| Instance | Driver lists `INPUT_BUFFER_EMPTY` | Our 5 ms timer does |
|---|---|---|
| REPL-attached | either | bulk `Read` into our ring (needed for Ctrl-C) |
| not REPL-attached | yes | **no `Read`**; only `GetControl`, and signals the wake event if data is waiting and our ring is empty |
| not REPL-attached | no | bulk `Read` into our ring (the driver may never fetch otherwise) |

In the "yes" row the driver's buffer is the buffer; our ring fills only when Python
reads (`read`/`any` pull from the driver on demand). The timer is skipped altogether
when no instance is open.

Bulk read: when data is waiting (or unconditionally, if the driver doesn't list the
bit), one `Read` of `min(ring free, 256)` bytes. If the ring is full we stop reading;
bytes stay in the driver and, with RTS/CTS, the sender is held off.

### 4.4 TX

- Bulk writes: each `Write` is handed the whole remainder. The spec makes this safe:
  on return `BufferSize` holds the bytes actually written, with `EFI_TIMEOUT` if it
  stopped early, so a partial write is always reported. (ftdi-uefi reports the bytes the
  chip confirmed; where an xHCI driver doesn't report partial bulk-OUT lengths, ftdi-uefi
  falls back to per-packet retries itself.)
- One loop for everyone: `Write` the remainder until done, an error other than
  `EFI_TIMEOUT`, or the caller's give-up time passes without progress.
- With a one-character `Timeout`, `Write` returns partial counts more often (each time
  the chip is momentarily full). The loop absorbs this; it costs extra calls, not lost
  data. **To verify on hardware**: if this is too busy, `Timeout` can be two character
  times without affecting the RX design.
- REPL stdout keeps today's rule: give up after 2 s without progress.

### 4.5 Readiness and wake

- Readable: our ring is non-empty, or (row "yes") `GetControl` says the driver has data.
- Writable: flow control off, or `CTS` asserted in `GetControl`.
- The timer calls the existing `mp_uefi_wake()` when an instance becomes readable, so
  `select.poll` and asyncio wake up exactly as stdin does today.

### 4.6 Concurrency

The timer runs at `TPL_CALLBACK`. Ring updates from Python code take the existing
atomic section. ftdi-uefi serves `GetControl`/`Read` at `TPL_CALLBACK` and doesn't hold
its lock across USB transfers in `Write`, so a blocked `Write` in the foreground doesn't
stop our timer, or the driver's.

## 5. REPL glue (`uefi_event.c`)

- **Serial mode**: at start-up the REPL attaches to one instance (`--serial` choice).
  The timer fills that instance's ring; bytes moving from it to the REPL go through the
  interrupt check against the current interrupt character, as on `stm32`. REPL stdout
  goes through the same instance's TX loop.
- **Console mode**: the REPL reads `ConIn` as today and attaches to no serial instance.
  A `UART` opened on the console's serial port takes the port from TerminalDxe, so
  Ctrl-C bytes on it are just data, and the console mirror to that port pauses until
  `deinit()`.
- `machine.UART(n)` on the REPL's own port shares the REPL's instance (and ring). For
  binary transfers the program calls `micropython.kbd_intr(-1)`, the standard idiom.

## 6. `machine.UART` API

Built on `extmod/machine_uart.c` with a port include file.

```python
UART(id, baudrate=None, bits=8, parity=None, stop=1, *, flow=None,
     timeout=0, timeout_char=0, rxbuf=...)
```

- **id**: port index (section 4.1). `ValueError` if there is no such port.
- **baudrate**: omitted keeps the port's current rate, read from `Mode->BaudRate`
  (115200 if `Mode` reports 0). This is what `esp32` does (it reads the rate back from
  the hardware); `rp2` likewise keeps the previous rate and uses its default only for a
  never-initialised port. It also leaves the console's speed alone with no special case.
- **bits / parity / stop**: passed to `SetAttributes`; whatever the driver refuses
  raises `OSError(EINVAL)`.
- **flow**: `None` (the default) means RTS/CTS on if the driver lists
  `HARDWARE_FLOW_CONTROL_ENABLE`, else off. `0` means off. `UART.RTS | UART.CTS` means
  on, and raises `OSError` if the driver can't do it. `SerialIo` has one flow-control
  bit, so `UART.RTS` or `UART.CTS` alone raises `ValueError`.
- **rxbuf**: our ring size (default 1 KiB; minimum 2 x the raw-paste window if the
  port is the REPL's).

### 6.1 Timeouts

Python timeouts are milliseconds and are implemented in our layer by waiting
(`mp_uefi_wfe`) on the wake event. They never reach the device; the device `Timeout`
stays one character time so the timer never blocks.

| Parameter | Meaning (same as `rp2`) | `0` means |
|---|---|---|
| `timeout` | wait for the first character of a read, and for progress on a write | don't wait: return what's there, or `None` if nothing (EAGAIN) |
| `timeout_char` | wait between characters once a read has started | raised to the minimum: one character time in ms, rounded up |

So both uses work: `timeout=0, timeout_char=50` polls for a start and then waits for
slow input in flight; `timeout=5000, timeout_char=2` waits for something to start and
returns at the first short pause.

Translation to the UEFI side, for the record: a Python `0` never becomes a UEFI `0`
(which would mean "device default"). The device `Timeout` is always one character time,
computed from the current baud rate.

### 6.2 Methods

- `read(n)`, `readinto(buf[, n])`, `readline()`: rp2 semantics with the timeouts above.
- `write(buf)`: writes until done or no progress for `timeout` ms (minimum one
  character time); returns the count written, or `None` if nothing could be written.
  asyncio's `drain()` already handles partial writes and `None`.
- `any()`: bytes in our ring, plus (row "yes") `1` if the driver reports data.
- `txdone()`: `OUTPUT_BUFFER_EMPTY` from `GetControl` when listed in `ControlMask`,
  else `True` after a completed `write`.
- `flush()`: wait for `txdone()`, bounded by the TX give-up time.
- `deinit()`: release the port (section 4.2) unless the REPL is attached to it.
- stream `ioctl`: `MP_STREAM_POLL` per section 4.5; `MP_STREAM_CLOSE` = `deinit`.

## 7. Launch options

- `--serial` (first port), `--serial=N` (index), `--serial=<text>` (first port whose
  device-path text contains `<text>`, e.g. `Uart` or `USB(0x0,0x0)`).
- `--baud=N` (default: keep the port's current rate, normally 115200).
- `--no-rtscts` (exists).
- On start-up in serial mode, print the settings the driver accepted on the REPL port.

## 8. Error handling

- `SetAttributes`/`SetControl` refusals: `OSError(EINVAL)`, previous settings kept.
- Device errors on `Read`: counted in the instance (for the parked stats) and the timer
  backs off to the next tick; no exception from the timer.
- Device errors on `Write` from Python: `OSError(EIO)` if nothing was written,
  otherwise the partial count.
- A port that disappears (USB unplug): `OSError(ENODEV)` from Python; the REPL falls
  back to `ConOut` if its port goes away (section 4.1).

## 9. Tests

- **QEMU, both arches**: argument validation (ids, flow `None`/`0`/RTS|CTS, timeouts,
  rxbuf), `UART(0)` sharing the REPL instance in serial mode, open/deinit/reopen,
  reconnect on exit (the console comes back), polling with `select.poll` and an asyncio
  `StreamReader` against a second port.
- **Second port for data tests**: x64 `q35` takes a second `-serial` (COM2); I'll check
  whether our QEMU's aa64 `virt` machine offers a second UART, otherwise data-path tests
  run on x64 only. The host side of the second port is a socket the test drives.
- **Hot-plug**: on hardware, unplug and re-plug the FTDI adapter with a `UART` open
  and with the REPL on it; check sticky `ENODEV`, recovery via `init()`, and REPL fallback/re-attach. In
  QEMU, a USB serial device can be removed and re-added with `device_del`/`device_add`
  from the monitor if our firmware has a driver for one; otherwise hardware only.
- **Hardware (systemtest, asustest)**: the bulk upload/output tests already used,
  repeated at 115200 and a higher rate with and without RTS/CTS, plus `--serial=`
  selection, `--baud=`, `--no-rtscts`, and Ctrl-C delivery while a program runs.

## 10. Decisions taken in review

1. Device `Timeout` is one character time for both RX and TX; TX partial writes are
   absorbed by the loop. Measure on hardware; two character times is the fallback.
2. Omitted `baudrate` keeps the current rate (as `esp32`/`rp2`), read from `Mode`.
3. `UART.RTS` or `UART.CTS` alone is a `ValueError`.
4. Writes are bulk; `BufferSize` on return gives the partial count.
5. Our timer doesn't `Read` a non-REPL port whose driver lists `INPUT_BUFFER_EMPTY`;
   it only checks readiness to wake waiters (section 4.3).
6. Ports are identified by device path in a session slot table; ids are a slot index or
   a device-path substring; handles are validated before every use (section 4.1).
7. A `UART` that loses its port stays in error (`ENODEV`) until `init()` or a new
   `UART()`; only the REPL's port re-attaches automatically (section 4.1).
