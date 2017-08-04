///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2015, Arduino LLC
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

#ifndef _DEVICES_H_
#define _DEVICES_H_

// CMDEX field should be 0xA5 to allow execution of any command.
#define CMDEX_KEY             (0xa500u)

#define ATSAM_APPLET_MAX_SIZE                (0x1000)

#define ATSAMD_CHIPID_MASK                   (0xFFFF00FFul)  // mask for DIE & REV bitfields removal in Samba::chipId()
#define ATSAMD_BOOTLOADER_SIZE               (0x00002000ul)  // 8192 bytes
#define ATSAMR_BOOTLOADER_SIZE               (0x00002000ul) // 8192 bytes, USB-CDC only
#define ATSAMD_FLASH_ROW_PAGES               (4ul)           // 4 pages per row

#define ATSAMD21J18A_NAME                    "ATSAMD21J18A"
#define ATSAMD21J18A_CHIPID                  (0x10010000ul)  // DIE & REV bitfields masked in Samba::chipId()
#define ATSAMD21J18A_FLASH_BASE              (0x00000000ul + ATSAMD_BOOTLOADER_SIZE)
#define ATSAMD21J18A_FLASH_PAGE_SIZE         (64ul)
#define ATSAMD21J18A_FLASH_PAGES             (4096ul)
#define ATSAMD21J18A_FLASH_PLANES            (1ul)
#define ATSAMD21J18A_FLASH_LOCK_REGIONS      (16ul)
#define ATSAMD21J18A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMD21J18A_STACK_ADDR              (0x20008000ul)
#define ATSAMD21J18A_NVMCTRL_BASE            (0x41004000ul)

#define ATSAMD21G18A_NAME                    "ATSAMD21G18A"
#define ATSAMD21G18A_CHIPID                  (0x10010005ul)  // DIE & REV bitfields masked in Samba::chipId()
#define ATSAMD21G18A_FLASH_BASE              (0x00000000ul + ATSAMD_BOOTLOADER_SIZE)
#define ATSAMD21G18A_FLASH_PAGE_SIZE         (64ul)
#define ATSAMD21G18A_FLASH_PAGES             (4096ul)
#define ATSAMD21G18A_FLASH_PLANES            (1ul)
#define ATSAMD21G18A_FLASH_LOCK_REGIONS      (16ul)
#define ATSAMD21G18A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMD21G18A_STACK_ADDR              (0x20008000ul)
#define ATSAMD21G18A_NVMCTRL_BASE            (0x41004000ul)

#define ATSAMD21E18A_NAME                    "ATSAMD21E18A"
#define ATSAMD21E18A_CHIPID                  (0x1001000aul)  // DIE & REV bitfields masked in Samba::chipId()
#define ATSAMD21E18A_FLASH_BASE              (0x00000000ul + ATSAMD_BOOTLOADER_SIZE)
#define ATSAMD21E18A_FLASH_PAGE_SIZE         (64ul)
#define ATSAMD21E18A_FLASH_PAGES             (4096ul)
#define ATSAMD21E18A_FLASH_PLANES            (1ul)
#define ATSAMD21E18A_FLASH_LOCK_REGIONS      (16ul)
#define ATSAMD21E18A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMD21E18A_STACK_ADDR              (0x20008000ul)
#define ATSAMD21E18A_NVMCTRL_BASE            (0x41004000ul)

#define ATSAMR21E18A_NAME                    "ATSAMR21E18A"
#define ATSAMR21E18A_CHIPID                  (0x1001001cul)  // DIE & REV bitfields masked in Samba::chipId()
#define ATSAMR21E18A_FLASH_BASE              (0x00000000ul + ATSAMR_BOOTLOADER_SIZE)
#define ATSAMR21E18A_FLASH_PAGE_SIZE         (64ul)
#define ATSAMR21E18A_FLASH_PAGES             (4096ul)
#define ATSAMR21E18A_FLASH_PLANES            (1ul)
#define ATSAMR21E18A_FLASH_LOCK_REGIONS      (16ul)
#define ATSAMR21E18A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMR21E18A_STACK_ADDR              (0x20008000ul)
#define ATSAMR21E18A_NVMCTRL_BASE            (0x41004000ul)

#define ATSAMD51J20A_NAME                    "ATSAMD51J20A"
#define ATSAMD51J20A_CHIPID                  (0x60060004) 
#define ATSAMD51J20A_FLASH_BASE              (0x00000000ul + ATSAMD_BOOTLOADER_SIZE)
#define ATSAMD51J20A_FLASH_PAGE_SIZE         (512ul)
#define ATSAMD51J20A_FLASH_PAGES             (2048ul)
#define ATSAMD51J20A_FLASH_PLANES            (1ul) // is it (2ul)? BANK A and BANK B ?
#define ATSAMD51J20A_FLASH_LOCK_REGIONS      (32ul)
#define ATSAMD51J20A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMD51J20A_STACK_ADDR              (0x20008000ul)
#define ATSAMD51J20A_NVMCTRL_BASE            (0x41004000ul)

#define ATSAMD51P20A_NAME                    "ATSAMD51P20A"
#define ATSAMD51P20A_CHIPID                  (0x60060000)  // DIE & REV bitfields masked in Samba::chipId()
#define ATSAMD51P20A_FLASH_BASE              (0x00000000ul + ATSAMD_BOOTLOADER_SIZE)
#define ATSAMD51P20A_FLASH_PAGE_SIZE         (512ul)
#define ATSAMD51P20A_FLASH_PAGES             (2048ul)
#define ATSAMD51P20A_FLASH_PLANES            (1ul) // is it (2ul)? BANK A and BANK B ?
#define ATSAMD51P20A_FLASH_LOCK_REGIONS      (32ul)
#define ATSAMD51P20A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMD51P20A_STACK_ADDR              (0x20008000ul)
#define ATSAMD51P20A_NVMCTRL_BASE            (0x41004000ul)

#define ATSAMD51G19A_NAME                    "ATSAMD51G19A"
#define ATSAMD51G19A_CHIPID                  (0x60060007)  // DIE & REV bitfields masked in Samba::chipId()
#define ATSAMD51G19A_FLASH_BASE              (0x00000000ul + ATSAMD_BOOTLOADER_SIZE)
#define ATSAMD51G19A_FLASH_PAGE_SIZE         (512ul)
#define ATSAMD51G19A_FLASH_PAGES             (1024ul)
#define ATSAMD51G19A_FLASH_PLANES            (1ul) // is it (2ul)? BANK A and BANK B ?
#define ATSAMD51G19A_FLASH_LOCK_REGIONS      (32ul)
#define ATSAMD51G19A_BUFFER_ADDR             (0x20004000ul)
#define ATSAMD51G19A_STACK_ADDR              (0x20008000ul)
#define ATSAMD51G19A_NVMCTRL_BASE            (0x41004000ul)

typedef struct {
	uint32_t CMD_LOCK_REGION;
	uint32_t CMD_UNLOCK_REGION;
	uint32_t CMD_ERASE_ROW;
	uint32_t CMD_WRITE_PAGE;
	uint32_t CMD_SET_SECURITY_BIT;
	uint32_t CMD_CLEAR_PAGE_BUFFER;
	
	uint32_t NVM_LOCK_REG;
	uint32_t NVM_INT_STATUS_REG;
	uint32_t NVM_ADDR_REG;
	
	uint32_t NVM_STATUS_REG;
	uint32_t NVMCTRL_STATUS_MASK;
	
} nvmCmdSet;

static nvmCmdSet samd21CmdSet = {
	/* CMD_LOCK_REGION */ (CMDEX_KEY | 0x0040u), 
	/* CMD_UNLOCK_REGION */ (CMDEX_KEY | 0x0041u), 
	/* CMD_ERASE_ROW */ (CMDEX_KEY | 0x0002u), 
	/* CMD_WRITE_PAGE */ (CMDEX_KEY | 0x0004u),
	/* CMD_SET_SECURITY_BIT */ (CMDEX_KEY | 0x0045u),
	/* CMD_CLEAR_PAGE_BUFFER */ (CMDEX_KEY | 0x0044u),
	
	/* NVM_LOCK_REG */			(ATSAMD21J18A_NVMCTRL_BASE + 0x20),
	/* NVM_INT_STATUS_REG */	(ATSAMD21J18A_NVMCTRL_BASE + 0x14),
	/* NVM_ADDR_REG */			(ATSAMD21J18A_NVMCTRL_BASE + 0x1c),
	
	/* NVM_STATUS_REG */		(ATSAMD21J18A_NVMCTRL_BASE + 0x18),
	/* NVMCTRL_STATUS_MASK */	0xFFEBu
};

static nvmCmdSet samd51CmdSet = {
	/* CMD_LOCK_REGION */ (CMDEX_KEY | 0x0011u), 
	/* CMD_UNLOCK_REGION */ (CMDEX_KEY | 0x0012u), 
	/* CMD_ERASE_ROW */ (CMDEX_KEY | 0x0001u), 
	/* CMD_WRITE_PAGE */ (CMDEX_KEY | 0x0003u),
	/* CMD_SET_SECURITY_BIT */ (CMDEX_KEY | 0x0016u),
	/* CMD_CLEAR_PAGE_BUFFER */ (CMDEX_KEY | 0x0015u),
	
	/* NVM_LOCK_REG */			(ATSAMD51P20A_NVMCTRL_BASE + 0x18),
	/* NVM_INT_STATUS_REG */	(ATSAMD51P20A_NVMCTRL_BASE + 0x12),
	/* NVM_ADDR_REG */			(ATSAMD51P20A_NVMCTRL_BASE + 0x14),
	
	/* NVM_STATUS_REG */		(ATSAMD51P20A_NVMCTRL_BASE + 0x12),
	/* NVMCTRL_STATUS_MASK */	0xFFEBu
};

#endif // _DEVICES_H_
