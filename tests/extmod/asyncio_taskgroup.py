# Test Lock class

import sys

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit
try:
    ExceptionGroup
except NameError:
    ExceptionGroup = asyncio.ExceptionGroup


class MyExc(Exception):
    pass


async def test_taskgroup_01():
    T1 = T2 = 0

    async def foo1():
        nonlocal T1
        await asyncio.sleep(0.1)
        T1 = 42

    async def foo2():
        nonlocal T2
        await asyncio.sleep(0.2)
        T2 = 11

    async with asyncio.TaskGroup() as g:
        t1 = g.create_task(foo1())
        t2 = g.create_task(foo2())

    print("T1", T1)
    print("T2", T2)


async def test_taskgroup_02():
    T1 = T2 = 0

    async def foo1():
        nonlocal T1
        await asyncio.sleep(0.1)
        T1 = 42

    async def foo2():
        nonlocal T2
        await asyncio.sleep(0.2)
        T2 = 11

    async with asyncio.TaskGroup() as g:
        t1 = g.create_task(foo1())
        await asyncio.sleep(0.15)
        t2 = g.create_task(foo2())

    print("T1", T1)
    print("T2", T2)


async def test_taskgroup_03():
    T1 = T2 = 0

    async def foo1():
        nonlocal T1
        await asyncio.sleep(1)
        T1 = 42

    async def foo2():
        nonlocal T2
        await asyncio.sleep(0.2)
        T2 = 11

    async with asyncio.TaskGroup() as g:
        t1 = g.create_task(foo1())
        await asyncio.sleep(0.15)
        # cancel t1 explicitly, i.e. everything should continue
        # working as expected.
        t1.cancel()

        t2 = g.create_task(foo2())

    print("T1", T1)  # 0
    print("T2", T2)


async def test_taskgroup_04():
    NUM = 0
    t2_cancel = False
    t2 = None

    async def foo1():
        await asyncio.sleep(0.1)
        1 / 0

    async def foo2():
        nonlocal NUM, t2_cancel
        try:
            await asyncio.sleep(1)
        except asyncio.CancelledError:
            t2_cancel = True
            raise
        NUM += 1

    async def runner():
        nonlocal NUM, t2

        async with asyncio.TaskGroup() as g:
            g.create_task(foo1())
            t2 = g.create_task(foo2())

        NUM += 10

    try:
        await asyncio.get_event_loop().create_task(runner())
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc

    print("NUM", NUM)
    print("T2C", t2_cancel)


async def test_taskgroup_05():
    NUM = 0
    t2_cancel = False
    runner_cancel = False

    async def foo1():
        await asyncio.sleep(0.1)
        1 / 0

    async def foo2():
        nonlocal NUM, t2_cancel
        try:
            await asyncio.sleep(5)
        except asyncio.CancelledError:
            t2_cancel = True
            raise
        NUM += 1

    async def runner():
        nonlocal NUM, runner_cancel

        async with asyncio.TaskGroup() as g:
            g.create_task(foo1())
            g.create_task(foo1())
            g.create_task(foo1())
            g.create_task(foo2())
            try:
                await asyncio.sleep(10)
            except asyncio.CancelledError as exc:
                runner_cancel = True
                raise exc

        NUM += 10

    # The 3 foo1 sub tasks can be racy when the host is busy - if the
    # cancellation happens in the middle, we'll see partial sub errors here
    try:
        await asyncio.get_event_loop().create_task(runner())
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc

    print("NUM", NUM)
    print("T2", t2_cancel)
    print("RUN", runner_cancel)


async def test_taskgroup_06():
    NUM = 0

    async def foo():
        nonlocal NUM
        try:
            await asyncio.sleep(5)
        except asyncio.CancelledError:
            NUM += 1
            raise

    async def runner():
        async with asyncio.TaskGroup() as g:
            for _ in range(5):
                g.create_task(foo())

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.1)

    print("D", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")

    print("NUM", NUM)


async def test_taskgroup_07():
    NUM = 0

    async def foo():
        nonlocal NUM
        try:
            await asyncio.sleep(5)
        except asyncio.CancelledError:
            NUM += 1
            raise

    async def runner():
        nonlocal NUM
        async with asyncio.TaskGroup() as g:
            for _ in range(5):
                g.create_task(foo())

            try:
                await asyncio.sleep(10)
            except asyncio.CancelledError:
                NUM += 10
                raise

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.1)

    print("D", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")

    print("NUM", NUM)


async def test_taskgroup_08():
    async def foo():
        await asyncio.sleep(0.1)
        1 / 0

    async def runner():
        async with asyncio.TaskGroup() as g:
            for _ in range(5):
                g.create_task(foo())

            await asyncio.sleep(10)

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.2)

    print("D", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc


async def test_taskgroup_09():
    t1 = t2 = None
    T1 = T2 = 0

    async def foo1():
        nonlocal T1
        await asyncio.sleep(1)
        T1 = 42

    async def foo2():
        nonlocal T2
        await asyncio.sleep(2)
        T2 = 11

    async def runner():
        nonlocal t1, t2
        async with asyncio.TaskGroup() as g:
            t1 = g.create_task(foo1())
            t2 = g.create_task(foo2())
            await asyncio.sleep(0.1)
            1 / 0

    try:
        await runner()
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc

    print("T1", T1)
    print("T2", T2)


async def test_taskgroup_10():
    t1 = t2 = None
    T1 = T2 = 0

    async def foo1():
        nonlocal T1
        await asyncio.sleep(1)
        T1 = 42

    async def foo2():
        nonlocal T2
        await asyncio.sleep(2)
        T2 = 11

    async def runner():
        nonlocal t1, t2
        async with asyncio.TaskGroup() as g:
            t1 = g.create_task(foo1())
            t2 = g.create_task(foo2())
            1 / 0

    try:
        await runner()
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc

    print("T1", T1)
    print("T2", T2)


async def test_taskgroup_11():
    async def foo():
        await asyncio.sleep(0.1)
        1 / 0

    async def runner():
        async with asyncio.TaskGroup():
            async with asyncio.TaskGroup() as g2:
                for _ in range(5):
                    g2.create_task(foo())

                try:
                    await asyncio.sleep(10)
                except asyncio.CancelledError:
                    raise

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.2)

    print("D", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc


async def test_taskgroup_12():
    async def foo():
        await asyncio.sleep(0.1)
        1 / 0

    async def runner():
        async with asyncio.TaskGroup() as g1:
            g1.create_task(asyncio.sleep(10))

            async with asyncio.TaskGroup() as g2:
                for _ in range(5):
                    g2.create_task(foo())

                try:
                    await asyncio.sleep(10)
                except asyncio.CancelledError:
                    raise

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.2)

    print("R", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc


async def test_taskgroup_13():
    async def crash_after(t):
        await asyncio.sleep(t)
        raise ValueError(t)

    async def runner():
        async with asyncio.TaskGroup() as g1:
            g1.create_task(crash_after(0.1))

            async with asyncio.TaskGroup() as g2:
                g2.create_task(crash_after(0.2))
                print("E1")
            print("E2")
        print("E3")

    r = asyncio.get_event_loop().create_task(runner())
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ValueError:
        print("VAL")
    except ExceptionGroup as exc:
        a, b = exc.split(ValueError)
        if a and not b:
            print("VAL")
        else:
            raise exc


async def test_taskgroup_14():
    async def crash_after(t):
        await asyncio.sleep(t)
        raise ValueError(t)

    async def runner():
        async with asyncio.TaskGroup() as g1:
            g1.create_task(crash_after(0.2))

            async with asyncio.TaskGroup() as g2:
                g2.create_task(crash_after(0.1))

    r = asyncio.get_event_loop().create_task(runner())
    try:
        await r
    except ValueError:
        print("VAL")
    except ExceptionGroup as exc:
        a, b = exc.split(ValueError)
        if a and not b:
            print("VAL")
        else:
            raise exc


async def test_taskgroup_15():
    async def crash_soon():
        await asyncio.sleep(0.3)
        1 / 0

    async def runner():
        async with asyncio.TaskGroup() as g1:
            g1.create_task(crash_soon())
            try:
                await asyncio.sleep(10)
            except asyncio.CancelledError:
                await asyncio.sleep(0.5)
                raise

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.1)

    print("R", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc


async def test_taskgroup_16():
    async def crash_soon():
        await asyncio.sleep(0.3)
        1 / 0

    async def nested_runner():
        async with asyncio.TaskGroup() as g1:
            g1.create_task(crash_soon())
            try:
                await asyncio.sleep(10)
            except asyncio.CancelledError:
                await asyncio.sleep(0.5)
                raise

    async def runner():
        t = asyncio.get_event_loop().create_task(nested_runner())
        await t

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.1)

    print("R", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")
    except ZeroDivisionError:
        print("ZERO")
    except ExceptionGroup as exc:
        a, b = exc.split(ZeroDivisionError)
        if a and not b:
            print("ZERO")
        else:
            raise exc


async def test_taskgroup_17():
    NUM = 0

    async def runner():
        nonlocal NUM
        async with asyncio.TaskGroup():
            try:
                await asyncio.sleep(10)
            except asyncio.CancelledError:
                NUM += 10
                raise

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.1)

    print("R", r.done())
    r.cancel()
    try:
        await r
    except asyncio.CancelledError:
        print("CANCEL")

    print("NUM", NUM)


async def test_taskgroup_18():
    NUM = 0

    async def runner():
        nonlocal NUM
        async with asyncio.TaskGroup():
            try:
                await asyncio.sleep(10)
            except asyncio.CancelledError:
                NUM += 10
                # This isn't a good idea, but we have to support
                # this weird case.
                raise MyExc

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.1)

    print("R", r.done())
    r.cancel()

    try:
        await r
    except MyExc:
        print("EXC")
    except ExceptionGroup as exc:
        a, b = exc.split(MyExc)
        if a and not b:
            print("EXC")
        else:
            raise exc

    print("NUM", NUM)


async def test_taskgroup_19():
    async def crash_soon():
        await asyncio.sleep(0.1)
        1 / 0

    async def nested():
        try:
            await asyncio.sleep(10)
        finally:
            raise MyExc

    async def runner():
        async with asyncio.TaskGroup() as g:
            g.create_task(crash_soon())
            await nested()

    r = asyncio.get_event_loop().create_task(runner())
    try:
        await r
    except ExceptionGroup:
        print("EXC OK")


async def test_taskgroup_20():
    if sys.implementation.name != "micropython":
        # fake this test
        print("KBD")
        return

    async def crash_soon():
        await asyncio.sleep(0.1)
        1 / 0

    async def nested():
        try:
            await asyncio.sleep(10)
        finally:
            raise KeyboardInterrupt

    async def runner():
        async with asyncio.TaskGroup() as g:
            g.create_task(crash_soon())
            await nested()

    try:
        await runner()
    except KeyboardInterrupt:
        print("KBD")


async def test_taskgroup_21():
    if sys.implementation.name != "micropython":
        # fake this test
        print("KBD")
        return

    async def crash_soon():
        await asyncio.sleep(0.1)
        raise KeyboardInterrupt

    async def nested():
        try:
            await asyncio.sleep(10)
        finally:
            raise TypeError

    async def runner():
        async with asyncio.TaskGroup() as g:
            g.create_task(crash_soon())
            await nested()

    try:
        await runner()
    except KeyboardInterrupt:
        print("KBD")


async def test_taskgroup_22():
    T1 = T2 = 0

    async def foo1():
        nonlocal T1
        await asyncio.sleep(1)
        T1 = 42

    async def foo2():
        nonlocal T2
        await asyncio.sleep(2)
        T2 = 11

    async def runner():
        async with asyncio.TaskGroup() as g:
            g.create_task(foo1())
            g.create_task(foo2())

    r = asyncio.get_event_loop().create_task(runner())
    await asyncio.sleep(0.05)
    r.cancel()

    try:
        await r
    except asyncio.CancelledError:
        print("Cancelled")

    print("T1", T1)
    print("T2", T2)


async def test_taskgroup_23():
    async def do_job(delay):
        await asyncio.sleep(delay)

    async with asyncio.TaskGroup() as g:
        for count in range(10):
            await asyncio.sleep(0.1)
            g.create_task(do_job(0.3))
            if count == 5 and len(g._tasks) >= 5:
                print("FAIL")
        await asyncio.sleep(1.35)
        print("END", len(g._tasks))


async def test_taskgroup_24():
    async def die():
        await asyncio.sleep(0.1)
        raise ValueError()

    async def nodie():
        await asyncio.sleep(0.3)

    try:
        async with asyncio.TaskGroup() as g:
            g.create_task(die())
    except ValueError:
        print("VAL")
    else:
        print("NO")

    try:
        async with asyncio.TaskGroup() as g:
            g.create_task(die())
            await asyncio.sleep(0.2)
    except asyncio.CancelledError:
        print("CANC")
    except ValueError:
        print("VAL")
    else:
        print("NO")

    try:
        async with asyncio.TaskGroup() as g:
            g.create_task(die())
            g.create_task(nodie())
            await asyncio.sleep(0.2)
    except asyncio.CancelledError:
        print("CANC")
    except ValueError:
        print("VAL")
    else:
        print("NO")


def exc_handler(loop, ctx):
    print("Not handled:", str(ctx["exception"]))


async def main():
    asyncio.get_event_loop().set_exception_handler(exc_handler)
    # Basics
    print("--- 01")
    await test_taskgroup_01()
    print("--- 02")
    await test_taskgroup_02()
    print("--- 03")
    await test_taskgroup_03()
    print("--- 04")
    await test_taskgroup_04()
    print("--- 05")
    await test_taskgroup_05()
    print("--- 06")
    await test_taskgroup_06()
    print("--- 07")
    await test_taskgroup_07()
    print("--- 08")
    await test_taskgroup_08()
    print("--- 09")
    await test_taskgroup_09()
    print("--- 10")
    await test_taskgroup_10()
    print("--- 11")
    await test_taskgroup_11()
    print("--- 12")
    await test_taskgroup_12()
    print("--- 13")
    await test_taskgroup_13()
    print("--- 14")
    await test_taskgroup_14()
    print("--- 15")
    await test_taskgroup_15()
    print("--- 16")
    await test_taskgroup_16()
    print("--- 17")
    await test_taskgroup_17()
    print("--- 18")
    await test_taskgroup_18()
    print("--- 19")
    await test_taskgroup_19()
    print("--- 20")
    await test_taskgroup_20()
    print("--- 21")
    await test_taskgroup_21()
    print("--- 22")
    await test_taskgroup_22()
    print("--- 23")
    await test_taskgroup_23()
    print("--- 24")
    await test_taskgroup_24()
    print("--- END")


asyncio.run(main())
