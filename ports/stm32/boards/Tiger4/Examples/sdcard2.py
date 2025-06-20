from machine import Pin
import pyb, os

def mount_sd():
    try:
        sd_sw = Pin(pyb.Pin.board.SD_SW, Pin.OUT) #Set SD_SW to Low
        sd_sw.value(0)
        pyb.SDCard().power(0)
        os.mount(pyb.SDCard(), '/sd')
        print("SD card mounted successfully")
    except OSError as e:
        print("Error mounting SD card:", e)

def unmount_sd():
    try:
        os.umount('/sd')
        sd_sw = Pin(pyb.Pin.board.SD_SW, Pin.OUT)
        sd_sw.value(1)  # Set SD_SW to High to disable the SD card
        pyb.SDCard().power(1)
        print("SD card unmounted successfully")
    except OSError as e:
        print("Error unmounting SD card:", e)

# Initial Mount
mount_sd()

# Unmount the SD card
unmount_sd()

# Remount the SD card after unmounting
mount_sd()

stat1 =os.listdir('/')
stat2 = os.statvfs('/sd')
print(stat1,stat2) #(stat.f_bsize, stat.f_frsize, stat.f_blocks, stat.f_bfree, stat.f_bavail, stat.f_files, stat.f_ffree, stat.f_favail, stat.f_flag, stat.f_namemax)

# try some standard file operations
f = open('/sd/test.txt', 'w')
f.write('Testing SD card write operations')
f.close()
f = open('/sd/test.txt', 'r')
f.read()
f.close()

