import _thread


class Queue(object):
    def __init__(self, maxsize=100):
        self.maxsize = maxsize
        self.__deque = []
        self.__lock_queue = _thread.allocate_lock()
        self.__lock_signal = _thread.allocate_lock()
        self.__lock_signal.acquire()

    def put(self, item=None):
        self.__lock_queue.acquire()
        status = self.__put(item)
        self.__lock_queue.release()
        if self.__lock_signal.locked():
            self.__lock_signal.release()
        return status

    def __pop(self):
        self.__lock_queue.acquire()
        try:
            status = self.__deque.pop(0)
        except Exception:
            return 0, None
        else:
            return 1, status
        finally:
            self.__lock_queue.release()

    def get(self):
        if not self.size():
            self.__lock_signal.acquire()
        flag, status = self.__pop()
        if not flag:
            return self.get()
        return status

    def __put(self, item):
        if self.size() > self.maxsize:
            return False
        else:
            self.__deque.append(item)
            return True

    def empty(self):
        return not self.size()

    def size(self):
        return len(self.__deque)

