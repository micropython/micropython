#ifndef CBORCONSTANTS_P_H
#define CBORCONSTANTS_P_H

/*
 * CBOR Major types
 * Encoded in the high 3 bits of the descriptor byte
 * See http://tools.ietf.org/html/rfc7049#section-2.1
 */
typedef enum CborMajorTypes {
    UnsignedIntegerType = 0U,
    NegativeIntegerType = 1U,
    ByteStringType = 2U,
    TextStringType = 3U,
    ArrayType = 4U,
    MapType = 5U,           /* a.k.a. object */
    TagType = 6U,
    SimpleTypesType = 7U
} CborMajorTypes;

/*
 * CBOR simple and floating point types
 * Encoded in the low 8 bits of the descriptor byte when the
 * Major Type is 7.
 */
typedef enum CborSimpleTypes {
    FalseValue              = 20,
    TrueValue               = 21,
    NullValue               = 22,
    UndefinedValue          = 23,
    SimpleTypeInNextByte    = 24,   /* not really a simple type */
    HalfPrecisionFloat      = 25,   /* ditto */
    SinglePrecisionFloat    = 26,   /* ditto */
    DoublePrecisionFloat    = 27,   /* ditto */
    Break                   = 31
} CborSimpleTypes;

enum {
    SmallValueBitLength     = 5U,
    SmallValueMask          = (1U << SmallValueBitLength) - 1,      /* 31 */
    Value8Bit               = 24U,
    Value16Bit              = 25U,
    Value32Bit              = 26U,
    Value64Bit              = 27U,
    IndefiniteLength        = 31U,

    MajorTypeShift          = SmallValueBitLength,
    MajorTypeMask           = (int) (~0U << MajorTypeShift),

    BreakByte               = (unsigned)Break | (SimpleTypesType << MajorTypeShift)
};

#endif /* CBORCONSTANTS_P_H */
