# test VFS functionality without any particular filesystem type

try:
    try:
        import uos_vfs as uos
        open = uos.vfs_open
    except ImportError:
        import uos
    uos.mount
except (ImportError, AttributeError):
    print("SKIP")
    import sys
    sys.exit()


class Filesystem:
    def __init__(self, id):
        self.id = id
    def mount(self, readonly, mkfs):
        print(self.id, 'mount', readonly, mkfs)
    def umount(self):
        print(self.id, 'umount')
    def listdir(self, dir):
        print(self.id, 'listdir', dir)
        return ['a%d' % self.id]
    def chdir(self, dir):
        print(self.id, 'chdir', dir)
    def open(self, file, mode):
        print(self.id, 'open', file, mode)


# basic mounting and listdir
uos.mount(Filesystem(1), '/test_mnt')
print(uos.listdir())

# referencing the mount point in different ways
print(uos.listdir('test_mnt'))
print(uos.listdir('/test_mnt'))

# mounting another filesystem
uos.mount(Filesystem(2), '/test_mnt2', readonly=True)
print(uos.listdir())
print(uos.listdir('/test_mnt2'))

# chdir
uos.chdir('test_mnt')
print(uos.listdir())

# open
open('test_file')
open('test_file', 'wb')

# umount
uos.umount('/test_mnt')
uos.umount('/test_mnt2')
