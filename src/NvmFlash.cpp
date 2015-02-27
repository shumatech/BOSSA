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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <iostream>
#include <exception>

#include "Samba.h"
#include "WordCopyApplet.h"
#include "NvmFlash.h"

using namespace std;

// NVM User row in flash, 64 bytes in length
#define NVMCTRL_USER_ROW _user

// System control brown out register.
// SYSCTRL base address + BOD33 reg offset

#define SYSCTRL_BOD33_REG                         (0x40000800ul + 0x34ul)
#define SYSCTRL_STATUS_REG_ENABLE_BIT             (0x2ul)
#define SYSCTRL_STATUS_REG_BOD33_MASK             (0xfffffffdul)
#define SYSCTRL_STATUS_REG_BOD33_RESET_ENABLE_BIT (0x8ul)
#define SYSCTRL_STATUS_REG_BOD33_RESET_MASK       (0xffffffe7ul)

// The base address of the NVM module in
// main memory + offset to the CTRLA register
#define NVM_CTRLA_REG                   (_regs+0x00)

#define NVM_CTRLB_REG                   (_regs+0x04)

// The NVM register that stores lock status
#define NVM_LOCK_REG                    (_regs+0x20)

// The interrupt status register
#define NVM_INT_STATUS_REG              (_regs+0x14)

// NVM ready bit mask
#define NVM_INT_STATUS_READY_MASK       0x1

// NVM input register to some of the CMDEX commands.
#define NVM_ADDR_REG                    (_regs+0x1c)

// NVM STATUS register
#define NVM_STATUS_REG                  (_regs+0x18)
#define NVMCTRL_STATUS_MASK             (0xFFEBu)

// CMDEX field should be 0xA5 to allow execution of any command.
#define CMDEX_KEY             (0xa500u)

// List of NVM Commands.//as per datasheet prefix CMDEX
#define CMD_LOCK_REGION       (CMDEX_KEY | 0x0040u)
#define CMD_UNLOCK_REGION     (CMDEX_KEY | 0x0041u)
#define CMD_ERASE_ROW         (CMDEX_KEY | 0x0002u)
#define CMD_WRITE_PAGE        (CMDEX_KEY | 0x0004u)
#define CMD_SET_SECURITY_BIT  (CMDEX_KEY | 0x0045u)
#define CMD_CLEAR_PAGE_BUFFER (CMDEX_KEY | 0x0044u)

#define ROW_SIZE FOUR_PAGES
// This is the word size allowed by the communication layer of this bossa client, not the NVM word size which is 2 bytes
#define SAMBA_API_WORD_SIZE (4ul) // bytes

// Maximum size of the samba bootloader in any configuration
#define BOOTLOADER_SIZE_IN_BYTES 8192

/* This class is designed specifically for SAM Dxx architecture in mind */
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
      _regs(regs), _canBrownout(canBrownout)
{
    // Upon power up the NVM controller goes through a power up sequence.
    // During this time, access to the NVM controller is halted. Upon power up the
    // the NVM controller is operational without any need for user configuration.
}

NvmFlash::~NvmFlash()
{
}

void
NvmFlash::eraseAll()
{
    // Leave the first 8KB, where bootloader resides, erase the rest.
    // Row is a concept used for convinence. When writing you have to write
    // page(s). When erasing you have to erase row(s).
    uint32_t total_rows = _pages/ROW_SIZE;

    // Calculate the number of rows that samba occupies.
    uint32_t boot_rows = (BOOTLOADER_SIZE_IN_BYTES/pagesize())/ROW_SIZE;

    // Clear error flags

    for (uint32_t row=boot_rows; row<total_rows; row++)
    {
        uint32_t addr_in_flash = _addr + (row * ROW_SIZE * pageSize());
        // The address is byte address, so convert it to word address.
        addr_in_flash = addr_in_flash / 2;

        // Safe wait. Check and see if this is needed all the time
        while (!nvmIsReady())
        {
        }

        // Clear error bits
        uint16_t status_reg = _samba.readWord(NVM_STATUS_REG) & 0xffff;
        _samba.writeWord(NVM_STATUS_REG, status_reg | NVMCTRL_STATUS_MASK);

        // Issue erase command
        _samba.writeWord(NVM_ADDR_REG, addr_in_flash);
        executeNvmCommand(CMD_ERASE_ROW);
    }
}

bool
NvmFlash::nvmIsReady()
{
    uint8_t int_flag = _samba.readByte(NVM_INT_STATUS_REG) & NVM_INT_STATUS_READY_MASK;

    return int_flag == 1;
}

void
NvmFlash::eraseAuto(bool enable)
{
    // Useless for SAMD, the flash controller doesn't have the auto erase function
}

bool
NvmFlash::isLocked()
{
    return getLockRegion(0);
}

// Returns true if locked, false otherwise.
bool
NvmFlash::getLockRegion(uint32_t region)
{
    if (region >= _lockRegions)
    {
        throw FlashRegionError();
    }

    uint32_t value = _samba.readWord(NVM_LOCK_REG);

    return ((value & (1 << region)) == 0); // 0 -> locked, 1 -> unlocked,
}

// Locks a given region number.
void
NvmFlash::setLockRegion(uint32_t region, bool enable)
{
    if (region >= _lockRegions)
    {
        throw FlashRegionError();
    }

    if (enable != getLockRegion(region))
    {
        if (enable)
        {
            // To lock a region you have to pass an address to the
            // ADDR register, and then execute "lock region" cmd
            // on the NVM controller.
            uint32_t addr_to_lock = getAddressByRegion(region);
            // addr_to_lock = addr_to_lock & 0x1fffff;
            _samba.writeWord(NVM_ADDR_REG, addr_to_lock);

            executeNvmCommand(CMD_LOCK_REGION);
        }
        else
        {
            uint32_t addr_to_unlock = getAddressByRegion(region);
            addr_to_unlock = addr_to_unlock & 0x1fffff;
            _samba.writeWord(NVM_ADDR_REG, addr_to_unlock);

            executeNvmCommand(CMD_UNLOCK_REGION);
        }
    }
}


// Read the security bit, returns true if set, false otherwise.
bool
NvmFlash::getSecurity()
{
    uint16_t status_reg_value = _samba.readWord(NVM_STATUS_REG) & 0xffff;

    // If the 8th bit is 1 then security bit is set, else unset.
    return (((status_reg_value >> 8) & 0x1) == 1);
}

// Set's the security bit.
void
NvmFlash::setSecurity()
{
    if (!getSecurity())
    {
        executeNvmCommand(CMD_SET_SECURITY_BIT);

        if (!getSecurity())
        {
            throw NvmFlashCmdError("Error when setting security bit");
        }
    }
}

// Enable/disable the Bod. The values are lost on target reset.
void
NvmFlash::setBod(bool enable)
{
    uint32_t bod33_ctrl_reg = _samba.readWord(SYSCTRL_BOD33_REG);

    if(enable)
    {
        bod33_ctrl_reg |= SYSCTRL_STATUS_REG_ENABLE_BIT; // Enable the bod control
        _samba.writeWord(SYSCTRL_BOD33_REG, bod33_ctrl_reg);
    }
    else
    {
        bod33_ctrl_reg &= SYSCTRL_STATUS_REG_BOD33_MASK;
        _samba.writeWord(SYSCTRL_BOD33_REG, bod33_ctrl_reg);
    }
}

bool
NvmFlash::getBod()
{
    uint32_t value = _samba.readWord(SYSCTRL_BOD33_REG);
    bool bod_set = (((value & SYSCTRL_STATUS_REG_ENABLE_BIT) >> 1) == 0x1); // If Bit 1 of the BOD33 register is 1, then it's enabled

    return bod_set;
}

bool
NvmFlash::getBor()
{
    uint32_t bod33_ctrl_reg = _samba.readWord(SYSCTRL_BOD33_REG);
    bool reset_enabled = ((bod33_ctrl_reg >> 3) & 0x1) == 0x1;

    return reset_enabled;
}

void
NvmFlash::setBor(bool enable)
{
    uint32_t bod33_ctrl_reg = _samba.readWord(SYSCTRL_BOD33_REG);

    if (enable)
    {
        bod33_ctrl_reg |= SYSCTRL_STATUS_REG_BOD33_RESET_ENABLE_BIT; // To enable brown out reset set bit 3.
        _samba.writeWord(SYSCTRL_BOD33_REG, bod33_ctrl_reg);
    }
    else
    {
        bod33_ctrl_reg &= SYSCTRL_STATUS_REG_BOD33_RESET_MASK;
        _samba.writeWord(SYSCTRL_BOD33_REG, bod33_ctrl_reg);
    }
}


bool
NvmFlash::getBootFlash()
{
    // Always boots from flash. No ROM boot available.
    return true;
}

void
NvmFlash::setBootFlash(bool enable)
{
    printf("Ignoring set boot from flash flag.\n");
}

const uint8_t*
NvmFlash::getCompletePage(const uint8_t* buffer, uint16_t size)
{
    uint8_t* page_buf;

    if (size < pageSize()) //If it is a partial page, do padding
    {
        page_buf = (uint8_t*)malloc(pageSize() * sizeof(uint8_t));

        // First copy all the bytes from source
        for (uint16_t m=0; m<size; m++)
        {
            page_buf[m] = buffer[m]; // strncpy or memcpy ?
        }

        // Now pad the remaining.
        for (uint16_t m=size; m<pageSize(); m++)
        {
            page_buf[m] = 255;
        }

        return (const uint8_t*)page_buf;
    }
    else if (size == pageSize())
    {
        return buffer; // Return the source as it is
    }
    else
    {
        throw NvmFlashCmdError(":(");
        // We are unlucky. Grab a beer.
    }
}

void
NvmFlash::setup_page_write()
{
    // Clear page buffer
    executeNvmCommand(CMD_CLEAR_PAGE_BUFFER);

    // Clear error flags in target
    uint16_t status_reg = _samba.readWord(NVM_STATUS_REG) & 0xffff;
    _samba.writeWord(NVM_STATUS_REG, status_reg | NVMCTRL_STATUS_MASK);

    // Configure manual page write. This is critical for the write to work
    // irrespective of full page or partial page.
    uint32_t ctrlb_reg = _samba.readWord(NVM_CTRLB_REG);
    _samba.writeWord(NVM_CTRLB_REG, ctrlb_reg | (0x1 << 7));
}

void
NvmFlash::loadBuffer(const uint8_t* data, uint16_t bufferSize)
{
    _bufferSize = bufferSize;
    _buffer = data;
}

// Reference : Atmel ASF nvm_write_buffer
void
NvmFlash::writePage(uint32_t page)
{
    // loadBuffer is called before this function is called each time.

    if (page >= _pages)
        throw FlashPageError();

    if (_bufferSize > pageSize())
        throw FlashPageError();

    if (!_buffer)
        throw NvmFlashCmdError("The input buffer is not valid");

    setup_page_write();

    // Compute the start address.
    uint32_t addr = _addr + (page * _size );
    uint32_t addr_cached = addr;
    uint32_t start = 0;

    // Get a full page. Sometimes the page size might be less than a page size, in which
    // case we should prepare a complete page by padding 0xff.
    const uint8_t* page_buf = getCompletePage(_buffer, _bufferSize);

    for (uint16_t i=0; i<(pageSize()/SAMBA_API_WORD_SIZE); i++)
    {
        start = i * SAMBA_API_WORD_SIZE;
        uint32_t data = (page_buf[start+3] << 24) |
                        (page_buf[start+2] << 16) |
                        (page_buf[start+1] <<  8) |
                        (page_buf[start]);
        while (!nvmIsReady())
            ;
        _samba.writeWord(addr, data);
        addr = addr+SAMBA_API_WORD_SIZE;
    }
    while (!nvmIsReady())
        ;
    _samba.writeWord(NVM_ADDR_REG,addr_cached >> 1);
    executeNvmCommand(CMD_WRITE_PAGE);
    // Reset the buffer, so that subsequent reads are clear
}

void
NvmFlash::readPage(uint32_t page, uint8_t* buf)
{
    if (page >= _pages)
    {
        throw FlashPageError();
    }

    // Convert page number into physical address.
    // The flash base should be defined as starting after bootloader (ie at 0x00002000 => page number 128)
    // flash_base_address + page.no * page_size
    uint32_t addr = _addr + (page * pageSize());
    _samba.read(addr, buf, pageSize());
}

// Returns the start address of a specified region number
// based on the flash specifications. The returned address is
// word address (not byte address).
uint32_t
NvmFlash::getAddressByRegion(uint32_t region_num)
{
    if (region_num >= _lockRegions)
    {
        throw FlashRegionError();
    }

    uint32_t size_of_region = (pageSize() * numPages()) / _lockRegions; // Flash Size / no of lock regions
    uint32_t addr = address() + (region_num * size_of_region);
    addr = addr / 2; // Convert byte address to word address

    return addr;
}

void
NvmFlash::executeNvmCommand(uint32_t cmd)
{
    // Wait for the nvm controller to be ready
    while (!nvmIsReady())
    {
    }

    // Send the comamnd to nvm controller.
    _samba.writeWord(NVM_CTRLA_REG, cmd);

    // Wait till the operation completes.
    while (!nvmIsReady())
    {
    }

    //TODO : check and return return error status from nvm status flag.
}
