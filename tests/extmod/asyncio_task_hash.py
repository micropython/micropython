# Test hash unary operator for a Task

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task():
    pass


async def main():
    # Confirm that the hash is an int
    print("=" * 10)
    t1 = asyncio.create_task(task())
    t2 = asyncio.create_task(task())
    print(type(hash(t2)))
    print(type(hash(t1)))

    # Check that two tasks don't have the same hash
    print("=" * 10)
    t1 = asyncio.create_task(task())
    t2 = asyncio.create_task(task())
    print(hash(t1) != hash(t2))

    # Add tasks to a set
    print("=" * 10)
    t1 = asyncio.create_task(task())
    t2 = asyncio.create_task(task())

    tasks = set()
    tasks.add(t1)
    print(t1 in tasks)
    print(t2 in tasks)


asyncio.run(main())
