#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import os
import errno
import json
import zlib
import tarfile
import tempfile
import ssl as ussl
import socket as usocket

debug = False
g_install_path = os.getcwd()  # Default install path
gzdict_sz = 16 + 15

def version():
    print('Python version 3.2 or above is required.')
    sys.exit(1)

if sys.version_info.major < 3:
    version()
elif sys.version_info.major == 3 and sys.version_info.minor < 2:
    version()

class NotFoundError(Exception):
    pass

# Read a line from a socket
def read_line(sock):
    ret = b''
    while True:
        c = sock.recv(1)
        if c == b'':
            break
        elif c == b'\n':
            ret += c
            break
        else:
            ret += c
    return ret

# Read multiple lines from a socket
def read_lines(sock):
    s = b''
    while True:
        s1 = read_line(sock)
        s += s1
        if s1 == b'\r\n' or s1 == b'':
            break
    return s

# Expects absolute path and *file* name
def _makedirs(name):
    dirname = os.path.dirname(name)
    def split_path(lst, path):
        q = os.path.split(path)
        if q[1] != '':
            lst.append(q[1])
            split_path(lst, q[0])

    lst = []
    split_path(lst, dirname)
    lst.reverse()
    mypath = os.path.abspath('/')
    for elem in lst:
        mypath = os.path.join(mypath, elem)
        if not os.path.exists(mypath):
            try:
                os.mkdir(mypath)
            except OSError as e:
                if e.args[0] != errno.EEXIST and e.args[0] != errno.EISDIR:
                    raise


def install_tar(f, prefix):
    meta = {}
    for info in f:
        #print(info)
        fname = info.name
        try:
            fname = fname[fname.index("/") + 1:]
        except ValueError:
            fname = ""

        save = True
        for p in ("setup.", "PKG-INFO", "README"):
            #print(fname, p)
            if fname.startswith(p) or ".egg-info" in fname:
                if fname.endswith("/requires.txt"):
                    meta["deps"] = f.extractfile(info).read()
                save = False
                if debug:
                    print("Skipping", fname)
                break

        if save:
            outfname = prefix + fname
            if info.type != tarfile.DIRTYPE:
                if debug:
                    print("Extracting " + outfname)
                _makedirs(outfname)
                subf = f.extractfile(info)
                with open(outfname, "wb") as outf:
                    outf.write(subf.read())
    return meta

warn_ussl = True
def url_open(url):
    global warn_ussl

    if debug:
        print(url)

    proto, _, host, urlpath = url.split('/', 3)
    try:
        ai = usocket.getaddrinfo(host, 443)
    except OSError as e:
        print("Unable to resolve %s (no Internet?)" % host)
        raise
    addr = ai[0][4]
    s = usocket.socket(ai[0][0])
    try:
        if proto == "https:":
            s = ussl.wrap_socket(s)
            if warn_ussl:
                print("Warning: %s SSL certificate is not validated" % host)
                warn_ussl = False
        s.connect(addr)
        s.setblocking(True)

        s.send(("GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n" % (urlpath, host)).encode('UTF8'))
        l = read_line(s)
        protover, status, msg = l.split(None, 2)
        if status != b"200":
            if status == b"404" or status == b"301":
                raise NotFoundError("Package not found")
            raise ValueError(status)
        while 1:
            l = read_line(s)
            if not l:
                raise ValueError("Unexpected EOF in HTTP headers")
            if l == b'\r\n':
                break
    except Exception as e:
        s.close()
        raise e

    return s

def get_pkg_metadata(name):
    f = url_open("https://pypi.python.org/pypi/%s/json" % name)
    s = read_lines(f)
    try:
        return json.loads(s.decode('UTF8'))
    finally:
        f.close()

def fatal(msg):
    print("Error:", msg)
    sys.exit(1)

def install_pkg(pkg_spec, install_path):
    data = get_pkg_metadata(pkg_spec)

    latest_ver = data["info"]["version"]
    packages = data["releases"][latest_ver]
    assert len(packages) == 1
    package_url = packages[0]["url"]
    print("Installing %s %s from %s" % (pkg_spec, latest_ver, package_url))
    f1 = url_open(package_url)
    s = read_lines(f1)
    try:
        str1 = zlib.decompress(s, gzdict_sz)
        with tempfile.TemporaryFile() as temp_file:
            temp_file.write(str1)
            temp_file.seek(0)
            with tarfile.TarFile(fileobj=temp_file) as tar_file:  # Expects a file object
                meta = install_tar(tar_file, install_path)
    finally:
        f1.close()
    return meta


def install(to_install):
    install_path = g_install_path
    install_path = os.path.join(install_path, '')  # Append final /
    if not isinstance(to_install, list):
        to_install = [to_install]
    print("Installing to: " + install_path)
    # sets would be perfect here, but don't depend on them
    installed = []
    try:
        while to_install:
            if debug:
                print("Queue:", to_install)
            pkg_spec = to_install.pop(0)
            if pkg_spec in installed:
                continue
            meta = install_pkg(pkg_spec, install_path)
            installed.append(pkg_spec)
            if debug:
                print(meta)
            deps = meta.get("deps", "").rstrip()
            if deps:
                deps = deps.decode("utf-8").split("\n")
                to_install.extend(deps)
    except Exception as e:
        print("Error installing '{}': {}, packages may be partially installed".format(
            pkg_spec, e), file=sys.stderr)

def help_msg():
    print("""\
micropip - Simple PyPI package manager for MicroPython
Runs on a PC under Python 3.2 or above, and installs to a PC directory for
subsequent transfer to target hardware.

Usage: micropip.py install [-p <path>] <package>... | -r <requirements.txt>
The above requires micropip.py to have executable permission.
Alternatively: python3 -m micropip install [-p <path>] <package>... | -r <requirements.txt>

If <path> is not given, packages will be installed into the current directory.

Note: only MicroPython packages (usually, named micropython-*) are supported
for installation, upip does not support arbitrary code in setup.py.
""")

def main():
    global debug
    global g_install_path

    if len(sys.argv) < 2 or sys.argv[1] == "-h" or sys.argv[1] == "--help":
        help_msg()
        return

    if sys.argv[1] != "install":
        fatal("Only 'install' command supported")

    to_install = []

    i = 2
    while i < len(sys.argv) and sys.argv[i][0] == "-":
        opt = sys.argv[i]
        i += 1
        if opt == "-h" or opt == "--help":
            help_msg()
            return
        elif opt == "-p":
            g_install_path = sys.argv[i]
            i += 1
        elif opt == "-r":
            list_file = sys.argv[i]
            i += 1
            with open(list_file) as f:
                while True:
                    l = f.readline()
                    if not l:
                        break
                    if l[0] == "#":
                        continue
                    to_install.append(l.rstrip())
        elif opt == "--debug":
            debug = True
        else:
            fatal("Unknown/unsupported option: " + opt)

    to_install.extend(sys.argv[i:])
    if not to_install:
        help_msg()
        return

    g_install_path = os.path.expanduser(g_install_path)
    g_install_path = os.path.abspath(g_install_path)
    install(to_install)


if __name__ == "__main__":
    main()
