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
