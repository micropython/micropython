"""Source this file into gdb `source ../../tools/gdb-stack-size.py` then run
   `stack-size` to print a backtrace with each frame size next to it."""


class StackSize(gdb.Command):
    def __init__(self):
        super(StackSize, self).__init__("stack-size", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        frame = gdb.newest_frame()
        total_size = 0
        while frame:
            sp = frame.read_register("sp")
            frame_up = frame.older()
            if not frame_up:
                break
            f = frame.function()
            l = frame.level()
            if l < 10:
                l = "#" + str(l) + " "
            else:
                l = "#" + str(l)
            size = frame_up.read_register("sp") - sp
            total_size += size
            print(l, sp, frame.type(), f, " " * (40 - len(str(f))), size)
            # print(dir(f))
            # Tweak this if for more detail for a specific function.
            if False and f.name == "mp_execute_bytecode":
                b = frame.block()
                prev_b = None
                while not b.is_static:
                    print("  block", hex(b.start), hex(b.end), b.function)
                    for sym in b:
                        if not sym.needs_frame:
                            continue
                        v = sym.value(frame)
                        print("   ", sym.addr_class, v.address, sym.type.sizeof, sym, sym.type, v)
                    prev_b = b
                    b = b.superblock

                    if b.function == f:
                        break
                b = prev_b
                print("pc scan", hex(b.start), hex(b.end))
                seen = set()
                for pc in range(b.start, b.end, 2):
                    b = gdb.block_for_pc(pc)
                    r = (b.start, b.end)
                    if r in seen:
                        continue
                    seen.add(r)
                    print(" ", hex(pc), hex(b.start), hex(b.end), b.function)
                    for sym in b:
                        if not sym.needs_frame:
                            continue
                        # if sym.type.sizeof <= 4:
                        #     continue
                        v = sym.value(frame)
                        print("   ", sym.addr_class, v.address, sym.type.sizeof, sym, sym.type, v)
            frame = frame_up
        print("total size:", total_size)


StackSize()
