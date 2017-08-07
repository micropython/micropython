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
#ifndef MICROPY_INCLUDED_CC3200_UTIL_FIFO_H
#define MICROPY_INCLUDED_CC3200_UTIL_FIFO_H

typedef struct {
    void *pvElements;
    unsigned int uiElementCount;
    unsigned int uiElementsMax;
    unsigned int uiFirst;
    unsigned int uiLast;
    void (*pfElementPush)(void * const pvFifo, const void * const pvElement);
    void (*pfElementPop)(void * const pvFifo, void * const pvElement);
}FIFO_t;

extern void FIFO_Init (FIFO_t *fifo, unsigned int uiElementsMax,
void (*pfElmentPush)(void * const pvFifo, const void * const pvElement),
void (*pfElementPop)(void * const pvFifo, void * const pvElement));
extern bool FIFO_bPushElement (FIFO_t *fifo, const void * const pvElement);
extern bool FIFO_bPopElement (FIFO_t *fifo, void * const pvElement);
extern bool FIFO_bPeekElement (FIFO_t *fifo, void * const pvElement);
extern bool FIFO_IsEmpty (FIFO_t *fifo);
extern bool FIFO_IsFull (FIFO_t *fifo);
extern void FIFO_Flush (FIFO_t *fifo);

#endif // MICROPY_INCLUDED_CC3200_UTIL_FIFO_H
