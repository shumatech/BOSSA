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
#include "Applet.h"

Applet::Applet(Samba& samba,
               uint32_t addr,
               uint8_t* code,
               uint32_t size,
               uint32_t start,
               uint32_t stack,
               uint32_t reset) :
    _samba(samba), _addr(addr), _size(size), _start(start), _stack(stack), _reset(reset)
{
    _samba.write(addr, code, size);
}

void
Applet::setStack(uint32_t stack)
{
    _samba.writeWord(_stack, stack);
}

void
Applet::run()
{
    // Add one to the start address for Thumb mode
    _samba.go(_start + 1);
}

void
Applet::runv()
{
    // Add one to the start address for Thumb mode
    _samba.writeWord(_reset, _start + 1);

    // The stack is the first reset vector
    _samba.go(_stack);
}
