# test VFS functionality without any particular filesystem type

try:
    import os, vfs
except ImportError:
    print("SKIP")
    raise SystemExit


class Filesystem:
    def __init__(self, id, paths=[]):
        self.id = id
        self.paths = paths

    def mount(self, readonly, mksfs):
        print("mount", self)

    def umount(self):
        print("umount", self)

    def stat(self, path):
        if path in self.paths:
            return (0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        else:
            raise OSError

    statvfs = stat

    def open(self, path, mode):
        pass

    def __repr__(self):
        return "Filesystem(%d)" % self.id


# first we umount any existing mount points the target may have
try:
    vfs.umount("/")
except OSError:
    pass
for path in os.listdir("/"):
    vfs.umount("/" + path)


print(vfs.mount())

vfs.mount(Filesystem(1), "/foo")

print(vfs.mount())

vfs.mount(Filesystem(2), "/bar/baz")

print(vfs.mount())

vfs.mount(Filesystem(3), "/bar")

print(vfs.mount())

vfs.umount("/bar/baz")

print(vfs.mount())

vfs.mount(Filesystem(4), "/")

print(vfs.mount())
