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
#ifndef _EFCFLASH_H
#define _EFCFLASH_H

#include <stdint.h>
#include <exception>

#include "Flash.h"

class EfcFlash : public Flash
{
public:
    EfcFlash(Samba& samba,
             const std::string& name,
             uint32_t addr,
             uint32_t pages,
             uint32_t size,
             uint32_t planes,
             uint32_t lockRegions,
             uint32_t user,
             uint32_t stack,
             bool canBootFlash);
    virtual ~EfcFlash();

    void eraseAll();
    void eraseAuto(bool enable);

    bool isLocked();
    bool getLockRegion(uint32_t region);
    void setLockRegion(uint32_t region, bool enable);

    bool getSecurity();
    void setSecurity();

    bool getBod();
    void setBod(bool enable);
    bool canBod() { return true; }

    bool getBor();
    void setBor(bool enable);
    bool canBor() { return true; }

    bool getBootFlash();
    void setBootFlash(bool enable);
    bool canBootFlash() { return _canBootFlash; }

    void writePage(uint32_t page);
    void readPage(uint32_t page, uint8_t* data);

private:
    bool _canBootFlash;

    void waitFSR();
    void writeFCR0(uint8_t cmd, uint32_t arg);
    void writeFCR1(uint8_t cmd, uint32_t arg);
    uint32_t readFSR0();
    uint32_t readFSR1();
};

#endif // _EFCFLASH_H
