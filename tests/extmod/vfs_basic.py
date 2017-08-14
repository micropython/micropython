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
    def ilistdir(self, dir):
        print(self.id, 'ilistdir', dir)
        return iter([('a%d' % self.id, 0, 0)])
    def chdir(self, dir):
        print(self.id, 'chdir', dir)
    def getcwd(self):
        print(self.id, 'getcwd')
        return 'dir%d' % self.id
    def mkdir(self, path):
        print(self.id, 'mkdir', path)
    def remove(self, path):
        print(self.id, 'remove', path)
    def rename(self, old_path, new_path):
        print(self.id, 'rename', old_path, new_path)
    def rmdir(self, path):
        print(self.id, 'rmdir', path)
    def stat(self, path):
        print(self.id, 'stat', path)
        return (self.id,)
    def statvfs(self, path):
        print(self.id, 'statvfs', path)
        return (self.id,)
    def open(self, file, mode):
        print(self.id, 'open', file, mode)


# first we umount any existing mount points the target may have
try:
    uos.umount('/')
except OSError:
    pass
for path in uos.listdir('/'):
    uos.umount('/' + path)

# stat root dir
print(uos.stat('/'))

# getcwd when in root dir
print(uos.getcwd())

# basic mounting and listdir
uos.mount(Filesystem(1), '/test_mnt')
print(uos.listdir())

# ilistdir
i = uos.ilistdir()
print(next(i))
try:
    next(i)
except StopIteration:
    print('StopIteration')
try:
    next(i)
except StopIteration:
    print('StopIteration')

# referencing the mount point in different ways
print(uos.listdir('test_mnt'))
print(uos.listdir('/test_mnt'))

# mounting another filesystem
uos.mount(Filesystem(2), '/test_mnt2', readonly=True)
print(uos.listdir())
print(uos.listdir('/test_mnt2'))

# mounting over an existing mount point
try:
    uos.mount(Filesystem(3), '/test_mnt2')
except OSError:
    print('OSError')

# mkdir of a mount point
try:
    uos.mkdir('/test_mnt')
except OSError:
    print('OSError')

# rename across a filesystem
try:
    uos.rename('/test_mnt/a', '/test_mnt2/b')
except OSError:
    print('OSError')

# delegating to mounted filesystem
uos.chdir('test_mnt')
print(uos.listdir())
print(uos.getcwd())
uos.mkdir('test_dir')
uos.remove('test_file')
uos.rename('test_file', 'test_file2')
uos.rmdir('test_dir')
print(uos.stat('test_file'))
print(uos.statvfs('/test_mnt'))
open('test_file')
open('test_file', 'wb')

# umount
uos.umount('/test_mnt')
uos.umount('/test_mnt2')

# umount a non-existent mount point
try:
    uos.umount('/test_mnt')
except OSError:
    print('OSError')

# root dir
uos.mount(Filesystem(3), '/')
print(uos.listdir())
open('test')

uos.mount(Filesystem(4), '/mnt')
print(uos.listdir())
print(uos.listdir('/mnt'))
uos.chdir('/mnt')
print(uos.listdir())

# chdir to a subdir within root-mounted vfs, and then listdir
uos.chdir('/subdir')
print(uos.listdir())
uos.chdir('/')

uos.umount('/')
print(uos.listdir('/'))
uos.umount('/mnt')
