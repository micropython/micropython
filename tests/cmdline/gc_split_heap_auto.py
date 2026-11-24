# cmdline: -X heapsize=128K
# A heap area added on demand must be kept while its first block is the only one in use.
import gc

store = [None] * 2000
for k in range(len(store)):
    store[k] = (k,)
    gc.collect()
    if k and store[k - 1][0] != k - 1:
        print("tuple", k - 1, "now reads", store[k - 1])
        break
else:
    print("no corruption")
