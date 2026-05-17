from array import array
import os

h = array("h", (0x4041 for _ in range(50)))
with open("/remote/testfile", "wb") as f:
    n = f.write(h)
os.remove("/remote/testfile")
print(n)
