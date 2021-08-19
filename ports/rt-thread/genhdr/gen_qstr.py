#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2017 SummerGift <zhangyuan@rt-thread.com>
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
#
 
from __future__ import print_function

import re
import sys

import platform
if platform.python_version_tuple()[0] == '2':
    bytes_cons = lambda val, enc=None: bytearray(val)
    from htmlentitydefs import codepoint2name
elif platform.python_version_tuple()[0] == '3':
    bytes_cons = bytes
    from html.entities import codepoint2name
# end compatibility code

# this must match the equivalent function in qstr.c
def compute_hash(qstr, bytes_hash):
    hash = 5381
    for b in qstr:
        hash = (hash * 33) ^ b
    # Make sure that valid hash is never zero, zero means "hash not computed"
    return (hash & ((1 << (8 * bytes_hash)) - 1)) or 1
    
def make_bytes(cfg_bytes_len, cfg_bytes_hash, qstr):
   
    #print("qstr:",qstr)
    #print("cfg_bytes_hash:",cfg_bytes_hash)
    #print("cfg_bytes_len:",cfg_bytes_len)

    qbytes = bytes_cons(qstr, 'utf8')
    
    #print("qbytes:",qbytes)
    
    qlen = len(qbytes)
    qhash = compute_hash(qbytes, cfg_bytes_hash)
    
    #print("qlen:",qlen)
    #print("cfg_bytes_hash:",cfg_bytes_hash)
    #print("qhash:",qhash)
    
    if all(32 <= ord(c) <= 126 and c != '\\' and c != '"' for c in qstr):
        # qstr is all printable ASCII so render it as-is (for easier debugging)
        qdata = qstr
    else:
        # qstr contains non-printable codes so render entire thing as hex pairs
        qdata = ''.join(('\\x%02x' % b) for b in qbytes)
    if qlen >= (1 << (8 * cfg_bytes_len)):
        print('qstr is too long:', qstr)
        assert False
    qlen_str = ('\\x%02x' * cfg_bytes_len) % tuple(((qlen >> (8 * i)) & 0xff) for i in range(cfg_bytes_len))
    qhash_str = ('\\x%02x' * cfg_bytes_hash) % tuple(((qhash >> (8 * i)) & 0xff) for i in range(cfg_bytes_hash))
    return 'QDEF(MP_QSTR_%s, (const byte*)"%s%s" "%s")' % (qdata,qhash_str, qlen_str, qdata)
    
if __name__ == "__main__":
    print ("This program will gen qstr for micropython:")
    
    for arg in sys.argv:
        if arg == 'gen_qstr.py':
            continue
        qstr_computer = make_bytes(1, 1, arg)
        print (qstr_computer)

