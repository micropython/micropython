# Used to set the encrypted mboot security key in the last 32 bytes of the mboot binary
import os
from optparse import OptionParser

if __name__ == "__main__":

    usage = """
    %prog [{-p|--public} key.pub {-b|--binary} file.bin"""
    parser = OptionParser(usage=usage)
    parser.add_option(
        "-p", "--public", dest="public_key", help="public key file", metavar="PUBLIC"
    )
    parser.add_option(
        "-b",
        "--binary",
        dest="binfile",
        help="set encryption key at end of binary",
        metavar="BINFILE",
    )
    parser.add_option(
        "-l",
        "--location",
        dest="location",
        default=32736,
        help="set encryption key at end of binary",
        metavar="BINFILE",
    )

    (options, args) = parser.parse_args()

    with open(options.public_key, "rb") as keyfile:
        key_public = keyfile.read()

    with open(options.binfile, "rb") as binfile:
        binary = binfile.read()

    location = int(options.location)

    pad = location - len(binary)
    if pad > 0:
        binary += b"\xFF" * pad

    with open(options.binfile, "wb") as binfile:
        binfile.write(binary[0:location])
        binfile.write(key_public)

    print("Set encryption key in", options.binfile)
