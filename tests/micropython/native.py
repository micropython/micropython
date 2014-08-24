@micropython.native
def native_test(x):
    print(1, [], x)

native_test(2)

# check that GC doesn't collect the native function
import gc
gc.collect()
native_test(3)
