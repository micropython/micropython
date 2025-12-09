import collections
import re
import sys

import gzip
import zlib


_COMPRESSED_MARKER = 0xFF


def check_non_ascii(msg):
    for c in msg:
        if ord(c) >= 0x80:
            print(
                'Unable to generate compressed data: message "{}" contains a non-ascii character "{}".'.format(
                    msg, c
                ),
                file=sys.stderr,
            )
            sys.exit(1)


# Replace <char><space> with <char | 0x80>.
# Trivial scheme to demo/test.
def space_compression(error_strings):
    for line in error_strings:
        check_non_ascii(line)
        result = ""
        for i in range(len(line)):
            if i > 0 and line[i] == " ":
                result = result[:-1]
                result += "\\{:03o}".format(ord(line[i - 1]))
            else:
                result += line[i]
        error_strings[line] = result
    return None


# Replace common words with <0x80 | index>.
# Index is into a table of words stored as aaaaa<0x80|a>bbb<0x80|b>...
# Replaced words are assumed to have spaces either side to avoid having to store the spaces in the compressed strings.
def word_compression(error_strings):
    topn = collections.Counter()

    for line in error_strings.keys():
        check_non_ascii(line)
        for word in line.split(" "):
            topn[word] += 1

    # Order not just by frequency, but by expected saving. i.e. prefer a longer string that is used less frequently.
    # Use the word itself for ties so that compression is deterministic.
    def bytes_saved(item):
        w, n = item
        return -((len(w) + 1) * (n - 1)), w

    top128 = sorted(topn.items(), key=bytes_saved)[:128]

    index = [w for w, _ in top128]
    index_lookup = {w: i for i, w in enumerate(index)}

    for line in error_strings.keys():
        result = ""
        need_space = False
        for word in line.split(" "):
            if word in index_lookup:
                result += "\\{:03o}".format(0b10000000 | index_lookup[word])
                need_space = False
            else:
                if need_space:
                    result += " "
                need_space = True
                result += word
        error_strings[line] = result.strip()

    return "".join(w[:-1] + "\\{:03o}".format(0b10000000 | ord(w[-1])) for w in index)


# Replace chars in text with variable length bit sequence.
# For comparison only (the table is not emitted).
def huffman_compression(error_strings):
    # https://github.com/tannewt/huffman
    import huffman

    all_strings = "".join(error_strings)
    cb = huffman.codebook(collections.Counter(all_strings).items())

    for line in error_strings:
        b = "1"
        for c in line:
            b += cb[c]
        n = len(b)
        if n % 8 != 0:
            n += 8 - (n % 8)
        result = ""
        for i in range(0, n, 8):
            result += "\\{:03o}".format(int(b[i : i + 8], 2))
        if len(result) > len(line) * 4:
            result = line
        error_strings[line] = result

    # TODO: This would be the prefix lengths and the table ordering.
    return "_" * (10 + len(cb))


# Replace common N-letter sequences with <0x80 | index>, where
# the common sequences are stored in a separate table.
# This isn't very useful, need a smarter way to find top-ngrams.
def ngram_compression(error_strings):
    topn = collections.Counter()
    N = 2

    for line in error_strings.keys():
        check_non_ascii(line)
        if len(line) < N:
            continue
        for i in range(0, len(line) - N, N):
            topn[line[i : i + N]] += 1

    def bytes_saved(item):
        w, n = item
        return -(len(w) * (n - 1))

    top128 = sorted(topn.items(), key=bytes_saved)[:128]

    index = [w for w, _ in top128]
    index_lookup = {w: i for i, w in enumerate(index)}

    for line in error_strings.keys():
        result = ""
        for i in range(0, len(line) - N + 1, N):
            word = line[i : i + N]
            if word in index_lookup:
                result += "\\{:03o}".format(0b10000000 | index_lookup[word])
            else:
                result += word
        if len(line) % N != 0:
            result += line[len(line) - len(line) % N :]
        error_strings[line] = result.strip()

    return "".join(index)


def main(collected_path, fn):
    error_strings = collections.OrderedDict()
    max_uncompressed_len = 0
    num_uses = 0

    # Read in all MP_ERROR_TEXT strings.
    with open(collected_path, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            num_uses += 1
            error_strings[line] = None
            max_uncompressed_len = max(max_uncompressed_len, len(line))

    # So that objexcept.c can figure out how big the buffer needs to be.
    print("#define MP_MAX_UNCOMPRESSED_TEXT_LEN ({})".format(max_uncompressed_len))

    # Run the compression.
    compressed_data = fn(error_strings)

    # Print the data table.
    print('MP_COMPRESSED_DATA("{}")'.format(compressed_data))

    # Print the replacements.
    for uncomp, comp in error_strings.items():
        if uncomp == comp:
            prefix = ""
        else:
            prefix = "\\{:03o}".format(_COMPRESSED_MARKER)
        print('MP_MATCH_COMPRESSED("{}", "{}{}")'.format(uncomp, prefix, comp))

    # Used to calculate the "true" length of the (escaped) compressed strings.
    def unescape(s):
        return re.sub(r"\\\d\d\d", "!", s)

    # Stats. Note this doesn't include the cost of the decompressor code.
    uncomp_len = sum(len(s) + 1 for s in error_strings.keys())
    comp_len = sum(1 + len(unescape(s)) + 1 for s in error_strings.values())
    data_len = len(compressed_data) + 1 if compressed_data else 0
    print("// Total input length:      {}".format(uncomp_len))
    print("// Total compressed length: {}".format(comp_len))
    print("// Total data length:       {}".format(data_len))
    print("// Predicted saving:        {}".format(uncomp_len - comp_len - data_len))

    # Somewhat meaningless comparison to zlib/gzip.
    all_input_bytes = "\\0".join(error_strings.keys()).encode()
    print()
    if hasattr(gzip, "compress"):
        gzip_len = len(gzip.compress(all_input_bytes)) + num_uses * 4
        print("// gzip length:             {}".format(gzip_len))
        print("// Percentage of gzip:      {:.1f}%".format(100 * (comp_len + data_len) / gzip_len))
    if hasattr(zlib, "compress"):
        zlib_len = len(zlib.compress(all_input_bytes)) + num_uses * 4
        print("// zlib length:             {}".format(zlib_len))
        print("// Percentage of zlib:      {:.1f}%".format(100 * (comp_len + data_len) / zlib_len))


if __name__ == "__main__":
    main(sys.argv[1], word_compression)
