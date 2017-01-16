try:
    extra_coverage
except NameError:
    print("SKIP")
    import sys
    sys.exit()

import uerrno

data = extra_coverage()

# test hashing of str/bytes that have an invalid hash
print(data[0], data[1])
print(hash(data[0]))
print(hash(data[1]))
print(hash(bytes(data[0], 'utf8')))
print(hash(str(data[1], 'utf8')))

# test streams
stream = data[2] # has set_error and set_buf. Write always returns error
stream.set_error(uerrno.EAGAIN) # non-blocking error
print(stream.read()) # read all encounters non-blocking error
print(stream.read(1)) # read 1 byte encounters non-blocking error
print(stream.readline()) # readline encounters non-blocking error
print(stream.readinto(bytearray(10))) # readinto encounters non-blocking error
print(stream.write(b'1')) # write encounters non-blocking error
print(stream.write1(b'1')) # write1 encounters non-blocking error
stream.set_buf(b'123')
print(stream.read(4)) # read encounters non-blocking error after successful reads
stream.set_buf(b'123')
print(stream.read1(4)) # read1 encounters non-blocking error after successful reads
stream.set_buf(b'123')
print(stream.readline(4)) # readline encounters non-blocking error after successful reads
try:
    print(stream.ioctl(0, 0)) # ioctl encounters non-blocking error; raises OSError
except OSError:
    print('OSError')
stream.set_error(0)
print(stream.ioctl(0, bytearray(10))) # successful ioctl call

stream2 = data[3] # is textio and sets .write = NULL
try:
    print(stream2.write(b'1')) # attempt to call NULL implementation
except OSError:
    print('OSError')
print(stream2.read(1)) # read 1 byte encounters non-blocking error with textio stream

# test basic import of frozen scripts
import frzstr1
import frzmpy1
