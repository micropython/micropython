# Test asyncio.wait_for, with dependent tasks
# https://github.com/micropython/micropython/issues/16759

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


# CPython 3.12 deprecated calling get_event_loop() when there is no current event
# loop, so to make this test run on CPython requires setting the event loop.
if hasattr(asyncio, "set_event_loop"):
    asyncio.set_event_loop(asyncio.new_event_loop())


class Worker:
    def __init__(self):
        self._eventLoop = None
        self._tasks = []

    def launchTask(self, asyncJob):
        if self._eventLoop is None:
            self._eventLoop = asyncio.get_event_loop()
        return self._eventLoop.create_task(asyncJob)

    async def job(self, prerequisite, taskName):
        if prerequisite:
            await prerequisite
        await asyncio.sleep(0.1)
        print(taskName, "work completed")

    def planTasks(self):
        self._tasks.append(self.launchTask(self.job(None, "task0")))
        self._tasks.append(self.launchTask(self.job(self._tasks[0], "task1")))
        self._tasks.append(self.launchTask(self.job(self._tasks[1], "task2")))

    async def waitForTask(self, taskIdx):
        return await self._tasks[taskIdx]

    def syncWaitForTask(self, taskIdx):
        return self._eventLoop.run_until_complete(self._tasks[taskIdx])


async def async_test():
    print("--- async test")
    worker = Worker()
    worker.planTasks()
    await worker.waitForTask(0)
    print("-> task0 done")
    await worker.waitForTask(2)
    print("-> task2 done")


def sync_test():
    print("--- sync test")
    worker = Worker()
    worker.planTasks()
    worker.syncWaitForTask(0)
    print("-> task0 done")
    worker.syncWaitForTask(2)
    print("-> task2 done")


asyncio.get_event_loop().run_until_complete(async_test())
sync_test()
