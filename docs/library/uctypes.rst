:mod:`uctypes` -- access C structures
=====================================

.. module:: uctypes
   :synopsis: access C structures

This module implements "foreign data interface" for MicroPython. The idea
behind it is similar to CPython's ``ctypes`` modules, but actual API is
different, steamlined and optimized for small size.

Defining structure layout
-------------------------

Structure layout is defined by a "descriptor" - a Python dictionary which
encodes field names as keys and other properties required to access them as
an associated values. Currently, uctypes requires explicit specification of
offsets for each field. Offset are given in bytes from structure start.

Following are encoding examples for various field types:

   Scalar types::

    "field_name": uctypes.UINT32 | 0

   in other words, value is scalar type identifier ORed with field offset
   (in bytes) from the start of the structure.

   Recursive structures::

    "sub": (2, {
        "b0": uctypes.UINT8 | 0,
        "b1": uctypes.UINT8 | 1,
    })

   i.e. value is a 2-tuple, first element of which is offset, and second is
   a structure descriptor dictionary (note: offsets in recursive descriptors
   are relative to a structure it defines).

   Arrays of primitive types::

      "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 2),

   i.e. value is a 2-tuple, first element of which is ARRAY flag ORed
   with offset, and second is scalar element type ORed number of elements
   in array.

   Arrays of aggregate types::

    "arr2": (uctypes.ARRAY | 0, 2, {"b": uctypes.UINT8 | 0}),

   i.e. value is a 3-tuple, first element of which is ARRAY flag ORed
   with offset, second is a number of elements in array, and third is
   descriptor of element type.

   Pointer to a primitive type::

    "ptr": (uctypes.PTR | 0, uctypes.UINT8),

   i.e. value is a 2-tuple, first element of which is PTR flag ORed
   with offset, and second is scalar element type.

   Pointer to aggregate type::

    "ptr2": (uctypes.PTR | 0, {"b": uctypes.UINT8 | 0}),

   i.e. value is a 2-tuple, first element of which is PTR flag ORed
   with offset, second is descriptor of type pointed to.

   Bitfields::

    "bitf0": uctypes.BFUINT16 | 0 | 0 << uctypes.BF_POS | 8 << uctypes.BF_LEN,

   i.e. value is type of scalar value containing given bitfield (typenames are
   similar to scalar types, but prefixes with "BF"), ORed with offset for
   scalar value containing the bitfield, and further ORed with values for
   bit offset and bit length of the bitfield within scalar value, shifted by
   BF_POS and BF_LEN positions, respectively. Bitfield position is counted
   from the least significant bit, and is the number of right-most bit of a
   field (in other words, it's a number of bits a scalar needs to be shifted
   right to extra the bitfield).

   In the example above, first UINT16 value will be extracted at offset 0
   (this detail may be important when accessing hardware registers, where
   particular access size and alignment are required), and then bitfield
   whose rightmost bit is least-significant bit of this UINT16, and length
   is 8 bits, will be extracted - effectively, this will access
   least-significant byte of UINT16.

   Note that bitfield operations are independent of target byte endianness,
   in particular, example above will access least-significant byte of UINT16
   in both little- and big-endian structures. But it depends on the least
   significant bit being numbered 0. Some targets may use different
   numbering in their native ABI, but ``uctypes`` always uses normalized
   numbering described above.

Module contents
---------------

.. class:: struct(descriptor, layout_type)

   Create a "foreign data structure" object based on its descriptor (encoded
   as a dictionary) and layout type.

.. data:: LITTLE_ENDIAN

   Little-endian packed structure. (Packed means that every field occupies
   exactly many bytes as defined in the descriptor, i.e. alignment is 1).

.. data:: BIG_ENDIAN

   Big-endian packed structure.

.. data:: NATIVE

   Native structure - with data endianness and alignment conforming to
   the target ABI.

(to be continued)

Structure objects
-----------------

Structure objects allow accessing individual fields using standard dot
notation: ``my_struct.field1``. If a field is of scalar type, getting
it will produce primitive value (Python integer or float) corresponding
to value contained in a field. Scalar field can also be assigned to.

If a field is an array, its individual elements can be accessed with
standard subscript operator - both read and assigned to.

If a field is a pointer, it can be dereferenced using ``[0]`` syntax
(corresponding to C ``*`` operator, though ``[0]`` works in C too).
Subscripting pointer with other integer values but 0 are supported too,
with the same semantics as in C.

Summing up, accessing structure fields generally follows C syntax,
except for pointer derefence, you need to use ``[0]`` operator instead
of ``*``.
