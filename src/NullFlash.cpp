///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2018, ShumaTech
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

#include "NullFlash.h"

NullFlash::NullFlash(
    Samba& samba,
    const std::string& name,
    uint32_t pages,
    uint32_t size,
    uint32_t user,
    uint32_t stack)
    :
    Flash(samba, name, 0, pages, size, 1, 0, user, stack)
{
}

NullFlash::~NullFlash()
{
}

void
NullFlash::erase(uint32_t offset, uint32_t size)
{
    throw FlashEraseError();
}

void
NullFlash::eraseAll(uint32_t offset)
{
    // Use the extended Samba command if available
    if (_samba.canChipErase())
    {
        _samba.chipErase(offset);
    }
    else
    {
        erase(offset, totalSize() - offset);
    }
}

void
NullFlash::eraseAuto(bool enable)
{
    _eraseAuto = enable;
}

std::vector<bool>
NullFlash::getLockRegions()
{
    std::vector<bool> regions(0);
    return regions;
}

bool
NullFlash::getSecurity()
{
    return false;
}

bool
NullFlash::getBod()
{
    return false;
}

bool
NullFlash::getBor()
{
    return false;
}

bool
NullFlash::getBootFlash()
{
    return true;
}

void
NullFlash::writeOptions()
{
    return;
}

void
NullFlash::writePage(uint32_t page)
{
    throw FlashPageError();
}

void
NullFlash::readPage(uint32_t page, uint8_t* buf)
{
    if (page >= _pages)
    {
        throw FlashPageError();
    }

    _samba.read(_addr + (page * _size), buf, _size);
}

void
NullFlash::writeBuffer(uint32_t dst_addr, uint32_t size)
{
    // Auto-erase if enabled
    if (_eraseAuto)
        erase(dst_addr, size);

    // Call the base class method
    Flash::writeBuffer(dst_addr, size);
}
