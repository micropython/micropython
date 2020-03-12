#
# upip - Package manager for MicroPython
#
# Copyright (c) 2015-2018 Paul Sokolovsky
#
# Licensed under the MIT license.
#
import sys
import gc
import uos as os
import uerrno as errno
import ujson as json
import uzlib
import upip_utarfile as tarfile
import ussl
import usocket

gc.collect()


debug = False
index_urls = ["https://micropython.org/pi", "https://pypi.org/pypi"]
install_path = None
cleanup_files = []
gzdict_sz = 16 + 15
fail_reason = None
file_buf = bytearray(512)


class NotFoundError(Exception):
    pass


def op_split(path):
    if path == "":
        return ("", "")
    r = path.rsplit("/", 1)
    if len(r) == 1:
        return ("", path)
    head = r[0]
    if not head:
        head = "/"
    return (head, r[1])


def op_basename(path):
    return op_split(path)[1]


# Expects *file* name
def _makedirs(name, mode=0o777):
    ret = False
    s = ""
    comps = name.rstrip("/").split("/")[:-1]
    if comps[0] == "":
        s = "/"
    for c in comps:
        if s and s[-1] != "/":
            s += "/"
        s += c
        try:
            os.mkdir(s)
            ret = True
        except OSError as e:
            if e.args[0] != errno.EEXIST and e.args[0] != errno.EISDIR:
                raise e
            ret = False
    return ret


def save_file(fname, subf):
    global file_buf
    with open(fname, "wb") as outf:
        while True:
            sz = subf.readinto(file_buf)
            if not sz:
                break
            outf.write(file_buf, sz)


def install_tar(f, prefix):
    meta = {}
    for info in f:
        # print(info)
        fname = info.name
        try:
            fname = fname[fname.index("/") + 1 :]
        except ValueError:
            pass

        save = True
        for p in ("setup.", "PKG-INFO", "README"):
            # print(fname, p)
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
                save_file(outfname, subf)
    return meta


def expandhome(s):
    if "~/" in s:
        h = os.getenv("HOME")
        s = s.replace("~/", h + "/")
    return s


warn_ussl = True


def url_open(url):
    global warn_ussl

    if debug:
        print(url)

    proto, _, host, urlpath = url.split("/", 3)
    host, port = (host.split(":", 1) + [443 if proto == "https:" else 80])[0:2]
    try:
        ai = usocket.getaddrinfo(host, int(port), 0, usocket.SOCK_STREAM)
        if not ai:
            raise OSError(errno.EINVAL)
    except OSError as e:
        fatal("Unable to resolve %s (no Internet?)" % host, e)
    # print("Address infos:", ai)
    ai = ai[0]

    s = usocket.socket(ai[0], ai[1], ai[2])
    try:
        # print("Connect address:", addr)
        s.connect(ai[-1])

        if proto == "https:":
            s = ussl.wrap_socket(s, server_hostname=host)
            if warn_ussl:
                print("Warning: %s SSL certificate is not validated" % host)
                warn_ussl = False

        # MicroPython rawsocket module supports file interface directly
        s.write("GET /%s HTTP/1.0\r\nHost: %s:%s\r\n\r\n" % (urlpath, host, port))
        line = s.readline()
        protover, status, msg = line.split(None, 2)
        if status != b"200":
            if status == b"404" or status == b"301":
                raise NotFoundError("Package not found")
            raise ValueError(status)
        while 1:
            line = s.readline()
            if not line:
                raise ValueError("Unexpected EOF in HTTP headers")
            if line == b"\r\n":
                break
    except Exception as e:
        s.close()
        raise e

    return s


def get_pkg_metadata(name):
    global warn_ussl
    for url in index_urls:
        try:
            f = url_open("%s/%s/json" % (url, name))
        except NotFoundError:
            warn_ussl = True
            continue
        try:
            return json.load(f)
        finally:
            f.close()
    raise NotFoundError("Package not found")


def fatal(msg, exc=None):
    print("Error:", msg)
    if exc and debug:
        raise exc
    sys.exit(1)


def install_pkg(pkg_spec, install_path):
    data = get_pkg_metadata(pkg_spec)

    latest_ver = data["info"]["version"]
    packages = data["releases"][latest_ver]
    del data
    gc.collect()

    packages = [pkg for pkg in packages if pkg.get("packagetype", "sdist") == "sdist"]
    package_url = packages[0]["url"]
    print("Installing %s %s from %s" % (pkg_spec, latest_ver, package_url))
    f1 = url_open(package_url)
    try:
        f2 = uzlib.DecompIO(f1, gzdict_sz)
        f3 = tarfile.TarFile(fileobj=f2)
        meta = install_tar(f3, install_path)
    finally:
        f1.close()
    del f3
    del f2
    gc.collect()
    return meta


def install(to_install, install_path=None):
    # Calculate gzip dictionary size to use
    global gzdict_sz, fail_reason
    fail_reason = None
    sz = gc.mem_free() + gc.mem_alloc()
    if sz <= 65536:
        gzdict_sz = 16 + 12

    if install_path is None:
        install_path = get_install_path()
    if install_path[-1] != "/":
        install_path += "/"
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
        print(
            "Error installing '{}': {}, packages may be partially installed".format(
                pkg_spec, e
            ),
            file=sys.stderr,
        )
        fail_reason = e


def get_install_path():
    global install_path
    if install_path is None:
        # sys.path[0] is current module's path
        install_path = sys.path[1]
    install_path = expandhome(install_path)
    return install_path


def cleanup():
    for fname in cleanup_files:
        try:
            os.unlink(fname)
        except OSError:
            print("Warning: Cannot delete " + fname)


def help():
    print(
        """\
upip - Simple PyPI package manager for MicroPython
Usage: micropython -m upip install [-p <path>] <package>... | -r <requirements.txt>
import upip; upip.install(package_or_list, [<path>])

If <path> is not given, packages will be installed into sys.path[1]
(can be set from MICROPYPATH environment variable, if current system
supports that)."""
    )
    print("Current value of sys.path[1]:", sys.path[1])
    print(
        """\

Note: only MicroPython packages (usually, named micropython-*) are supported
for installation, upip does not support arbitrary code in setup.py.
"""
    )


def main():
    global debug
    global index_urls
    global install_path
    install_path = None

    if len(sys.argv) < 2 or sys.argv[1] == "-h" or sys.argv[1] == "--help":
        help()
        return

    if sys.argv[1] != "install":
        fatal("Only 'install' command supported")

    to_install = []

    i = 2
    while i < len(sys.argv) and sys.argv[i][0] == "-":
        opt = sys.argv[i]
        i += 1
        if opt == "-h" or opt == "--help":
            help()
            return
        elif opt == "-p":
            install_path = sys.argv[i]
            i += 1
        elif opt == "-r":
            list_file = sys.argv[i]
            i += 1
            with open(list_file) as f:
                while True:
                    line = f.readline()
                    if not line:
                        break
                    if line[0] == "#":
                        continue
                    to_install.append(line.rstrip())
        elif opt == "-i":
            index_urls = [sys.argv[i]]
            i += 1
        elif opt == "--debug":
            debug = True
        else:
            fatal("Unknown/unsupported option: " + opt)

    to_install.extend(sys.argv[i:])
    if not to_install:
        help()
        return

    install(to_install)

    if not debug:
        cleanup()


if __name__ == "__main__":
    main()
