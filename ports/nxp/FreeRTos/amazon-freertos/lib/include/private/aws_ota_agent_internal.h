/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_ota_agent_internal.h
 * @brief Macros, enums, variables, and definitions internal to the OTA Agent module and
 * shared by other OTA modules and testing files.
 */

#ifndef _AWS_OTA_AGENT_INTERAL_H_
#define _AWS_OTA_AGENT_INTERAL_H_

#include "aws_ota_agent_config.h"
#include "jsmn.h"

#define LOG2_BITS_PER_BYTE      3UL                             /* Log base 2 of bits per byte. */
#define BITS_PER_BYTE           ( 1UL << LOG2_BITS_PER_BYTE )   /* Number of bits in a byte. This is used by the block bitmap implementation. */
#define OTA_FILE_BLOCK_SIZE     ( 1UL << otaconfigLOG2_FILE_BLOCK_SIZE ) /* Data section size of the file data block message (excludes the header). */

typedef enum
{
    eIngest_Result_FileComplete = -1,      /* The file transfer is complete and the signature check passed. */
    eIngest_Result_SigCheckFail = -2,      /* The file transfer is complete but the signature check failed. */
    eIngest_Result_FileCloseFail = -3,     /* There was a problem trying to close the receive file. */
    eIngest_Result_NullContext = -4,       /* The specified OTA context pointer is null. */
    eIngest_Result_BadFileHandle = -5,     /* The receive file pointer is invalid. */
    eIngest_Result_UnexpectedBlock = -6,   /* We were asked to ingest a block but weren't expecting one. */
    eIngest_Result_BlockOutOfRange = -7,   /* The received block is out of the expected range. */
    eIngest_Result_BadData = -8,           /* The data block from the server was malformed. */
    eIngest_Result_WriteBlockFailed = -9,  /* The PAL layer failed to write the file block. */
    eIngest_Result_NullResultPointer = -10,/* The pointer to the close result pointer was null. */
    eIngest_Result_Uninitialized = -127,   /* Software BUG: We forgot to set the result code. */
    eIngest_Result_Accepted_Continue = 0,  /* The block was accepted and we're expecting more. */
    eIngest_Result_Duplicate_Continue = 1, /* The block was a duplicate but that's OK. Continue. */
} IngestResult_t;

/* Generic JSON document parser errors. */

typedef enum
{
    eDocParseErr_Unknown = -1,          /* The error code has not yet been set by a logic path. */
    eDocParseErr_None = 0,
    eDocParseErr_OutOfMemory,           /* We failed to allocate enough memory for a field. */
    eDocParseErr_FieldTypeMismatch,     /* The field type parsed does not match the document model. */
    eDocParseErr_Base64Decode,          /* There was an error decoding the base64 data. */
    eDocParseErr_InvalidNumChar,        /* There was an invalid character in a numeric value field. */
    eDocParseErr_DuplicatesNotAllowed,  /* A duplicate parameter was found in the job document. */
    eDocParseErr_MalformedDoc,          /* The document didn't fulfill the model requirements. */
    eDocParseErr_JasmineCountMismatch,  /* The second pass of jsmn_parse() didn't match the first pass. */
    eDocParseErr_TooManyTokens,         /* We can't support the number of JSON tokens in the document. */
    eDocParseErr_NoTokens,              /* No JSON tokens were detected in the document. */
    eDocParseErr_NullModelPointer,      /* The pointer to the document model was NULL. */
    eDocParseErr_NullBodyPointer,       /* The document model's internal body pointer was NULL. */
    eDocParseErr_NullDocPointer,        /* The pointer to the JSON document was NULL. */
    eDocParseErr_TooManyParams,         /* The document model has more parameters than we can handle. */
    eDocParseErr_ParamKeyNotInModel,    /* The document model doesn't include the specified parameter key. */
    eDocParseErr_InvalidModelParamType, /* The document model specified an invalid parameter type. */
    eDocParseErr_InvalidToken           /* The Jasmine token was invalid, producing a NULL pointer. */
} DocParseErr_t;

/* Document model parameter types used by the JSON document parser. */

typedef enum
{
    eModelParamType_StringCopy,
    eModelParamType_StringInDoc, /* Only use this type if you can process before freeing the document memory. */
    eModelParamType_Object,
    eModelParamType_Array,
    eModelParamType_UInt32,
    eModelParamType_SigBase64,
    eModelParamType_Ident
} ModelParamType_t;

/* This is a document parameter structure used by the document model. It determines
 * the type of parameter specified by the key name and where to store the parameter
 * locally when it is extracted from the JSON document. It also contains the
 * expected Jasmine type of the value field for validation.
 *
 * NOTE: The ulDestOffset field may be either an offset into the models context structure
 *       or an absolute memory pointer, although it is usually an offset.
 *       If the value of ulDestOffset is less than the size of the context structure,
 *       which is fairly small, it will add the offset of the active context structure
 *       to attain the effective address (somewhere in RAM). Otherwise, it is interpreted
 *       as an absolute memory address and used as is (useful for singleton parameters).
 *       This requires absolute memory pointers to be greater than the size of the
 *       context structure to avoid the address being misinterpreted as an offset.
 */
typedef struct
{
    const char * pcSrcKey;  /* Expected key name. */
    const bool_t bRequired; /* If true, this parameter must exist in the document. */
    union
    {
        const uint32_t ulDestOffset;        /* Pointer or offset to where we'll store the value, if not ~0. */
        void * const pvDestOffset;          /* Pointer or offset to where we'll store the value, if not ~0. */
    };
    const ModelParamType_t xModelParamType; /* We extract the value, if found, based on this type. */
    const jsmntype_t eJasmineType;          /* The JSON value type must match that specified here. */
} JSON_DocParam_t;


/* The document model is currently limited to 32 parameters per the implementation,
 * although it may be easily expanded to more in the future by simply expanding
 * the parameter bitmap.
 *
 * The document model is used to control what JSON parameters are expected from a
 * document and where to store the parameters, if desired, in a destination context.
 * We currently only store parameters into an OTA_FileContext_t but it could be used
 * for any structure since we don't use a type pointer.
 */
typedef struct
{
    uint32_t ulContextBase;            /* The base address of the destination OTA context structure. */
    uint32_t ulContextSize;            /* The size, in bytes, of the destination context structure. */
    const JSON_DocParam_t * pxBodyDef; /* Pointer to the document model body definition. */
    uint16_t usNumModelParams;         /* The number of entries in the document model (limited to 32). */
    uint32_t ulParamsReceivedBitmap;   /* Bitmap of the parameters received based on the model. */
    uint32_t ulParamsRequiredBitmap;   /* Bitmap of the parameters required from the model. */
} JSON_DocModel_t;

#endif /* ifndef _AWS_OTA_AGENT_INTERAL_H_ */
