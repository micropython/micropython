async def f():
    pass

try:
    f()  # Should not crash
except Exception as e:
    print('failed to invoke')

try:
    next(f())
    print('This should fail because async def returns a coroutine, and next() is not allowed')
except Exception as e:
    print('pass')
