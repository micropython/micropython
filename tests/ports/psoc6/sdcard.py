import machine
import os

machine_name = os.uname().machine

print("\n***** Test 1: File transfer to SD Card in LFS2 farmat *****\n")
print(machine_name)

test_string = "This is a test string."
long_test_string = "This is a very long string. And as a long string that it is, it is only getting longer and longer and the string goes. How long shall it be? Well, not really sure, but let´s try it like this."

# first priority is always LFS2 filesystem as it is the default
if "VfsLfs2" in dir(os):
    # open a file and do some operation
    print("write to /sd/test_sd_lfs2_short.txt")
    f = open("/sd/test_sd_lfs2_short.txt", "w")
    f.write(test_string)
    f.close()

    # read back the contents
    f = open("/sd/test_sd_lfs2_short.txt", "r")
    if f.read() == test_string:
        print("Test successful")
    f.close()

    # open a file and do some operation
    print("write to /sd/test_sd_lfs2_long.txt")
    f = open("/sd/test_sd_lfs2_long.txt", "w")
    f.write(long_test_string)
    f.close()
    # read back the contents
    f = open("/sd/test_sd_lfs2_long.txt", "r")
    if f.read() == long_test_string:
        print("Test successful")
    f.close()
