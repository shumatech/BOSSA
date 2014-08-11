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

#include <stdint.h>
#include <malloc.h>
#include <memory>
#include <iostream>
#include <exception>

#include "Samba.h"
#include "WordCopyApplet.h"
#include "NvmFlash.h"

using namespace std;


//NVM User row in flash, 64 bytes in length
#define NVMCTRL_USER_ROW _user

//System control brown out register.
//SYSCTRL base address + BOD33 reg offset

#define SYSCTRL_BOD33_REG (0x40000800 + 0x34) 
#define SYSCTRL_STATUS_REG_ENABLE_BIT 0x2
#define SYSCTRL_STATUS_REG_BOD33_MASK 0xfffffffd;
#define SYSCTRL_STATUS_REG_BOD33_RESET_ENABLE_BIT 0x8
#define SYSCTRL_STATUS_REG_BOD33_RESET_MASK 0xffffffe7

//The _regs parameter to this class is the module base address.
//redefined here with a more appropriate name
#define MODULE_BASE_ADDR _regs

//The base address of the NVM module in 
//main memory + offset to the CTRLA register
#define NVM_CTRLA_REG (MODULE_BASE_ADDR+0x00)

#define NVM_CTRLB_REG (MODULE_BASE_ADDR+0x04)

//The NVM register that stores lock status
#define NVM_LOCK_REG (MODULE_BASE_ADDR+0x20)


//The interrupt status register
#define NVM_INT_STATUS_REG (MODULE_BASE_ADDR+0x14) 

//NVM ready bit mask
#define NVM_INT_STATUS_READY_MASK 0x1

//NVM input register to some of the CMDEX commands.
#define NVM_ADDR_REG (MODULE_BASE_ADDR+0x1c)

//NVM STATUS register
#define NVM_STATUS_REG (MODULE_BASE_ADDR+0x18)
#define NVMCTRL_STATUS_MASK 0xffeb

//CMDEX should be 0xA5 to execute any command 
//on the NVM controller's APB bus.
#define CMDEX 0xa500   

//List of NVM Commands.//as per datasheet prefix CMDEX
#define CMD_LOCK_REGION   (CMDEX | 0x0040)
#define CMD_UNLOCK_REGION (CMDEX | 0x0041)
#define CMD_ERASE_ROW     (CMDEX | 0x0002)
#define CMD_WRITE_PAGE    (CMDEX | 0x0004)
#define CMD_SET_SECURITY_BIT (CMDEX | 0x0045)
#define CMD_CLEAR_PAGE_BUFFER (CMDEX | 0x0044)


//Just for readability
#define FOUR_PAGES 4
//This is the word size allowed by the commu. layer of this bossa client. Not the NVM word size which is 2 bytes
#define SAMBA_API_WORD_SIZE 4 //bytes
#define ROW_SIZE FOUR_PAGES 
#define PAGE_SIZE_IN_BYTES pageSize()

//Maximum size of the samba bootloader in any configuration
#define BOOTLOADER_SIZE_IN_BYTES 8192 


/* This class is designed specifically for M0+ architecture in mind */
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
	_regs(regs), _canBrownout(canBrownout), _eraseAuto(true)
{
	///Upon power up the NVM controller goes through a power up sequence. 
	//During this time, access to the NVM controller is halted. Upon power up the
	//the NVM controller is operational without any need for user configuration.
}

NvmFlash::~NvmFlash() 
{

}

	void
NvmFlash::eraseAll()
{
	//Leave the first 8KB, where samba resides, erase the rest.
	//Row is a concept used for convinence. When writing you have to write 
	//page(s). When erasing you have to erase row(s).
	uint32_t total_rows = _pages/ROW_SIZE;

	//Calculate the number of rows that samba occupies.
	uint32_t boot_rows = (BOOTLOADER_SIZE_IN_BYTES/PAGE_SIZE_IN_BYTES)/ROW_SIZE;

	//clear error flags

	for(uint32_t row=boot_rows;row<total_rows;row++)
	{
		uint32_t addr_in_flash = _addr + (row * ROW_SIZE * PAGE_SIZE_IN_BYTES);
		// the address is byte address, so convert it to word address.
		addr_in_flash = addr_in_flash / 2;

		//safe wait. Check and see if this is needed all the time
		while(!nvm_is_ready())
		{
			std::cout<<endl<<"Waiting ..... ";
		}

		//clear error bits
		uint16_t status_reg = _samba.readWord(NVM_STATUS_REG) & 0xffff;
		_samba.writeWord(NVM_STATUS_REG, status_reg | NVMCTRL_STATUS_MASK);

		//issue erase command
		_samba.writeWord(NVM_ADDR_REG, addr_in_flash);
		execute_nvm_command(CMD_ERASE_ROW);
	}
}


	bool
NvmFlash::nvm_is_ready()
{
	uint8_t int_flag = _samba.readByte(NVM_INT_STATUS_REG)
		& NVM_INT_STATUS_READY_MASK;

	return int_flag == 1;
}

//Practically we are not using this 
	void 
NvmFlash::eraseAuto(bool enable)
{
	_eraseAuto = enable;
}


	bool 
NvmFlash::isLocked()
{
	return getLockRegion(0);
}

///Returns true if locked, false otherwise.
	bool 
NvmFlash::getLockRegion(uint32_t region)
{
	if(region >= _lockRegions)
		throw FlashRegionError();

	uint32_t value = _samba.readWord(NVM_LOCK_REG); 
	return ((value & (1 << region)) == 0); // 0 -> locked, 1 -> unlocked, 
}

//Locks a given region number.
	void 
NvmFlash::setLockRegion(uint32_t region, bool enable)
{
	if(region >= _lockRegions)
		throw FlashRegionError();

	if(enable != getLockRegion(region))
	{
		if(enable)
		{
			//To lock a region you have to pass an address to the
			//ADDR register, and then execute "lock region" cmd 
			//on the NVM controller.
			uint32_t addr_to_lock = getAddressByRegion(region);
			//addr_to_lock = addr_to_lock & 0x1fffff;
			_samba.writeWord(NVM_ADDR_REG, addr_to_lock);

			execute_nvm_command(CMD_LOCK_REGION);
		}    
		else
		{
			uint32_t addr_to_unlock = getAddressByRegion(region);
			addr_to_unlock = addr_to_unlock & 0x1fffff; 
			_samba.writeWord(NVM_ADDR_REG, addr_to_unlock);

			execute_nvm_command(CMD_UNLOCK_REGION);
		}
	}
}


//Read the security bit, returns true if set, false otherwise.
	bool 
NvmFlash::getSecurity()
{
	uint16_t status_reg_value = _samba.readWord(NVM_STATUS_REG) & 0xffff;

	//If the 8th bit is 1 then security bit is set, else unset.
	return (((status_reg_value >> 8) & 0x1) == 1);
}

//Set's the security bit.
	void 
NvmFlash::setSecurity()
{
	if(!getSecurity()) //If security bit is not set
	{

		execute_nvm_command(CMD_SET_SECURITY_BIT);	

		if(!getSecurity())
			throw NvmFlashCmdError("Error when setting security bit");
	}
}

//Enable/disable the Bod. The values are lost on target reset.
	void 
NvmFlash::setBod(bool enable)
{
	uint32_t bod33_ctrl_reg = _samba.readWord(SYSCTRL_BOD33_REG);

	if(enable)
	{
		bod33_ctrl_reg |= SYSCTRL_STATUS_REG_ENABLE_BIT;//Enable the bod control
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
	bool bod_set =  (((value & SYSCTRL_STATUS_REG_ENABLE_BIT) >> 1) == 0x1); //If Bit 1 of the BOD33 register is 1, then it's enabled
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

	if(enable)
	{
		bod33_ctrl_reg |= SYSCTRL_STATUS_REG_BOD33_RESET_ENABLE_BIT; //To enable brown out reset set bit 3.
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
	//Always boots from flash. No ROM boot available.
	return true;
}

	void 
NvmFlash::setBootFlash(bool enable)
{
	std::cout<<"Flash boot is the only available option";
}


const uint8_t*
NvmFlash::get_complete_page(const uint8_t* buffer, uint16_t size)
{

	uint8_t* page_buf;

	if(size < PAGE_SIZE_IN_BYTES)//If it is a partial page, do padding
	{
		page_buf = (uint8_t*)malloc(PAGE_SIZE_IN_BYTES * sizeof(uint8_t));

		//First copy all the bytes from source
		for(uint16_t m=0;m<size;m++)
		{
			page_buf[m] = buffer[m];//strncpy or memcpy ?
		}
 
		//Now pad the remaining.
		for(uint16_t m=size;m<PAGE_SIZE_IN_BYTES;m++)
		{
			page_buf[m] = 255;
		}

		return (const uint8_t*)page_buf;
	}
	else if(size == PAGE_SIZE_IN_BYTES)
	{
		return buffer;//return the source as it is
	}
	else
	{ 
		throw NvmFlashCmdError(":(");
	/* We are unlucky. Grab a beer. */ 
	}
}

void
NvmFlash::setup_page_write()
{
	//clear page buffer
	execute_nvm_command(CMD_CLEAR_PAGE_BUFFER);

	//clear error flags in target
	uint16_t status_reg = _samba.readWord(NVM_STATUS_REG) & 0xffff;
	_samba.writeWord(NVM_STATUS_REG, status_reg | NVMCTRL_STATUS_MASK);

	//Configure manual page write. This is critical for the write to work
	//irrespective of full page or partial page.
	uint32_t ctrlb_reg = _samba.readWord(NVM_CTRLB_REG);
	_samba.writeWord(NVM_CTRLB_REG, ctrlb_reg | (0x1 << 7));
}

//During writePage/loadBuffer we do not know if we are in middle of a 
//write operation. So the context to call erase is not possible.
//So we introduce a new callback which is called before a page write
//session. This is called before first call to a writePage().

void
NvmFlash::beforeWrite()
{
	eraseAll();
}

void 
NvmFlash::loadBuffer(const uint8_t* data, uint16_t bufferSize)
{
	_bufferSize = bufferSize;
	_buffer = data;
}

//Reference : Atmel ASF nvm_write_buffer
void 
NvmFlash::writePage(uint32_t page)
{
	//loadBuffer is called before this function is called each time.

	if (page >= _pages)
		throw FlashPageError();

	if(_bufferSize > PAGE_SIZE_IN_BYTES)
		throw FlashPageError();

	if(!_buffer)
		throw NvmFlashCmdError("The input buffer is not valid");


	setup_page_write();

	//compute the start address.
	uint32_t addr = _addr + (page * PAGE_SIZE_IN_BYTES);
	uint32_t addr_cached = addr;
	uint32_t start = 0;

	//Get a full page. Sometimes the page size might be less than a page size, in which
	//case we should prepare a complete page by padding 0xff.
	const uint8_t* page_buf = get_complete_page(_buffer, _bufferSize);

	
	for(uint16_t i=0;i<(PAGE_SIZE_IN_BYTES/SAMBA_API_WORD_SIZE); i++ )
	{
		start = i * SAMBA_API_WORD_SIZE;
		uint32_t data = (page_buf[start+3] << 24) | (page_buf[start+2] << 16) | (page_buf[start+1] << 8) | (page_buf[start]);
		while(!nvm_is_ready());
		_samba.writeWord(addr, data);
		addr = addr+SAMBA_API_WORD_SIZE;
	}
	while(!nvm_is_ready());
	_samba.writeWord(NVM_ADDR_REG,addr_cached >> 1);
	execute_nvm_command(CMD_WRITE_PAGE);
	//Reset the buffer, so that subsequent reads are clear
}


	void 
NvmFlash::readPage(uint32_t page, uint8_t* buf)
{
	if(page >= _pages)
		throw FlashPageError();

	//Convert page number into physical address. flash_base_address + page.no * page_size
	uint32_t addr = _addr + (page * PAGE_SIZE_IN_BYTES);
	_samba.read(addr, buf, PAGE_SIZE_IN_BYTES);
}

///Returns the start address of a specified region number
///based on the flash specifications. The returned address is 
///word address (not byte address).

	uint32_t 
NvmFlash::getAddressByRegion(uint32_t region_num)
{
	if(region_num >= _lockRegions)
		throw FlashRegionError();
	uint32_t size_of_region = (PAGE_SIZE_IN_BYTES * numPages())/_lockRegions; //Flash Size / no of lock regions 
	uint32_t addr = address() + (region_num * size_of_region); addr = addr / 2; //Convert byte address to word address;
	return addr;
}

	void
NvmFlash::execute_nvm_command(uint32_t cmd)
{
	//wait for the nvm controller to be ready
	while(!nvm_is_ready()); 
	//send the comamnd to nvm controller.
	_samba.writeWord(NVM_CTRLA_REG, cmd);

	//wait till the operation completes.
	while(!nvm_is_ready());

	//TODO : check and return return error status from nvm status flag.
}
