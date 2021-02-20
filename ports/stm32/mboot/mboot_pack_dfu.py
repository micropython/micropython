# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020-2021 Damien P. George
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
"""
Utility to create compressed, encrypted and signed DFU files.
"""

import argparse
import os
import re
import struct
import sys
import zlib

sys.path.append(os.path.dirname(__file__) + "/../../../tools")
import dfu

try:
    import pyhy
except ImportError:
    raise SystemExit(
        "ERROR: pyhy not found. Please install python pyhy for encrypted mboot support: pip3 install pyhy"
    )


# Currenty supported version of a packed DFU file.
MBOOT_PACK_HEADER_VERSION = 1

# Must match MBOOT_PACK_HYDRO_CONTEXT in mboot/pack.h
MBOOT_PACK_HYDRO_CONTEXT = "mbootenc"

# Must match enum in mboot/pack.h.
MBOOT_PACK_CHUNK_META = 0
MBOOT_PACK_CHUNK_FULL_SIG = 1
MBOOT_PACK_CHUNK_FW_RAW = 2
MBOOT_PACK_CHUNK_FW_GZIP = 3


class Keys:
    def __init__(self, filename):
        self.filename = filename

    def generate(self):
        kp = pyhy.hydro_sign_keygen()
        self.sign_sk = kp.sk
        self.sign_pk = kp.pk
        self.secretbox = pyhy.hydro_secretbox_keygen()

    def _save_data(self, name, data, file_, hide=False):
        prefix = "//" if hide else ""
        data = ",".join("0x{:02x}".format(b) for b in data)
        file_.write("{}const uint8_t {}[] = {{{}}};\n".format(prefix, name, data))

    def _load_data(self, name, line):
        line = line.split(name + "[] = ")
        if len(line) != 2:
            raise Exception("malformed input keys: {}".format(line))
        data = line[1].strip()
        return bytes(int(value, 16) for value in data[1:-2].split(","))

    def save(self):
        with open(self.filename, "w") as f:
            self._save_data("mboot_pack_sign_secret_key", self.sign_sk, f, hide=True)
            self._save_data("mboot_pack_sign_public_key", self.sign_pk, f)
            self._save_data("mboot_pack_secretbox_key", self.secretbox, f)

    def load(self):
        with open(self.filename) as f:
            self.sign_sk = self._load_data("mboot_pack_sign_secret_key", f.readline())
            self.sign_pk = self._load_data("mboot_pack_sign_public_key", f.readline())
            self.secretbox = self._load_data("mboot_pack_secretbox_key", f.readline())


def dfu_read(filename):
    elems = []

    with open(filename, "rb") as f:
        hdr = f.read(11)
        sig, ver, size, num_targ = struct.unpack("<5sBIB", hdr)
        file_offset = 11

        for i in range(num_targ):
            hdr = f.read(274)
            sig, alt, has_name, name, t_size, num_elem = struct.unpack("<6sBi255sII", hdr)

            file_offset += 274
            file_offset_t = file_offset
            for j in range(num_elem):
                hdr = f.read(8)
                addr, e_size = struct.unpack("<II", hdr)
                data = f.read(e_size)
                elems.append((addr, data))
                file_offset += 8 + e_size

            if t_size != file_offset - file_offset_t:
                raise Exception("corrupt DFU {} {}".format(t_size, file_offset - file_offset_t))

        if size != file_offset:
            raise Exception("corrupt DFU {} {}".format(size, file_offset))

        hdr = f.read(16)
        hdr = struct.unpack("<HHHH3sBI", hdr)
        vid_pid = "0x{:04x}:0x{:04x}".format(hdr[2], hdr[1])

    return vid_pid, elems


def compress(data):
    c = zlib.compressobj(level=9, memLevel=9, wbits=-15)  # wsize=15, no header
    return c.compress(data) + c.flush()


def encrypt(keys, data):
    return pyhy.hydro_secretbox_encrypt(data, 0, MBOOT_PACK_HYDRO_CONTEXT, keys.secretbox)


def sign(keys, data):
    return pyhy.hydro_sign_create(data, MBOOT_PACK_HYDRO_CONTEXT, keys.sign_sk)


def pack_chunk(keys, format_, chunk_addr, chunk_payload):
    header = struct.pack(
        "<BBBBII", MBOOT_PACK_HEADER_VERSION, format_, 0, 0, chunk_addr, len(chunk_payload)
    )
    chunk = header + chunk_payload
    sig = sign(keys, chunk)
    chunk = chunk + sig
    return chunk


def data_chunks(data, n):
    for i in range(0, len(data), n):
        yield data[i : i + n]


def generate_keys(keys, args):
    keys.generate()
    keys.save()


def pack_dfu(keys, args):
    chunk_size = int(args.chunk_size[0])

    # Load previously generated keys.
    keys.load()

    # Read the input DFU file.
    vid_pid, elems = dfu_read(args.infile[0])

    # Ensure firmware sections are processed in order of destination memory address.
    elems = sorted(elems, key=lambda e: e[0])

    # Build list of packed chunks.
    target = []
    full_fw = b""
    full_signature_payload = b""
    for address, fw in elems:
        # Update full firmware and full signature chunk.
        full_fw += fw
        full_signature_payload += struct.pack("<II", address, len(fw))

        # Split the firmware into chunks, encrypt and sign the chunks
        # then register them as individual DFU targets.
        for i, chunk in enumerate(data_chunks(fw, chunk_size)):
            chunk_addr = address + i * chunk_size
            if args.gzip:
                chunk = compress(chunk)
            chunk = encrypt(keys, chunk)
            chunk = pack_chunk(
                keys,
                MBOOT_PACK_CHUNK_FW_GZIP if args.gzip else MBOOT_PACK_CHUNK_FW_RAW,
                chunk_addr,
                chunk,
            )
            target.append({"address": chunk_addr, "data": chunk})

    # Add full signature to targets, at location following the last chunk.
    chunk_addr += chunk_size
    sig = sign(keys, full_fw)
    full_signature_payload += sig
    full_signature_chunk = pack_chunk(
        keys, MBOOT_PACK_CHUNK_FULL_SIG, chunk_addr, full_signature_payload
    )
    target.append({"address": chunk_addr, "data": full_signature_chunk})

    # Build the packed DFU file of all the encrypted and signed chunks.
    dfu.build(args.outfile[0], [target], vid_pid)

    # Verify the packed DFU file.
    verify_pack_dfu(keys, args.outfile[0])


def verify_pack_dfu(keys, filename):
    """Verify packed dfu file against keys. Gathers decrypted binary data."""
    full_sig = pyhy.hydro_sign(MBOOT_PACK_HYDRO_CONTEXT)
    _, elems = dfu_read(filename)
    base_addr = None
    binary_data = b""

    for addr, data in elems:
        if base_addr is None:
            base_addr = addr

        header = struct.unpack("<BBBBII", data[:12])
        chunk = data[12 : 12 + header[5]]
        sig = data[12 + header[5] :]
        sig_pass = pyhy.hydro_sign_verify(
            sig, data[:12] + chunk, MBOOT_PACK_HYDRO_CONTEXT, keys.sign_pk
        )
        assert sig_pass

        if header[1] == MBOOT_PACK_CHUNK_FULL_SIG:
            actual_sig = chunk[-64:]
        else:
            chunk = pyhy.hydro_secretbox_decrypt(
                chunk, 0, MBOOT_PACK_HYDRO_CONTEXT, keys.secretbox
            )
            assert chunk is not None
            if header[1] == MBOOT_PACK_CHUNK_FW_GZIP:
                chunk = zlib.decompress(chunk, wbits=-15)
            full_sig.update(chunk)
            assert addr == base_addr + len(binary_data)
            binary_data += chunk

    full_sig_pass = full_sig.final_verify(actual_sig, keys.sign_pk)
    assert full_sig_pass
    return [{"address": base_addr, "data": binary_data}]


def unpack_dfu(keys, args):
    # Load previously generated keys.
    keys.load()

    # Build a DFU file from the decrypted binary data.
    data = verify_pack_dfu(keys, args.infile[0])
    dfu.build(args.outfile[0], [data])


def main():
    cmd_parser = argparse.ArgumentParser(description="Build signed/encrypted DFU files")
    cmd_parser.add_argument("-k", "--keys", default="mboot_keys.h", help="filename for keys")
    subparsers = cmd_parser.add_subparsers()

    parser_gk = subparsers.add_parser("generate-keys", help="generate keys")
    parser_gk.set_defaults(func=generate_keys)

    parser_ed = subparsers.add_parser("pack-dfu", help="encrypt and sign a DFU file")
    parser_ed.add_argument("-z", "--gzip", action="store_true", help="compress chunks")
    parser_ed.add_argument("chunk_size", nargs=1, help="maximum size in bytes of each chunk")
    parser_ed.add_argument("infile", nargs=1, help="input DFU file")
    parser_ed.add_argument("outfile", nargs=1, help="output DFU file")
    parser_ed.set_defaults(func=pack_dfu)

    parser_dd = subparsers.add_parser("unpack-dfu", help="decrypt a signed/encrypted DFU file")
    parser_dd.add_argument("infile", nargs=1, help="input packed DFU file")
    parser_dd.add_argument("outfile", nargs=1, help="output DFU file")
    parser_dd.set_defaults(func=unpack_dfu)

    args = cmd_parser.parse_args()

    keys = Keys(args.keys)
    args.func(keys, args)


if __name__ == "__main__":
    main()
