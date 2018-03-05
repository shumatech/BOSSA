///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2015, Arduino LLC
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

#ifndef _NVMFLASH_H
#define _NVMFLASH_H

#include <stdint.h>
#include <exception>

#include "Flash.h"

class NvmFlashCmdError : public std::exception
{
public:
    NvmFlashCmdError(const char* message) : exception(), _message(message) {};
    const char* what() const throw() { return _message; }
protected:
    const char* _message;
};

class NvmFlash : public Flash
{
public:
    NvmFlash(Samba& samba,
             const std::string& name,
             uint32_t addr,
             uint32_t pages,
             uint32_t size,
             uint32_t planes,
             uint32_t lockRegions,
             uint32_t user,
             uint32_t stack,
             bool canBrownout);

    virtual ~NvmFlash() = 0;

    void eraseAll(uint32_t offset);
    void eraseAuto(bool enable);

    bool isLocked();
    bool getLockRegion(uint32_t region);
    void setLockRegion(uint32_t region, bool enable);

    bool getSecurity();
    void setSecurity();

    bool getBod();
    void setBod(bool enable);
    bool canBod() { return _canBrownout; }

    bool getBor();
    void setBor(bool enable);
    bool canBor() { return _canBrownout; }

    bool getBootFlash();
    void setBootFlash(bool enable);
    bool canBootFlash() { return false; }

    void writePage(uint32_t page);
    void readPage(uint32_t page, uint8_t* data);

    void writeBuffer(uint32_t dst_addr, uint32_t size);

    static const int PagesPerErase;

protected:
    bool     _canBrownout;
    bool     _eraseAuto;

    uint32_t _regBase;
    uint8_t  _regCTRLA;
    uint8_t  _regCTRLB;
    uint8_t  _regINTFLAG;
    uint8_t  _regSTATUS;
    uint8_t  _regADDR;
    uint8_t  _regLOCK;

    uint8_t  _cmdER;
    uint8_t  _cmdWP;
    uint8_t  _cmdLR;
    uint8_t  _cmdUR;
    uint8_t  _cmdSSB;
    uint8_t  _cmdPBC;

    uint32_t readReg(uint8_t reg);
    void writeReg(uint8_t reg, uint32_t value);

    uint32_t getAddressByRegion(uint32_t region);
    bool nvmIsReady();
    void executeNvmCommand(uint8_t cmd);
    void erase(uint32_t offset, uint32_t size);
};

class NvmFlashD2x : public NvmFlash
{
public:
    NvmFlashD2x(Samba& samba,
                const std::string& name,
                uint32_t addr,
                uint32_t pages,
                uint32_t size,
                uint32_t planes,
                uint32_t lockRegions,
                uint32_t user,
                uint32_t stack,
                bool canBrownout);

    virtual ~NvmFlashD2x() {}
};

#endif // _NVMFLASH_H
