:mod:`uasyncio` --- asynchronous I/O scheduler
==============================================

.. module:: uasyncio
   :synopsis: asynchronous I/O scheduler for writing concurrent code

|see_cpython_module|
`asyncio <https://docs.python.org/3.8/library/asyncio.html>`_

Example::

    import uasyncio

    async def blink(led, period_ms):
        while True:
            led.on()
            await uasyncio.sleep_ms(5)
            led.off()
            await uasyncio.sleep_ms(period_ms)

    async def main(led1, led2):
        uasyncio.create_task(blink(led1, 700))
        uasyncio.create_task(blink(led2, 400))
        await uasyncio.sleep_ms(10_000)

    # Running on a pyboard
    from pyb import LED
    uasyncio.run(main(LED(1), LED(2)))

    # Running on a generic board
    from machine import Pin
    uasyncio.run(main(Pin(1), Pin(2)))

Core functions
--------------

.. function:: create_task(coro)

    Create a new task from the given coroutine and schedule it to run.

    Returns the corresponding `Task` object.

.. function:: run(coro)

    Create a new task from the given coroutine and run it until it completes.

    Returns the value returned by *coro*.

.. function:: sleep(t)

    Sleep for *t* seconds (can be a float).

    This is a coroutine.

.. function:: sleep_ms(t)

    Sleep for *t* milliseconds.

    This is a coroutine, and a MicroPython extension.

Additional functions
--------------------

.. function:: wait_for(awaitable, timeout)

    Wait for the *awaitable* to complete, but cancel it if it takes longer
    that *timeout* seconds.  If *awaitable* is not a task then a task will be
    created from it.

    If a timeout occurs, it cancels the task and raises ``asyncio.TimeoutError``:
    this should be trapped by the caller.

    Returns the return value of *awaitable*.

    This is a coroutine.

.. function:: wait_for_ms(awaitable, timeout)

    Similar to `wait_for` but *timeout* is an integer in milliseconds.

    This is a coroutine, and a MicroPython extension.

.. function:: gather(\*awaitables, return_exceptions=False)

    Run all *awaitables* concurrently.  Any *awaitables* that are not tasks are
    promoted to tasks.

    Returns a list of return values of all *awaitables*.

    This is a coroutine.

class Task
----------

.. class:: Task()

    This object wraps a coroutine into a running task.  Tasks can be waited on
    using ``await task``, which will wait for the task to complete and return
    the return value of the task.

    Tasks should not be created directly, rather use `create_task` to create them.

.. method:: Task.cancel()

    Cancel the task by injecting a ``CancelledError`` into it.  The task may
    or may not ignore this exception.

class Event
-----------

.. class:: Event()

    Create a new event which can be used to synchronise tasks.  Events start
    in the cleared state.

.. method:: Event.is_set()

    Returns ``True`` if the event is set, ``False`` otherwise.

.. method:: Event.set()

    Set the event.  Any tasks waiting on the event will be scheduled to run.

.. method:: Event.clear()

    Clear the event.

.. method:: Event.wait()

    Wait for the event to be set.  If the event is already set then it returns
    immediately.

    This is a coroutine.

class Lock
----------

.. class:: Lock()

    Create a new lock which can be used to coordinate tasks.  Locks start in
    the unlocked state.

    In addition to the methods below, locks can be used in an ``async with`` statement.

.. method:: Lock.locked()

    Returns ``True`` if the lock is locked, otherwise ``False``.

.. method:: Lock.acquire()

    Wait for the lock to be in the unlocked state and then lock it in an atomic
    way.  Only one task can acquire the lock at any one time.

    This is a coroutine.

.. method:: Lock.release()

    Release the lock.  If any tasks are waiting on the lock then the next one in the
    queue is scheduled to run and the lock remains locked.  Otherwise, no tasks are
    waiting an the lock becomes unlocked.

TCP stream connections
----------------------

.. function:: open_connection(host, port)

    Open a TCP connection to the given *host* and *port*.  The *host* address will be
    resolved using `socket.getaddrinfo`, which is currently a blocking call.

    Returns a pair of streams: a reader and a writer stream.
    Will raise a socket-specific ``OSError`` if the host could not be resolved or if
    the connection could not be made.

    This is a coroutine.

.. function:: start_server(callback, host, port, backlog=5)

    Start a TCP server on the given *host* and *port*.  The *callback* will be
    called with incoming, accepted connections, and be passed 2 arguments: reader
    and writer streams for the connection.

    Returns a `Server` object.

    This is a coroutine.

.. class:: Stream()
    
    This represents a TCP stream connection.  To minimise code this class implements
    both a reader and a writer, and both ``StreamReader`` and ``StreamWriter`` alias to
    this class.

.. method:: Stream.get_extra_info(v)

    Get extra information about the stream, given by *v*.  The valid values for *v* are:
    ``peername``.

.. method:: Stream.close()

    Close the stream.

.. method:: Stream.wait_closed()

    Wait for the stream to close.

    This is a coroutine.

.. method:: Stream.read(n)

    Read up to *n* bytes and return them.

    This is a coroutine.

.. method:: Stream.readline()

    Read a line and return it.

    This is a coroutine.

.. method:: Stream.write(buf)

    Accumulated *buf* to the output buffer.  The data is only flushed when
    `Stream.drain` is called.  It is recommended to call `Stream.drain` immediately
    after calling this function.

.. method:: Stream.drain()

    Drain (write) all buffered output data out to the stream.

    This is a coroutine.

.. class:: Server()

    This represents the server class returned from `start_server`.  It can be used
    in an ``async with`` statement to close the server upon exit.

.. method:: Server.close()

    Close the server.

.. method:: Server.wait_closed()

    Wait for the server to close.

    This is a coroutine.

Event Loop
----------

.. function:: get_event_loop()

    Return the event loop used to schedule and run tasks.  See `Loop`.

.. function:: new_event_loop()

    Reset the event loop and return it.

    Note: since MicroPython only has a single event loop this function just
    resets the loop's state, it does not create a new one.

.. class:: Loop()

    This represents the object which schedules and runs tasks.  It cannot be
    created, use `get_event_loop` instead.

.. method:: Loop.create_task(coro)

    Create a task from the given *coro* and return the new `Task` object.

.. method:: Loop.run_forever()

    Run the event loop until `stop()` is called.

.. method:: Loop.run_until_complete(awaitable)

    Run the given *awaitable* until it completes.  If *awaitable* is not a task
    then it will be promoted to one.

.. method:: Loop.stop()

    Stop the event loop.

.. method:: Loop.close()

    Close the event loop.

.. method:: Loop.set_exception_handler(handler)

    Set the exception handler to call when a Task raises an exception that is not
    caught.  The *handler* should accept two arguments: ``(loop, context)``.

.. method:: Loop.get_exception_handler()

    Get the current exception handler.  Returns the handler, or ``None`` if no
    custom handler is set.

.. method:: Loop.default_exception_handler(context)

    The default exception handler that is called.

.. method:: Loop.call_exception_handler(context)

    Call the current exception handler.  The argument *context* is passed through and
    is a dictionary containing keys: ``'message'``, ``'exception'``, ``'future'``.
