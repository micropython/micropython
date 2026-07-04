/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// MicroPython UEFI port — entry point.
//
// efi_main sets up the heap + GC and reaches mp_init(); then it either runs the
// interactive REPL (default) or, when built -DMPY_UEFI_SELFTEST, a headless
// self-test that signals the test harness and exits.

#include <stdint.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/runtime/pyexec.h"
#include "extmod/vfs.h"

#include "efi.h"
#include "uefi_port.h"

// Live UEFI tables, set on entry.
EFI_SYSTEM_TABLE *mp_uefi_st;
EFI_HANDLE mp_uefi_image;

static char *stack_top;

// Heap: page allocation owned entirely by MicroPython's GC. Sized at runtime to
// half of free conventional RAM (see efi_main), floored to MPY_UEFI_HEAP_MIN and
// retried smaller if the firmware can't satisfy the request.
#define MPY_UEFI_HEAP_MIN (2 * 1024 * 1024)
// Interpreter C-stack limit (distinct from the heap; the UEFI stack is far smaller
// than the heap, so it must NOT track heap size).
#define MPY_UEFI_STACK_LIMIT (1 * 1024 * 1024)

#if defined(MPY_UEFI_SELFTEST)

static void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        nlr_raise((mp_obj_t)nlr.ret_val); // propagate to caller (selftest fails)
    }
}

// Headless self-test, executed in-process. If it runs without an uncaught
// exception, we print the PASS sentinel the harness keys on.
static const char *selftest_src =
    "print('MicroPython on UEFI')\n"
    // Heap is now sized from real memory (half of free RAM), not the old fixed 2MB.
    "import gc\n"
    "_heap = gc.mem_alloc() + gc.mem_free()\n"
    "print('heap bytes:', _heap)\n"
    "assert _heap > 32 * 1024 * 1024, _heap\n"
    "x = sum(i * i for i in range(11))\n"
    "assert x == 385, x\n"
    "print('compute ok:', x)\n"
    // Allocate ~4MB of short-lived strings on a 2MB heap so gc_collect() runs
    // many times: this is what exercises the ABI-specific register/stack scan.
    "acc = 0\n"
    "for i in range(100000):\n"
    "    acc += len(str(i))\n"
    "assert acc == 488890, acc\n"
    "print('gc stress ok:', acc)\n"
    // Double-precision float: arithmetic, formatting, and a libm call (pow via **).
    "f = 3.0\n"
    "for _ in range(10):\n"
    "    f = f * 1.5 - 0.5\n"
    "assert abs(f - 116.330078125) < 1e-9, f\n"
    "assert abs(1.0 / 3.0 - 0.3333333333333333) < 1e-15\n"
    "assert abs(2.0 ** 0.5 - 1.4142135623730951) < 1e-12\n"
    "print('float ok:', f)\n"
    // Timebase + the yielding delay (which parks in WaitForEvent on a one-shot
    // timer): assert a real hardware counter advances across a ~20ms sleep. This
    // exercises mp_uefi_wfe end-to-end while the ConIn poll timer runs in the
    // background.
    "import time\n"
    "t0 = time.ticks_ms()\n"
    "time.sleep_ms(20)\n"
    "dt = time.ticks_diff(time.ticks_ms(), t0)\n"
    "assert dt >= 18, dt\n"
    "print('timebase ok:', dt)\n"
    // machine.Timer: a periodic UEFI timer event whose notify schedules a soft
    // Python callback. Count ticks over a bounded window (sleep_ms yields, so the
    // scheduler drains the callbacks), then stop it. Proves the event->VM bridge.
    "import machine\n"
    "ticks = []\n"
    "tmr = machine.Timer(-1, mode=machine.Timer.PERIODIC, period=5,\n"
    "                    callback=lambda t: ticks.append(1))\n"
    "deadline = time.ticks_add(time.ticks_ms(), 200)\n"
    "while time.ticks_diff(deadline, time.ticks_ms()) > 0 and len(ticks) < 5:\n"
    "    time.sleep_ms(5)\n"
    "tmr.deinit()\n"
    "assert len(ticks) >= 5, len(ticks)\n"
    "print('timer ok:', len(ticks))\n"
    // Timer lifecycle (Model B): armed timers are tracked module state (kept alive
    // in a registry), so deinit() is the stop control. Verify deinit() halts a
    // periodic timer's callbacks, and that GC of the deinit'd object is harmless.
    "import gc\n"
    "cnt2 = [0]\n"
    "def _cb2(t):\n"
    "    cnt2[0] += 1\n"
    "t2 = machine.Timer(-1, period=30, mode=machine.Timer.PERIODIC, callback=_cb2)\n"
    "while cnt2[0] < 3:\n"
    "    time.sleep_ms(10)\n"
    "t2.deinit()\n"
    "del t2\n"
    "gc.collect()\n"
    "_n = cnt2[0]\n"
    "time.sleep_ms(150)\n"
    "assert cnt2[0] == _n, (_n, cnt2[0])\n"
    "print('timer deinit ok')\n"
    // Leave a timer armed with no deinit: the teardown hook (machine_timer_deinit_all,
    // before mp_deinit) must close its event so its notify cannot fire into a
    // torn-down VM. If teardown failed, the run would not reach a clean PASS.
    "_leak = machine.Timer(-1, period=15, mode=machine.Timer.PERIODIC, callback=_cb2)\n"
    "print('timer teardown armed')\n"
    // select.poll over sys.stdin: with no input buffered, a bounded poll must time
    // out (empty list) rather than spin or block forever — proving the poll loop
    // sleeps in our WFE on its timeout timer and that stdin is a pollable stream.
    "import select, sys\n"
    "_p = select.poll()\n"
    "_p.register(sys.stdin, select.POLLIN)\n"
    "_t0 = time.ticks_ms()\n"
    "_r = _p.poll(60)\n"
    "_el = time.ticks_diff(time.ticks_ms(), _t0)\n"
    "assert _r == [], _r\n"
    "assert _el >= 55, _el\n"
    "print('select ok:', _el)\n"
    // asyncio (frozen package) end-to-end: async/await, sleep_ms (timer via the WFE
    // timeout), and gather. Three workers with staggered delays must complete in
    // delay order, proving the event loop drives off our select/WFE machinery.
    "import asyncio\n"
    "_log = []\n"
    "async def _worker(n, d):\n"
    "    await asyncio.sleep_ms(d)\n"
    "    _log.append(n)\n"
    "async def _amain():\n"
    "    await asyncio.gather(_worker(1, 60), _worker(2, 20), _worker(3, 40))\n"
    "asyncio.run(_amain())\n"
    "assert _log == [2, 3, 1], _log\n"
    "print('asyncio ok:', _log)\n"
    // efi.Event: signal/is_signaled/clear, a timed-out wait, and pollability via
    // select. A machine.Timer callback signals the event, and a blocking wait()
    // returns when the timer fires — proving an external signal funnels through the
    // notify -> wakeup loop and that efi.Event is select-pollable.
    "import efi\n"
    "ev = efi.Event()\n"
    "assert ev.is_signaled() == False\n"
    "ev.signal()\n"
    "assert ev.is_signaled() == True\n"
    "assert ev.wait(0) == True\n"            // already signalled
    "ev.clear()\n"
    "assert ev.is_signaled() == False\n"
    "assert ev.wait(40) == False\n"          // times out (rides the WFE)
    "_pe = select.poll()\n"
    "_pe.register(ev, select.POLLIN)\n"
    "assert _pe.poll(0) == []\n"             // not signalled -> not ready
    "ev.signal()\n"
    "assert len(_pe.poll(0)) == 1\n"         // signalled -> ready
    "ev2 = efi.Event()\n"
    "_et = machine.Timer(-1, period=30, mode=machine.Timer.ONE_SHOT,\n"
    "                    callback=lambda t: ev2.signal())\n"
    "assert ev2.wait(2000) == True\n"        // woken by the timer's signal
    "ev.close(); ev2.close(); _et.deinit()\n"
    "print('efi.Event ok')\n"
    // Filesystem (boot volume mounted at /): read a staged file, listdir, stat, and
    // import a module from disk. (Read paths are reliable under QEMU's FAT.)
    "import os\n"
    "with open('hello.txt') as f:\n"
    "    _d = f.read()\n"
    "assert _d == 'hello from efi fs', _d\n"
    "assert 'hello.txt' in os.listdir('/'), os.listdir('/')\n"
    "assert os.stat('hello.txt')[6] == 17, os.stat('hello.txt')\n"
    "import fsmod\n"
    "assert fsmod.VALUE == 42, fsmod.VALUE\n"
    // Runtime import of a precompiled .mpy off the VFS (MICROPY_PERSISTENT_CODE_LOAD).
    "assert 'mpytest.mpy' in os.listdir('/'), os.listdir('/')\n"
    "import mpytest\n"
    "assert mpytest.answer() == 42, mpytest.answer()\n"
    "assert mpytest.WHO == 'mpy-load', mpytest.WHO\n"
    "print('mpy load ok:', mpytest.WHO, mpytest.answer())\n"
    // marshal: compile -> dumps -> loads -> exec round-trip.
    "import marshal\n"
    "_code = compile('def _f(x):\\n return x * 7\\n', '<m>', 'exec')\n"
    "_blob = marshal.dumps(_code)\n"
    "assert isinstance(_blob, (bytes, bytearray)) and len(_blob) > 0\n"
    "_ns = {}\n"
    "exec(marshal.loads(_blob), _ns)\n"
    "assert _ns['_f'](6) == 42, _ns['_f'](6)\n"
    "print('marshal ok:', len(_blob), 'bytes round-tripped')\n"
    // samples/ staged into the boot volume and readable through a subdirectory path
    "assert 'samples' in os.listdir('/'), os.listdir('/')\n"
    "with open('/samples/sysinfo.py') as f:\n"
    "    assert len(f.read()) > 100\n"
    "print('fs ok:', os.listdir('/'))\n"
    // The enabled stdlib modules must import (link) in the freestanding build; their
    // behaviour is covered by the upstream test suite, so we don't re-test it here.
    "import re, json, struct, binascii, hashlib, heapq, cmath, random, framebuf, platform, deflate\n"
    "print('modules ok:', platform.platform())\n"
    // os/random host hooks: entropy seed, urandom, uname, statvfs, getcwd.
    "random.seed()\n"
    "assert 0 <= random.getrandbits(30) < (1 << 30)\n"
    "_u = os.urandom(16)\n"
    "assert isinstance(_u, bytes) and len(_u) == 16, _u\n"
    "assert os.uname().sysname == 'uefi', os.uname()\n"
    "_sv = os.statvfs('/')\n"
    "assert len(_sv) == 10 and _sv[0] > 0, _sv\n"
    "assert os.getcwd() == '/', os.getcwd()\n"
    "os.chdir('/EFI')\n"
    "assert os.getcwd() == '/EFI', os.getcwd()\n"
    "assert 'BOOT' in os.listdir(), os.listdir()\n"  // listdir() of cwd /EFI
    "os.chdir('/')\n"
    "print('os/random ok:', os.uname().machine)\n"
    // time wall clock (RTC via RuntimeServices->GetTime), 1970 epoch.
    "import time\n"
    "assert isinstance(time.time(), int) and time.time() >= 0\n"
    "_lt = time.localtime(1000000000)\n"     // 2001-09-09 with the Unix epoch
    "assert _lt[0] == 2001 and _lt[1] == 9, _lt\n"
    "assert time.mktime(time.gmtime(12345)) == 12345\n"
    "assert isinstance(time.time_ns(), int)\n"
    "print('time ok:', time.localtime()[0])\n"
    // machine basics: mem8/16/32 (poke a bytearray via its address), freq,
    // unique_id (SMBIOS), reset_cause, WDT (long timeout so it can't fire here).
    "import machine, uctypes\n"
    "_mb = bytearray(8)\n"
    "_ma = uctypes.addressof(_mb)\n"
    "machine.mem32[_ma] = 0xdeadbeef\n"
    "assert machine.mem32[_ma] == 0xdeadbeef, hex(machine.mem32[_ma])\n"
    "machine.mem8[_ma] = 0x42\n"
    "assert machine.mem8[_ma] == 0x42\n"
    "assert machine.freq() > 0\n"
    "assert len(machine.unique_id()) == 16\n"
    "assert isinstance(machine.reset_cause(), int)\n"
    "machine.WDT(timeout=60000).feed()\n"
    "assert callable(help)\n"
    "print('machine ok:', machine.freq())\n"
    // gzip: the frozen port shim (modules/gzip.py) over the C deflate module.
    "import gzip\n"
    "_gz = gzip.compress(b'hello hello hello hello')\n"
    "assert gzip.decompress(_gz) == b'hello hello hello hello'\n"
    "os.sync()\n"                                    // port no-op (no FatFs)
    "print('gzip/sync ok')\n"
    // uefi module: raw layer + EFIAPI call trampoline.
    "import uefi, uctypes\n"
    // handle enumeration (LocateHandleBuffer AllHandles=0)
    "_st, _handles = uefi.raw.locate_handle_buffer(0, None)\n"
    "assert _st == 0, _st\n"
    "assert len(_handles) > 0\n"
    // table pointers are sane
    "assert uefi.raw.boot_services_ptr() != 0\n"
    "assert uefi.raw.boot_services_active()\n"
    // locate SIMPLE_TEXT_OUTPUT, read its OutputString fn ptr, call it via the trampoline
    "_st, _iface = uefi.raw.locate_protocol(uefi.GUID('387477c2-69c7-11d2-8e39-00a0c969723b').bytes)\n"
    "assert _st == 0 and _iface != 0, (_st, _iface)\n"
    "_v = uctypes.struct(_iface, {'output_string': uctypes.UINT64 | 8}, uctypes.LITTLE_ENDIAN)\n"
    "def _w16(s):\n"
    "    b = bytearray()\n"
    "    for ch in s:\n"
    "        c = ord(ch); b.append(c & 0xFF); b.append((c >> 8) & 0xFF)\n"
    "    b.append(0); b.append(0)\n"
    "    return bytes(b)\n"
    "_msg = _w16('[uefi.raw trampoline OK]\\r\\n')\n"
    "assert uefi.raw.call(_v.output_string, _iface, uctypes.addressof(_msg)) == 0\n"
    // PoolBuffer round-trip
    "_pb = uefi.PoolBuffer(64)\n"
    "assert _pb.ptr != 0\n"
    "_pb.close()\n"
    // status.check maps the error range to uefi.Error
    "try:\n"
    "    uefi.check(uefi.status.NOT_FOUND)\n"
    "    raise AssertionError('no raise')\n"
    "except uefi.Error as _e:\n"
    "    assert _e.status == uefi.status.NOT_FOUND\n"
    "    assert _e.name == 'EFI_NOT_FOUND', _e.name\n"
    "print('uefi raw ok')\n"
    // uefi module: handles + descriptor-driven protocols.
    "from uefi import protocols as _P\n"
    // LOADED_IMAGE on our own image handle: pure field reads via uctypes overlay.
    "_img = uefi.Handle(uefi.raw.image_handle())\n"
    "_li = _P.LOADED_IMAGE.open(_img)\n"
    "assert _li.image_base != 0\n"
    "assert _li.image_size > 0\n"
    "_dev = _li.device_handle\n"
    "_li.close()\n"
    "assert _dev != 0\n"
    // protocol enumeration on the image handle finds LOADED_IMAGE
    "assert _img.supports(uefi.guid.LOADED_IMAGE_PROTOCOL)\n"
    // BLOCK_IO: locate disks, overlay media geometry, dispatch a method (Reset).
    "_disks = uefi.handle.locate_handles(uefi.guid.BLOCK_IO_PROTOCOL)\n"
    "assert len(_disks) > 0\n"
    "_phys = None\n"
    "_media = None\n"
    "for _h in _disks:\n"
    "    _bio = _P.BLOCK_IO.open(_h)\n"
    "    _m = _bio.struct_at('media', _P.BLOCK_IO_MEDIA)\n"
    "    if _m.media_present and not _m.logical_partition and _m.block_size > 0:\n"
    "        _phys = _bio; _media = _m; break\n"
    "    _bio.close()\n"
    "assert _phys is not None, 'no physical block device'\n"
    "assert _media.block_size in (512, 4096), _media.block_size\n"
    "_phys.reset(False)\n"  // method stub -> EFIAPI trampoline; raises on error
    "_phys.close()\n"
    "print('blockio block_size', _media.block_size, 'last_block', _media.last_block)\n"
    // GOP is optional (headless aa64 'virt' may lack a GPU): exercise it if present.
    "try:\n"
    "    _gop = _P.GOP.locate()\n"
    "    _mode = _gop.struct_at('mode', _P.GOP_MODE)\n"
    "    _info = _mode.struct_at('info', _P.GOP_INFO)\n"
    "    assert _info.horizontal_resolution > 0\n"
    "    print('gop', _info.horizontal_resolution, 'x', _info.vertical_resolution)\n"
    "    _gop.close()\n"
    "except uefi.Error as _e:\n"
    "    print('gop absent', _e.name)\n"
    // uefi module: NVRAM variables + Variable Policy.
    "from uefi import variable as _V\n"
    // Use a private namespace + volatile (BOOTSERVICE_ACCESS, no NON_VOLATILE) vars
    // so nothing persists and no flash is written.
    "_NS = uefi.GUID('11112222-3333-4444-5555-666677778888')\n"
    "assert not _V.exists('mpytest', _NS)\n"
    "_V.set('mpytest', _NS, b'hello', _V.BOOTSERVICE_ACCESS)\n"
    "assert _V.exists('mpytest', _NS)\n"
    "assert _V.get('mpytest', _NS) == b'hello'\n"
    "_a, _d = _V.get_with_attributes('mpytest', _NS)\n"
    "assert _d == b'hello' and (_a & _V.BOOTSERVICE_ACCESS)\n"
    // enumerate finds it within our namespace
    "_names = [_n for (_n, _g) in _V.enumerate(_NS)]\n"
    "assert 'mpytest' in _names, _names\n"
    // update + delete
    "_V.set('mpytest', _NS, b'world!!', _V.BOOTSERVICE_ACCESS)\n"
    "assert _V.get('mpytest', _NS) == b'world!!'\n"
    "_V.delete('mpytest', _NS)\n"
    "assert not _V.exists('mpytest', _NS)\n"
    "print('uefi variables ok')\n"
    // Variable Policy: skip cleanly if the protocol is absent. When present we
    // build + register a real policy entry (exercising the entry layout and the
    // RegisterVariablePolicy dispatch); enforcement is only asserted when the
    // engine actually blocks (it reports disabled on stock OVMF/AAVMF).
    "if _V.VariablePolicy.available():\n"
    "    _en = _V.VariablePolicy.is_enabled()\n"
    "    try:\n"
    "        _V.VariablePolicy.register(_NS, 'policed', min_size=4, max_size=8)\n"
    "        _reg = True\n"
    "    except uefi.Error as _e:\n"
    "        _reg = False\n"
    "        print('varpolicy register ->', _e.name)\n"
    "    if _reg:\n"
    "        _V.set('policed', _NS, b'okok', _V.BOOTSERVICE_ACCESS)\n"  // in-range write
    "        assert _V.get('policed', _NS) == b'okok'\n"
    "        _blocked = False\n"
    "        try:\n"
    "            _V.set('policed', _NS, b'xx', _V.BOOTSERVICE_ACCESS)\n"  // undersize
    "        except uefi.Error:\n"
    "            _blocked = True\n"
    "        if _en:\n"
    "            assert _blocked, 'enabled policy did not block undersize write'\n"
    "        print('varpolicy register ok, undersize_blocked=%d' % _blocked)\n"
    "    print('varpolicy ok (enabled=%d)' % _en)\n"
    "else:\n"
    "    print('varpolicy absent')\n"
    // Discovery parity: enough raw/handle API to back the shell's dh/drivers/devices.
    "_dh = uefi.handle.all_handles()\n"
    "assert len(_dh) > 5\n"
    "_aprotos = uefi.handle.all_protocols()\n"
    "assert len(_aprotos) > 10, len(_aprotos)\n"
    // `drivers`: handles exposing a Driver Binding protocol
    "_drv = uefi.handle.locate_handles(uefi.guid.DRIVER_BINDING_PROTOCOL)\n"
    "assert len(_drv) > 0\n"
    // `dh -d` / `devices`: open-info reveals BY_DRIVER/BY_CHILD usage on a device
    "_seen = False\n"
    "for _h in uefi.handle.locate_handles(uefi.guid.BLOCK_IO_PROTOCOL):\n"
    "    for _g in _h.protocols():\n"
    "        for _oi in _h.open_info(_g):\n"
    "            if _oi.by_driver or _oi.by_child:\n"
    "                _seen = True\n"
    "assert _seen, 'no BY_DRIVER/BY_CHILD open info'\n"
    "print('uefi discovery ok: handles=%d protocols=%d drivers=%d' % (len(_dh), len(_aprotos), len(_drv)))\n"
    // uefi module: events, timers, hot-plug.
    "from uefi import event as _EV\n"
    // ProtocolWatch over RegisterProtocolNotify: installing a fresh protocol fires it.
    "_PG = uefi.GUID('aabbccdd-1122-3344-5566-77889900aabb')\n"
    "_w = _EV.ProtocolWatch(_PG)\n"
    "assert _w.poll() == []\n"                       // nothing installed yet
    "_if = uefi.PoolBuffer(16)\n"                    // a real (non-null) interface pointer
    "_st, _nh = uefi.raw.install_protocol_interface(0, _PG.bytes, _if.ptr)\n"
    "assert _st == 0, _st\n"
    "_arr = _w.poll()\n"
    "assert len(_arr) >= 1, len(_arr)\n"             // the watch saw the new handle
    "assert _arr[0].ptr == _nh, (_arr[0].ptr, _nh)\n"
    "uefi.raw.uninstall_protocol_interface(_nh, _PG.bytes, _if.ptr)\n"
    "_if.close()\n"
    "_w.close()\n"
    "print('protocolwatch ok')\n"
    // Timer: a oneshot EVT_TIMER event, waited on via the WFE.
    "_t = _EV.Timer()\n"
    "_t.set_oneshot(30)\n"
    "assert _t.wait(2000) == True\n"                 // fires well within 2 s
    "_t.close()\n"
    "print('timer ok')\n"
    // Event.wait_async: an asyncio task suspends until another signals the event.
    "import asyncio\n"
    "_ev = uefi.Event()\n"
    "async def _fire():\n"
    "    await asyncio.sleep_ms(20)\n"
    "    _ev.signal()\n"
    "async def _amain4():\n"
    "    await asyncio.gather(_fire(), _EV.wait_async(_ev))\n"
    "asyncio.run(_amain4())\n"
    "assert _ev.is_signaled()\n"
    "_ev.close()\n"
    "print('event async ok')\n"
    // Registry cross-check: which named uefi.guid GUIDs are live in this firmware
    // (_aprotos is the all_protocols() set from the discovery block above).
    "_reg = uefi.guid.registry()\n"
    "_found = sorted(_n for _n, _g in _reg.items() if _g in _aprotos)\n"
    // these are always present, so finding them proves our GUID bytes match the firmware's
    "for _must in ('LOADED_IMAGE_PROTOCOL', 'DEVICE_PATH_PROTOCOL', 'BLOCK_IO_PROTOCOL'):\n"
    "    assert _must in _found, _must\n"
    "assert uefi.guid.name_for(uefi.guid.BLOCK_IO_PROTOCOL) == 'BLOCK_IO_PROTOCOL'\n"
    "assert uefi.guid.name_for(uefi.GUID('00000000-0000-0000-0000-000000000000')) is None\n"
    "print('uefi guid registry: %d of %d named GUIDs present in the protocol db' % (len(_found), len(_reg)))\n"
    "print('  present:', ', '.join(_found))\n"
    "print('uefi guidcheck ok')\n"
    // GUID casting: GUID / canonical string / bare-hex string / {braces} / bytes
    // are all accepted and equal, and endpoints take any of them interchangeably.
    "_ga = uefi.GUID('964e5b21-6459-11d2-8e39-00a0c969723b')\n"
    "_gb = uefi.GUID('964e5b21645911d28e3900a0c969723b')\n"
    "_gc = uefi.GUID('{964E5B21-6459-11D2-8E39-00A0C969723B}')\n"
    "_gd = uefi.GUID(_ga.bytes)\n"
    "assert _ga == _gb == _gc == _gd == uefi.guid.BLOCK_IO_PROTOCOL\n"
    "assert uefi.GUID(uefi.guid.BLOCK_IO_PROTOCOL) == _ga\n"
    "for _gform in (uefi.guid.BLOCK_IO_PROTOCOL, '964e5b21-6459-11d2-8e39-00a0c969723b',\n"
    "               '964e5b21645911d28e3900a0c969723b', _ga.bytes):\n"
    "    assert len(uefi.handle.locate_handles(_gform)) > 0\n"
    "try:\n"
    "    uefi.GUID('nonsense')\n"
    "    raise AssertionError('bad guid accepted')\n"
    "except ValueError:\n"
    "    pass\n"
    "print('uefi guid casting ok')\n"
    // uefi module: device paths + remaining pre-baked descriptors.
    "from uefi import device_path as _DP\n"
    "_img = uefi.Handle(uefi.raw.image_handle())\n"
    "_li = uefi.protocols.LOADED_IMAGE.open(_img)\n"
    "_devh = uefi.Handle(_li.device_handle)\n"
    "_li.close()\n"
    // parse the boot volume's device path from its raw pointer
    "_ph = _devh.open(uefi.guid.DEVICE_PATH_PROTOCOL)\n"
    "_dp = _DP.DevicePath.from_ptr(_ph.ptr)\n"
    "assert len(_dp) >= 1\n"
    // our parse -> serialize must be byte-identical to the original device path
    "_mine = _dp.to_bytes()\n"
    "assert uctypes.bytes_at(_ph.ptr, len(_mine)) == _mine\n"
    "_ph.close()\n"
    // to_text via DevicePathToText (no-This protocol), and from_text round-trips
    "_txt = _dp.to_text()\n"
    "assert len(_txt) > 0\n"
    "print('boot volume path:', _txt)\n"
    "assert _DP.DevicePath.from_text(_txt).to_text() == _txt\n"
    "assert len(_devh.device_path()) == len(_dp)\n"
    // LoadedImage.file_path -> filename via the Media/FilePath node accessor
    "_li2 = uefi.protocols.LOADED_IMAGE.open(_img)\n"
    "_fp = _li2.file_path\n"
    "if _fp:\n"
    "    _name = _DP.DevicePath.from_ptr(_fp).file_path()\n"
    "    print('image file path:', _name)\n"
    "_li2.close()\n"
    "print('uefi devicepath ok')\n"
    // PCI_IO.get_location through the pre-baked descriptor + trampoline
    "_pci = uefi.handle.locate_handles(uefi.guid.PCI_IO_PROTOCOL)\n"
    "if _pci:\n"
    "    _p = uefi.protocols.PCI_IO.open(_pci[0])\n"
    "    _b = [bytearray(8) for _ in range(4)]\n"
    "    _p.get_location(_b[0], _b[1], _b[2], _b[3])\n"
    "    print('pci loc seg/bus/dev/fn:', tuple(int.from_bytes(_x, 'little') for _x in _b))\n"
    "    _p.close()\n"
    // GOP back-buffer + Blt(BufferToVideo) — the full path the drawing sample uses.
    "try:\n"
    "    _g = uefi.protocols.GOP.locate()\n"
    "    _in = _g.struct_at('mode', uefi.protocols.GOP_MODE).struct_at('info', uefi.protocols.GOP_INFO)\n"
    "    _w = _in.horizontal_resolution; _hh = _in.vertical_resolution\n"
    "    _bb = bytearray(bytes((0x20, 0, 0, 0)) * (_w * _hh))\n"  // fill dark blue via byte-repeat
    "    _pi = uctypes.struct(uctypes.addressof(_bb), {'a': (uctypes.ARRAY | 0, uctypes.UINT32 | (_w * _hh))}, uctypes.LITTLE_ENDIAN).a\n"
    "    for _k in range(min(_w, _hh)):\n"                       // a diagonal red line
    "        _pi[_k * _w + _k] = 0xFF0000\n"
    "    _g.blt(uctypes.addressof(_bb), 2, 0, 0, 0, 0, _w, _hh, 0)\n"  // 2 = EfiBltBufferToVideo
    "    print('gop blt ok', _w, 'x', _hh)\n"
    "except uefi.Error:\n"
    "    print('gop absent (blt skipped)')\n"
    // uefi.Display: GOP as a framebuf.FrameBuffer (modes + draw + convert + show).
    "try:\n"
    "    _disp = uefi.Display()\n"
    "    _modes = _disp.modes()\n"
    "    assert len(_modes) >= 1, _modes\n"
    "    _disp.fb.fill(0)\n"
    "    _disp.fb.ellipse(_disp.width // 2, _disp.height // 2, 40, 40, 0xFFFF)\n"
    "    _disp.fb.line(0, 0, _disp.width - 1, _disp.height - 1, 0xF800)\n"
    "    _disp.fb.text('UEFI', 8, 8, 0x07E0)\n"
    "    _disp.show()\n"
    "    print('display ok:', len(_modes), 'modes,', _disp.width, 'x', _disp.height)\n"
    "except uefi.Error:\n"
    "    print('display absent (no GOP)')\n"
    // uefi module: driver/image loading + controller binding.
    "from uefi import driver as _D\n"
    // Load a *fresh in-memory copy* of our own .efi (found via LoadedImage.file_path)
    // without starting it — a hermetic exercise of LoadImage/LoadedImage/UnloadImage
    // that never re-enters the interpreter. Skips cleanly under Secure Boot.
    "_img = uefi.Handle(uefi.raw.image_handle())\n"
    "_li = uefi.protocols.LOADED_IMAGE.open(_img)\n"
    "_selfpath = uefi.device_path.DevicePath.from_ptr(_li.file_path).file_path() if _li.file_path else None\n"
    "_li.close()\n"
    "if _selfpath:\n"
    "    _vpath = _selfpath.replace('\\\\', '/')\n"      // '\\EFI\\BOOT\\BOOTxx.EFI' -> '/EFI/BOOT/...'
    "    if not _vpath.startswith('/'):\n"
    "        _vpath = '/' + _vpath\n"
    "    try:\n"
    "        _di = _D.Image.load_from_file(_vpath)\n"
    "        assert _di.ptr != 0\n"
    "        _dli = _di.loaded_image()\n"
    "        assert _dli.image_size > 0 and _dli.image_base != 0\n"
    "        _dli.close()\n"
    "        _di.unload()\n"
    "        print('image load/unload ok:', _vpath)\n"
    "    except uefi.Error as _e:\n"
    "        print('image load skipped:', _e.name)\n"
    "else:\n"
    "    print('image load skipped: no self file path')\n"
    // ConnectController: reconnect an existing controller (non-destructive — an
    // already-bound controller just no-ops). NOT_FOUND (nothing to bind) is fine.
    "_ctrls = (uefi.handle.locate_handles(uefi.guid.PCI_IO_PROTOCOL)\n"
    "          or uefi.handle.locate_handles(uefi.guid.BLOCK_IO_PROTOCOL)\n"
    "          or uefi.handle.all_handles())\n"
    "_D.connect(_ctrls[0])\n"
    // DisconnectController: run it on an *unbound* controller (no driver managing
    // it), which the spec defines as a SUCCESS no-op — so we exercise the call
    // without tearing down any live device (e.g. the mounted boot volume).
    "_ub = None\n"
    "for _h in uefi.handle.all_handles():\n"
    "    if not any(_oi.by_driver for _g in _h.protocols() for _oi in _h.open_info(_g)):\n"
    "        _ub = _h; break\n"
    "if _ub is not None:\n"
    "    _D.disconnect(_ub)\n"
    "    print('connect/disconnect ok')\n"
    "else:\n"
    "    print('connect ok (no unbound controller to disconnect)')\n"
    // uefi module: boot-manager + system services.
    "from uefi import system as _SY, boot as _BT\n"
    // system: memory map, configuration tables, firmware identity, watchdog.
    "_mkey, _mm = _SY.memory_map()\n"
    "assert len(_mm) > 0\n"
    "_conv = sum(_d.num_pages for _d in _mm if _d.type == 7)\n"   // EfiConventionalMemory
    "assert _conv > 0\n"
    "print('memmap:', len(_mm), 'regions,', _conv, 'conventional pages')\n"
    "_ct = _SY.configuration_tables()\n"
    "assert len(_ct) > 0, len(_ct)\n"
    "assert (_SY.acpi_rsdp() or _SY.smbios() or _SY.device_tree()) != 0\n"  // >=1 present on both
    "_fv = _SY.firmware_vendor()\n"
    "assert _fv\n"
    "print('firmware:', _fv, 'rev', hex(_SY.firmware_revision()), '/', len(_ct), 'config tables')\n"
    "_SY.set_watchdog(0)\n"                                        // disarm (idempotent)
    // RTC read + set round-trip, monotonic counter, driver names (drivers/devtree).
    "_tm = _SY.get_time()\n"
    "assert _tm.year >= 2000, _tm.year\n"
    "try:\n"
    "    _SY.set_time(_tm.year, _tm.month, _tm.day, _tm.hour, _tm.minute, _tm.second)\n"
    "    assert _SY.get_time().year == _tm.year\n"
    "    print('rtc:', _tm, '(set ok)')\n"
    "except uefi.Error as _e:\n"
    "    print('rtc:', _tm, '(set unsupported:', _e.name, ')')\n"
    "_c1 = _SY.monotonic_count(); _c2 = _SY.monotonic_count()\n"
    "assert _c2 > _c1, (_c1, _c2)\n"
    "print('monotonic count advanced by', _c2 - _c1)\n"
    "_dbs = uefi.handle.locate_handles(uefi.guid.DRIVER_BINDING_PROTOCOL)\n"
    "_named = 0\n"
    "for _h in _dbs:\n"
    "    _nm = _D.driver_name(_h)\n"
    "    if _nm:\n"
    "        _named += 1\n"
    "        if _named <= 3: print('  driver:', _nm)\n"
    "print('driver names:', _named, 'of', len(_dbs))\n"
    "print('system ok')\n"
    // boot: EFI_LOAD_OPTION round-trip, BootOrder read, and set_load_options.
    "_img = uefi.Handle(uefi.raw.image_handle())\n"
    "_li = uefi.protocols.LOADED_IMAGE.open(_img)\n"
    "_bootdev = uefi.Handle(_li.device_handle)\n"
    "_selfpath = uefi.device_path.DevicePath.from_ptr(_li.file_path).file_path() if _li.file_path else None\n"
    "_li.close()\n"
    "_voldp = _bootdev.device_path()\n"
    "assert _voldp is not None\n"
    "_lo = _BT.LoadOption('MicroPython Test', _voldp, b'\\x01\\x02\\x03')\n"
    "_lo2 = _BT.LoadOption.parse(_lo.to_bytes())\n"               // packed EFI_LOAD_OPTION round-trip
    "assert _lo2.description == 'MicroPython Test', _lo2.description\n"
    "assert _lo2.optional_data == b'\\x01\\x02\\x03', _lo2.optional_data\n"
    "assert _lo2.device_path.to_bytes() == _voldp.to_bytes()\n"
    "assert _lo2.active\n"
    "print('loadoption round-trip ok:', _lo2.description)\n"
    "_bo = _BT.boot_order()\n"                                    // present on OVMF, often AAVMF
    "_parsed = 0\n"
    "for _n in _bo:\n"
    "    try:\n"
    "        _BT.load_option(_n); _parsed += 1\n"
    "    except uefi.Error:\n"
    "        pass\n"
    "print('bootorder:', len(_bo), 'entries,', _parsed, 'parsed')\n"
    // set_load_options: load a fresh copy of our own image, attach a command line,
    // confirm LoadedImage reflects it, then unload (never started).
    "if _selfpath:\n"
    "    _vp = _selfpath.replace('\\\\', '/')\n"
    "    if not _vp.startswith('/'):\n"
    "        _vp = '/' + _vp\n"
    "    try:\n"
    "        _bi = uefi.Image.load_from_file(_vp)\n"
    "        _bi.set_load_options('script.py --verbose')\n"
    "        _bli = _bi.loaded_image()\n"
    "        assert _bli.load_options != 0\n"
    "        assert _bli.load_options_size == len('script.py --verbose') * 2 + 2\n"  // UTF-16LE + NUL
    "        _bli.close()\n"
    "        _bi.unload()\n"
    "        print('set_load_options ok')\n"
    "    except uefi.Error as _e:\n"
    "        print('set_load_options skipped:', _e.name)\n"
    // LOAD_FILE2 provider: present an in-memory blob (as a Linux initrd would be),
    // then read it back by invoking the installed protocol's LoadFile through the
    // real EFIAPI path (uefi.raw.call) — the same two-phase probe/copy the OS does.
    "from uefi import load_file as _LF\n"
    "import uctypes as _uct\n"
    "_blob = b'INITRD-ACID-TEST-' + bytes(range(48))\n"
    "_fp = _LF.FileProvider(_blob, _LF.linux_initrd_path())\n"
    "assert _fp.handle != 0\n"
    "_lf_fn = int.from_bytes(_uct.bytes_at(_fp._token, 8), 'little')\n"  // record[0] = LoadFile fn
    "_sz = uefi.PoolBuffer(8)\n"
    "_sz.write((0).to_bytes(8, 'little'))\n"
    "_r1 = uefi.raw.call(_lf_fn, _fp._token, 0, 0, _sz.ptr, 0)\n"        // probe: Buffer=NULL
    "_need = int.from_bytes(_sz.bytes(0, 8), 'little')\n"
    "assert _r1 != 0 and _need == len(_blob), (_r1, _need, len(_blob))\n"  // EFI_BUFFER_TOO_SMALL
    "_out = uefi.PoolBuffer(_need)\n"
    "_sz.write(_need.to_bytes(8, 'little'))\n"
    "_r2 = uefi.raw.call(_lf_fn, _fp._token, 0, 0, _sz.ptr, _out.ptr)\n"
    "assert _r2 == 0, _r2\n"
    "assert _out.bytes(0, _need) == _blob\n"
    "_out.close(); _sz.close(); _fp.close()\n"
    "print('load_file2 provider ok:', _need, 'bytes round-tripped')\n"
    // TLS/ssl hermetic test (no network): same for TLS=mbedtls and TLS=efi; skips on none.
    "try:\n"
    "    import tls as _tls, ssl as _ssl, io as _io\n"
    // ssl shim maps onto the C `tls` module (identical surface for both backends).
    "    assert _tls.SSLContext is _ssl.SSLContext\n"
    "    assert _ssl.SSLError is OSError\n"
    "    for _cst in ('CERT_NONE', 'CERT_OPTIONAL', 'CERT_REQUIRED'):\n"
    "        assert getattr(_tls, _cst) == getattr(_ssl, _cst)\n"
    // no-arg ctor -> TypeError; unknown attr load/store -> AttributeError.
    "    try:\n"
    "        _tls.SSLContext(); raise AssertionError('no-arg SSLContext accepted')\n"
    "    except TypeError:\n"
    "        pass\n"
    "    _ctx = _tls.SSLContext(_tls.PROTOCOL_TLS_CLIENT)\n"
    "    try:\n"
    "        _ctx.does_not_exist; raise AssertionError('missing attr loaded')\n"
    "    except AttributeError:\n"
    "        pass\n"
    "    try:\n"
    "        _ctx.does_not_exist = 1; raise AssertionError('missing attr stored')\n"
    "    except AttributeError:\n"
    "        pass\n"
    // ssl_sslcontext_verify_mode.py: server default is CERT_NONE; get/set round-trips.
    "    _sctx = _tls.SSLContext(_tls.PROTOCOL_TLS_SERVER)\n"
    "    assert _sctx.verify_mode == _tls.CERT_NONE\n"
    "    for _vm in (_tls.CERT_NONE, _tls.CERT_OPTIONAL, _tls.CERT_REQUIRED):\n"
    "        _ctx.verify_mode = _vm\n"
    "        assert _ctx.verify_mode == _vm\n"
    "    assert 'SSLContext' in str(_sctx)\n"
    // Wrap a fake io.IOBase stream (no handshake). EFI needs TlsDxe, absent in the
    // lean firmware, so a provider OSError is an accepted skip; mbedTLS must succeed.
    "    _is_mbed = hasattr(_tls, 'MBEDTLS_VERSION')\n"
    "    class _FakeSock(_io.IOBase):\n"
    "        def write(self, _b): return len(_b)\n"
    "        def readinto(self, _b): return 0\n"
    "        def ioctl(self, _c, _a): return 0\n"
    "        def setblocking(self, _v): pass\n"
    "    try:\n"
    "        _sw = _ssl.wrap_socket(_FakeSock(), server_side=1, do_handshake=0)\n"
    "        _sw.setblocking(False)\n"
    "        _sw.close(); _sw.close()\n"
    "        print('tls/ssl ok:', _tls.__name__, 'backend, ssl shim + wrap')\n"
    "    except OSError as _we:\n"
    "        if _is_mbed:\n"
    "            raise\n"
    "        print('tls/ssl ok:', _tls.__name__, 'backend, ssl shim (wrap needs TlsDxe):', _we)\n"
    "except ImportError:\n"
    "    print('tls/ssl skipped: no TLS backend (TLS=none)')\n"
    // Multi-volume FS: mount every SimpleFileSystem volume by hand and confirm at
    // least one exposes our staged ESP contents through os/open (the `map`/`FSx:`
    // capability — reaching a volume other than the boot mount at "/").
    "from uefi import fs as _FS\n"
    "_vols = _FS.volumes()\n"
    "assert len(_vols) >= 1, len(_vols)\n"
    "_found = False\n"
    "for _i, _h in enumerate(_vols):\n"
    "    _pt = '/uefitest%d' % _i\n"
    "    try:\n"
    "        _FS.mount(_h, _pt)\n"
    "    except OSError:\n"
    "        continue\n"
    "    try:\n"
    "        _entries = os.listdir(_pt)\n"
    "        if 'samples' in _entries or 'EFI' in _entries:\n"
    "            _found = True\n"
    "            assert b'42' in open(_pt + '/fsmod.py', 'rb').read()\n"  // read through the new mount
    "    except OSError:\n"
    "        pass\n"
    "    _FS.umount(_pt)\n"
    "assert _found, 'no volume exposed the ESP contents'\n"
    "print('fs multi-volume ok:', len(_vols), 'volume(s)')\n"
    // Console: text-mode query + non-blocking key read via the trampoline. No
    // clear/colour/cursor calls here — those emit control codes into the serial
    // console the harness parses; they're exercised by the interactive samples.
    "from uefi import console as _CON\n"
    "_con = _CON.Console()\n"
    "assert _con.mode_count() >= 1\n"
    "_cols, _rows = _con.size()\n"
    "assert _cols > 0 and _rows > 0, (_cols, _rows)\n"
    "assert _con.inp.wait_for_key != 0\n"            // the ConIn WaitForKey event exists
    "_k = _con.read_key()\n"                          // non-blocking; None unless a key is queued
    "assert _k is None or (isinstance(_k, tuple) and len(_k) == 2)\n"
    "assert _CON.attribute(_CON.WHITE, _CON.BLUE) == 0x1F\n"
    "print('console ok:', _con.mode_count(), 'modes,', _cols, 'x', _rows)\n"
    // network module: the module loads, discovery is lazy, and it degrades
    // cleanly with no NIC device attached (the hermetic harness has none). When a NIC
    // *is* present (the --net firmware) the same block reads its MAC.
    "import network\n"
    "_ifs = network.interfaces()\n"
    "network.hostname('mpy-uefi')\n"
    "assert network.hostname() == 'mpy-uefi'\n"
    "assert network.country() == 'XX'\n"
    "network.country('GB')\n"
    "assert network.country() == 'GB'\n"
    "if _ifs:\n"
    "    _lan = _ifs[0]\n"
    "    _mac = _lan.config('mac')\n"
    "    assert isinstance(_mac, bytes) and len(_mac) == 6, _mac\n"
    "    print('network ok:', len(_ifs), 'NIC(s), mac', ':'.join('%02x' % _b for _b in _mac))\n"
    // DHCP is non-blocking (MCU idiom): start it, then poll isconnected() until the
    // lease lands. active(True) first connects the stack (BDS didn't) so IP4Config2
    // exists. On the QEMU SLIRP NIC this returns 10.0.2.15 / gw 10.0.2.2.
    "    import time as _time\n"
    "    _lan.active(True)\n"                     // connect the stack (BDS didn't) + start the NIC
    "    try:\n"
    "        _lan.ipconfig(dhcp4=True)\n"          // non-blocking DHCP start
    "        _has_ip4 = True\n"
    "    except OSError:\n"
    "        _has_ip4 = False\n"                   // default firmware may lack the IP4 stack
    "    if _has_ip4:\n"
    "        _t0 = _time.ticks_ms()\n"            // poll until the lease lands (MCU idiom)
    "        while not _lan.isconnected() and _time.ticks_diff(_time.ticks_ms(), _t0) < 20000:\n"
    "            _time.sleep_ms(200)\n"
    "        if _lan.isconnected():\n"
    "            _a4 = _lan.ipconfig('addr4')\n"
    "            assert _a4[0] and _a4[0] != '0.0.0.0', _a4\n"
    "            assert _lan.ipconfig('has_dhcp4')\n"
    "            print('network dhcp ok: addr', _a4[0], 'mask', _a4[1], 'gw', _lan.ipconfig('gw4'))\n"
    "        else:\n"
    "            print('network dhcp: no lease in 20s (addr', _lan.ipconfig('addr4')[0], ')')\n"
    "    else:\n"
    "        print('network dhcp: no IP4 stack in this firmware')\n"
    "else:\n"
    "    try:\n"
    "        network.LAN()\n"
    "        raise AssertionError('LAN with no NIC should raise')\n"
    "    except OSError:\n"
    "        pass\n"
    "    print('network ok (no NIC present)')\n"
    // socket module: getaddrinfo. Numeric passthrough is hermetic (no NIC
    // needed); DNS resolution runs only when an interface is up (--net firmware),
    // over EFI_DNS4 using the DHCP-provided resolver (SLIRP's 10.0.2.3).
    "import socket\n"
    "_ai = socket.getaddrinfo('10.0.2.2', 80)\n"
    "assert _ai[0][0] == socket.AF_INET, _ai\n"
    "assert _ai[0][4] == ('10.0.2.2', 80), _ai\n"
    "print('socket getaddrinfo numeric ok:', _ai[0][4])\n"
    "if _ifs and _ifs[0].isconnected():\n"
    // SLIRP's 10.0.2.3 doesn't forward DNS inside Docker; point at a public resolver
    // (reached via Docker NAT). Exercises the network.ipconfig(dns=) override too.
    "    network.ipconfig(dns='8.8.8.8')\n"
    "    assert network.ipconfig('dns') == '8.8.8.8'\n"
    "    try:\n"
    "        _dns = socket.getaddrinfo('example.com', 80)\n"
    "        assert _dns and _dns[0][4][0].count('.') == 3, _dns\n"
    "        print('socket dns ok: example.com ->', _dns[0][4][0])\n"
    "    except OSError as _e:\n"
    "        print('socket dns: resolve failed (no outbound?)', _e)\n"
    // socket module: UDP socket sendto/recvfrom. Validated by sending a
    // DNS query over our UDP socket to a real resolver (8.8.8.8:53) and reading the
    // response — a request/response that exercises the full Transmit/Receive path.
    "if _ifs and _ifs[0].isconnected():\n"
    "    try:\n"
    "        _u = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)\n"
    "        _u.settimeout(5)\n"
    "        _q = bytes([0x12,0x34, 0x01,0x00, 0,1, 0,0, 0,0, 0,0]) + b'\\x07example\\x03com\\x00' + bytes([0,1, 0,1])\n"
    "        _u.sendto(_q, ('8.8.8.8', 53))\n"
    "        _r, _src = _u.recvfrom(512)\n"
    "        _u.close()\n"
    "        assert _r[:2] == _q[:2], _r[:2]\n"          // DNS response echoes the query id
    "        assert _r[2] & 0x80, 'QR bit not set'\n"    // it's a response
    "        print('socket udp ok:', len(_r), 'bytes from', _src)\n"
    "    except OSError as _e:\n"
    "        print('socket udp: failed (no outbound?)', _e)\n"
    // socket module: TCP client. Resolve a name, open a TCP connection,
    // send an HTTP request, and read the reply (the classic MicroPython idiom).
    "if _ifs and _ifs[0].isconnected():\n"
    "    try:\n"
    "        _addr = socket.getaddrinfo('example.com', 80)[0][-1]\n"
    "        _t = socket.socket(socket.AF_INET, socket.SOCK_STREAM)\n"
    "        _t.settimeout(10)\n"
    "        _t.connect(_addr)\n"
    "        _t.send(b'GET / HTTP/1.1\\r\\nHost: example.com\\r\\nConnection: close\\r\\n\\r\\n')\n"
    "        _resp = b''\n"
    "        while True:\n"
    "            _c = _t.recv(512)\n"
    "            if not _c:\n"
    "                break\n"
    "            _resp += _c\n"
    "        _t.close()\n"
    "        assert _resp.startswith(b'HTTP/1.'), _resp[:24]\n"
    "        print('socket tcp ok:', len(_resp), 'bytes,', _resp.split(b'\\r\\n', 1)[0])\n"
    "    except OSError as _e:\n"
    "        print('socket tcp: failed (no outbound?)', _e)\n"
    // socket: the socket is a pollable stream, so stock asyncio.open_connection
    // works (non-blocking connect via EINPROGRESS + poll(WR); reads via poll(RD)). A
    // concurrent heartbeat task proves the I/O yields instead of blocking the loop.
    "if _ifs and _ifs[0].isconnected():\n"
    "    import asyncio\n"
    "    _ticks = []\n"
    "    async def _beat():\n"
    "        for _i in range(40):\n"
    "            _ticks.append(_i)\n"
    "            await asyncio.sleep_ms(20)\n"
    "    async def _fetch():\n"
    "        _r, _w = await asyncio.open_connection('example.com', 80)\n"
    "        _w.write(b'GET / HTTP/1.1\\r\\nHost: example.com\\r\\nConnection: close\\r\\n\\r\\n')\n"
    "        await _w.drain()\n"
    "        _buf = b''\n"
    "        while True:\n"
    "            _chunk = await _r.read(512)\n"
    "            if not _chunk:\n"
    "                break\n"
    "            _buf += _chunk\n"
    "        _w.close()\n"
    "        return _buf\n"
    "    async def _amain():\n"
    "        _bt = asyncio.create_task(_beat())\n"
    "        _resp = await _fetch()\n"
    "        _bt.cancel()\n"
    "        return _resp\n"
    "    try:\n"
    "        _resp = asyncio.run(_amain())\n"
    "        assert _resp.startswith(b'HTTP/1.'), _resp[:24]\n"
    "        assert len(_ticks) >= 2, _ticks\n"    // the heartbeat kept ticking during the fetch
    "        print('socket asyncio ok:', len(_resp), 'bytes, heartbeat ticks', len(_ticks))\n"
    "    except OSError as _e:\n"
    "        print('socket asyncio: failed (no outbound?)', _e)\n"
    // network module: ICMP ping over EFI_IP4 (raw). Ping the SLIRP gateway,
    // falling back to a public host.
    "if _ifs and _ifs[0].isconnected():\n"
    "    _rtt = network.ping('10.0.2.2', 2000)\n"
    "    if _rtt is None:\n"
    "        _rtt = network.ping('8.8.8.8', 2000)\n"
    "    if _rtt is not None:\n"
    "        print('network ping ok: rtt', _rtt, 'ms')\n"
    "    else:\n"
    "        print('network ping: no reply')\n"
    // socket: raw ICMP socket — build an echo request in Python, send it, read
    // the reply. Proves socket(SOCK_RAW, IPPROTO_ICMP) over EFI_IP4.
    "if _ifs and _ifs[0].isconnected():\n"
    "    try:\n"
    "        _rs = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)\n"
    "        _rs.settimeout(3)\n"
    "        _icmp = bytearray(b'\\x08\\x00\\x00\\x00\\xab\\xcd\\x00\\x01') + b'micropython!'\n"
    "        _sum = 0\n"
    "        for _i in range(0, len(_icmp) - 1, 2):\n"
    "            _sum += (_icmp[_i] << 8) | _icmp[_i + 1]\n"
    "        while _sum >> 16:\n"
    "            _sum = (_sum & 0xFFFF) + (_sum >> 16)\n"
    "        _sum = (~_sum) & 0xFFFF\n"
    "        _icmp[2] = _sum >> 8\n"
    "        _icmp[3] = _sum & 0xFF\n"
    "        _rs.sendto(bytes(_icmp), ('10.0.2.2', 0))\n"
    "        _rep, _src = _rs.recvfrom(128)\n"
    "        _rs.close()\n"
    "        assert _rep[0] == 0, _rep[0]\n"                 // ICMP echo reply
    "        assert _rep[4:6] == b'\\xab\\xcd', _rep[4:6]\n"  // id echoed
    "        print('socket raw icmp ok:', len(_rep), 'bytes from', _src[0])\n"
    "    except OSError as _e:\n"
    "        print('socket raw icmp: failed', _e)\n"
    // socket module: TCP server. Listen + accept a connection driven by the
    // host (SLIRP hostfwd -> the harness client), echo what it sends.
    "if _ifs and _ifs[0].isconnected():\n"
    "    try:\n"
    "        _srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)\n"
    "        _srv.bind(('0.0.0.0', 8888))\n"
    "        _srv.listen(1)\n"
    "        _srv.settimeout(15)\n"
    "        _conn, _peer = _srv.accept()\n"
    "        _got = _conn.recv(64)\n"
    "        _conn.send(_got)\n"
    "        _conn.close()\n"
    "        _srv.close()\n"
    "        assert _got == b'HELLO-FROM-HOST', _got\n"
    "        print('socket server ok: echoed', _got, 'from', _peer[0])\n"
    "    except OSError as _e:\n"
    "        print('socket server: no client', _e)\n"
    // socket module: async TCP server via asyncio.start_server. The
    // listener is poll-driven (async accept: the persistent Accept + POLL_RD), and
    // a stock asyncio server task echoes one host-driven client on port 8889.
    "if _ifs and _ifs[0].isconnected():\n"
    "    try:\n"
    "        import asyncio\n"
    "        _adone = asyncio.Event()\n"
    "        _ares = {}\n"
    "        async def _ahandle(_r, _w):\n"
    "            _ares['data'] = await _r.read(64)\n"
    "            _w.write(_ares['data'])\n"
    "            await _w.drain()\n"
    "            _w.close()\n"
    "            await _w.wait_closed()\n"
    "            _adone.set()\n"
    "        async def _amain6b():\n"
    "            _sv = await asyncio.start_server(_ahandle, '0.0.0.0', 8889)\n"
    "            try:\n"
    "                await asyncio.wait_for(_adone.wait(), 20)\n"
    "            except asyncio.TimeoutError:\n"
    "                pass\n"
    "            _sv.close()\n"
    "            await _sv.wait_closed()\n"
    "        asyncio.run(_amain6b())\n"
    "        assert _ares.get('data') == b'ASYNC-FROM-HOST', _ares\n"
    "        print('socket async server ok: echoed', _ares['data'])\n"
    "    except OSError as _e:\n"
    "        print('socket async server: no client', _e)\n"
    // Network TLS, live parity gate for both backends: a real handshake against the
    // host TLS echo peer (harness, 10.0.2.2:8890 via SLIRP). CERT_NONE round-trip.
    "if _ifs and _ifs[0].isconnected():\n"
    "    try:\n"
    "        import socket as _sock, ssl as _tssl\n"
    "        _ta = _sock.getaddrinfo('10.0.2.2', 8890)[0][-1]\n"
    "        _tsk = _sock.socket()\n"
    "        _tsk.connect(_ta)\n"
    "        _tctx = _tssl.SSLContext(_tssl.PROTOCOL_TLS_CLIENT)\n"
    "        _tctx.verify_mode = _tssl.CERT_NONE\n"
    "        _tss = _tctx.wrap_socket(_tsk, server_hostname='mpy-uefi.test')\n"
    "        _tss.write(b'TLS-HELLO-FROM-GUEST')\n"
    "        _techo = _tss.read(64)\n"
    "        _tss.close()\n"
    "        assert _techo == b'TLS-HELLO-FROM-GUEST', _techo\n"
    "        print('tls net ok: TLS round-trip echoed', _techo)\n"
    // Verify against the staged CA, hostname matching the cert SAN -> ok.
    "        _ca = open('/tls_test_cert.der', 'rb').read()\n"
    "        _vs = _sock.socket(); _vs.connect(_sock.getaddrinfo('10.0.2.2', 8890)[0][-1])\n"
    "        _vctx = _tssl.SSLContext(_tssl.PROTOCOL_TLS_CLIENT)\n"
    "        _vctx.verify_mode = _tssl.CERT_REQUIRED\n"
    "        _vctx.load_verify_locations(_ca)\n"
    "        _vss = _vctx.wrap_socket(_vs, server_hostname='mpy-uefi.test')\n"
    "        _vss.write(b'VERIFY-OK'); _ve = _vss.read(16); _vss.close()\n"
    "        assert _ve == b'VERIFY-OK', _ve\n"
    "        print('tls net ok: verified peer, echoed', _ve)\n"
    // Correct CA but a hostname the cert doesn't cover -> reject.
    "        _rs = _sock.socket(); _rs.connect(_sock.getaddrinfo('10.0.2.2', 8890)[0][-1])\n"
    "        _rctx = _tssl.SSLContext(_tssl.PROTOCOL_TLS_CLIENT)\n"
    "        _rctx.verify_mode = _tssl.CERT_REQUIRED\n"
    "        _rctx.load_verify_locations(_ca)\n"
    "        _rejected = False\n"
    "        try:\n"
    "            _rss = _rctx.wrap_socket(_rs, server_hostname='wrong.invalid')\n"
    "            _rss.write(b'X'); _rss.read(1); _rss.close()\n"
    "        except (OSError, ValueError):\n"
    "            _rejected = True\n"
    "        try:\n"
    "            _rs.close()\n"
    "        except OSError:\n"
    "            pass\n"
    "        assert _rejected, 'untrusted peer was NOT rejected'\n"
    "        print('tls net ok: untrusted peer rejected')\n"
    "    except OSError as _te:\n"
    "        print('tls net: no TLS peer / provider', _te)\n"
    // Run each non-graphics sample end-to-end so the shipped code is known-good.
    // (draw.py needs GOP + blocks on a keypress, so it's exercised separately.)
    "print('--- running samples ---')\n"
    "for _s in ('sysinfo', 'devices', 'paths', 'variables', 'hotplug'):\n"
    "    print('### ' + _s)\n"
    "    exec(open('/samples/' + _s + '.py').read(), {})\n"
    "print('samples ok')\n";

static int run_selftest(void) {
    int ok = 1;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        do_str(selftest_src, MP_PARSE_FILE_INPUT);
        nlr_pop();
    } else {
        ok = 0;
    }
    return ok;
}

#else // interactive REPL

static void run_repl(void) {
    mp_hal_stdout_tx_str("\r\nMicroPython on UEFI\r\n");
    mp_hal_stdout_tx_str("Type help() for more information; Ctrl-D to exit.\r\n");
    mp_hal_stdout_tx_str("Samples: exec(open('/samples/sysinfo.py').read())  "
        "(see /samples/README.md)\r\n");
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }
}

#endif

// Stop all background activity before tearing down the interpreter: machine
// timers first (their notifies schedule into the VM), then the core async
// machinery (ConIn poll timer + wait events).
static void mpy_uefi_teardown(void) {
    #if MICROPY_PY_MACHINE
    machine_timer_deinit_all();
    #endif
    efi_event_deinit_all();
    mp_uefi_async_deinit();
}

// ── Script-on-the-command-line support (`app.efi foo.py [args]`) ────────────
// Narrow a null-terminated CHAR16 into `out` (ASCII; non-ASCII -> '?'). Returns
// the length written (excluding the terminator).
static size_t char16_to_cstr(const CHAR16 *w, char *out, size_t cap) {
    size_t n = 0;
    while (w[n] != 0 && n + 1 < cap) {
        CHAR16 c = w[n];
        out[n] = (c < 0x80) ? (char)c : '?';
        n++;
    }
    out[n] = 0;
    return n;
}

// The shell installs EFI_SHELL_PARAMETERS_PROTOCOL on our image handle to pass
// argc/argv. Absent when launched by the boot manager (no args) -> NULL.
static EFI_SHELL_PARAMETERS_PROTOCOL *uefi_shell_params(void) {
    static const EFI_GUID g = EFI_SHELL_PARAMETERS_PROTOCOL_GUID;
    void *p = NULL;
    if (EFI_ERROR(mp_uefi_st->BootServices->HandleProtocol(mp_uefi_image, (EFI_GUID *)&g, &p))) {
        return NULL;
    }
    return (EFI_SHELL_PARAMETERS_PROTOCOL *)p;
}

// If launched with a script argument (`app.efi foo.py [args]`), set sys.argv and
// run that file — like `python foo.py` — and return 1. Otherwise return 0 (REPL).
static int uefi_run_script(void) {
    EFI_SHELL_PARAMETERS_PROTOCOL *sp = uefi_shell_params();
    if (sp == NULL || sp->Argc < 2) {
        return 0;
    }
    char buf[260];
    // sys.argv = [script, arg1, ...]  (Argv[0] is our own image name, skipped).
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);
    for (UINTN i = 1; i < sp->Argc; i++) {
        size_t n = char16_to_cstr(sp->Argv[i], buf, sizeof(buf));
        mp_obj_list_append(mp_sys_argv, mp_obj_new_str(buf, n));
    }
    // Open via the VFS (mounted at "/"); accept shell-style backslashes too.
    char path[260];
    char16_to_cstr(sp->Argv[1], path, sizeof(path));
    for (char *c = path; *c; c++) {
        if (*c == '\\') {
            *c = '/';
        }
    }
    pyexec_file(path);
    return 1;
}

// Total free conventional RAM (bytes), from the UEFI memory map. 0 if unavailable.
static uint64_t uefi_conventional_free(EFI_BOOT_SERVICES *bs) {
    UINTN size = 0, mapkey = 0, descsize = 0;
    UINT32 descver = 0;
    if (bs->GetMemoryMap(&size, NULL, &mapkey, &descsize, &descver) != EFI_BUFFER_TOO_SMALL) {
        return 0;
    }
    size += descsize * 8; // slack: allocating the buffer can grow the map
    EFI_MEMORY_DESCRIPTOR *map = NULL;
    if (EFI_ERROR(bs->AllocatePool(EfiLoaderData, size, (void **)&map)) || map == NULL) {
        return 0;
    }
    uint64_t free_pages = 0;
    if (!EFI_ERROR(bs->GetMemoryMap(&size, map, &mapkey, &descsize, &descver)) && descsize != 0) {
        for (UINT8 *p = (UINT8 *)map; p + descsize <= (UINT8 *)map + size; p += descsize) {
            EFI_MEMORY_DESCRIPTOR *d = (EFI_MEMORY_DESCRIPTOR *)p; // iterate by descsize, not sizeof
            if (d->Type == EfiConventionalMemory) {
                free_pages += d->NumberOfPages;
            }
        }
    }
    bs->FreePool(map);
    return free_pages * EFI_PAGE_SIZE;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
    mp_uefi_st = system_table;
    mp_uefi_image = image_handle;
    EFI_BOOT_SERVICES *bs = system_table->BootServices;

    // We are a guest holding the platform watchdog — disarm it.
    bs->SetWatchdogTimer(0, 0, 0, NULL);

    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    // Size the GC heap from real memory: half of free conventional RAM, leaving the
    // rest for the firmware / driver loads. Fall back to 64 MiB if the map is
    // unreadable, then retry-halve until the firmware can satisfy the request.
    uint64_t freebytes = uefi_conventional_free(bs);
    uint64_t want = freebytes ? (freebytes / 2) : (64ULL * 1024 * 1024);
    if (want < MPY_UEFI_HEAP_MIN) {
        want = MPY_UEFI_HEAP_MIN;
    }
    UINTN pages = (UINTN)(want / EFI_PAGE_SIZE);
    EFI_PHYSICAL_ADDRESS addr = 0;
    EFI_STATUS s = EFI_OUT_OF_RESOURCES;
    while (pages >= MPY_UEFI_HEAP_MIN / EFI_PAGE_SIZE) {
        s = bs->AllocatePages(AllocateAnyPages, EfiLoaderData, pages, &addr);
        if (!EFI_ERROR(s)) {
            break;
        }
        pages /= 2;
    }
    if (EFI_ERROR(s) || addr == 0) {
        mp_hal_stdout_tx_str("MPY-UEFI: heap AllocatePages failed\r\n");
        mp_uefi_exit(0);
    }
    void *heap = (void *)(uintptr_t)addr;
    UINTN heap_size = pages * EFI_PAGE_SIZE;

    mp_stack_set_top(stack_top);
    mp_stack_set_limit(MPY_UEFI_STACK_LIMIT);
    gc_init(heap, (char *)heap + heap_size);
    mp_init();

    // Bring up the timebase + event machinery (stdin poll timer, wait/wakeup
    // events) now that the VM is live and can take scheduled callbacks.
    mp_uefi_async_init();

    // Mount the boot volume at "/" (best-effort; non-fatal if absent).
    mp_uefi_vfs_mount_boot();

    #if defined(MPY_UEFI_SELFTEST)
    int ok = run_selftest();
    mpy_uefi_teardown();
    mp_deinit();
    mp_hal_stdout_tx_str(ok ? "MPY-UEFI: SELFTEST PASS\r\n" : "MPY-UEFI: SELFTEST FAIL\r\n");
    mp_uefi_exit(ok);
    #else
    // `app.efi foo.py [args]` runs the script and exits; bare launch -> REPL.
    if (!uefi_run_script()) {
        run_repl();
    }
    mpy_uefi_teardown();
    mp_deinit();
    // Return the heap pages to the firmware now the interpreter is gone (it was
    // AllocatePages, so this must be FreePages, not FreePool). Safe: the teardown
    // above closed every UEFI event/timer, so nothing can fire into it.
    bs->FreePages(addr, pages);
    // Clean exit: an application returns control to its caller (the shell, or the
    // boot manager) — it must NOT power the machine off. The marker lets the test
    // harness detect the exit without depending on what the caller does next.
    mp_hal_stdout_tx_str("MPY-UEFI: exit\r\n");
    return EFI_SUCCESS;
    #endif
}

// Text printed by help() (MICROPY_PY_BUILTINS_HELP_TEXT).
const char uefi_help_text[] =
    "Welcome to MicroPython on UEFI!\n"
    "\n"
    "For online docs please visit http://docs.micropython.org/\n"
    "\n"
    "Control commands:\n"
    "  CTRL-C        -- interrupt a running program\n"
    "  CTRL-D        -- on a blank line, exit\n"
    "\n"
    "The filesystem of the boot volume is mounted at '/'. Useful modules:\n"
    "  os, time, machine, efi, asyncio, select, json, re, struct, hashlib\n"
    "\n"
    "For a list of available modules, type help('modules').\n";

// gc_collect() lives in uefi_gccollect.c (register dump is ABI-specific).

// mp_import_stat() and the open() builtin are provided inline by py/builtin.h
// (routing to the VFS) when MICROPY_VFS is enabled; the lexer's file reader comes
// from extmod/vfs_reader.c via MICROPY_READER_VFS. The boot volume is mounted in
// efi_main() by mp_uefi_vfs_mount_boot().

void nlr_jump_fail(void *val) {
    (void)val;
    mp_hal_stdout_tx_str("MPY-UEFI: FATAL nlr_jump_fail\r\n");
    mp_uefi_exit(0);
}

void MP_NORETURN __fatal_error(const char *msg) {
    mp_hal_stdout_tx_str("MPY-UEFI: FATAL ");
    mp_hal_stdout_tx_str(msg);
    mp_hal_stdout_tx_str("\r\n");
    mp_uefi_exit(0);
}

// ---- test-harness exit path (mirrors tests/sanity) ----
#if defined(__x86_64__)
static inline void qemu_isa_debug_exit(uint8_t code) {
    __asm__ __volatile__ ("outb %0, %1" : : "a" (code), "Nd" ((uint16_t)0xf4)); // codespell:ignore
}
#endif

void mp_uefi_exit(int success) {
    #if defined(__x86_64__)
    qemu_isa_debug_exit(success ? 0x10 : 0x11); // -> host exit 33 (pass) / 35 (fail)
    #endif
    mp_uefi_st->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    for (;;) {
        __asm__ __volatile__ ("");
    }
}
