
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

#include <stdint.h>
#include <memory>
#include <exception>

#include "Samba.h"
#include "WordCopyApplet.h"
#include "NvmFlash.h"

NvmFlash::NvmFlash(Samba& samba,
             const std::string& name,
             uint32_t addr,
             uint32_t pages,
             uint32_t size,
             uint32_t planes,
             uint32_t lockRegions,
             uint32_t user,
             uint32_t stack,
             uint32_t regs,
             bool canBrownout)
    : Flash(samba, name, addr, pages, size, planes, lockRegions, user, stack),
    _regs(regs), _canBrownout(canBrownout), _eraseAuto(true)
{
   ///Upon power up the NVM controller goes through a power up sequence. 
   //During this time, access to the NVM controller is halted. Upon power up the
   //the NVM controller is operational without any need for user configuration.
}

NvmFlash::~NvmFlash() 
{

}

void
NvmFlash::eraseAll()
{

}

void 
NvmFlash::eraseAuto(bool enable)
{
}


bool 
NvmFlash::isLocked()
{
    return false;
}

bool 
NvmFlash::getLockRegion(uint32_t region)
{
    return false;
}

void 
NvmFlash::setLockRegion(uint32_t region, bool enable)
{
}

void 
NvmFlash::lockAll()
{
}

void 
NvmFlash::unlockAll()
{
}

bool 
NvmFlash::getSecurity()
{
    return false;
}


void 
NvmFlash::setSecurity()
{

}

void 
NvmFlash::setBod(bool enable)
{

}

bool 
NvmFlash::getBod()
{
    return false;
}

bool 
NvmFlash::getBor()
{
    return false;
}

void 
NvmFlash::setBor(bool enable)
{

}


bool 
NvmFlash::getBootFlash()
{
    return false;
}

void 
NvmFlash::setBootFlash(bool enable)
{

}

void 
NvmFlash::loadBuffer(const uint8_t* data)
{

}

void 
NvmFlash::writePage(uint32_t page)
{

    if (page >= _pages)
        throw FlashPageError();
}

void 
NvmFlash::readPage(uint32_t page, uint8_t* data)
{

}
