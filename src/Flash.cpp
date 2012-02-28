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
#include "Flash.h"

#include <assert.h>

Flash::Flash(Samba& samba,
             const std::string& name,
             uint32_t addr,
             uint32_t pages,
             uint32_t size,
             uint32_t planes,
             uint32_t lockRegions,
             uint32_t user,
             uint32_t stack)
    : _samba(samba), _name(name), _addr(addr), _pages(pages), _size(size),
      _planes(planes), _lockRegions(lockRegions), _user(user), _wordCopy(samba, user)
{
    assert((size & (size - 1)) == 0);
    assert((pages & (pages - 1)) == 0);
    assert((lockRegions & (lockRegions - 1)) == 0);

    _wordCopy.setWords(size / sizeof(uint32_t));
    _wordCopy.setStack(stack);

    _onBufferA = true;
    _pageBufferA = _user + _wordCopy.size();
    _pageBufferB = _pageBufferA + size;
}

void
Flash::lockAll()
{
    for (uint32_t region = 0; region < _lockRegions; region++)
        setLockRegion(region, true);
}

void
Flash::unlockAll()
{
    for (uint32_t region = 0; region < _lockRegions; region++)
        setLockRegion(region, false);
}

void
Flash::loadBuffer(const uint8_t* data)
{
    _samba.write(_onBufferA ? _pageBufferA : _pageBufferB, data, _size);
}
