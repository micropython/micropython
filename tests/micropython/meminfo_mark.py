# coverage for AT_MARK case from gc_dump_alloc_table

try:
    import micropython
    import gc
    import sys
except ImportError:
    print("SKIP")
    raise SystemExit


def is_webassembly_with_micropy_gc_split_heap_auto():
    return sys.platform == "webassembly" and sys.implementation._build == "pyscript"


# these functions are not always available
if (
    is_webassembly_with_micropy_gc_split_heap_auto()
    or not hasattr(micropython, "mem_info")
    or not hasattr(gc, "is_finalized")
):
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
