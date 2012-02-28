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
#include "EfcFlash.h"

#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#define EFC_KEY         0x5a

#define EFC0_FMR        0xffffff60
#define EFC0_FCR        0xffffff64
#define EFC0_FSR        0xffffff68

#define EFC1_FMR        0xffffff70
#define EFC1_FCR        0xffffff74
#define EFC1_FSR        0xffffff78

#define EFC_FCMD_WP     0x1
#define EFC_FCMD_SLB    0x2
#define EFC_FCMD_WPL    0x3
#define EFC_FCMD_CLB    0x4
#define EFC_FCMD_EA     0x8
#define EFC_FCMD_SGPB   0xb
#define EFC_FCMD_CGPB   0xd
#define EFC_FCMD_SSB    0xf

EfcFlash::EfcFlash(Samba& samba,
                   const std::string& name,
                   uint32_t addr,
                   uint32_t pages,
                   uint32_t size,
                   uint32_t planes,
                   uint32_t lockRegions,
                   uint32_t user,
                   uint32_t stack,
                   bool canBootFlash)
    : Flash(samba, name, addr, pages, size, planes, lockRegions, user, stack),
      _canBootFlash(canBootFlash)
{
    assert(planes == 1 || planes == 2);
    assert(pages <= planes * 1024);
    assert(lockRegions <= 32);

    eraseAuto(true);
}

EfcFlash::~EfcFlash()
{
}

void
EfcFlash::eraseAll()
{
    waitFSR();
    writeFCR0(EFC_FCMD_EA, 0);
    if (_planes == 2)
    {
        waitFSR();
        writeFCR0(EFC_FCMD_EA, _pages / 2);
    }
}

void
EfcFlash::eraseAuto(bool enable)
{
    uint32_t fmr;

    waitFSR();
    fmr = _samba.readWord(EFC0_FMR);
    if (enable)
        fmr &= ~(1 << 7);
    else
        fmr |= (1 << 7);

    _samba.writeWord(EFC0_FMR, fmr);
    if (_planes == 2)
    {
        waitFSR();
        _samba.writeWord(EFC1_FMR, fmr);
    }
}

bool
EfcFlash::isLocked()
{
    if ((readFSR0() & (0xffff << 16)) != 0)
        return true;

    if (_planes == 2)
        if ((readFSR1() & (0xffff << 16)) != 0)
            return true;

    return false;
}

bool
EfcFlash::getLockRegion(uint32_t region)
{
    if (region >= _lockRegions)
        throw FlashRegionError();

    if (_planes == 2 && region >= _lockRegions / 2)
        return (readFSR1() & (1 << (16 + region - _lockRegions / 2)));
    else
        return (readFSR0() & (1 << (16 + region)));
}

void
EfcFlash::setLockRegion(uint32_t region, bool enable)
{
    uint32_t page;

    if (region >= _lockRegions)
        throw FlashRegionError();

    if (enable != getLockRegion(region))
    {
        if (_planes == 2 && region >= _lockRegions / 2)
        {
            page = (region - _lockRegions / 2) * _pages / _lockRegions;
            waitFSR();
            writeFCR1(enable ? EFC_FCMD_SLB : EFC_FCMD_CLB, page);
        }
        else
        {
            page = region * _pages / _lockRegions;
            waitFSR();
            writeFCR0(enable ? EFC_FCMD_SLB : EFC_FCMD_CLB, page);
        }
    }
}

bool
EfcFlash::getSecurity()
{
    return (readFSR0() & (1 << 4));
}

void
EfcFlash::setSecurity()
{
    waitFSR();
    writeFCR0(EFC_FCMD_SSB, 0);
}

bool
EfcFlash::getBod()
{
    return (readFSR0() & (1 << 8));
}

void
EfcFlash::setBod(bool enable)
{
    waitFSR();
    writeFCR0(enable ? EFC_FCMD_SGPB : EFC_FCMD_CGPB, 0);
}

bool
EfcFlash::getBor()
{
    return (readFSR0() & (2 << 8));
}

void
EfcFlash::setBor(bool enable)
{
    waitFSR();
    writeFCR0(enable ? EFC_FCMD_SGPB : EFC_FCMD_CGPB, 1);
}

bool
EfcFlash::getBootFlash()
{
    if (!_canBootFlash)
        return false;

    return (readFSR0() & (1 << 10));
}

void
EfcFlash::setBootFlash(bool enable)
{
    if (!_canBootFlash)
        return;

    waitFSR();
    writeFCR0(enable ? EFC_FCMD_SGPB : EFC_FCMD_CGPB, 2);
}

void
EfcFlash::writePage(uint32_t page)
{
    if (page >= _pages)
        throw FlashPageError();

    _wordCopy.setDstAddr(_addr + page * _size);
    _wordCopy.setSrcAddr(_onBufferA ? _pageBufferA : _pageBufferB);
    _onBufferA = !_onBufferA;
    waitFSR();
    _wordCopy.run();
    if (_planes == 2 && page >= _pages / 2)
        writeFCR1(EFC_FCMD_WP, page - _pages / 2);
    else
        writeFCR0(EFC_FCMD_WP, page);
}

void
EfcFlash::readPage(uint32_t page, uint8_t* data)
{
    if (page >= _pages)
        throw FlashPageError();

    waitFSR();
    _samba.read(_addr + page * _size, data, _size);
}

void
EfcFlash::waitFSR()
{
    uint32_t tries = 0;
    uint32_t fsr0;
    uint32_t fsr1 = 0x1;

    while (++tries <= 500)
    {
        fsr0 = readFSR0();
        if (fsr0 & (1 << 2))
            throw FlashLockError();

        if (_planes == 2)
        {
            fsr1 = readFSR1();
            if (fsr1 & (1 << 2))
                throw FlashLockError();
        }
        if (fsr0 & fsr1 & 0x1)
            break;
        usleep(100);
    }
    if (tries > 500)
        throw FlashCmdError();
}

void
EfcFlash::writeFCR0(uint8_t cmd, uint32_t arg)
{
    _samba.writeWord(EFC0_FCR, (EFC_KEY << 24) | (arg << 8) | cmd);
}

void
EfcFlash::writeFCR1(uint8_t cmd, uint32_t arg)
{
    _samba.writeWord(EFC1_FCR, (EFC_KEY << 24) | (arg << 8) | cmd);
}

uint32_t
EfcFlash::readFSR0()
{
    return _samba.readWord(EFC0_FSR);
}

uint32_t
EfcFlash::readFSR1()
{
    return _samba.readWord(EFC1_FSR);
}
