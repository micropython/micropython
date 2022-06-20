# Adapted with permission from the EdgeDB project.
# Adapted for MicroPython


__all__ = ["TaskGroup"]

from . import core
from . import Event

DEBUG = False


class _TaskCancel:
    # The purpose of this class is to process cancelling a task that has
    # not even started, without performing surgery on uasyncio.
    #
    def __init__(self, tg, task):
        self.tg = tg
        self.task = task

    def remove(self, task):
        # Called while processing a cancellation of a task that has not yet
        # run.
        # The pending task is on the main run queue. uasyncio doesn't
        # expect that at this point, thus we remove it.
        core._task_queue.remove(task)
        # It is also in the taskgroup's list of tasks.
        tg = self.tg
        tg._tasks.remove(task)
        # Wake up the taskgroup if its last child just got terminated.
        if tg._tasks and tg._on_completed is not None:
            tg._on_completed.set()

    def __bool__(self):
        # Called when the uasyncio main loop determines whether to throw an
        # exception into the new task.
        # After this point the new task will definitely start, thus the
        # "TaskGroup._run_task" wrapper will take over and we can remove
        # this hack.
        self.task.data = None
        return False


class TaskGroup:
    def __init__(self):
        self._entered = False
        self._exiting = False
        self._aborting = False
        self._loop = None
        self._parent_task = None
        self._parent_cancel_requested = False
        self._tasks = set()
        self._errors = []
        self._base_error = None
        self._on_completed = None

    def __repr__(self):
        info = [""]
        if self._tasks:
            info.append(f"tasks={len(self._tasks)}")
        if self._errors:
            info.append(f"errors={len(self._errors)}")
        if self._aborting:
            info.append("cancelling")
        elif self._entered:
            info.append("entered")

        info_str = " ".join(info)
        return f"<TaskGroup{info_str}>"

    async def __aenter__(self):
        if self._entered:
            raise RuntimeError(f"TaskGroup {repr(self)} has been already entered")
        self._entered = True

        if self._loop is None:
            self._loop = core.get_event_loop()

        self._parent_task = core.current_task()
        if self._parent_task is None:
            raise RuntimeError(f"TaskGroup {repr(self)} cannot determine the parent task")

        return self

    async def __aexit__(self, et, exc, tb):
        self._exiting = True
        propagate_cancellation_error = None

        if exc is not None and self._is_base_error(exc) and self._base_error is None:
            self._base_error = exc

        if et is not None:
            if et is core.CancelledError:
                # micropython doesn't have an uncancel counter
                if self._parent_cancel_requested:
                    # Do nothing, i.e. swallow the error.
                    pass
                else:
                    propagate_cancellation_error = exc

            if not self._aborting:
                # Our parent task is being cancelled:
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

        # We use a while-loop here because the wait on "self._on_completed"
        # can be cancelled multiple times if our parent task
        # is being cancelled repeatedly (or even once, when
        # our own cancellation is already in progress)
        while self._tasks:
            if self._on_completed is None:
                self._on_completed = Event()

            try:
                await self._on_completed.wait()
            except core.CancelledError as ex:
                if not self._aborting:
                    # Our parent task is being cancelled:
                    #
                    #    async def wrapper():
                    #        async with TaskGroup() as g:
                    #            g.create_task(foo)
                    #
                    # "wrapper" is being cancelled while "foo" is
                    # still running.
                    propagate_cancellation_error = ex
                    self._abort()

            self._on_completed = None

        assert not self._tasks

        if self._base_error is not None:
            raise self._base_error

        if propagate_cancellation_error is not None:
            # The wrapping task was cancelled; since we're done with
            # closing all child tasks, just propagate the cancellation
            # request now.
            raise propagate_cancellation_error

        if et is not None and et is not core.CancelledError:
            self._errors.append(exc)

        if self._errors:
            errors = self._errors
            self._errors = None

            if len(errors) == 1:
                me = errors[0]
            else:
                EGroup = core.ExceptionGroup
                for err in errors:
                    if not isinstance(err, Exception):
                        EGroup = core.baseExceptionGroup
                        break
                me = EGroup("unhandled errors in a TaskGroup", errors)
            raise me

        # at this point, if we're cancelled we don't propagate the
        # exception. So, well, don't.
        return et is core.CancelledError

    def create_task(self, coro):
        if not self._entered:
            raise RuntimeError(f"TaskGroup {repr(self)} has not been entered")
        if self._exiting and not self._tasks:
            raise RuntimeError(f"TaskGroup {repr(self)} is finished")

        k = [None]
        t = self._loop.create_task(self._run_task(k, coro))
        t.data = _TaskCancel(self, t)
        k[0] = t  # pass the task to itself

        self._tasks.add(t)
        return t

    def cancel(self):
        # Extension (not in CPython): cancel a taskgroup
        # TODO this waits for the parent to die before killing the child
        # tasks. Shouldn't that be the other way round?
        try:
            self._parent_task.cancel()
        except RuntimeError:
            self._parent_cancel_requested = True
            raise core.CancelledError()

    # Since Python 3.8 Tasks propagate all exceptions correctly,
    # except for KeyboardInterrupt and SystemExit which are
    # still considered special.

    def _is_base_error(self, exc: BaseException) -> bool:
        assert isinstance(exc, BaseException)
        return isinstance(exc, (SystemExit, KeyboardInterrupt))

    def _abort(self):
        self._aborting = True

        for t in self._tasks:
            if not t.done():
                t.cancel()

    async def _run_task(self, k, coro):
        task = k[0]
        assert task is not None

        try:
            await coro
        except core.CancelledError as e:
            exc = e
        except BaseException as e:
            if DEBUG:
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
                    "message": f"Task {repr(task)} has errored out but its parent task {self._parent_task} is already completed",
                    "exception": exc,
                    "task": task,
                }
            )
            return

        if not self._aborting and not self._parent_cancel_requested:
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
