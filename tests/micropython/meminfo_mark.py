# coverage for AT_MARK case from gc_dump_alloc_table

try:
    import micropython
    import gc
    import sys

    micropython.mem_info
    gc.is_finalized
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


if sys.platform == "webassembly" and sys.implementation._build == "pyscript":
    # GC cannot run in the middle of python bytecode on this platform.
    print("SKIP")
    raise SystemExit


class MeminfoDuringCollect:
    cancel = False

    def __del__(self):
        if not self.cancel:
            micropython.mem_info(1)


MeminfoDuringCollect()
MeminfoDuringCollect()
MeminfoDuringCollect()
MeminfoDuringCollect()
MeminfoDuringCollect()
gc.collect()
MeminfoDuringCollect.cancel = True
print("complete")
