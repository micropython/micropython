/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "osi.h"
#include "fifo.h"
#include "socketfifo.h"


/*----------------------------------------------------------------------------
 ** Declare private functions
 */
static void socketfifo_Push (void * const pvFifo, const void * const pvElement);
static void socketfifo_Pop (void * const pvFifo, void * const pvElement);

/*----------------------------------------------------------------------------
 ** Declare private data
 */
static FIFO_t *socketfifo;

/*----------------------------------------------------------------------------
 ** Define public functions
 */
void SOCKETFIFO_Init (FIFO_t *fifo, void *elements, uint32_t maxcount) {
    // Initialize global data
    socketfifo = fifo;
    socketfifo->pvElements = elements;
    FIFO_Init (socketfifo, maxcount, socketfifo_Push, socketfifo_Pop);
}

bool SOCKETFIFO_Push (const void * const element) {
    return FIFO_bPushElement (socketfifo, element);
}

bool SOCKETFIFO_Pop (void * const element) {
    return FIFO_bPopElement (socketfifo, element);
}

bool SOCKETFIFO_Peek (void * const element) {
    return FIFO_bPeekElement (socketfifo, element);
}

bool SOCKETFIFO_IsEmpty (void) {
    return FIFO_IsEmpty (socketfifo);
}

bool SOCKETFIFO_IsFull (void) {
    return FIFO_IsFull (socketfifo);
}

void SOCKETFIFO_Flush (void) {
    SocketFifoElement_t element;
    while (SOCKETFIFO_Pop(&element)) {
        if (element.freedata) {
            mem_Free(element.data);
        }
    }
}

unsigned int SOCKETFIFO_Count (void) {
    return socketfifo->uiElementCount;
}

/*----------------------------------------------------------------------------
 ** Define private functions
 */
static void socketfifo_Push (void * const pvFifo, const void * const pvElement) {
    if ((pvFifo != NULL) && (NULL != pvElement)) {
        unsigned int uiLast = ((FIFO_t *)pvFifo)->uiLast;
        memcpy (&((SocketFifoElement_t *)((FIFO_t *)pvFifo)->pvElements)[uiLast], pvElement, sizeof(SocketFifoElement_t));
    }
}

static void socketfifo_Pop (void * const pvFifo, void * const pvElement) {
    if ((pvFifo != NULL) && (NULL != pvElement)) {
        unsigned int uiFirst = ((FIFO_t *)pvFifo)->uiFirst;
        memcpy (pvElement, &((SocketFifoElement_t *)((FIFO_t *)pvFifo)->pvElements)[uiFirst], sizeof(SocketFifoElement_t));
    }
}

