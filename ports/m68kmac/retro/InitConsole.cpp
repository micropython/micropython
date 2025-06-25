/*
    Copyright 2014 Wolfgang Thaller.

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

#include <string>

#include <Quickdraw.h>
#include <MacMemory.h>
#include <Sound.h>
#include <Events.h>
#include <Fonts.h>

#include <sys/types.h>
#include <string.h>

#include "MacUtils.h"
#include "Console.h"
#include "ConsoleWindow.h"

namespace retro
{
    void InitConsole();
}

using namespace retro;

void retro::InitConsole()
{
    if(Console::currentInstance)
        return;
    Console::currentInstance = (Console*) -1;

#if !TARGET_API_MAC_CARBON
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();

    Rect r = qd.screenBits.bounds;
#else
    Rect r = (*GetMainDevice())->gdRect;
#endif
    {
        // give MultiFinder a chance to bring the App to front
        // see Technote TB 35 - MultiFinder Miscellanea
        // "If your application [...] has the canBackground bit set in the
        //  size resource, then it should call _EventAvail several times
        //  (or _WaitNextEvent or _GetNextEvent) before putting up the splash
        //  screen, or the splash screen will come up behind the frontmost
        //  layer. If the canBackground bit is set, MultiFinder will not move
        //  your layer to the front until you call _GetNextEvent,
        //  _WaitNextEvent, or _EventAvail."
        
        EventRecord event;
        for(int i = 0; i < 5; i++)
            EventAvail(everyEvent, &event);
    }

    r.top += 40;
    InsetRect(&r, 5,5);
    
    Console::currentInstance = new ConsoleWindow(r, "\pRetro68 Console");
    InitCursor();
}

extern "C" ssize_t _consolewrite(int fd, const void *buf, size_t count)
{
    if(!Console::currentInstance)
        InitConsole();
    if(Console::currentInstance == (Console*)-1)
        return 0;

    Console::currentInstance->write((const char*)buf, count);
    return count;
}

extern "C" ssize_t _consoleread(int fd, void *buf, size_t count)
{
#if 0
    if(!Console::currentInstance)
        InitConsole();
    if(Console::currentInstance == (Console*)-1)
        return 0;

    static std::string consoleBuf;
    if(consoleBuf.size() == 0)
       consoleBuf = Console::currentInstance->ReadLine();

    if(count > consoleBuf.size())
        count = consoleBuf.size();
    memcpy(buf, consoleBuf.data(), count);
    consoleBuf = consoleBuf.substr(count);
    return count;
#endif
    return 0;
}
