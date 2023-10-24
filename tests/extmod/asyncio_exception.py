# Test general exception handling

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


# main task raising an exception
async def main():
    print("main start")
    raise ValueError(1)
    print("main done")


try:
    asyncio.run(main())
except ValueError as er:
    print("ValueError", er.args[0])


# sub-task raising an exception
async def task():
    print("task start")
    raise ValueError(2)
    print("task done")


async def main():
    print("main start")
    t = asyncio.create_task(task())
    await t
    print("main done")


try:
    asyncio.run(main())
except ValueError as er:
    print("ValueError", er.args[0])


# main task raising an exception with sub-task not yet scheduled
# TODO not currently working, task is never scheduled
async def task():
    # print('task run') uncomment this line when it works
    pass


async def main():
    print("main start")
    asyncio.create_task(task())
    raise ValueError(3)
    print("main done")


try:
    asyncio.run(main())
except ValueError as er:
    print("ValueError", er.args[0])
