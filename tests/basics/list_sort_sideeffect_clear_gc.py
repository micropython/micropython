import gc
class check:
    def __init__(self, arr1, arr2):
        self.arr1 = arr1
        self.arr2 = arr2
    
    def __lt__(self, other):
        self.arr1.clear()
        self.arr2.extend([777] * 5)
        gc.collect()
        self.arr2.clear()
        return True

d1 = []
d2 = []
contaminators = [check(d1, d2) for _ in range(3)]
d1.extend(contaminators)
d2.extend(contaminators)
d1.sort()