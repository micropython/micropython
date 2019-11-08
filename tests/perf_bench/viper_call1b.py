@micropython.viper
def f1b(x) -> int:
    return int(x)

@micropython.native
def call(r):
    f = f1b
    for _ in r:
        f(1)

bm_params = {
    (50, 10): (15000,),
    (100, 10): (30000,),
    (1000, 10): (300000,),
    (5000, 10): (1500000,),
}

def bm_setup(params):
    return lambda: call(range(params[0])), lambda: (params[0] // 1000, None)
