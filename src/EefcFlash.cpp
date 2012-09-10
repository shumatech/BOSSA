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
#include "EefcFlash.h"

#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#define EEFC_KEY        0x5a

#define EEFC0_FMR       (_regs + 0x00)
#define EEFC0_FCR       (_regs + 0x04)
#define EEFC0_FSR       (_regs + 0x08)
#define EEFC0_FRR       (_regs + 0x0C)

#define EEFC1_FMR       (_regs + 0x200)
#define EEFC1_FCR       (_regs + 0x204)
#define EEFC1_FSR       (_regs + 0x208)
#define EEFC1_FRR       (_regs + 0x20C)

#define EEFC_FCMD_GETD  0x0
#define EEFC_FCMD_WP    0x1
#define EEFC_FCMD_WPL   0x2
#define EEFC_FCMD_EWP   0x3
#define EEFC_FCMD_EWPL  0x4
#define EEFC_FCMD_EA    0x5
#define EEFC_FCMD_SLB   0x8
#define EEFC_FCMD_CLB   0x9
#define EEFC_FCMD_GLB   0xa
#define EEFC_FCMD_SGPB  0xb
#define EEFC_FCMD_CGPB  0xc
#define EEFC_FCMD_GGPB  0xd

EefcFlash::EefcFlash(Samba& samba,
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
    assert(planes == 1 || planes == 2);
    assert(pages <= 2048);
    assert(lockRegions <= 32);

    // SAM3 Errata (FWS must be 6)
    _samba.writeWord(EEFC0_FMR, 0x6 << 8);
    if (planes == 2)
        _samba.writeWord(EEFC1_FMR, 0x6 << 8);
}

EefcFlash::~EefcFlash()
{
}

void
EefcFlash::eraseAll()
{
    waitFSR();
    writeFCR0(EEFC_FCMD_EA, 0);
    if (_planes == 2)
    {
        waitFSR();
        writeFCR1(EEFC_FCMD_EA, 0);
    }
}

void
EefcFlash::eraseAuto(bool enable)
{
    _eraseAuto = enable;
}

bool
EefcFlash::isLocked()
{
    waitFSR();
    writeFCR0(EEFC_FCMD_GLB, 0);
    waitFSR();
    if (readFRR0())
        return true;
    if (_planes == 2)
    {
        writeFCR1(EEFC_FCMD_GLB, 0);
        waitFSR();
        if (readFRR1())
            return true;
    }

    return false;
}

bool
EefcFlash::getLockRegion(uint32_t region)
{
    if (region >= _lockRegions)
        throw FlashRegionError();

    waitFSR();
    if (_planes == 2 && region >= _lockRegions / 2)
    {
        writeFCR1(EEFC_FCMD_GLB, 0);
        waitFSR();
        if (readFRR1() & (1 << (region - _lockRegions / 2)))
            return true;
    }
    else
    {
        writeFCR0(EEFC_FCMD_GLB, 0);
        waitFSR();
        if (readFRR0() & (1 << region))
            return true;
    }

    return false;
}

void
EefcFlash::setLockRegion(uint32_t region, bool enable)
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
            writeFCR1(enable ? EEFC_FCMD_SLB : EEFC_FCMD_CLB, page);
        }
        else
        {
            page = region * _pages / _lockRegions;
            waitFSR();
            writeFCR0(enable ? EEFC_FCMD_SLB : EEFC_FCMD_CLB, page);
        }
    }
}

bool
EefcFlash::getSecurity()
{
    waitFSR();
    writeFCR0(EEFC_FCMD_GGPB, 0);
    waitFSR();
    return (readFRR0() & (1 << 0));
}

void
EefcFlash::setSecurity()
{
    waitFSR();
    writeFCR0(EEFC_FCMD_SGPB, 0);
}

bool
EefcFlash::getBod()
{
    if (!_canBrownout)
        return false;

    waitFSR();
    writeFCR0(EEFC_FCMD_GGPB, 0);
    waitFSR();
    return (readFRR0() & (1 << 1));
}

void
EefcFlash::setBod(bool enable)
{
    if (!_canBrownout)
        return;

    waitFSR();
    writeFCR0(enable ? EEFC_FCMD_SGPB : EEFC_FCMD_CGPB, 1);
}

bool
EefcFlash::getBor()
{
    if (!_canBrownout)
        return false;

    waitFSR();
    writeFCR0(EEFC_FCMD_GGPB, 0);
    waitFSR();
    return (readFRR0() & (1 << 2));
}

void
EefcFlash::setBor(bool enable)
{
    if (!_canBrownout)
        return;

    waitFSR();
    writeFCR0(enable ? EEFC_FCMD_SGPB : EEFC_FCMD_CGPB, 2);
}

bool
EefcFlash::getBootFlash()
{
    waitFSR();
    writeFCR0(EEFC_FCMD_GGPB, 0);
    waitFSR();
    return (readFRR0() & (1 << (_canBrownout ? 3 : 1)));
}

void
EefcFlash::setBootFlash(bool enable)
{
    waitFSR();
    writeFCR0(enable ? EEFC_FCMD_SGPB : EEFC_FCMD_CGPB, (_canBrownout ? 3 : 1));
    waitFSR();
    usleep(10000);
}

void
EefcFlash::writePage(uint32_t page)
{
    if (page >= _pages)
        throw FlashPageError();

    _wordCopy.setDstAddr(_addr + page * _size);
    _wordCopy.setSrcAddr(_onBufferA ? _pageBufferA : _pageBufferB);
    _onBufferA = !_onBufferA;
    waitFSR();
    _wordCopy.runv();
    if (_planes == 2 && page >= _pages / 2)
        writeFCR1(_eraseAuto ? EEFC_FCMD_EWP : EEFC_FCMD_WP, page - _pages / 2);
    else
        writeFCR0(_eraseAuto ? EEFC_FCMD_EWP : EEFC_FCMD_WP, page);
}

void
EefcFlash::readPage(uint32_t page, uint8_t* data)
{
    if (page >= _pages)
        throw FlashPageError();

    // The SAM3 firmware has a bug where it returns all zeros for reads
    // directly from the flash so instead, we copy the flash page to
    // SRAM and read it from there.
    _wordCopy.setDstAddr(_onBufferA ? _pageBufferA : _pageBufferB);
    _wordCopy.setSrcAddr(_addr + page * _size);
    waitFSR();
    _wordCopy.runv();
    _samba.read(_onBufferA ? _pageBufferA : _pageBufferB, data, _size);
}

void
EefcFlash::waitFSR()
{
    uint32_t tries = 0;
    uint32_t fsr0;
    uint32_t fsr1 = 0x1;

    while (++tries <= 500)
    {
        fsr0 = _samba.readWord(EEFC0_FSR);
        if (fsr0 & (1 << 2))
            throw FlashLockError();

        if (_planes == 2)
        {
            fsr1 = _samba.readWord(EEFC1_FSR);
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
EefcFlash::writeFCR0(uint8_t cmd, uint32_t arg)
{
    _samba.writeWord(EEFC0_FCR, (EEFC_KEY << 24) | (arg << 8) | cmd);
}

void
EefcFlash::writeFCR1(uint8_t cmd, uint32_t arg)
{
    _samba.writeWord(EEFC1_FCR, (EEFC_KEY << 24) | (arg << 8) | cmd);
}

uint32_t
EefcFlash::readFRR0()
{
    return _samba.readWord(EEFC0_FRR);
}

uint32_t
EefcFlash::readFRR1()
{
    return _samba.readWord(EEFC1_FRR);
}
