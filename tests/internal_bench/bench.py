import time


ITERS = 20000000

def run(f):
    t = time.time()
    f(ITERS)
    t = time.time() - t
    print(t)
