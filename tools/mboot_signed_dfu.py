import os
import re
import sys
import zlib
import struct
import hashlib
import intelhex
from optparse import OptionParser

import dfu

try:
    import pyhy
except ImportError:
    raise SystemExit(
        "ERROR: pyhy not found. Please install python pyhy for encrypted mboot support\n> pip3 install pyhy"
    )

DEFAULT_CHUNKSIZE = 24 * 1024
DEFAULT_DEVICE = "0x0483:0xdf11"
FOOTER_LENGTH = 0xD0
FOOTER_VERSION = 1

ENABLE_ZIP = True

# Must match ENC_CONTEXT in mboot.c
HYDROGEN_CONTEXT = "mbootenc"


def generate_fw_footer(version, section_info, public_key):
    """
    From mboot.c: 
    typedef struct __attribute__ ((__packed__)) _fw_header_v1 {
        uint8_t footer_vers;
        char version[15];
        uint8_t public_key[32];  // hydro_sign_PUBLICKEYBYTES
        struct {
            uint32_t address;
            uint32_t length;
            uint8_t  sha256[32];
        } sections[4];
    } fw_footer_v1_t;
    """
    header_v1_fmt = "<B 15s 32s"  # II32s II32s II32s II32s"
    vers = version[0:15].encode("utf8")

    section_length = 40

    if len(section_info) > 4:
        raise SystemExit("More than 4 firmware sections is not supported")

    section_header = b""
    for s in section_info:
        section_header += struct.pack("<II", s[0], s[1]) + s[2]

    for _ in range(4 - (len(section_info))):
        section_header += b"\xFF" * 40

    header = struct.pack(
        header_v1_fmt, FOOTER_VERSION, vers, public_key,  # footer version  # fw semantic version
    )
    return header + section_header


def encrypt(data):
    """ We use the public key as the symmetric key for encryption as the device has it stored already """
    return pyhy.hydro_secretbox_encrypt(data, 0, HYDROGEN_CONTEXT, key_public)


def sign(data):
    sig = pyhy.hydro_sign_create(data, HYDROGEN_CONTEXT, key_private)
    return sig


def chunk_header(address, data, is_zipped=False):
    """
    typedef struct __attribute__ ((__packed__)) _chunk_header_v1 {
        uint8_t header_vers;
        uint8_t format;
        uint32_t address;
        uint32_t length;
        uint8_t signature[64];  // hydro_sign_BYTES
    } chunk_header_v1_t;

    enum chunk_format {
        RAW=0,
        GZIP=1
    };
    """
    chunk_header_v1_fmt = "<B B I I 64s"
    sig = sign(data)
    header = struct.pack(
        chunk_header_v1_fmt, 1, 1 if is_zipped else 0, address, len(data), sig,  # header version
    )
    return header


def data_chunks(data, n):
    for i in range(0, len(data), n):
        yield data[i : i + n]


if __name__ == "__main__":

    usage = """
    %prog [{-c|--chunksize} chunksize] {-s|--secret} key {-p|--public} key.pub  {-b|--build} address:file.bin [-b address:file.bin ...] [{-D|--device}=vendor:device] outfile.dfu"""
    parser = OptionParser(usage=usage)
    parser.add_option(
        "-v",
        "--version",
        dest="version",
        help="sematic version, up to 15 characters, or mpversion.h",
        metavar="VERSION",
    )
    parser.add_option(
        "-s", "--secret", dest="secret_key", help="private key file", metavar="PRIVATE"
    )
    parser.add_option(
        "-p", "--public", dest="public_key", help="public key file", metavar="PUBLIC"
    )
    parser.add_option(
        "-b",
        "--build",
        action="append",
        dest="binfiles",
        help="build a DFU file from given BINFILES",
        metavar="BINFILES",
    )
    parser.add_option(
        "-D",
        "--device",
        action="store",
        dest="device",
        help="build for DEVICE, defaults to %s" % DEFAULT_DEVICE,
        metavar="DEVICE",
    )
    parser.add_option(
        "-c",
        "--chunksize",
        dest="chunksize",
        help="chunks must fit in ram while mboot verifies and decrypts, defaults to %s"
        % DEFAULT_CHUNKSIZE,
        metavar="CHUNKSIZE",
    )
    (options, args) = parser.parse_args()

    device = DEFAULT_DEVICE
    if options.device:
        device = options.device
    try:
        v, d = map(lambda x: int(x, 0) & 0xFFFF, device.split(":", 1))
    except:
        raise SystemExit("Invalid device '%s'." % device)

    chunk_size = DEFAULT_CHUNKSIZE
    if options.chunksize:
        chunk_size = options.chunksize

    if not os.path.exists(options.secret_key):
        raise SystemExit("Please create signing/encryption key in %s" % options.secret_key)
    with open(options.secret_key, "rb") as keyfile:
        key_private = keyfile.read()

    with open(options.public_key, "rb") as keyfile:
        key_public = keyfile.read()

    if os.path.exists(options.version):
        with open(options.version, "r") as mpversion_h:
            mpversion = mpversion_h.read()
        vers_match = re.search(r'MICROPY_GIT_TAG "(.*)"', mpversion)
        version = vers_match.group(1) if vers_match else ""
    else:
        version = options.version

    if options.binfiles and len(args) >= 1:
        outfile = args[0]
        target = []
        section_info = []
        footer = None

        binfiles = []
        for arg in options.binfiles:
            try:
                address, binfile = arg.split(":", 1)
            except ValueError:
                raise SystemExit("Address:file couple '%s' invalid." % arg)
            try:
                address = int(address, 0) & 0xFFFFFFFF
            except ValueError:
                raise SystemExit("Address %s invalid." % address)
            if not os.path.isfile(binfile):
                raise SystemExit("Unreadable file '%s'." % binfile)

            binfiles.append((address, binfile))

        # Ensure firmware sections are processed in order
        for section, b in enumerate(sorted(binfiles, key=lambda b: b[0])):
            address, binfile = b
            with open(binfile, "rb") as fwfile:
                fw = fwfile.read()
            length = len(fw)

            last_section = section == len(binfiles) - 1

            if last_section:
                footer_addr = address + length - FOOTER_LENGTH
                print("Footer address: 0x%08x" % footer_addr)
                # Check firmware then truncate section before footer
                assert (
                    fw[-FOOTER_LENGTH] == FOOTER_VERSION
                ), "Firmware has incorrect FOOTER_VERSION: %s" % list(fw[-FOOTER_LENGTH:])
                fw = fw[0:-FOOTER_LENGTH]

            # first byte is skipped for hash as it contains the APP_VALIDITY_BITS
            # which will be changed in firmware once hash is successfully verified.
            start = 1 if section == 0 else 0
            sha256 = hashlib.sha256(fw[start:]).digest()
            section_info.append([address, len(fw), sha256])

            if last_section:
                # Last section, generate footer, append back to firmware
                footer = generate_fw_footer(version, section_info, key_public)
                assert FOOTER_LENGTH == len(footer), "Footer incorrect length, check version"
                fw += footer

            # Split the firmware into chunks, encrypt and sign the chunks
            # then register them as individual dfu targets
            for i, chunk in enumerate(data_chunks(fw, chunk_size)):
                chunk_addr = address + i * chunk_size
                if ENABLE_ZIP:
                    c = zlib.compressobj(level=9, memLevel=9, wbits=-15)  # wsize=15, no header
                    chunk = c.compress(chunk) + c.flush()
                encrypted = encrypt(chunk)
                c_header = chunk_header(chunk_addr, encrypted, is_zipped=ENABLE_ZIP)
                chunk = c_header + encrypted

                target.append({"address": chunk_addr, "data": chunk})

        # Save un-encrypted bin, hex and dfu file of just the footer. This can be written
        # to new device to store the key and enable encryption support
        with open(binfile[:-4] + ".ftr" + binfile[-4:], "wb") as hdrfile:
            hdrfile.write(footer)
        ih = intelhex.IntelHex()
        ih.frombytes(bytearray(footer), offset=footer_addr)
        with open(binfile[:-4] + ".ftr.hex", "w") as hdrfile:
            ih.tofile(hdrfile, format="hex")

        footer_target = [{"address": footer_addr, "data": footer}]
        dfu.build(binfile[:-4] + ".ftr.dfu", [footer_target], device)

        # if ENABLE_ZIP:
        #     c = zlib.compressobj(level=9, memLevel=9, wbits=-15) # wsize=15, no header
        #     footer = c.compress(footer) + c.flush()

        # Create encrypted copy of footer for inclusion in main dfu file
        # encrypted = encrypt(footer)
        # c_footer = chunk_header(footer_addr, encrypted, is_zipped=ENABLE_ZIP)
        # fw_footer_chunk = c_footer + encrypted

        # target.append({ 'address': footer_addr, 'data': fw_footer_chunk })

        # Build dfu file of all the encrypted & signed chunks with footer
        dfu.build(outfile, [target], device, pad=not ENABLE_ZIP)
        print("Written", outfile)

    else:
        parser.print_help()
        sys.exit(1)
