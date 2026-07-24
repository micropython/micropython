# machine basics for the UEFI port: mem8/16/32 (poke a bytearray via its address),
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
print(machine.freq() > 0)
print(len(machine.unique_id()) == 16)
print(isinstance(machine.reset_cause(), int))
machine.WDT(timeout=60000).feed()  # long timeout so it can't fire here
print("machine ok")
