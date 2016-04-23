#pragma once

#include <functional>
#include "system_tick_hal.h"

typedef uint16_t product_id_t;
typedef uint16_t product_firmware_version_t;

namespace particle { namespace protocol {

#ifndef PRODUCT_ID
#define PRODUCT_ID (0xffff)
#endif

#ifndef PRODUCT_FIRMWARE_VERSION
#define PRODUCT_FIRMWARE_VERSION (0xffff)
#endif

enum ProtocolError
{
	NO_ERROR,				// 0
	PING_TIMEOUT,			// 1
	IO_ERROR,				// 2
	INVALID_STATE,			// 3
	INSUFFICIENT_STORAGE,	// 4
	MALFORMED_MESSAGE,		// 5
	DECRYPTION_ERROR,		// 6
	ENCRYPTION_ERROR,		// 7
	AUTHENTICATION_ERROR,	// 8
	BANDWIDTH_EXCEEDED,		// 9
	MESSAGE_TIMEOUT,			// 10
	MISSING_MESSAGE_ID,		// 11
	MESSAGE_RESET,			// 12
	SESSION_RESUMED,			// 13
	UNKNOWN = 0x7FFFF
};

typedef uint16_t chunk_index_t;

const chunk_index_t NO_CHUNKS_MISSING = 65535;
const chunk_index_t MAX_CHUNKS = 65535;
const size_t MISSED_CHUNKS_TO_SEND = 50;
const size_t MAX_FUNCTION_ARG_LENGTH = 64;
const size_t MAX_FUNCTION_KEY_LENGTH = 12;
const size_t MAX_VARIABLE_KEY_LENGTH = 12;
const size_t MAX_EVENT_NAME_LENGTH = 64;
const size_t MAX_EVENT_DATA_LENGTH = 64;
const size_t MAX_EVENT_TTL_SECONDS = 16777215;

#ifndef PROTOCOL_BUFFER_SIZE
    #if PLATFORM_ID<2
        #define PROTOCOL_BUFFER_SIZE 640
    #else
        #define PROTOCOL_BUFFER_SIZE 800
    #endif
#endif


namespace ChunkReceivedCode {
  enum Enum {
    OK = 0x44,
    BAD = 0x80
  };
}

enum DescriptionType {
    DESCRIBE_SYSTEM = 1<<1,            // modules
    DESCRIBE_APPLICATION = 1<<2,       // functions and variables
	DESCRIBE_ALL = DESCRIBE_SYSTEM | DESCRIBE_APPLICATION
};



typedef std::function<system_tick_t()> millis_callback;
typedef std::function<int()> callback;

const product_id_t UNDEFINED_PRODUCT_ID = product_id_t(-1);
const product_firmware_version_t UNDEFINED_PRODUCT_VERSION = product_firmware_version_t(-1);


}}
