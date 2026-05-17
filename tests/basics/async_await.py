# test basic await expression
# adapted from PEP0492

async def abinary(n):
    print(n)
    if n <= 0:
        return 1
    l = await abinary(n - 1)
    r = await abinary(n - 1)
    return l + 1 + r

o = abinary(4)
try:
    while True:
        o.send(None)
except StopIteration:
    print('finished')
