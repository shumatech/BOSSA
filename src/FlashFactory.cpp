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
#include "FlashFactory.h"

#include "Devices.h"
#include "EfcFlash.h"
#include "EefcFlash.h"
#include "NvmFlash.h"

FlashFactory::FlashFactory()
{
}

FlashFactory::~FlashFactory()
{
}

Flash::Ptr
FlashFactory::create(Samba& samba, uint32_t chipId)
{
    Flash* flash;

    switch (chipId)
    {
    //
    // SAMD21
    //
    case ATSAMD21J18A_CHIPID:
        flash = new NvmFlash( samba, ATSAMD21J18A_NAME, ATSAMD21J18A_FLASH_BASE, ATSAMD21J18A_FLASH_PAGES, ATSAMD21J18A_FLASH_PAGE_SIZE,
                              ATSAMD21J18A_FLASH_PLANES, ATSAMD21J18A_FLASH_LOCK_REGIONS,
                              ATSAMD21J18A_BUFFER_ADDR, ATSAMD21J18A_STACK_ADDR, ATSAMD21J18A_NVMCTRL_BASE, /*canBrownout*/true ) ;
        break ;

    case ATSAMD21G18A_CHIPID:
        flash = new NvmFlash( samba, ATSAMD21G18A_NAME, ATSAMD21G18A_FLASH_BASE, ATSAMD21G18A_FLASH_PAGES, ATSAMD21G18A_FLASH_PAGE_SIZE,
                              ATSAMD21G18A_FLASH_PLANES, ATSAMD21G18A_FLASH_LOCK_REGIONS,
                              ATSAMD21G18A_BUFFER_ADDR, ATSAMD21G18A_STACK_ADDR, ATSAMD21G18A_NVMCTRL_BASE, /*canBrownout*/true ) ;
        break ;

    //
    // SAM7SE
    //
    case 0x272a0a40:
        flash = new EfcFlash(samba, "AT91SAM7SE512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x208000, true);
        break;
    case 0x272a0940:
        flash = new EfcFlash(samba, "AT91SAM7SE256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x208000, true);
        break;
    case 0x272a0340:
        flash = new EfcFlash(samba, "AT91SAM7SE32", 0x100000, 256, 128, 1, 8, 0x201400, 0x201C00, true);
        break;
    //
    // SAM7S
    //
    case 0x270b0a40:
        flash = new EfcFlash(samba, "AT91SAM7S512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x210000, false);
        break;
    case 0x270d0940: // A
    case 0x270b0940: // B/C
        flash = new EfcFlash(samba, "AT91SAM7S256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x210000, false);
        break;
    case 0x270c0740: // A
    case 0x270a0740: // B/C
        flash = new EfcFlash(samba, "AT91SAM7S128", 0x100000, 512, 256, 1, 8, 0x202000, 0x208000, false);
        break;
    case 0x27090540:
        flash = new EfcFlash(samba, "AT91SAM7S64", 0x100000, 512, 128, 1, 16, 0x202000, 0x204000, false);
        break;
    case 0x27080340:
        flash = new EfcFlash(samba, "AT91SAM7S32", 0x100000, 256, 128, 1, 8, 0x201400, 0x202000, false);
        break;
    case 0x27050240:
        flash = new EfcFlash(samba, "AT91SAM7S16", 0x100000, 256, 64, 1, 8, 0x200000, 0x200e00, false);
        break;
    //
    // SAM7XC
    //
    case 0x271c0a40:
        flash = new EfcFlash(samba, "AT91SAMXC512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x220000, true);
        break;
    case 0x271b0940:
        flash = new EfcFlash(samba, "AT91SAMXC256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x210000, true);
        break;
    case 0x271a0740:
        flash = new EfcFlash(samba, "AT91SAMXC128", 0x100000, 512, 256, 1, 8, 0x202000, 0x208000, true);
        break;
    //
    // SAM7X
    //
    case 0x275c0a40:
        flash = new EfcFlash(samba, "AT91SAMX512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x220000, true);
        break;
    case 0x275b0940:
        flash = new EfcFlash(samba, "AT91SAMX256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x210000, true);
        break;
    case 0x275a0740:
        flash = new EfcFlash(samba, "AT91SAMX128", 0x100000, 512, 256, 1, 8, 0x202000, 0x208000, true);
        break;
    //
    // SAM4S
    //
    case 0x288c0ce0 : // A
    case 0x289c0ce0 : // B
    case 0x28ac0ce0 : // C
        flash = new EefcFlash(samba, "ATSAM4S16", 0x400000, 2048, 512, 1, 128, 0x20001000, 0x20020000, 0x400e0a00, false);
        break;
    case 0x288c0ae0 : // A
    case 0x289c0ae0 : // B
    case 0x28ac0ae0 : // C
        flash = new EefcFlash(samba, "ATSAM4S8", 0x400000, 1024, 512, 1, 64, 0x20001000, 0x20020000, 0x400e0a00, false);
        break;
    //
    // SAM3N
    //
    case 0x29340960 : // A
    case 0x29440960 : // B
    case 0x29540960 : // C
        flash = new EefcFlash(samba, "ATSAM3N4", 0x400000, 1024, 256, 1, 16, 0x20001000, 0x20006000, 0x400e0a00, false);
        break;
    case 0x29390760 : // A
    case 0x29490760 : // B
    case 0x29590760 : // C
        flash = new EefcFlash(samba, "ATSAM3N2", 0x400000, 512, 256, 1, 8, 0x20001000, 0x20004000, 0x400e0a00, false);
        break;
    case 0x29380560 : // A
    case 0x29480560 : // B
    case 0x29580560 : // C
        flash = new EefcFlash(samba, "ATSAM3N1", 0x400000, 256, 256, 1, 4, 0x20000800, 0x20002000, 0x400e0a00, false);
        break;
    //
    // SAM3S
    //
    case 0x28800960 : // A
    case 0x28900960 : // B
    case 0x28a00960 : // C
        flash = new EefcFlash(samba, "ATSAM3S4", 0x400000, 1024, 256, 1, 16, 0x20001000, 0x2000c000, 0x400e0a00, false);
        break;
    case 0x288a0760 : // A
    case 0x289a0760 : // B
    case 0x28aa0760 : // C
        flash = new EefcFlash(samba, "ATSAM3S2", 0x400000, 512, 256, 1, 8, 0x20000800, 0x20008000, 0x400e0a00, false);
        break;
    case 0x288a0560 : // A
    case 0x289a0560 : // B
    case 0x28aa0560 : // C
        flash = new EefcFlash(samba, "ATSAM3S1", 0x400000, 256, 256, 1, 4, 0x20000800, 0x20004000, 0x400e0a00, false);
        break;
    //
    // SAM3U
    //
    case 0x28000960 : // C
    case 0x28100960 : // E
        flash = new EefcFlash(samba, "ATSAM3U4", 0xE0000, 1024, 256, 2, 32, 0x20001000, 0x20008000, 0x400e0800, false);
        break;
    case 0x280a0760 : // C
    case 0x281a0760 : // E
        flash = new EefcFlash(samba, "ATSAM3U2", 0x80000, 512, 256, 1, 16, 0x20001000, 0x20004000, 0x400e0800, false);
        break;
    case 0x28090560 : // C
    case 0x28190560 : // E
        flash = new EefcFlash(samba, "ATSAM3U1", 0x80000, 256, 256, 1, 8, 0x20001000, 0x20002000, 0x400e0800, false);
        break;
    //
    // SAM3X
    //
    case 0x286e0a60 : // 8H
    case 0x285e0a60 : // 8E
    case 0x284e0a60 : // 8C
        flash = new EefcFlash(samba, "ATSAM3X8", 0x80000, 2048, 256, 2, 32, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x285b0960 : // 4E
    case 0x284b0960 : // 4C
        flash = new EefcFlash(samba, "ATSAM3X4", 0x80000, 1024, 256, 2, 16, 0x20001000, 0x20008000, 0x400e0a00, false);
        break;
    //
    // SAM3A
    //
    case 0x283e0A60 : // 8C
        flash = new EefcFlash(samba, "ATSAM3A8", 0x80000, 2048, 256, 2, 32, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x283b0960 : // 4C
        flash = new EefcFlash(samba, "ATSAM3A4", 0x80000, 1024, 256, 2, 16, 0x20001000, 0x20008000, 0x400e0a00, false);
        break;
    //
    // SAM7L
    //
    case 0x27330740 :
        flash = new EefcFlash(samba, "ATSAM7L128", 0x100000, 512, 256, 1, 16, 0x2ffb40, 0x300700, 0xffffff60, false);
        break;
    case 0x27330540 :
        flash = new EefcFlash(samba, "ATSAM7L64", 0x100000, 256, 256, 1, 8, 0x2ffb40, 0x300700, 0xffffff60, false);
        break;
    //
    // SAM9XE
    //
    case 0x329aa3a0 :
        flash = new EefcFlash(samba, "ATSAM9XE512", 0x200000, 1024, 512, 1, 32, 0x300000, 0x307000, 0xfffffa00, true);
        break;
    case 0x329a93a0 :
        flash = new EefcFlash(samba, "ATSAM9XE256", 0x200000, 512, 512, 1, 16, 0x300000, 0x307000, 0xfffffa00, true);
        break;
    case 0x329973a0 :
        flash = new EefcFlash(samba, "ATSAM9XE128", 0x200000, 256, 512, 1, 8, 0x300000, 0x303000, 0xfffffa00, true);
        break;

    default:
        flash = NULL;
        break;
    }

    return Flash::Ptr(flash);
}

