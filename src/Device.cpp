///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2017, ShumaTech
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
#include "Device.h"
#include "EfcFlash.h"
#include "EefcFlash.h"
#include "NvmFlash.h"

void
Device::create()
{
    Flash* flashPtr;
    uint32_t chipId = 0;
    uint32_t extChipId = 0;
    uint32_t deviceId = 0;

    // Device identification must be performed carefully to avoid reading from
    // addresses that devices do not support.

    // All devices support addresss 0 as the ARM reset vector so if the vector is
    // a ARM7TDMI branch, then assume we have an Atmel SAM7/9 CHIPID register
    if ((_samba.readWord(0x0) & 0xff000000) == 0xea000000)
    {
        chipId = _samba.readWord(0xfffff240);
    }
    // Next try the ARM CPUID register since all Coretex-M devices support it.
    // If it identifies a Coretex M0+, then assume we have a SAMD device
    // that only supports the ARM device ID register
    else if ((_samba.readWord(0xe000ed00) & 0x0000fff0) == 0xC600)
    {
        deviceId = _samba.readWord(0x41002018);
    }
    // Assume we have a SAM3, SAM4 or SAME70 so check the CHIPID registers
    else if ((chipId = _samba.readWord(0x400e0740)) != 0)
    {
        extChipId = _samba.readWord(0x400e0744);
    }
    else if ((chipId = _samba.readWord(0x400e0940)) != 0)
    {
        extChipId = _samba.readWord(0x400e0944);
    }
    // Else we don't know what the device is
    else
    {
        throw DeviceUnsupportedError();
    }

    // Instantiate the proper flash for the device
    switch (chipId & 0x7fffffe0)
    {
    //
    // SAM7SE
    //
    case 0x272a0a40:
        _family = FAMILY_SAM7SE;
        flashPtr = new EfcFlash(_samba, "AT91SAM7SE512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x208000, true);
        break;
    case 0x272a0940:
        _family = FAMILY_SAM7SE;
        flashPtr = new EfcFlash(_samba, "AT91SAM7SE256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x208000, true);
        break;
    case 0x272a0340:
        _family = FAMILY_SAM7SE;
        flashPtr = new EfcFlash(_samba, "AT91SAM7SE32", 0x100000, 256, 128, 1, 8, 0x201400, 0x201C00, true);
        break;
    //
    // SAM7S
    //
    case 0x270b0a40:
        _family = FAMILY_SAM7S;
        flashPtr = new EfcFlash(_samba, "AT91SAM7S512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x210000, false);
        break;
    case 0x270d0940: // A
    case 0x270b0940: // B/C
        _family = FAMILY_SAM7S;
        flashPtr = new EfcFlash(_samba, "AT91SAM7S256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x210000, false);
        break;
    case 0x270c0740: // A
    case 0x270a0740: // B/C
        _family = FAMILY_SAM7S;
        flashPtr = new EfcFlash(_samba, "AT91SAM7S128", 0x100000, 512, 256, 1, 8, 0x202000, 0x208000, false);
        break;
    case 0x27090540:
        _family = FAMILY_SAM7S;
        flashPtr = new EfcFlash(_samba, "AT91SAM7S64", 0x100000, 512, 128, 1, 16, 0x202000, 0x204000, false);
        break;
    case 0x27080340:
        _family = FAMILY_SAM7S;
        flashPtr = new EfcFlash(_samba, "AT91SAM7S32", 0x100000, 256, 128, 1, 8, 0x201400, 0x202000, false);
        break;
    case 0x27050240:
        _family = FAMILY_SAM7S;
        flashPtr = new EfcFlash(_samba, "AT91SAM7S16", 0x100000, 256, 64, 1, 8, 0x200000, 0x200e00, false);
        break;
    //
    // SAM7XC
    //
    case 0x271c0a40:
        _family = FAMILY_SAM7XC;
        flashPtr = new EfcFlash(_samba, "AT91SAMXC512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x220000, true);
        break;
    case 0x271b0940:
        _family = FAMILY_SAM7XC;
        flashPtr = new EfcFlash(_samba, "AT91SAMXC256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x210000, true);
        break;
    case 0x271a0740:
        _family = FAMILY_SAM7XC;
        flashPtr = new EfcFlash(_samba, "AT91SAMXC128", 0x100000, 512, 256, 1, 8, 0x202000, 0x208000, true);
        break;
    //
    // SAM7X
    //
    case 0x275c0a40:
        _family = FAMILY_SAM7X;
        flashPtr = new EfcFlash(_samba, "AT91SAMX512", 0x100000, 2048, 256, 2, 32, 0x202000, 0x220000, true);
        break;
    case 0x275b0940:
        _family = FAMILY_SAM7X;
        flashPtr = new EfcFlash(_samba, "AT91SAMX256", 0x100000, 1024, 256, 1, 16, 0x202000, 0x210000, true);
        break;
    case 0x275a0740:
        _family = FAMILY_SAM7X;
        flashPtr = new EfcFlash(_samba, "AT91SAMX128", 0x100000, 512, 256, 1, 8, 0x202000, 0x208000, true);
        break;
    //
    // SAM4S
    //
    case 0x288c0ce0 : // A
    case 0x289c0ce0 : // B
    case 0x28ac0ce0 : // C
        _family = FAMILY_SAM4S;
        flashPtr = new EefcFlash(_samba, "ATSAM4S16", 0x400000, 2048, 512, 1, 128, 0x20001000, 0x20020000, 0x400e0a00, false);
        break;
    case 0x288c0ae0 : // A
    case 0x289c0ae0 : // B
    case 0x28ac0ae0 : // C
        _family = FAMILY_SAM4S;
        flashPtr = new EefcFlash(_samba, "ATSAM4S8", 0x400000, 1024, 512, 1, 64, 0x20001000, 0x20020000, 0x400e0a00, false);
        break;
    case 0x288b09e0 : // A
    case 0x289b09e0 : // B
    case 0x28ab09e0 : // C
        _family = FAMILY_SAM4S;
        flashPtr = new EefcFlash(_samba, "ATSAM4S4", 0x400000, 512, 512, 1, 16, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x288b07e0 : // A
    case 0x289b07e0 : // B
    case 0x28ab07e0 : // C
        _family = FAMILY_SAM4S;
        flashPtr = new EefcFlash(_samba, "ATSAM4S2", 0x400000, 256, 512, 1, 16, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    //
    // SAM3N
    //
    case 0x29340960 : // A
    case 0x29440960 : // B
    case 0x29540960 : // C
        _family = FAMILY_SAM3N;
        flashPtr = new EefcFlash(_samba, "ATSAM3N4", 0x400000, 1024, 256, 1, 16, 0x20001000, 0x20006000, 0x400e0a00, false);
        break;
    case 0x29390760 : // A
    case 0x29490760 : // B
    case 0x29590760 : // C
        _family = FAMILY_SAM3N;
        flashPtr = new EefcFlash(_samba, "ATSAM3N2", 0x400000, 512, 256, 1, 8, 0x20001000, 0x20004000, 0x400e0a00, false);
        break;
    case 0x29380560 : // A
    case 0x29480560 : // B
    case 0x29580560 : // C
        _family = FAMILY_SAM3N;
        flashPtr = new EefcFlash(_samba, "ATSAM3N1", 0x400000, 256, 256, 1, 4, 0x20000800, 0x20002000, 0x400e0a00, false);
        break;
    case 0x29380360 : // A
    case 0x29480360 : // B
    case 0x29580360 : // C
        _family = FAMILY_SAM3N;
        flashPtr = new EefcFlash(_samba, "ATSAM3N0", 0x400000, 128, 256, 1, 1, 0x20000800, 0x20002000, 0x400e0a00, false);
        break;
    //
    // SAM3S
    //
    case 0x299b0a60 : // B
    case 0x29ab0a60 : // C
        _family = FAMILY_SAM3S;
        flashPtr = new EefcFlash(_samba, "ATSAM3SD8", 0x400000, 2048, 256, 1, 16, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x289b0a60 : // B
    case 0x28ab0a60 : // C
        _family = FAMILY_SAM3S;
        flashPtr = new EefcFlash(_samba, "ATSAM3S8", 0x400000, 2048, 256, 1, 16, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x28800960 : // A
    case 0x28900960 : // B
    case 0x28a00960 : // C
        _family = FAMILY_SAM3S;
        flashPtr = new EefcFlash(_samba, "ATSAM3S4", 0x400000, 1024, 256, 1, 16, 0x20001000, 0x2000c000, 0x400e0a00, false);
        break;
    case 0x288a0760 : // A
    case 0x289a0760 : // B
    case 0x28aa0760 : // C
        _family = FAMILY_SAM3S;
        flashPtr = new EefcFlash(_samba, "ATSAM3S2", 0x400000, 512, 256, 1, 8, 0x20000800, 0x20008000, 0x400e0a00, false);
        break;
    case 0x288a0560 : // A
    case 0x289a0560 : // B
    case 0x28aa0560 : // C
        _family = FAMILY_SAM3S;
        flashPtr = new EefcFlash(_samba, "ATSAM3S1", 0x400000, 256, 256, 1, 4, 0x20000800, 0x20004000, 0x400e0a00, false);
        break;
    //
    // SAM3U
    //
    case 0x28000960 : // C
    case 0x28100960 : // E
        _family = FAMILY_SAM3U;
        flashPtr = new EefcFlash(_samba, "ATSAM3U4", 0xE0000, 1024, 256, 2, 32, 0x20001000, 0x20008000, 0x400e0800, false);
        break;
    case 0x280a0760 : // C
    case 0x281a0760 : // E
        _family = FAMILY_SAM3U;
        flashPtr = new EefcFlash(_samba, "ATSAM3U2", 0x80000, 512, 256, 1, 16, 0x20001000, 0x20004000, 0x400e0800, false);
        break;
    case 0x28090560 : // C
    case 0x28190560 : // E
        _family = FAMILY_SAM3U;
        flashPtr = new EefcFlash(_samba, "ATSAM3U1", 0x80000, 256, 256, 1, 8, 0x20001000, 0x20002000, 0x400e0800, false);
        break;
    //
    // SAM3X
    //
    case 0x286e0a60 : // 8H
    case 0x285e0a60 : // 8E
    case 0x284e0a60 : // 8C
        _family = FAMILY_SAM3X;
        flashPtr = new EefcFlash(_samba, "ATSAM3X8", 0x80000, 2048, 256, 2, 32, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x285b0960 : // 4E
    case 0x284b0960 : // 4C
        _family = FAMILY_SAM3X;
        flashPtr = new EefcFlash(_samba, "ATSAM3X4", 0x80000, 1024, 256, 2, 16, 0x20001000, 0x20008000, 0x400e0a00, false);
        break;
    //
    // SAM3A
    //
    case 0x283e0A60 : // 8C
        _family = FAMILY_SAM3A;
        flashPtr = new EefcFlash(_samba, "ATSAM3A8", 0x80000, 2048, 256, 2, 32, 0x20001000, 0x20010000, 0x400e0a00, false);
        break;
    case 0x283b0960 : // 4C
        _family = FAMILY_SAM3A;
        flashPtr = new EefcFlash(_samba, "ATSAM3A4", 0x80000, 1024, 256, 2, 16, 0x20001000, 0x20008000, 0x400e0a00, false);
        break;
    //
    // SAM7L
    //
    case 0x27330740 :
        _family = FAMILY_SAM7L;
        flashPtr = new EefcFlash(_samba, "ATSAM7L128", 0x100000, 512, 256, 1, 16, 0x2ffb40, 0x300700, 0xffffff60, false);
        break;
    case 0x27330540 :
        _family = FAMILY_SAM7L;
        flashPtr = new EefcFlash(_samba, "ATSAM7L64", 0x100000, 256, 256, 1, 8, 0x2ffb40, 0x300700, 0xffffff60, false);
        break;
    //
    // SAM9XE
    //
    case 0x329aa3a0 :
        _family = FAMILY_SAM9XE;
        flashPtr = new EefcFlash(_samba, "ATSAM9XE512", 0x200000, 1024, 512, 1, 32, 0x300000, 0x307000, 0xfffffa00, true);
        break;
    case 0x329a93a0 :
        _family = FAMILY_SAM9XE;
        flashPtr = new EefcFlash(_samba, "ATSAM9XE256", 0x200000, 512, 512, 1, 16, 0x300000, 0x307000, 0xfffffa00, true);
        break;
    case 0x329973a0 :
        _family = FAMILY_SAM9XE;
        flashPtr = new EefcFlash(_samba, "ATSAM9XE128", 0x200000, 256, 512, 1, 8, 0x300000, 0x303000, 0xfffffa00, true);
        break;
    //
    // SAM4E
    //
    case 0x23cc0ce0:
        switch (extChipId)
        {
        case 0x00120200: // E
        case 0x00120201: // C
            _family = FAMILY_SAM4E;
            flashPtr = new EefcFlash(_samba, "ATSAM4E16", 0x400000, 2048, 512, 1, 128, 0x20001000, 0x20020000, 0x400e0a00, false);
            break;
        case 0x00120208: // E
        case 0x00120209: // C
            _family = FAMILY_SAM4E;
            flashPtr = new EefcFlash(_samba, "ATSAM4E8", 0x400000, 1024, 512, 1, 64, 0x20001000, 0x20020000, 0x400e0a00, false);
            break;
        }
        break;
    //
    // SAME70
    //
    case 0x21020a00:
        _family = FAMILY_SAME70;
        flashPtr = new EefcFlash(_samba, "ATSAME70x19", 0x400000, 1024, 512, 1, 32, 0x20401000, 0x20401040, 0x400e0c00, false);
        break;
    case 0x21020c00:
        _family = FAMILY_SAME70;
        flashPtr = new EefcFlash(_samba, "ATSAME70x20", 0x400000, 2048, 512, 1, 64, 0x20401000, 0x20401040, 0x400e0c00, false);
        break;
    case 0x21020e00:
        _family = FAMILY_SAME70;
        flashPtr = new EefcFlash(_samba, "ATSAME70x21", 0x400000, 4096, 512, 1, 128, 0x20401000, 0x20401040, 0x400e0c00, false);
        break;
    //
    // No CHIPID devices
    //
    case 0:
        switch (deviceId & 0xffff00ff)
        {
        //
        // SAMD21
        //
        case 0x10010003: // J15A
        case 0x10010008: // G15A
        case 0x1001000d: // E15A
        case 0x10010021: // J15B
        case 0x10010024: // G15B
        case 0x10010027: // E15B
        case 0x10010056: // E15B WLCSP
        case 0x10010063: // E15C WLCSP
            _family = FAMILY_SAMD21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMD21x15", 0x0, 512, 64, 1, 16, 0x20000800, 0x20001000, true) ;
            break;

        case 0x10010002: // J16A
        case 0x10010007: // G16A
        case 0x1001000c: // E16A
        case 0x10010020: // J16B
        case 0x10010023: // G16B
        case 0x10010026: // E16B
        case 0x10010055: // E16B WLCSP
        case 0x10010062: // E16C WLCSP
            _family = FAMILY_SAMD21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMD21x16", 0x0, 1024, 64, 1, 16, 0x20001000, 0x20002000, true) ;
            break;

        case 0x10010001: // J17A
        case 0x10010006: // G17A
        case 0x1001000b: // E17A
        case 0x10010010: // G17A WLCSP
            _family = FAMILY_SAMD21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMD21x17", 0x0, 2048, 64, 1, 16, 0x20002000, 0x20004000, true) ;
            break;

        case 0x10010000: // J18A
        case 0x10010005: // G18A
        case 0x1001000a: // E18A
        case 0x1001000f: // G18A WLCSP
            _family = FAMILY_SAMD21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMD21x18", 0x0, 4096, 64, 1, 16, 0x20004000, 0x20008000, true) ;
            break;

        //
        // SAMR21
        //
        case 0x1001001e: // E16A
        case 0x1001001b: // G16A
            _family = FAMILY_SAMR21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMR21x16", 0x0, 1024, 64, 1, 16, 0x20001000, 0x20002000, true) ;
            break;

        case 0x1001001d: // E17A
        case 0x1001001a: // G17A
            _family = FAMILY_SAMR21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMR21x17", 0x0, 2048, 64, 1, 16, 0x20002000, 0x20004000, true) ;
            break;

        case 0x1001001c: // E18A
        case 0x10010019: // G18A
            _family = FAMILY_SAMR21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMR21x18", 0x0, 4096, 64, 1, 16, 0x20004000, 0x20008000, true) ;
            break;

        case 0x10010018: // E19A
            _family = FAMILY_SAMR21;
            flashPtr = new NvmFlashD2x(_samba, "ATSAMR21x19", 0x0, 4096, 64, 1, 16, 0x20004000, 0x20008000, true) ;
            break;

        default:
            throw DeviceUnsupportedError();
            break;
        }
        break;

    //
    // Unsupported device
    //
    default:
        throw DeviceUnsupportedError();
        break;
    }

    _flash = std::unique_ptr<Flash>(flashPtr);
}

void
Device::reset(void)
{
    switch (_family)
    {
    case FAMILY_SAMD21:
    case FAMILY_SAMR21:
        _samba.writeWord(0xE000ED0C, 0x05FA0004);
        break;

    case FAMILY_SAM3X:
    case FAMILY_SAM3S:
    case FAMILY_SAM3A:
        _samba.writeWord(0x400E1A00, 0xA500000D);
        break;

    case FAMILY_SAM3U:
        _samba.writeWord(0x400E1200, 0xA500000D);
        break;

    case FAMILY_SAM3N:
    case FAMILY_SAM4S:
        _samba.writeWord(0x400E1400, 0xA500000D);
        break;

    case FAMILY_SAM4E:
        _samba.writeWord(0x400E1800, 0xA500000D);
        break;

    case FAMILY_SAM7S:
    case FAMILY_SAM7SE:
    case FAMILY_SAM7X:
    case FAMILY_SAM7XC:
    case FAMILY_SAM7L:
    case FAMILY_SAM9XE:
        _samba.writeWord(0xFFFFFD00, 0xA500000D);
        break;

    default:
        break;
    }
}


