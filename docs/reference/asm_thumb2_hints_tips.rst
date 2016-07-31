Hints and tips
==============

The following are some examples of the use of the inline assembler and some
information on how to work around its limitations. In this document the term
"assembler function" refers to a function declared in Python with the 
``@micropython.asm_thumb`` decorator, whereas "subroutine" refers to assembler
code called from within an assembler function.

Code branches and subroutines
-----------------------------

It is important to appreciate that labels are local to an assembler function.
There is currently no way for a subroutine defined in one function to be called
from another.

To call a subroutine the instruction ``bl(LABEL)`` is issued. This transfers
control to the instruction following the ``label(LABEL)`` directive and stores
the return address in the link register (``lr`` or ``r14``). To return the
instruction ``bx(lr)`` is issued which causes execution to continue with
the instruction following the subroutine call. This mechanism implies that, if
a subroutine is to call another, it must save the link register prior to
the call and restore it before terminating.

The following rather contrived example illustrates a function call. Note that
it's necessary at the start to branch around all subroutine calls: subroutines
end execution with ``bx(lr)`` while the outer function simply "drops off the end"
in the style of Python functions.

::

    @micropython.asm_thumb
    def quad(r0):
        b(START)
        label(DOUBLE)
        add(r0, r0, r0)
        bx(lr)
        label(START)
        bl(DOUBLE)
        bl(DOUBLE)

    print(quad(10))

The following code example demonstrates a nested (recursive) call: the classic
Fibonacci sequence. Here, prior to a recursive call, the link register is saved
along with other registers which the program logic requires to be preserved.

::

    @micropython.asm_thumb
    def fib(r0):
        b(START)
        label(DOFIB)
        push({r1, r2, lr})
        cmp(r0, 1)
        ble(FIBDONE)
        sub(r0, 1)
        mov(r2, r0) # r2 = n -1
        bl(DOFIB)
        mov(r1, r0) # r1 = fib(n -1)
        sub(r0, r2, 1)
        bl(DOFIB)   # r0 = fib(n -2)
        add(r0, r0, r1)
        label(FIBDONE)
        pop({r1, r2, lr})
        bx(lr)
        label(START)
        bl(DOFIB)

    for n in range(10):
        print(fib(n))

Argument passing and return
---------------------------

The tutorial details the fact that assembler functions can support from zero to
three arguments, which must (if used) be named ``r0``, ``r1`` and ``r2``. When
the code executes the registers will be initialised to those values.

The data types which can be passed in this way are integers and memory
addresses. With current firmware all possible 32 bit values may be passed and
returned. If the return value may have the most significant bit set a Python
type hint should be employed to enable MicroPython to determine whether the
value should be interpreted as a signed or unsigned integer: types are
``int`` or ``uint``.

::

    @micropython.asm_thumb
    def uadd(r0, r1) -> uint:
        add(r0, r0, r1)

``hex(uadd(0x40000000,0x40000000))`` will return 0x80000000, demonstrating the
passing and return of integers where bits 30 and 31 differ.

The limitations on the number of arguments and return values can be overcome by means
of the ``array`` module which enables any number of values of any type to be accessed.

Multiple arguments
~~~~~~~~~~~~~~~~~~

If a Python array of integers is passed as an argument to an assembler
function, the function will receive the address of a contiguous set of integers.
Thus multiple arguments can be passed as elements of a single array. Similarly a
function can return multiple values by assigning them to array elements.
Assembler functions have no means of determining the length of an array:
this will need to be passed to the function.

This use of arrays can be extended to enable more than three arrays to be used. 
This is done using indirection: the ``uctypes`` module supports ``addressof()`` 
which will return the address of an array passed as its argument. Thus you can
populate an integer array with the addresses of other arrays:

::

    from uctypes import addressof
    @micropython.asm_thumb
    def getindirect(r0):
        ldr(r0, [r0, 0]) # Address of array loaded from passed array
        ldr(r0, [r0, 4]) # Return element 1 of indirect array (24)

    def testindirect():
        a = array.array('i',[23, 24])
        b = array.array('i',[0,0])
        b[0] = addressof(a)
        print(getindirect(b))

Non-integer data types
~~~~~~~~~~~~~~~~~~~~~~

These may be handled by means of arrays of the appropriate data type. For
example, single precision floating point data may be processed as follows.
This code example takes an array of floats and replaces its contents with
their squares.

::

    from array import array

    @micropython.asm_thumb
    def square(r0, r1):
        label(LOOP)
        vldr(s0, [r0, 0])
        vmul(s0, s0, s0)
        vstr(s0, [r0, 0])
        add(r0, 4)
        sub(r1, 1)
        bgt(LOOP)

    a = array('f', (x for x in range(10)))
    square(a, len(a))
    print(a)

The uctypes module supports the use of data structures beyond simple
arrays. It enables a Python data structure to be mapped onto a bytearray
instance which may then be passed to the assembler function.

Named constants
---------------

Assembler code may be made more readable and maintainable by using named
constants rather than littering code with numbers. This may be achieved
thus:

::

    MYDATA = const(33)

    @micropython.asm_thumb
    def foo():
        mov(r0, MYDATA)

The const() construct causes MicroPython to replace the variable name
with its value at compile time. If constants are declared in an outer
Python scope they can be shared between multiple assembler functions and
with Python code.

Assembler code as class methods
-------------------------------

MicroPython passes the address of the object instance as the first argument
to class methods. This is normally of little use to an assembler function.
It can be avoided by declaring the function as a static method thus:

::

    class foo:
      @staticmethod
      @micropython.asm_thumb
      def bar(r0):
        add(r0, r0, r0)

Use of unsupported instructions
-------------------------------

These can be coded using the data statement as shown below. While
``push()`` and ``pop()`` are supported the example below illustrates the
principle. The necessary machine code may be found in the ARM v7-M
Architecture Reference Manual. Note that the first argument of data
calls such as

::

    data(2, 0xe92d, 0x0f00) # push r8,r9,r10,r11

indicates that each subsequent argument is a two byte quantity.

Overcoming MicroPython's integer restriction
--------------------------------------------

The Pyboard chip includes a CRC generator. Its use presents a problem in
MicroPython because the returned values cover the full gamut of 32 bit
quantities whereas small integers in MicroPython cannot have differing values
in bits 30 and 31. This limitation is overcome with the following code, which
uses assembler to put the result into an array and Python code to
coerce the result into an arbitrary precision unsigned integer.

::

    from array import array
    import stm

    def enable_crc():
        stm.mem32[stm.RCC + stm.RCC_AHB1ENR] |= 0x1000

    def reset_crc():
        stm.mem32[stm.CRC+stm.CRC_CR] = 1

    @micropython.asm_thumb
    def getval(r0, r1):
        movwt(r3, stm.CRC + stm.CRC_DR)
        str(r1, [r3, 0])
        ldr(r2, [r3, 0])
        str(r2, [r0, 0])

    def getcrc(value):
        a = array('i', [0])
        getval(a, value)
        return a[0] & 0xffffffff # coerce to arbitrary precision

    enable_crc()
    reset_crc()
    for x in range(20):
        print(hex(getcrc(0)))
