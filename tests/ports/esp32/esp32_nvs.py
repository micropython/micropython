# Test the esp32 NVS class - access to esp-idf's Non-Volatile-Storage

from esp32 import NVS

nvs = NVS("mp-test")

# test setting and getting an integer kv
nvs.set_i32("key1", 1234)
print(nvs.get_i32("key1"))
nvs.set_i32("key2", -503)
print(nvs.get_i32("key2"))
print(nvs.get_i32("key1"))

# test setting and getting a blob kv using a bytearray
blob1 = "testing a string as a blob"
nvs.set_blob("blob1", blob1)
buf1 = bytearray(len(blob1))
len1 = nvs.get_blob("blob1", buf1)
print(buf1)
print(len(blob1), len1)

# test setting and getting a blob kv using a string
blob2 = b"testing a bytearray"
nvs.set_blob("blob2", blob2)
buf2 = bytearray(len(blob2))
len2 = nvs.get_blob("blob2", buf2)
print(buf2)
print(len(blob2), len2)

# test raising of error exceptions
nvs.erase_key("key1")
try:
    nvs.erase_key("key1")  # not found
except OSError as e:
    print(e)
try:
    nvs.get_i32("key1")  # not found
except OSError as e:
    print(e)
try:
    nvs.get_i32("blob1")  # not found (blob1 exists but diff type)
except OSError as e:
    print(e)
try:
    buf3 = bytearray(10)
    nvs.get_blob("blob1", buf3)  # invalid length (too short)
except OSError as e:
    print(e)

nvs.commit()  # we're not verifying that this does anything, just doesn't error

# test using a second namespace and that it doesn't interfere with first
nvs2 = NVS("mp-test2")
try:
    print(nvs2.get_i32("key2"))
except OSError as e:
    print(e)
nvs2.set_i32("key2", 7654)
print(nvs.get_i32("key2"))
print(nvs2.get_i32("key2"))

# clean-up (the namespaces will remain)
nvs.erase_key("key2")
nvs.erase_key("blob1")
nvs.erase_key("blob2")
nvs2.erase_key("key2")
nvs.commit()
