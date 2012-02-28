///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (C) 2011-2012 ShumaTech http://www.shumatech.com/
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////
#ifndef _APPLET_H
#define _APPLET_H

#include <stdint.h>

#include "Samba.h"

class Applet
{
public:
    Applet(Samba& samba,
           uint32_t addr,
           uint8_t* code,
           uint32_t size,
           uint32_t start,
           uint32_t stack,
           uint32_t reset);
    virtual ~Applet() {}

    virtual uint32_t size() { return _size; }
    virtual uint32_t addr() { return _addr; }

    virtual void setStack(uint32_t stack);

    virtual void run();
    virtual void runv();

protected:
    Samba& _samba;
    uint32_t _addr;
    uint32_t _size;
    uint32_t _start;
    uint32_t _stack;
    uint32_t _reset;
};

#endif // _APPLET_H
