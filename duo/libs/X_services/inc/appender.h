/**
 ******************************************************************************
 * @file    appender.h
 * @author  Matthew McGowan
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef APPENDER_H
#define	APPENDER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef bool (*appender_fn)(void* appender, const uint8_t* data, size_t length);



#ifdef	__cplusplus
}

/**
 * OO version of the appender function.
 */
class Appender {
public:
    virtual bool append(const uint8_t* data, size_t length)=0;

    bool append(const char* data) {
        return append((const uint8_t*)data, strlen(data));
    }
    bool append(char c) {
        return append((const uint8_t*)&c, 1);
    }
};

inline bool append_instance(void* appender, const uint8_t* data, size_t length) {
    Appender* a = (Appender*)appender;
    return a->append(data, length);
}

class BufferAppender : public Appender {
    uint8_t* buffer;
    uint8_t* end;

public:

    BufferAppender(uint8_t* start, size_t length) {
        this->buffer = start;
        this->end = start + length;
    }

    bool append(const uint8_t* data, size_t length) {
        bool has_space = (size_t(end-buffer)>=length);
        if (has_space) {
            memcpy(buffer, data, length);
            buffer += length;
        }
        return has_space;
    }

    bool append(const char* data) {
        return Appender::append(data);
    }
    bool append(char c) {
        return Appender::append(c);
    }


    const uint8_t* next() { return buffer; }
};

#endif

#endif	/* APPENDER_H */

