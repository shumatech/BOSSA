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
#ifndef _FLASH_H
#define _FLASH_H

#include <stdint.h>
#include <memory>
#include <exception>

#include "Samba.h"
#include "WordCopyApplet.h"

class FlashPageError : public std::exception
{
public:
    FlashPageError() : exception() {};
    const char* what() const throw() { return "Invalid flash page"; }
};

class FlashRegionError : public std::exception
{
public:
    FlashRegionError() : exception() {};
    const char* what() const throw() { return "Invalid lock region"; }
};

class FlashLockError : public std::exception
{
public:
    FlashLockError() : exception() {};
    const char* what() const throw() { return "Flash page is locked"; }
};

class FlashCmdError : public std::exception
{
public:
    FlashCmdError() : exception() {};
    const char* what() const throw() { return "Flash command failed"; }
};

class Flash
{
public:
    Flash(Samba& samba,
          const std::string& name,
          uint32_t addr,
          uint32_t pages,
          uint32_t size,
          uint32_t planes,
          uint32_t lockRegions,
          uint32_t user,
          uint32_t stack);
    virtual ~Flash() {}

    const std::string& name() { return _name; }

    virtual uint32_t address() { return _addr; }
    virtual uint32_t pageSize() { return _size; }
    virtual uint32_t numPages() { return _pages; }
    virtual uint32_t numPlanes() { return _planes; }

    virtual void eraseAll() = 0;
    virtual void eraseAuto(bool enable) = 0;

    virtual uint32_t lockRegions() { return _lockRegions; }
    virtual bool isLocked() = 0;
    virtual bool getLockRegion(uint32_t region) = 0;
    virtual void setLockRegion(uint32_t region, bool enable) = 0;
    virtual void lockAll();
    virtual void unlockAll();

    virtual bool getSecurity() = 0;
    virtual void setSecurity() = 0;

    virtual bool getBod() = 0;
    virtual void setBod(bool enable) = 0;
    virtual bool canBod() = 0;

    virtual bool getBor() = 0;
    virtual void setBor(bool enable) = 0;
    virtual bool canBor() = 0;

    virtual bool getBootFlash() = 0;
    virtual void setBootFlash(bool enable) = 0;
    virtual bool canBootFlash() = 0;

    virtual void loadBuffer(const uint8_t* data);
    virtual void writePage(uint32_t page) = 0;
    virtual void readPage(uint32_t page, uint8_t* data) = 0;

    typedef std::auto_ptr<Flash> Ptr;

protected:
    Samba& _samba;
    std::string _name;
    uint32_t _addr;
    uint32_t _pages;
    uint32_t _size;
    uint32_t _planes;
    uint32_t _lockRegions;
    uint32_t _user;
    WordCopyApplet _wordCopy;

    bool _onBufferA;
    uint32_t _pageBufferA;
    uint32_t _pageBufferB;
};

#endif // _FLASH_H
