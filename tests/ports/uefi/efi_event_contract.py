# Coercion class 6: Timer and ProtocolWatch honour the same contract as uefi.Event —
# `.ptr` and int(), is_signaled/clear/wait, and a `.event` that select and asyncio
# take — so the two objects you most want to wait on aren't the two you can't.
# (Pollability lives on `.event` because the C-level stream protocol cannot be
# supplied from Python; `.event` is the same underlying uefi.Event.)
try:
    import uefi
    import select
except ImportError:
    print("SKIP")
    raise SystemExit

# A GUID nothing will ever install, so the watch stays quiet unless we fire it.
WATCH = uefi.GUID("aabbccdd-1122-3344-5566-77889900ccdd")

ev = uefi.Event()
t = uefi.Timer()
w = uefi.ProtocolWatch(WATCH)

# ── the .ptr contract ───────────────────────────────────────────────────────
print(t.ptr != 0 and w.ptr != 0)
print(int(t) == t.ptr and int(w) == w.ptr)
print(uefi.raw.set_timer(t, 0, 0) == uefi.status.SUCCESS)  # a Timer is an event arg
print(t.is_signaled() is False and w.is_signaled() is False)

# ── pollable through .event ─────────────────────────────────────────────────
print(select.select([ev, t.event, w.event], [], [], 0) == ([], [], []))
t.set_oneshot(30)
print(t.wait(2000))  # fires well within 2 s
print(t.is_signaled())
print(select.select([t.event], [], [], 0)[0] == [t.event])
t.clear()
print(select.select([t.event], [], [], 0)[0] == [])

p = select.poll()
p.register(t.event, select.POLLIN)
p.register(w.event, select.POLLIN)
print(p.poll(0) == [])
t.set_oneshot(30)
print(t.wait(2000))
print(len(p.poll(0)) == 1)
t.clear()

# ── the watch signals through the same path ─────────────────────────────────
iface = uefi.PoolBuffer(16)
st, nh = uefi.raw.install_protocol_interface(0, WATCH, iface)
print(st == uefi.status.SUCCESS)
print(len(p.poll(0)) == 1)  # the watch is now ready, the timer is not
print([x.ptr for x in w.poll()] == [nh])
uefi.raw.uninstall_protocol_interface(nh, WATCH, iface)
iface.close()

# ── ConIn's firmware-owned key event, adopted as a pollable Event ───────────
c = uefi.Console()
ke = c.key_event()
print(int(ke) == c.inp.wait_for_key and ke is c.key_event())
print(select.select([ke], [], [], 0)[0] == [])  # no key pending under the test harness

t.close()
w.close()
ev.close()
print("uefi event contract ok")
