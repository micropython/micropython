/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"

// All the qstr definitions in this file are available as constants.
// That is, they are in ROM and you can reference them simply as MP_QSTR_xxxx.

// qstr configuration passed to makeqstrdata.py of the form QCFG(key, value)
QCFG(BYTES_IN_LEN, MICROPY_QSTR_BYTES_IN_LEN)
QCFG(BYTES_IN_HASH, MICROPY_QSTR_BYTES_IN_HASH)

Q()
Q(*)
Q(_)
Q(__build_class__)
Q(__class__)
Q(__doc__)
Q(__import__)
Q(__init__)
Q(__new__)
Q(__locals__)
Q(__main__)
Q(__module__)
Q(__name__)
Q(__dict__)
Q(__hash__)
Q(__next__)
Q(__qualname__)
Q(__path__)
Q(__repl_print__)
#if MICROPY_PY___FILE__
Q(__file__)
#endif

Q(__bool__)
Q(__contains__)
Q(__enter__)
Q(__exit__)
Q(__len__)
Q(__iter__)
Q(__getitem__)
Q(__setitem__)
Q(__delitem__)
Q(__add__)
Q(__sub__)
Q(__repr__)
Q(__str__)
#if MICROPY_PY_DESCRIPTORS
Q(__get__)
Q(__set__)
Q(__delete__)
#endif
Q(__getattr__)
Q(__del__)
Q(__call__)
Q(__lt__)
Q(__gt__)
Q(__eq__)
Q(__le__)
Q(__ge__)
Q(__reversed__)
#if MICROPY_PY_ALL_SPECIAL_METHODS
Q(__mul__)
Q(__truediv__)
Q(__floordiv__)
Q(__iadd__)
Q(__isub__)
Q(__invert__)
Q(__neg__)
Q(__pos__)
#endif

Q(micropython)
Q(bytecode)
Q(const)

#if MICROPY_EMIT_NATIVE
Q(native)
Q(viper)
Q(uint)
Q(ptr)
Q(ptr8)
Q(ptr16)
Q(ptr32)
#endif

#if MICROPY_EMIT_INLINE_THUMB
Q(asm_thumb)
Q(label)
Q(align)
Q(data)
Q(uint)
Q(nop)
Q(mov)
Q(and_)
Q(cmp)
Q(add)
Q(sub)
Q(lsl)
Q(lsr)
Q(asr)
Q(ldr)
Q(ldrb)
Q(ldrh)
Q(str)
Q(strb)
Q(strh)
Q(b)
Q(bl)
Q(bx)
Q(push)
Q(pop)
Q(cpsid)
Q(cpsie)
Q(wfi)
Q(clz)
Q(rbit)
Q(movw)
Q(movt)
Q(movwt)
Q(mrs)
Q(sdiv)
Q(udiv)
Q(ldrex)
Q(strex)
#if MICROPY_EMIT_INLINE_THUMB_FLOAT
Q(vcmp)
Q(vneg)
Q(vcvt_f32_s32)
Q(vcvt_s32_f32)
Q(vsqrt)
Q(vmov)
Q(vmrs)
Q(vldr)
Q(vstr)
#endif
#endif

Q(builtins)

Q(Ellipsis)
Q(StopIteration)
#if MICROPY_PY_BUILTINS_NOTIMPLEMENTED
Q(NotImplemented)
#endif

Q(BaseException)
Q(ArithmeticError)
Q(AssertionError)
Q(AttributeError)
Q(BufferError)
Q(EOFError)
Q(Exception)
Q(FileExistsError)
Q(FileNotFoundError)
Q(FloatingPointError)
Q(GeneratorExit)
Q(ImportError)
Q(IndentationError)
Q(IndexError)
Q(KeyboardInterrupt)
Q(KeyError)
Q(LookupError)
Q(MemoryError)
Q(NameError)
Q(NotImplementedError)
Q(OSError)
#if MICROPY_PY_BUILTINS_TIMEOUTERROR
Q(TimeoutError)
#endif
Q(OverflowError)
Q(RuntimeError)
Q(SyntaxError)
Q(SystemExit)
Q(TypeError)
Q(UnboundLocalError)
Q(ValueError)
#if MICROPY_EMIT_NATIVE
Q(ViperTypeError)
#endif
Q(ZeroDivisionError)
#if MICROPY_PY_BUILTINS_STR_UNICODE
Q(UnicodeError)
#endif

Q(None)
Q(False)
Q(True)
Q(object)

Q(NoneType)

#if MICROPY_PY_COLLECTIONS_ORDEREDDICT
Q(OrderedDict)
#endif

Q(abs)
Q(all)
Q(any)
Q(args)
#if MICROPY_PY_ARRAY
Q(array)
#endif
Q(bin)
Q({:#b})
Q(bool)
#if MICROPY_PY_BUILTINS_BYTEARRAY
Q(bytearray)
#endif
#if MICROPY_PY_BUILTINS_MEMORYVIEW
Q(memoryview)
#endif
Q(bytes)
Q(callable)
Q(chr)
Q(classmethod)
Q(_collections)
#if MICROPY_PY_BUILTINS_COMPLEX
Q(complex)
Q(real)
Q(imag)
#endif
Q(dict)
Q(dir)
Q(divmod)
#if MICROPY_PY_BUILTINS_ENUMERATE
Q(enumerate)
#endif
Q(eval)
Q(exec)
#if MICROPY_PY_BUILTINS_EXECFILE
Q(execfile)
#endif
#if MICROPY_PY_BUILTINS_FILTER
Q(filter)
#endif
#if MICROPY_PY_BUILTINS_FLOAT
Q(float)
#endif
Q(from_bytes)
Q(getattr)
Q(setattr)
Q(globals)
Q(hasattr)
Q(hash)
Q(hex)
Q(%#x)
Q(id)
Q(int)
Q(isinstance)
Q(issubclass)
Q(iter)
Q(len)
Q(list)
Q(locals)
Q(map)
#if MICROPY_PY_BUILTINS_MIN_MAX
Q(max)
Q(min)
Q(default)
#endif
Q(namedtuple)
Q(next)
Q(oct)
Q(%#o)
Q(open)
Q(ord)
Q(path)
Q(pow)
Q(print)
Q(range)
Q(read)
Q(repr)
Q(reversed)
Q(round)
Q(sorted)
Q(staticmethod)
Q(sum)
Q(super)
Q(str)
Q(sys)
Q(to_bytes)
Q(tuple)
Q(type)
Q(value)
Q(write)
Q(zip)

#if MICROPY_PY_BUILTINS_COMPILE
Q(compile)
Q(code)
Q(single)
#endif

Q(sep)
Q(end)

#if MICROPY_PY_BUILTINS_RANGE_ATTRS
Q(step)
Q(stop)
#endif

Q(clear)
Q(copy)
Q(fromkeys)
Q(get)
Q(items)
Q(keys)
Q(pop)
Q(popitem)
Q(setdefault)
Q(update)
Q(values)
Q(append)
Q(close)
Q(send)
Q(throw)
Q(count)
Q(extend)
Q(index)
Q(remove)
Q(insert)
Q(pop)
Q(sort)
Q(join)
Q(strip)
Q(lstrip)
Q(rstrip)
Q(format)
Q(key)
Q(reverse)
Q(add)
Q(clear)
Q(copy)
Q(pop)
Q(remove)
Q(find)
Q(rfind)
Q(rindex)
Q(split)
#if MICROPY_PY_BUILTINS_STR_SPLITLINES
Q(splitlines)
Q(keepends)
Q(\n)
#endif
Q(rsplit)
Q(startswith)
Q(endswith)
Q(replace)
Q(partition)
Q(rpartition)
Q(lower)
Q(upper)
Q(isspace)
Q(isalpha)
Q(isdigit)
Q(isupper)
Q(islower)
Q(iterable)
Q(start)

Q(bound_method)
Q(closure)
Q(dict_view)
Q(function)
Q(generator)
Q(iterator)
Q(module)
Q(slice)

#if MICROPY_PY_BUILTINS_SET
Q(discard)
Q(difference)
Q(difference_update)
Q(intersection)
Q(intersection_update)
Q(isdisjoint)
Q(issubset)
Q(issuperset)
Q(set)
Q(symmetric_difference)
Q(symmetric_difference_update)
Q(union)
Q(update)
#endif

#if MICROPY_PY_BUILTINS_FROZENSET
Q(frozenset)
#endif

#if MICROPY_PY_MATH || MICROPY_PY_CMATH
Q(math)
Q(e)
Q(pi)
Q(sqrt)
Q(pow)
Q(exp)
#if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
Q(expm1)
#endif
Q(log)
#if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
Q(log2)
Q(log10)
Q(cosh)
Q(sinh)
Q(tanh)
Q(acosh)
Q(asinh)
Q(atanh)
#endif
Q(cos)
Q(sin)
Q(tan)
Q(acos)
Q(asin)
Q(atan)
Q(atan2)
Q(ceil)
Q(copysign)
Q(fabs)
Q(fmod)
Q(floor)
Q(isfinite)
Q(isinf)
Q(isnan)
Q(trunc)
Q(modf)
Q(frexp)
Q(ldexp)
Q(degrees)
Q(radians)
#if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
Q(erf)
Q(erfc)
Q(gamma)
Q(lgamma)
#endif
#endif

#if MICROPY_PY_CMATH
Q(cmath)
Q(phase)
Q(polar)
Q(rect)
#endif

#if MICROPY_PY_MICROPYTHON_MEM_INFO
#if MICROPY_MEM_STATS
Q(mem_total)
Q(mem_current)
Q(mem_peak)
#endif
Q(mem_info)
Q(qstr_info)
#if MICROPY_STACK_CHECK
Q(stack_use)
#endif
#endif
#if MICROPY_ENABLE_GC
Q(heap_lock)
Q(heap_unlock)
#endif

#if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF && (MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE == 0)
Q(alloc_emergency_exception_buf)
#endif
Q(maximum recursion depth exceeded)

Q(<module>)
Q(<lambda>)
Q(<listcomp>)
Q(<dictcomp>)
Q(<setcomp>)
Q(<genexpr>)
Q(<string>)
Q(<stdin>)

#if MICROPY_CPYTHON_COMPAT
Q(encode)
Q(decode)
Q(utf-8)
#endif

#if MICROPY_PY_SYS
Q(argv)
Q(byteorder)
Q(big)
Q(exit)
Q(little)
#ifdef MICROPY_PY_SYS_PLATFORM
Q(platform)
#endif
Q(stdin)
Q(stdout)
Q(stderr)
#if MICROPY_PY_SYS_STDIO_BUFFER
Q(buffer)
#endif
Q(version)
Q(version_info)
#if MICROPY_PY_ATTRTUPLE
Q(name)
#endif
Q(implementation)
#if MICROPY_PY_SYS_MAXSIZE
Q(maxsize)
#endif
#if MICROPY_PY_SYS_MODULES
Q(modules)
#endif
#if MICROPY_PY_SYS_EXC_INFO
Q(exc_info)
#endif
Q(print_exception)
#endif

#if MICROPY_PY_STRUCT
Q(struct)
Q(ustruct)
Q(pack)
Q(pack_into)
Q(unpack)
Q(unpack_from)
Q(calcsize)
#endif

#if MICROPY_PY_UCTYPES
Q(uctypes)
Q(struct)
Q(sizeof)
Q(addressof)
Q(bytes_at)
Q(bytearray_at)

Q(NATIVE)
Q(LITTLE_ENDIAN)
Q(BIG_ENDIAN)

Q(VOID)

Q(UINT8)
Q(INT8)
Q(UINT16)
Q(INT16)
Q(UINT32)
Q(INT32)
Q(UINT64)
Q(INT64)

Q(BFUINT8)
Q(BFINT8)
Q(BFUINT16)
Q(BFINT16)
Q(BFUINT32)
Q(BFINT32)

Q(FLOAT32)
Q(FLOAT64)

Q(ARRAY)
Q(PTR)
//Q(BITFIELD)

Q(BF_POS)
Q(BF_LEN)
#endif

#if MICROPY_PY_IO
Q(_io)
Q(readall)
Q(readinto)
Q(readline)
Q(readlines)
Q(seek)
Q(tell)
Q(FileIO)
Q(TextIOWrapper)
Q(StringIO)
Q(BytesIO)
Q(getvalue)
Q(file)
Q(mode)
Q(r)
Q(encoding)
#endif

#if MICROPY_PY_GC
Q(gc)
Q(collect)
Q(disable)
Q(enable)
Q(isenabled)
Q(mem_free)
Q(mem_alloc)
#endif

#if MICROPY_PY_BUILTINS_PROPERTY
Q(property)
Q(getter)
Q(setter)
Q(deleter)
Q(doc)
#endif

#if MICROPY_PY_UZLIB
Q(uzlib)
Q(decompress)
#endif

#if MICROPY_PY_UJSON
Q(ujson)
Q(dumps)
Q(loads)
#endif

#if MICROPY_PY_URE
Q(ure)
Q(compile)
Q(match)
Q(search)
Q(group)
Q(DEBUG)
#endif

#if MICROPY_PY_UHEAPQ
Q(uheapq)
Q(heappush)
Q(heappop)
Q(heapify)
#endif

#if MICROPY_PY_UHASHLIB
Q(uhashlib)
Q(update)
Q(digest)
Q(sha256)
Q(sha1)
#endif

#if MICROPY_PY_UBINASCII
Q(ubinascii)
Q(hexlify)
Q(unhexlify)
Q(a2b_base64)
Q(b2a_base64)
#endif

#if MICROPY_PY_MACHINE
Q(umachine)
Q(mem)
Q(mem8)
Q(mem16)
Q(mem32)
#endif

#if MICROPY_PY_USSL
Q(ussl)
Q(wrap_socket)
#endif

#if MICROPY_PY_LWIP
// for lwip module
Q(lwip)
Q(reset)
Q(callback)
Q(socket)
Q(AF_INET)
Q(AF_INET6)
Q(SOCK_STREAM)
Q(SOCK_DGRAM)
Q(SOCK_RAW)
// for lwip.socket
Q(close)
Q(bind)
Q(listen)
Q(accept)
Q(connect)
Q(send)
Q(recv)
Q(sendto)
Q(recvfrom)
Q(settimeout)
#if MICROPY_PY_LWIP_SLIP
// for lwip.slip
Q(slip)
Q(status)
#endif
#endif

#if MICROPY_FSUSERMOUNT
// for user-mountable block devices
Q(mount)
Q(umount)
Q(readonly)
Q(mkfs)
Q(readblocks)
Q(writeblocks)
Q(ioctl)
Q(sync)
Q(count)
#endif

#if MICROPY_PY_OS_DUPTERM
Q(dupterm)
#endif

#if MICROPY_PY_URANDOM
Q(urandom)
Q(getrandbits)
Q(seed)
#if MICROPY_PY_URANDOM_EXTRA_FUNCS
Q(randrange)
Q(randint)
Q(choice)
Q(random)
Q(uniform)
#endif
#endif

#if MICROPY_VFS_FAT
Q(VfsFat)
Q(flush)
#endif
