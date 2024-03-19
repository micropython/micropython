import os, mcx

ifs = mcx.Flash()

prog_size = ifs.progsize()

try:
    vfs = os.VfsLfs2(ifs, progsize=prog_size)
except:
    os.VfsLfs2.mkfs(ifs, progsize=prog_size)
    vfs = os.VfsLfs2(ifs, progsize=prog_size)

os.mount(vfs, "/")

del os, ifs, vfs, prog_size
