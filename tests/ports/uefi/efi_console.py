# Console: text-mode query + non-blocking key read via the trampoline, and the
# attribute helper. No clear/colour/cursor calls (those emit control codes into the
# serial console the harness parses).
try:
    import uefi
    from uefi import console as CON
except ImportError:
    print("SKIP")
    raise SystemExit

con = CON.Console()
print(con.mode_count() >= 1)
cols, rows = con.size()
print(cols > 0 and rows > 0)
print(con.inp.wait_for_key != 0)  # the ConIn WaitForKey event exists
k = con.read_key()  # non-blocking; None unless a key is queued
print(k is None or (isinstance(k, tuple) and len(k) == 2))
print(CON.attribute(CON.WHITE, CON.BLUE) == 0x1F)
print("console ok")
