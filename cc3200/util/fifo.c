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

#include "fifo.h"


/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void FIFO_Init (FIFO_t *fifo, unsigned int uiElementsMax,
                void (*pfElmentPush)(void * const pvFifo, const void * const pvElement),
                void (*pfElementPop)(void * const pvFifo, void * const pvElement)) {
    if (fifo) {
        fifo->uiFirst = 0;
        fifo->uiLast = uiElementsMax - 1;
        fifo->uiElementCount = 0;
        fifo->uiElementsMax = uiElementsMax;
        fifo->pfElementPush = pfElmentPush;
        fifo->pfElementPop = pfElementPop;
    }
}

bool FIFO_bPushElement (FIFO_t *fifo, const void * const pvElement) {
    if (!fifo) {
        return false;
    }
    // Check if the queue is full
    if (true == FIFO_IsFull (fifo)) {
        return false;
    }

    // Increment the element count
    if (fifo->uiElementsMax > fifo->uiElementCount) {
        fifo->uiElementCount++;
    }
    fifo->uiLast++;
    if (fifo->uiLast == fifo->uiElementsMax) {
        fifo->uiLast = 0;
    }
    // Insert the element into the queue
    fifo->pfElementPush(fifo, pvElement);
    return true;
}

bool FIFO_bPopElement (FIFO_t *fifo, void * const pvElement) {
    if (!fifo) {
        return false;
    }
    // Check if the queue is empty
    if (true == FIFO_IsEmpty (fifo)) {
        return false;
    }

    // Get the element from the queue
    fifo->pfElementPop(fifo, pvElement);
    // Decrement the element count
    if (fifo->uiElementCount > 0) {
        fifo->uiElementCount--;
    }
    fifo->uiFirst++;
    if (fifo->uiFirst == fifo->uiElementsMax) {
        fifo->uiFirst = 0;
    }
    return true;
}

bool FIFO_bPeekElement (FIFO_t *fifo, void * const pvElement) {
    if (!fifo) {
        return false;
    }
    // Check if the queue is empty
    if (true == FIFO_IsEmpty (fifo)) {
        return false;
    }
    // Get the element from the queue
    fifo->pfElementPop(fifo, pvElement);
    return true;
}

bool FIFO_IsEmpty (FIFO_t *fifo) {
    if (fifo) {
        return ((fifo->uiElementCount == 0) ? true : false);
    }
    return false;
}

bool FIFO_IsFull (FIFO_t *fifo) {
    if (fifo) {
        return ((fifo->uiElementCount < fifo->uiElementsMax) ? false : true);
    }
    return false;
}

void FIFO_Flush (FIFO_t *fifo) {
    if (fifo) {
        fifo->uiElementCount = 0;
        fifo->uiFirst = 0;
        fifo->uiLast = fifo->uiElementsMax - 1;
    }
}
