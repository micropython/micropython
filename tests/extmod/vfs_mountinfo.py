# test VFS functionality without any particular filesystem type

try:
    import os, vfs
except ImportError:
    print("SKIP")
    raise SystemExit
import errno


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


def check_lists():
    it = vfs.ilistmounts()
    try:
        while True:
            mnt = next(it)
            print(mnt)
    except StopIteration:
        print("StopIteration")

    print(vfs.listmounts())


def check_path(path):
    print(repr(path), ':', vfs.mountinfo(path))


# first we umount any existing mount points the target may have
try:
    vfs.umount("/")
except OSError:
    pass
for path in os.listdir("/"):
    vfs.umount("/" + path)


check_lists()
check_path("")
check_path("/")
check_path("/foo")

vfs.mount(Filesystem(1), "/foo")

check_lists()
check_path("")
check_path("/")
check_path("/foo")
check_path("/foo/bar")
check_path("/bar")

vfs.mount(Filesystem(2), "/bar/baz")

check_lists()
check_path("/bar")
check_path("/bar/baz")

vfs.mount(Filesystem(3), "/bar")

check_lists()
check_path("/bar")
check_path("/bar/baz")
