# This tests string handling operations

ZEN = """
The Zen of Python
Beautiful is better than ugly.
Explicit is better than implicit.
Simple is better than complex.
Complex is better than complicated.
Flat is better than nested.
Sparse is better than dense.
Readability counts.
Special cases aren't special enough to break the rules.
Although practicality beats purity.
Errors should never pass silently.
Unless explicitly silenced.
In the face of ambiguity, refuse the temptation to guess.
There should be one-- and preferably only one --obvious way to do it.
Although that way may not be obvious at first unless you're Dutch.
Now is better than never.
Although never is often better than *right* now.
If the implementation is hard to explain, it's a bad idea.
If the implementation is easy to explain, it may be a good idea.
Namespaces are one honking great idea -- let's do more of those!
"""


def test(niter):
    counts = {}
    for _ in range(niter):
        x = ZEN.replace("\n", " ").split(" ")
        y = " ".join(x)
        for i in range(50):
            a = ZEN[i : i * 2]
            b = a + "hello world"
        for c in ZEN:
            i = ord(c)
            c = chr(i)
    return (x[0], a)


###########################################################################
# Benchmark interface

bm_params = {
    (32, 10): (2,),
    (50, 10): (3,),
    (100, 10): (6,),
    (500, 10): (30,),
    (1000, 10): (60,),
    (5000, 10): (300,),
}


def bm_setup(params):
    (niter,) = params
    state = None

    def run():
        nonlocal state
        state = test(niter)

    def result():
        return niter, state

    return run, result
