/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#ifndef SOCKETFIFO_H_
#define SOCKETFIFO_H_

/*----------------------------------------------------------------------------
 ** Imports
 */

/*----------------------------------------------------------------------------
 ** Define constants
 */

/*----------------------------------------------------------------------------
 ** Define types
 */

typedef struct {
    void                    *data;
    signed short            *sd;
    unsigned short          datasize;
    unsigned char           closesockets;
    bool                    freedata;

}SocketFifoElement_t;

/*----------------------------------------------------------------------------
 ** Declare public functions
 */
extern void SOCKETFIFO_Init (FIFO_t *fifo, void *elements, uint32_t maxcount);
extern bool SOCKETFIFO_Push (const void * const element);
extern bool SOCKETFIFO_Pop (void * const element);
extern bool SOCKETFIFO_Peek (void * const element);
extern bool SOCKETFIFO_IsEmpty (void);
extern bool SOCKETFIFO_IsFull (void);
extern void SOCKETFIFO_Flush (void);
extern unsigned int SOCKETFIFO_Count (void);

#endif /* SOCKETFIFO_H_ */
