@micropython.viper
def f2b(x:int, y:int) -> int:
    return x + y

@micropython.native
def call(r):
    f = f2b
    for _ in r:
        f(1, 2)

bm_params = {
    (50, 10): (15000,),
    (100, 10): (30000,),
    (1000, 10): (300000,),
    (5000, 10): (1500000,),
}

def bm_setup(params):
    return lambda: call(range(params[0])), lambda: (params[0] // 1000, None)
