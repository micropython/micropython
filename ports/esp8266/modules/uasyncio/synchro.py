from uasyncio import core

class Lock:

    def __init__(self):
        self.locked = False
        self.wlist = []

    def release(self):
        assert self.locked
        self.locked = False
        if self.wlist:
            #print(self.wlist)
            coro = self.wlist.pop(0)
            core.get_event_loop().call_soon(coro)

    def acquire(self):
        # As release() is not coro, assume we just released and going to acquire again
        # so, yield first to let someone else to acquire it first
        yield
        #print("acquire:", self.locked)
        while 1:
            if not self.locked:
                self.locked = True
                return True
            #print("putting", core.get_event_loop().cur_task, "on waiting list")
            self.wlist.append(core.get_event_loop().cur_task)
            yield False
