# Adapted with permission from the EdgeDB project.
# MIT license; Adaption for MicroPython (c) 2023 Matthias Urlichs

from micropython import const

from . import core
from . import Event

_DEBUG = const(False)
_s_new = const(0)
_s_entered = const(1)
_s_exiting = const(2)
_s_aborting = const(3)


class TaskGroup:
    def __init__(self):
        self._state = _s_new
        self._loop = None
        self._parent_task = None
        self._parent_cancel_requested = False
        self._tasks = set()
        self._pending = set()
        self._errors = []
        self._base_error = None
        self._on_completed = None

    def __repr__(self):
        if _DEBUG:
            info = [""]
            if self._tasks:
                info.append("tasks=" + str(len(self._tasks)))
            if self._errors:
                info.append("errors={}" + str(len(self._errors)))
            if self._state == _s_aborting:
                info.append("cancelling")
            elif self._state > _s_new:
                info.append("entered")
            return "<TaskGroup{}>".format(" ".join(info))
        else:
            return "<TaskGroup>"

    async def __aenter__(self):
        if self._state != _s_new:
            raise RuntimeError("TaskGroup has been already entered")
        self._state = _s_entered

        if self._loop is None:
            self._loop = core.get_event_loop()

        self._parent_task = core.current_task()
        if self._parent_task is None:
            raise RuntimeError("TaskGroup cannot determine the parent task")

        return self

    async def __aexit__(self, et, exc, tb):
        if self._state == _s_entered:
            self._state = _s_exiting
        propagate_cancellation_error = None

        if exc is not None and self._is_base_error(exc) and self._base_error is None:
            self._base_error = exc

        if et is not None:
            if et is core.CancelledError:
                # micropython doesn't have an uncancel counter
                # if self._parent_cancel_requested and not self._parent_task.uncancel():
                # Do nothing, i.e. swallow the error.
                #   pass
                # else:
                propagate_cancellation_error = exc

            if self._state < _s_aborting:
                #
                #    async with TaskGroup() as g:
                #        g.create_task(...)
                #        await ...  # <- CancelledError
                #
                # or there's an exception in "async with":
                #
                #    async with TaskGroup() as g:
                #        g.create_task(...)
                #        1 / 0
                #
                self._abort()

            # Tasks that didn't yet enter our _run_task wrapper
            # get cancelled off immediately
            for task in self._pending:
                task.cancel()
                self._tasks.discard(task)
        # We use a while loop here because "self._on_completed"
        # can be cancelled multiple times if our parent task
        # gets cancelled repeatedly (or even once, when
        # our own cancellation is already in progress)
        while self._tasks:
            if self._on_completed is None:
                self._on_completed = Event()

            try:
                await self._on_completed.wait()
            except core.CancelledError as ex:
                if self._state < _s_aborting:
                    # Our parent gets cancelled.
                    #
                    #    async def wrapper():
                    #        async with TaskGroup() as g:
                    #            g.create_task(foo)
                    #
                    # "wrapper" gets cancelled while "foo" is
                    # still running.
                    propagate_cancellation_error = ex
                    self._abort()

            self._on_completed = None

        assert not self._tasks
        self._parent_task = None

        if self._base_error is not None:
            # SystemExit and Keyboardinterrupt get propagated as they are
            raise self._base_error

        if et is not None and et is not core.CancelledError:
            self._errors.append(exc)

        if self._errors:
            # Exceptions are heavy objects that can have object
            # cycles (bad for GC); let's not keep a reference to
            # a bunch of them.
            errors = self._errors
            self._errors = None

            if len(errors) == 1:
                me = errors[0]
            else:
                if _DEBUG:
                    import sys

                    for err in errors:
                        sys.print_exception(err)
                EGroup = core.ExceptionGroup
                for err in errors:
                    if not isinstance(err, Exception):
                        EGroup = core.BaseExceptionGroup
                        break
                me = EGroup("unhandled errors in a TaskGroup", errors)
            raise me

        elif propagate_cancellation_error is not None:
            # The wrapping task was cancelled; since we're done with
            # closing all child tasks, just propagate the cancellation
            # request now.
            raise propagate_cancellation_error

    def create_task(self, coro):
        if self._state == _s_new:
            raise RuntimeError("TaskGroup has not been entered")
        if self._state == _s_exiting and not self._tasks:
            raise RuntimeError("TaskGroup is finished")
        if self._state == _s_aborting:
            raise RuntimeError("TaskGroup is finished")

        k = [None]
        t = self._loop.create_task(self._run_task(k, coro))
        k[0] = t  # sigh
        self._tasks.add(t)
        self._pending.add(t)
        return t

    def cancel(self):
        # Extension (not in CPython): Stop a whole taskgroup
        if self._parent_task is None:
            return
        try:
            self._parent_task.cancel()
        except RuntimeError:
            raise core.CancelledError()

    def _is_base_error(self, exc: BaseException) -> bool:
        # KeyboardInterrupt and SystemExit are "special": they should
        # never be wrapped with a [Base]ExceptionGroup.
        assert isinstance(exc, BaseException)
        return isinstance(exc, (SystemExit, KeyboardInterrupt))

    def _abort(self):
        self._state = _s_aborting

        for t in self._tasks:
            if not t.done():
                t.cancel()

    async def _run_task(self, k, coro):
        task = k[0]
        assert task is not None
        self._pending.remove(task)

        try:
            await coro
        except core.CancelledError as e:
            exc = e
        except BaseException as e:
            if _DEBUG:
                import sys

                sys.print_exception(e)

            exc = e
        else:
            exc = None
        finally:
            self._tasks.discard(task)
            if self._on_completed is not None and not self._tasks:
                self._on_completed.set()

        if type(exc) is core.CancelledError:
            return

        if exc is None:
            return

        self._errors.append(exc)
        if self._is_base_error(exc) and self._base_error is None:
            self._base_error = exc

        if self._parent_task.done():
            # Not sure if this case is possible, but we want to handle
            # it anyways.
            self._loop.call_exception_handler(
                {
                    "message": "Task has errored out but its parent is already completed",
                    "exception": exc,
                    "task": task,
                }
            )
            return

        if self._state < _s_aborting and not self._parent_cancel_requested:
            # If parent task *is not* being cancelled, it means that we want
            # to manually cancel it to abort whatever is being run right now
            # in the TaskGroup.  But we want to mark parent task as
            # "not cancelled" later in __aexit__.  Example situation that
            # we need to handle:
            #
            #    async def foo():
            #        try:
            #            async with TaskGroup() as g:
            #                g.create_task(crash_soon())
            #                await something  # <- this needs to be canceled
            #                                 #    by the TaskGroup, e.g.
            #                                 #    foo() needs to be cancelled
            #        except Exception:
            #            # Ignore any exceptions raised in the TaskGroup
            #            pass
            #        await something_else     # this line has to be called
            #                                 # after TaskGroup is finished.
            self._abort()
            self._parent_cancel_requested = True
            self._parent_task.cancel()
