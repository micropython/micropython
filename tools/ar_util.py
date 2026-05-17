#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2024 Volodymyr Shymanskyy
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import os
import re
import hashlib
import functools
import pickle

from elftools.elf import elffile
from collections import defaultdict

try:
    from ar import Archive
except:
    Archive = None


class PickleCache:
    def __init__(self, path, prefix=""):
        self.path = path
        self._get_fn = lambda key: os.path.join(path, prefix + key[:24])

    def store(self, key, data):
        os.makedirs(self.path, exist_ok=True)
        # See also https://bford.info/cachedir/
        cachedir_tag_path = os.path.join(self.path, "CACHEDIR.TAG")
        if not os.path.exists(cachedir_tag_path):
            with open(cachedir_tag_path, "w") as f:
                f.write(
                    "Signature: 8a477f597d28d172789f06886806bc55\n"
                    "# This file is a cache directory tag created by MicroPython.\n"
                    "# For information about cache directory tags see https://bford.info/cachedir/\n"
                )
        with open(self._get_fn(key), "wb") as f:
            pickle.dump(data, f)

    def load(self, key):
        with open(self._get_fn(key), "rb") as f:
            return pickle.load(f)


def cached(key, cache):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            cache_key = key(*args, **kwargs)
            try:
                d = cache.load(cache_key)
                if d["key"] != cache_key:
                    raise Exception("Cache key mismatch")
                return d["data"]
            except Exception:
                res = func(*args, **kwargs)
                try:
                    cache.store(
                        cache_key,
                        {
                            "key": cache_key,
                            "data": res,
                        },
                    )
                except Exception:
                    pass
                return res

        return wrapper

    return decorator


class CachedArFile:
    def __init__(self, fn):
        if not Archive:
            raise RuntimeError("Please run 'pip install ar' to link .a files")
        self.fn = fn
        self._archive = Archive(open(fn, "rb"))
        info = self.load_symbols()
        self.objs = info["objs"]
        self.symbols = info["symbols"]

    def open(self, obj):
        return self._archive.open(obj, "rb")

    def _cache_key(self):
        sha = hashlib.sha256()
        with open(self.fn, "rb") as f:
            for chunk in iter(lambda: f.read(4096), b""):
                sha.update(chunk)
        # Change this salt if the cache data format changes
        sha.update(bytes.fromhex("00000000000000000000000000000001"))
        return sha.hexdigest()

    @cached(key=_cache_key, cache=PickleCache(path=".mpy_ld_cache", prefix="ar_"))
    def load_symbols(self):
        print("Loading", self.fn)
        objs = defaultdict(lambda: {"def": set(), "undef": set(), "weak": set()})
        symbols = {}
        for entry in self._archive:
            obj_name = entry.name
            elf = elffile.ELFFile(self.open(obj_name))
            symtab = elf.get_section_by_name(".symtab")
            if not symtab:
                continue

            obj = objs[obj_name]

            for symbol in symtab.iter_symbols():
                sym_name = symbol.name
                sym_bind = symbol["st_info"]["bind"]

                if sym_bind in ("STB_GLOBAL", "STB_WEAK"):
                    if symbol.entry["st_shndx"] != "SHN_UNDEF":
                        obj["def"].add(sym_name)
                        symbols[sym_name] = obj_name
                    else:
                        obj["undef"].add(sym_name)

                    if sym_bind == "STB_WEAK":
                        obj["weak"].add(sym_name)

        return {"objs": dict(objs), "symbols": symbols}


def resolve(archives, symbols):
    resolved_objs = []  # Object files needed to resolve symbols
    unresolved_symbols = set()
    provided_symbols = {}  # Which symbol is provided by which object
    symbol_stack = list(symbols)

    # A helper function to handle symbol resolution from a particular object
    def add_obj(archive, symbol):
        obj_name = archive.symbols[symbol]
        obj_info = archive.objs[obj_name]

        obj_tuple = (archive, obj_name)
        if obj_tuple in resolved_objs:
            return  # Already processed this object

        resolved_objs.append(obj_tuple)

        # Add the symbols this object defines
        for defined_symbol in obj_info["def"]:
            if defined_symbol in provided_symbols and not defined_symbol.startswith(
                "__x86.get_pc_thunk."
            ):
                if defined_symbol in obj_info["weak"]:
                    continue
                else:
                    raise RuntimeError(f"Multiple definitions for {defined_symbol}")
            provided_symbols[defined_symbol] = obj_name  # TODO: mark weak if needed

        # Recursively add undefined symbols from this object
        for undef_symbol in obj_info["undef"]:
            if undef_symbol in obj_info["weak"]:
                print(f"Skippping weak dependency: {undef_symbol}")
                continue
            if undef_symbol not in provided_symbols:
                symbol_stack.append(undef_symbol)  # Add undefined symbol to resolve

    while symbol_stack:
        symbol = symbol_stack.pop(0)

        if symbol in provided_symbols:
            continue  # Symbol is already resolved

        found = False
        for archive in archives:
            if symbol in archive.symbols:
                add_obj(archive, symbol)
                found = True
                break

        if not found:
            unresolved_symbols.add(symbol)

    return resolved_objs, list(unresolved_symbols)


def expand_ld_script(fn):
    # This function parses a subset of ld scripts
    # Typically these are just groups of static lib references
    group_pattern = re.compile(r"GROUP\s*\(\s*([^\)]+)\s*\)", re.MULTILINE)
    output_format_pattern = re.compile(r"OUTPUT_FORMAT\s*\(\s*([^\)]+)\s*\)", re.MULTILINE)
    comment_pattern = re.compile(r"/\*.*?\*/", re.MULTILINE | re.DOTALL)

    with open(fn, "r") as f:
        content = f.read()
        content = comment_pattern.sub("", content).strip()

    # Ensure no unrecognized instructions
    leftovers = content
    for pattern in (group_pattern, output_format_pattern):
        leftovers = pattern.sub("", leftovers)
    if leftovers.strip():
        raise ValueError("Invalid instruction found in the ld script:" + leftovers)

    # Extract files from GROUP instructions
    files = []
    for match in group_pattern.findall(content):
        files.extend([file.strip() for file in re.split(r"[,\s]+", match) if file.strip()])

    return files


def load_archive(fn):
    ar_header = b"!<arch>\012"
    with open(fn, "rb") as f:
        is_ar_file = f.read(len(ar_header)) == ar_header
    if is_ar_file:
        return [CachedArFile(fn)]
    else:
        return [CachedArFile(item) for item in expand_ld_script(fn)]
