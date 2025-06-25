/*
    Copyright 2012-2020 Wolfgang Thaller, Davide Bucci

    This file is part of Retro68.

    Retro68 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Retro68 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    Under Section 7 of GPL version 3, you are granted additional
    permissions described in the GCC Runtime Library Exception, version
    3.1, as published by the Free Software Foundation.

    You should have received a copy of the GNU General Public License
    along with Retro68.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef RETRO68_CONSOLE_H_
#define RETRO68_CONSOLE_H_

#include <Quickdraw.h>
#include <vector>

namespace retro
{
    class Console;
    typedef void (Console::*EscapeSequenceFunction)();

    class Attributes
    {
public:

        bool isBold(void) const;
        bool isUnderline(void) const;
        bool isItalic(void) const;

        void setBold(const bool v);
        void setUnderline(const bool v);
        void setItalic(const bool v);

        Attributes(void);
        void reset(void);

private:

        bool cBold;
        bool cUnderline;
        bool cItalic;
    };

    class AttributedChar
    {
public:
        char c;
        Attributes attrs;
        AttributedChar(char cc, Attributes aa) {
            c = cc;
            attrs = aa;
        }
    };

    enum class State
    {
        noSequence,
        waitingForSequenceStart,
        waitingForControlSequence,
        waitingForM,
        waitingForOSCStart,
        waitingForSemicolon,
        inWindowName
    };

    class Console
    {
public:
        Console();
        Console(GrafPtr port, Rect r);
        ~Console();

        void Reshape(Rect newBounds);

        void Draw(Rect r);
        void Draw() {
            Draw(bounds);
        }
        void putch(char c);

        void write(const char *s, int n);

        static Console *currentInstance;

        short GetRows() const {
            return rows;
        }
        short GetCols() const {
            return cols;
        }

        virtual void setWindowName(const char *newName) {
        };


        void Idle();

        bool IsEOF() const {
            return eof;
        }

private:

        State sequenceState;
        GrafPtr consolePort = nullptr;
        Rect bounds;
        Attributes currentAttr;
        EscapeSequenceFunction escapeSequenceMap[256] = {};

        std::vector < AttributedChar > chars, onscreen;

        short cellSizeX;
        short cellSizeY;

        short rows = 0, cols = 0;

        short cursorX, cursorY;
        short savedCursorX, savedCursorY;

        Rect dirtyRect = {};

        long blinkTicks = 0;
        bool cursorDrawn = false;
        bool cursorVisible = true;
        bool cursorRequestedHidden = false;
        bool eof = false;

public:
        void PutCharNoUpdate(char c);
        void Update();

private:
        short CalcStartX(short x, short y);
        Rect CellRect(short x, short y);
        void DrawCell(short x, short y, bool erase = true);
        void DrawCells(short x1, short x2, short y, bool erase = true);
        void ScrollUp(short n = 1);
        bool ProcessEscSequence(char c);
        void SetAttributes(Attributes aa);

        void InvalidateCursor();

public:
        virtual int WaitNextChar(unsigned long timeout = ~-0UL);
        virtual bool Available(unsigned long timeout = ~-0UL);
private:
        void InitEscapeSequenceMap();
        void SetCursorPosition();
        void EraseInDisplay();
        void SetDisplayAttributes();
        void ClearWindow();
        void ClearFromCursorToEndOfWindow();
        void ClearFromTopOfWindowToCursor();
        void HandleControlSequence(char);
        void MoveCursorUp();
        void MoveCursorDown();
        void MoveCursorForward();
        void MoveCursorBack();
        void MoveCursorNextLine();
        void MoveCursorPreviousLine();
        void MoveCursorHorizonalAbsolute();
        void EraseInLine();
        void ClearFromCursorToEndOfLine();
        void ClearFromBeginningOfLineToCursor();
        void ClearEntireLine();
        void ShowCursor();
        void HideCursor();
        void SaveCursorPosition();
        void RestoreCursorPosition();
        void SetCursorX(int newX);
        int GetCursorX();
        void SetCursorY(int newY);
        int GetCursorY();

        int getArgDefault(size_t i, int defval) {
            if (i < args.size()) {
                return args[i];
            }
            return defval;
        }

        std::vector < char > title;
        std::vector < int > args;
        int cur_arg;
        bool got_something;
protected:
        void Init(GrafPtr port, Rect r);
    };


}

#endif /* RETRO68_CONSOLE_H_ */
