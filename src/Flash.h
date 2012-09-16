///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2012, ShumaTech
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
