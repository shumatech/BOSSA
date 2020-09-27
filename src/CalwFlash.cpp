///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2020, ShumaTech
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
#include "CalwFlash.h"

#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#define CALW_KEY        0xa5

#define CALW_FCR        (_regs + 0x00)
#define CALW_FCMD       (_regs + 0x04)
#define CALW_FSR        (_regs + 0x08)
#define CALW_FPR        (_regs + 0x0C)

#define CALW_FCMD_NOP   0x00
#define CALW_FCMD_WP    0x01
#define CALW_FCMD_EP    0x02
#define CALW_FCMD_CPB   0x03
#define CALW_FCMD_LP    0x04
#define CALW_FCMD_UP    0x05
#define CALW_FCMD_EA    0x06
#define CALW_FCMD_WGPB  0x07
#define CALW_FCMD_EGPB  0x08
#define CALW_FCMD_SSB   0x09
#define CALW_FCMD_PGPFB 0x0a
#define CALW_FCMD_EAGPF 0x0b
#define CALW_FCMD_QPR   0x0c
#define CALW_FCMD_WUP   0x0d
#define CALW_FCMD_EUP   0x0e
#define CALW_FCMD_QPRUP 0x0f
#define CALW_FCMD_HSEN  0x10
#define CALW_FCMD_HSDIS 0x11

                                 /* SCS_BASE  + SCB offset + AIRCR offset) */
#define NVIC_SCB_AIRCR            (0xE000E000UL + 0x0D00UL + 0x00CUL)
#define SCB_AIRCR_VECTKEY         0x5FAUL
#define SCB_AIRCR_VECTKEY_Pos     16U
#define SCB_AIRCR_SYSRESETREQ_Pos 2U
#define SCB_AIRCR_SYSRESETREQ_Msk (1UL << SCB_AIRCR_SYSRESETREQ_Pos)
#define SCB_AIRCR_RESET_CPU_VAL   ((SCB_AIRCR_VECTKEY << \
                                    SCB_AIRCR_VECTKEY_Pos) | \
                                  SCB_AIRCR_SYSRESETREQ_Msk)

CalwFlash::CalwFlash(Samba& samba,
                     const std::string& name,
                     uint32_t addr,
                     uint32_t pages,
                     uint32_t size,
                     uint32_t planes,
                     uint32_t lockRegions,
                     uint32_t user,
                     uint32_t stack,
                     uint32_t regs)
    : Flash(samba, name, addr, pages, size, planes, lockRegions, user, stack),
      _regs(regs)
{
    assert(planes == 1);
    assert(pages <= 1024);
    assert(lockRegions <= 16);

    // Force 1WS
    writeFCR(0x40);
}

CalwFlash::~CalwFlash()
{
}

void
CalwFlash::eraseAll(uint32_t offset)
{
    // Do NOT a full chip erase, SAM-BA bootloader resides at offset lower than 0x4000
    if (offset < 0x4000)
    {
	throw FlashOverrideSambaError();
    }
    // Else we must do it by pages
    else
    {
        // Offset must be on an erase page boundary
        if (offset % _size)
            throw FlashEraseError();

        // Erase each PagesPerErase set of pages
        for (uint32_t pageNum = offset / _size; pageNum < _pages; pageNum++)
        {
            waitFSR();
            writeFCMD(CALW_FCMD_EP, pageNum);
        }
    }
}

std::vector<bool>
CalwFlash::getLockRegions()
{
    std::vector<bool> regions(_lockRegions);
    uint32_t locks;

    waitFSR();
    locks = readFSR();
    locks >>= 16;
    for (uint32_t region = 0; region < _lockRegions; region++)
    {
	regions[region] = locks & (1 << region);
    }

    return regions;
}

void
CalwFlash::writePage(uint32_t page)
{
    if (page >= _pages)
        throw FlashPageError();

    waitFSR();
    writeFCMD(CALW_FCMD_CPB, 0);

    _wordCopy.setDstAddr(_addr + (page * _size));
    _wordCopy.setSrcAddr(_onBufferA ? _pageBufferA : _pageBufferB);
    _wordCopy.setWords(_size / sizeof(uint32_t));
    _onBufferA = !_onBufferA;
    waitFSR();
    _wordCopy.runv();
    writeFCMD(CALW_FCMD_WP, page);
}

void
CalwFlash::readPage(uint32_t page, uint8_t* data)
{
    if (page >= _pages)
        throw FlashPageError();

    waitFSR();
    _samba.read(_addr + page * _size, data, _size);
}

void
CalwFlash::resetCPU(void)
{
    printf("Reset CPU\n");

    _samba.writeWord(NVIC_SCB_AIRCR, SCB_AIRCR_RESET_CPU_VAL);
}

void
CalwFlash::waitFSR(int seconds)
{
    int tries = seconds * 1000;
    uint32_t fsr0;

    while (tries-- > 0)
    {
        fsr0 = _samba.readWord(CALW_FSR);
        if (fsr0 & 0x8)
            throw FlashCmdError();
        if (fsr0 & 0x4)
            throw FlashLockError();
        if (fsr0 & 0x1)
            break;
        usleep(1000);
    }
    if (tries == 0)
        throw FlashTimeoutError();
}

void
CalwFlash::writeFCR(uint32_t arg)
{
    _samba.writeWord(CALW_FCR, arg);
}

void
CalwFlash::writeFCMD(uint8_t cmd, uint32_t arg)
{
    _samba.writeWord(CALW_FCMD, (CALW_KEY << 24) | (arg << 8) | cmd);
}

uint32_t
CalwFlash::readFSR()
{
    return _samba.readWord(CALW_FSR);
}
