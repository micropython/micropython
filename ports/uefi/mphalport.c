/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// UEFI HAL: console I/O over SIMPLE_TEXT_OUTPUT/INPUT, plus delay via BootServices->Stall.

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "efi.h"
#include "uefi_port.h"

// Output goes through ConOut, a UTF-16 terminal *emulation* API, not a byte-clean
// pipe: it does not interpret raw VT100 escapes, and it gives special meaning only
// to NUL/BS/LF/CR. So the port drives the console through ConOut's own API rather
// than embedding escapes in the byte stream:
//   * MICROPY_HAL_HAS_VT100 = 0, and readline's cursor moves / line erases are
//     implemented below via ConOut SetCursorPosition (NOT backspace — see the
//     mp_hal_move_cursor_back note for why that matters).
//   * mp_hal_stdout_tx_strn adds the common terminal effectors ConOut is missing
//     (FF -> clear, TAB -> tab stops, BEL/NUL -> dropped).
// ConOut generates the correct control codes for the underlying transport itself
// (serial terminal or GOP text), so no raw escape ever has to survive it.

// Flush the pending CHAR16 run to ConOut and reset the fill index.
static void conout_flush(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out, CHAR16 *buf, size_t *i) {
    if (*i) {
        buf[*i] = 0;
        out->OutputString(out, buf);
        *i = 0;
    }
}

// Send a string (UTF-8/ASCII) to ConOut, expanding each byte to CHAR16.
//
// ConOut->OutputString only gives special meaning to NUL, BS (\b), LF (\n) and
// CR (\r) — every other control byte is drawn as a glyph/box. We add the common
// terminal effectors ConOut is missing:
//   * FF  (\f, 0x0C) -> clear the screen and home the cursor (ClearScreen).
//   * TAB (\t, 0x09) -> advance to the next 8-column tab stop with spaces.
//   * BEL (\a, 0x07) -> dropped (UEFI has no portable bell) rather than drawn.
//   * NUL (0x00)     -> dropped (it would otherwise terminate the CHAR16 run early).
// BS/LF/CR pass straight through to ConOut, which handles them. The cooked path
// inserts CR before LF on top of this.
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out = mp_uefi_st->ConOut;
    CHAR16 buf[65];
    size_t i = 0;
    for (size_t n = 0; n < len; n++) {
        uint8_t ch = (uint8_t)str[n];
        switch (ch) {
            case '\0':                       // NUL: drop (would truncate the run)
            case '\a':                       // BEL: no portable bell, drop
                continue;
            case '\f':                       // FF: clear screen + home cursor
                conout_flush(out, buf, &i);
                out->ClearScreen(out);
                // Some consoles' ClearScreen resets the column but not the row (or
                // don't update Mode); home explicitly so the cursor state is right.
                out->SetCursorPosition(out, 0, 0);
                continue;
            case '\t': {                     // TAB: spaces to the next 8-col stop
                conout_flush(out, buf, &i);
                int spaces = 8 - (out->Mode->CursorColumn & 7);
                CHAR16 sp[9];
                for (int s = 0; s < spaces; s++) {
                    sp[s] = ' ';
                }
                sp[spaces] = 0;
                out->OutputString(out, sp);
                continue;
            }
        }
        buf[i++] = (CHAR16)ch;
        if (i == sizeof(buf) / sizeof(buf[0]) - 1) {
            conout_flush(out, buf, &i);
        }
    }
    conout_flush(out, buf, &i);
    return len;
}

// stdin (mp_hal_stdin_rx_chr / mp_hal_stdio_poll / mp_hal_set_interrupt_char) is
// in uefi_event.c, where it is fed by the ConIn poll notify via a ring buffer.
// The timing HAL (mp_hal_ticks_* / mp_hal_delay_*) is in uefi_time.c.

// --- readline cursor control without VT100 (ConOut isn't byte-clean) ---
// readline drives cursor movement through these (MICROPY_HAL_HAS_VT100 = 0).
//
// We must NOT move the cursor with backspace: on ConOut — and on the serial
// terminals its output is routed to — BACKSPACE can be *destructive* (it blanks
// the cell it moves onto). readline moves left without redrawing the tail (left
// arrow / Home just call mp_hal_move_cursor_back), so a destructive BS erases the
// tail of the line on every leftward move; it only reappears when a rightward move
// reprints it. Instead we reposition via ConOut->SetCursorPosition, which is
// non-destructive by construction and lets ConOut emit the correct control codes
// itself (no raw escapes in the output stream). ConOut->Mode tracks the live
// cursor column/row, so we compute the target from there.
void mp_hal_move_cursor_back(unsigned int pos) {
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out = mp_uefi_st->ConOut;
    SIMPLE_TEXT_OUTPUT_MODE *mode = out->Mode;
    int col = mode->CursorColumn;
    int row = mode->CursorRow;
    if ((unsigned int)col >= pos) {
        // Common case: stays on the current line.
        out->SetCursorPosition(out, (UINTN)(col - (int)pos), (UINTN)row);
        return;
    }
    // Wrapping back over earlier line(s): need the mode's column count.
    UINTN cols = 80, rows = 0;
    out->QueryMode(out, (UINTN)mode->Mode, &cols, &rows);
    long idx = (long)row * (long)cols + col - (long)pos;
    if (idx < 0) {
        idx = 0;
    }
    out->SetCursorPosition(out, (UINTN)(idx % (long)cols), (UINTN)(idx / (long)cols));
}

void mp_hal_erase_line_from_cursor(unsigned int n_chars_to_erase) {
    // Overwrite the trailing chars with spaces (this advances the cursor), then
    // restore the cursor to where it started via the non-destructive move above.
    for (unsigned int i = 0; i < n_chars_to_erase; i++) {
        mp_hal_stdout_tx_strn(" ", 1);
    }
    mp_hal_move_cursor_back(n_chars_to_erase);
}
