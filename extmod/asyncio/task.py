# MicroPython asyncio module
# MIT license; Copyright (c) 2019-2020 Damien P. George

# This file contains the core TaskQueue based on a pairing heap, and the core Task class.
# They can optionally be replaced by C implementations.

from . import core


# pairing-heap meld of 2 heaps; O(1)
def ph_meld(h1, h2):
    if h1 is None:
        return h2
    if h2 is None:
        return h1
    lt = core.ticks_diff(h1.ph_key, h2.ph_key) < 0
    if lt:
        if h1.ph_child is None:
            h1.ph_child = h2
        else:
            h1.ph_child_last.ph_next = h2
        h1.ph_child_last = h2
        h2.ph_next = None
        h2.ph_rightmost_parent = h1
        return h1
    else:
        h1.ph_next = h2.ph_child
        h2.ph_child = h1
        if h1.ph_next is None:
            h2.ph_child_last = h1
            h1.ph_rightmost_parent = h2
        return h2


# pairing-heap pairing operation; amortised O(log N)
def ph_pairing(child):
    heap = None
    while child is not None:
        n1 = child
        child = child.ph_next
        n1.ph_next = None
        if child is not None:
            n2 = child
            child = child.ph_next
            n2.ph_next = None
            n1 = ph_meld(n1, n2)
        heap = ph_meld(heap, n1)
    return heap


# pairing-heap delete of a node; stable, amortised O(log N)
def ph_delete(heap, node):
    if node is heap:
        child = heap.ph_child
        node.ph_child = None
        return ph_pairing(child)
    # Find parent of node
    parent = node
    while parent.ph_next is not None:
        parent = parent.ph_next
    parent = parent.ph_rightmost_parent
    # Replace node with pairing of its children
    if node is parent.ph_child and node.ph_child is None:
        parent.ph_child = node.ph_next
        node.ph_next = None
        return heap
    elif node is parent.ph_child:
        child = node.ph_child
        next = node.ph_next
        node.ph_child = None
        node.ph_next = None
        node = ph_pairing(child)
        parent.ph_child = node
    else:
        n = parent.ph_child
        while node is not n.ph_next:
            n = n.ph_next
        child = node.ph_child
        next = node.ph_next
        node.ph_child = None
        node.ph_next = None
        node = ph_pairing(child)
        if node is None:
            node = n
        else:
            n.ph_next = node
    node.ph_next = next
    if next is None:
        node.ph_rightmost_parent = parent
        parent.ph_child_last = node
    return heap


# TaskQueue class based on the above pairing-heap functions.
class TaskQueue:
    def __init__(self):
        self.heap = None

    def peek(self):
        return self.heap

    def push(self, v, key=None):
        v.ph_key = key
        self.push_raw(v)

    def push_raw(self, v):
        assert v.ph_child is None
        assert v.ph_next is None
        v.data = None
        if v.ph_key is None:
            if isinstance(v.state, TaskQueue):
                # RTOS-like priority inheritance
                v.ph_key = v.state.peek().ph_key
            else:
                v.ph_key = core.ticks()
        self.heap = ph_meld(v, self.heap)

    def pop(self):
        v = self.heap
        assert v.ph_next is None
        self.heap = ph_pairing(v.ph_child)
        v.ph_child = None
        v.ph_key = None
        return v

    def remove(self, v):
        self.heap = ph_delete(self.heap, v)
        v.ph_key = None


# Task class representing a coroutine, can be waited on and cancelled.
class Task:
    def __init__(self, coro, globals=None):
        self.coro = coro  # Coroutine of this Task
        self.data = None  # General data for queue it is waiting on
        self.state = True  # None, False, True, a callable, or a TaskQueue instance
        self.ph_key = None  # Pairing heap
        self.ph_child = None  # Paring heap
        self.ph_child_last = None  # Paring heap
        self.ph_next = None  # Paring heap
        self.ph_rightmost_parent = None  # Paring heap

    def __iter__(self):
        if not self.state:
            # Task finished, signal that is has been await'ed on.
            self.state = False
        elif self.state is True:
            # Allocated head of linked list of Tasks waiting on completion of this task.
            self.state = TaskQueue()
        elif type(self.state) is not TaskQueue:
            # Task has state used for another purpose, so can't also wait on it.
            raise RuntimeError("can't wait")
        return self

    def __next__(self):
        if not self.state:
            # Task finished, raise return value to caller so it can continue.
            raise self.data
        else:
            # Put calling task on waiting queue.
            self.state.push_raw(core.cur_task)
            # Set calling task's data to this task that it waits on, to double-link it.
            core.cur_task.data = self

    def done(self):
        return not self.state

    def cancel(self):
        # Check if task is already finished.
        if not self.state:
            return False
        # Can't cancel self (not supported yet).
        if self is core.cur_task:
            raise RuntimeError("can't cancel self")
        # If Task waits on another task then forward the cancel to the one it's waiting on.
        while isinstance(self.data, Task):
            self = self.data
        # Reschedule Task as a cancelled task.
        if hasattr(self.data, "remove"):
            # Not on the main running queue, remove the task from the queue it's on.
            self.data.remove(self)
            core._task_queue.push(self)
        elif core.ticks_diff(self.ph_key, core.ticks()) > 0:
            # On the main running queue but scheduled in the future, so bring it forward to now.
            core._task_queue.remove(self)
            core._task_queue.push(self)
        self.data = core.CancelledError
        return True

    def prioritize(self, relative=0):
        # set this tasks's queue priority
        return self.prioritize_raw(core.ticks_add(core.ticks(), relative))

    def prioritize_raw(self, ph_key):
        if not self.state:
            return False

        if self.ph_key is None or core.ticks_diff(ph_key, self.ph_key) < 0:
            self.ph_key = ph_key

            if isinstance(self.data, Task):
                # waiting on another task, bump that task's priority forward
                return self.data.prioritize_raw(ph_key)

            elif isinstance(self.data, TaskQueue):
                # waiting in a queue, bump ourselves forward in that queue
                self.data.remove(self)
                self.data.push_raw(self)
