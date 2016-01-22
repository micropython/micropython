try:
    import uzipfile as zipfile
    import ubinascii as binascii
except ImportError:
    import zipfile as zipfile
    import binascii

EINVAL = 22

files = {
    'empty.zip':    b'504b0506000000000000000000000000000000000000',
    'stored.zip':   b'504b03040a000000000090933748fbbdefe6150000001500000012001c007a697066696c655f73746f7265642e7478745554090003cf36a456cf36a45675780b000104d588000004e90300007072696e74282748656c6c6f20576f726c6427290a504b01021e030a000000000090933748fbbdefe61500000015000000120018000000000001000000b481000000007a697066696c655f73746f7265642e7478745554050003cf36a45675780b000104d588000004e9030000504b0506000000000100010058000000610000000000',
    'deflated.zip': b'504b0304140000000800f293374836763c6d270000002c00000014001c007a697066696c655f6465666c617465642e74787455540900038837a4560238a45675780b000104d588000004e90300000bc9c82c5600a2448592d4e21285b4cc9c54ae10a8584946aa42716a727e5e8a424e665e2a1700504b01021e03140000000800f293374836763c6d270000002c000000140018000000000001000000b481000000007a697066696c655f6465666c617465642e74787455540500038837a45675780b000104d588000004e9030000504b050600000000010001005a000000750000000000',
    'deflated.txt': b'54686973206973206120746573742066696c650a5468697320697320746865207365636f6e64206c696e650a',
    'stored.txt':   b'7072696e74282748656c6c6f20576f726c6427290a',
}

for filename, data in sorted(files.items()):
    # with doesn't work in conjunction with --emit native (part of the test run)
    f = open(filename, 'wb')
    f.write(binascii.unhexlify(data))
    f.close()

for filename in sorted(files):
    print(filename, 'is_zipfile:', zipfile.is_zipfile(filename))
    if zipfile.is_zipfile(filename):
        zf = zipfile.ZipFile(filename)
        zf_names = zf.namelist()
        print(filename, 'namelist:', zf_names)
        zf.close()

def check_file(zip_filename, data_filename, data):
    if type(zip_filename) is str:
        print('check_file zip filename:', zip_filename, 'data file', data_filename)
    else:
        print('check_file zip file', zip_filename.name, 'data file', data_filename)
    zf = zipfile.ZipFile(zip_filename)
    zef = zf.open(data_filename)
    buf = zef.read(len(data)//2 + 10)
    cmp = binascii.unhexlify(data)
    print('file compare:', buf == cmp)
    zef.close()
    zf.close()

check_file('stored.zip', 'zipfile_stored.txt', files['stored.txt'])
check_file('deflated.zip', 'zipfile_deflated.txt', files['deflated.txt'])


class ByteFile:
    """Class which treats a byestring as a file-like object."""

    def __init__(self, bytes, name=None):
        self.bytes = bytes
        self.posn = 0
        if name is None:
            name = 'ByteFile object'
        self.name = name

    def close(self):
        pass

    def read(self, num_bytes=-1):
        """CPython's zipfile implementation needs read to be implemented."""
        #print('read', num_bytes)
        if num_bytes < 0:
            return self.readall()
        size = num_bytes
        if self.posn + size > len(self.bytes):
            size = len(self.bytes) - self.posn
        if size <= 0:
            return b''
        buf = bytearray(self.bytes[self.posn:self.posn+size])
        #print('read @', self.posn, binascii.hexlify(buf))
        self.posn += len(buf)
        return buf

    def readall(self):
        buf = self.bytes[self.posn:]
        #print('readall @', self.posn, binascii.hexlify(buf))
        self.posn = len(self.bytes)
        return buf

    def readinto(self, buf):
        size = len(buf)
        if self.posn + size > len(self.bytes):
            size = len(self.bytes) - self.posn
        if size <= 0:
            return 0
        buf[0:size] = self.bytes[self.posn:self.posn+size]
        #print('readinto @', self.posn, binascii.hexlify(buf[0:size]))
        self.posn += size
        return size

    def seek(self, offset, whence):
        if whence == 0:
            self.posn = offset
        elif whence == 1:
            self.posn += offset
        elif whence == 2:
            self.posn = len(self.bytes) + offset
        #print('seek', offset, whence, 'got', self.posn)
        if self.posn < 0 or self.posn > len(self.bytes):
            raise OSError(EINVAL)
        return self.posn

    def tell(self):
        return self.posn

print('===== Checking ByteFile =====')

for filename, data in sorted(files.items()):
    bf = ByteFile(binascii.unhexlify(data), name=filename)
    print(filename, 'is_zipfile:', zipfile.is_zipfile(bf))
    if zipfile.is_zipfile(bf):
        zf = zipfile.ZipFile(bf)
        zf_names = zf.namelist()
        print(filename, 'namelist:', zf_names)
        zf.close()

check_file(ByteFile(binascii.unhexlify(files['stored.zip'])), 'zipfile_stored.txt', files['stored.txt'])
check_file(ByteFile(binascii.unhexlify(files['deflated.zip'])), 'zipfile_deflated.txt', files['deflated.txt'])
