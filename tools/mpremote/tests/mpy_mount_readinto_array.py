from array import array
import os

with open("/remote/test", "wb") as f:
    f.write(b"abcd")
a = array("H", (0 for _ in range(2)))
with open("/remote/test", "rb") as f:
    n = f.readinto(a)
os.remove("/remote/test")
print(n)
print(list(a))
