# machine basics for the UEFI port: mem8/16/32/64 (poke a bytearray via its address),
# freq (monotonic counter Hz), unique_id (SMBIOS), reset_cause, WDT (long timeout).
try:
    import machine
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

mb = bytearray(8)
ma = uctypes.addressof(mb)
machine.mem32[ma] = 0xDEADBEEF
print(machine.mem32[ma] == 0xDEADBEEF)
machine.mem8[ma] = 0x42
print(machine.mem8[ma] == 0x42)
machine.mem64[ma] = 0xFFFF_FFFF_FFFF_FFFF  # mem64 must stay enabled on this 64-bit port
print(machine.mem64[ma] == 0xFFFF_FFFF_FFFF_FFFF)
# The null page is rejected before any access happens (one hook guards reads and writes).
for m in (machine.mem8, machine.mem32, machine.mem64):
    try:
        m[0x100] = 0
    except ValueError as e:
        print("ValueError", "null page" in str(e))
print(machine.freq() > 0)
print(len(machine.unique_id()) == 16)
print(isinstance(machine.reset_cause(), int))
machine.WDT(timeout=60000).feed()  # long timeout so it can't fire here
print("machine ok")
