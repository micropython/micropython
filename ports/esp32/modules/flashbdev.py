from esp32 import Partition

bdev = Partition.find(Partition.TYPE_DATA, label='vfs')
bdev = bdev[0] if bdev else None
