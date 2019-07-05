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

#ifndef _NULLFLASH_H
#define _NULLFLASH_H

#include <stdint.h>
#include <exception>

#include "Flash.h"

class NullFlash : public Flash
{
public:
    NullFlash(
        Samba& samba,
        const std::string& name,
        uint32_t pages,
        uint32_t size,
        uint32_t user,
        uint32_t stack);

    virtual ~NullFlash();

    void eraseAll(uint32_t offset);
    void eraseAuto(bool enable);

    std::vector<bool> getLockRegions();

    bool getSecurity();

    bool getBod();
    bool canBod() { return false; }

    bool getBor();
    bool canBor() { return false; }

    bool getBootFlash();
    bool canBootFlash() { return false; }

    void writeOptions();

    void writePage(uint32_t page);
    void readPage(uint32_t page, uint8_t* data);

    void writeBuffer(uint32_t dst_addr, uint32_t size);

protected:
    bool     _eraseAuto;
    void erase(uint32_t offset, uint32_t size);
};

#endif // _NULLFLASH_H
