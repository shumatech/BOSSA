///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (C) 2011 ShumaTech http://www.shumatech.com/
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
#include <string>
#include <exception>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "CmdOpts.h"
#include "Samba.h"
#include "PortFactory.h"
#include "FlashFactory.h"

using namespace std;

class FileOpenError : public exception
{
public:
    FileOpenError() : exception() {};
    const char* what() const throw() { return "Unable to open file"; }
};

class FileIoError : public exception
{
public:
    FileIoError() : exception() {};
    const char* what() const throw() { return "File I/O operation failed"; }
};

class FileSizeError : public exception
{
public:
    FileSizeError() : exception() {};
    const char* what() const throw() { return "File operation exceeds flash size"; }
};

class BossaConfig
{
public:
    BossaConfig();
    virtual ~BossaConfig() {}

    bool erase;
    bool write;
    bool read;
    bool verify;
    bool port;
    bool boot;
    bool bor;
    bool bod;
    bool lock;
    bool unlock;
    bool security;
    bool info;
    bool debug;
    bool help;

    int readArg;
    string portArg;
    int bootArg;
    int bodArg;
    int borArg;
    string lockArg;
    string unlockArg;
};

BossaConfig::BossaConfig()
{
    erase = false;
    write = false;
    read = false;
    verify = false;
    port = false;
    boot = false;
    bod = false;
    bor = false;
    lock = false;
    security = false;
    info = false;
    help = false;
    
    readArg = 0;
    bootArg = 1;
    bodArg = 1;
    borArg = 1;
}

static BossaConfig config;
static Option opts[] =
{
    {
      'e', "erase", &config.erase,
      { ArgNone },
      "erase the entire flash"
    },
    {
      'w', "write", &config.write,
      { ArgNone },
      "write FILE to the flash; accelerated when\n"
      "combined with erase option"
    },
    {
      'r', "read", &config.read,
      { ArgOptional, ArgInt, "SIZE", { &config.readArg } },
      "read SIZE from flash and store in FILE;\n"
      "read entire flash if SIZE not specified"
    },
    {
      'v', "verify", &config.verify,
      { ArgNone },
      "verify FILE matches flash contents"
    },
    {
      'p', "port", &config.port,
      { ArgRequired, ArgString, "PORT", { &config.portArg } },
      "use serial PORT to communicate to device;\n"
      "default behavior is to auto-scan all serial ports"
    },
    {
      'b', "boot", &config.boot,
      { ArgOptional, ArgInt, "BOOL", { &config.bootArg } },
      "boot from ROM if BOOL is 0;\n"
      "boot from FLASH if BOOL is 1 [default];\n"
      "option is ignored on unsupported devices"
    },
    {
      'c', "bod", &config.bod,
      { ArgOptional, ArgInt, "BOOL", { &config.bodArg } },
      "no brownout detection if BOOL is 0;\n"
      "brownout detection is on if BOOL is 1 [default]"
    },
    {
      't', "bor", &config.bor,
      { ArgOptional, ArgInt, "BOOL", { &config.borArg } },
      "no brownout reset if BOOL is 0;\n"
      "brownout reset is on if BOOL is 1 [default]"
    },
    {
      'l', "lock", &config.lock,
      { ArgOptional, ArgString, "REGION", { &config.lockArg } },
      "lock the flash REGION as a comma-separated list;\n"
      "lock all if not given [default]"
    },
    {
      'u', "unlock", &config.unlock,
      { ArgOptional, ArgString, "REGION", { &config.unlockArg } },
      "unlock the flash REGION as a comma-separated list;\n"
      "unlock all if not given [default]"
    },
    {
      's', "security", &config.security,
      { ArgNone },
      "set the flash security flag"
    },
    {
      'i', "info", &config.info,
      { ArgNone },
      "display device information"
    },
    {
      'd', "debug", &config.debug,
      { ArgNone },
      "print debug messages"
    },
    {
      'h', "help", &config.help,
      { ArgNone },
      "display this help text"
    }
};

void
progressBar(int num, int div)
{
    int ticks;
    int bars = 30;
    
    printf("\r[");
    ticks = num * bars / div;
    while (ticks-- > 0)
    {
        putchar('=');
        bars--;
    }
    while (bars-- > 0)
    { 
        putchar(' ');
    }
    printf("] %d%% (%d/%d pages)", num * 100 / div, num, div);
    fflush(stdout);
}

void
eraseFlash(Flash::Ptr& flash)
{
    printf("Erase flash\n");
    flash->eraseAll();
    flash->eraseAuto(false);
}

void
writeFlash(Flash::Ptr& flash, const char* filename)
{
    FILE* infile;
    uint32_t pageSize = flash->pageSize();
    uint8_t buffer[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;
    long fsize;
    size_t fbytes;

    infile = fopen(filename, "rb");
    if (!infile)
        throw FileOpenError();

    try
    {
        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError();
        rewind(infile);
        
        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > flash->numPages())
            throw FileSizeError();

        printf("Write %ld bytes to flash\n", fsize);
        
        while ((fbytes = fread(buffer, 1, pageSize, infile)) > 0)
        {
            if (pageNum % 10 == 0)
                progressBar(pageNum, numPages);
                
            flash->loadBuffer(buffer);
            flash->writePage(pageNum);
            
            pageNum++;
            if (pageNum == numPages)
                break;
            if (fbytes != pageSize)
                throw FileIoError();
        }
        if (fbytes <= 0)
            throw FileIoError();
        progressBar(pageNum, numPages);
        printf("\n");
    }
    catch(exception& e)
    {
        fclose(infile);
        throw;
    }
    fclose(infile);
}

bool
verifyFlash(Flash::Ptr& flash, const char* filename)
{
    FILE* infile;
    uint32_t pageSize = flash->pageSize();
    uint8_t bufferA[pageSize];
    uint8_t bufferB[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;
    uint32_t byteErrors = 0;
    uint32_t pageErrors = 0;
    uint32_t totalErrors = 0;
    long fsize;
    size_t fbytes;
    
    infile = fopen(filename, "rb");
    if (!infile)
        throw FileOpenError();

    try
    {
        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError();
        rewind(infile);
        
        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > flash->numPages())
            throw FileSizeError();
        
        printf("Verify %ld bytes of flash\n", fsize);
        
        while ((fbytes = fread(bufferA, 1, pageSize, infile)) > 0)
        {
            if (pageNum % 10 == 0)
                progressBar(pageNum, numPages);
            
            flash->readPage(pageNum, bufferB);
            
            byteErrors = 0;
            for (uint32_t i = 0; i < fbytes; i++)
            {
                if (bufferA[i] != bufferB[i])
                    byteErrors++;
            }
            if (byteErrors != 0)
            {
                pageErrors++;
                totalErrors += byteErrors;
            }
            
            pageNum++;
            if (pageNum == numPages)
                break;
            if (fbytes != pageSize)
                throw FileIoError();
        }
        if (fbytes <= 0)
            throw FileIoError();
        progressBar(pageNum, numPages);
        printf("\n");
    }
    catch(exception& e)
    {
        fclose(infile);
        throw;
    }
    fclose(infile);
    
    if (byteErrors != 0)
    {
        printf("Verify failed\n");
        printf("Page errors: %d\n", pageErrors);
        printf("Byte errors: %d\n", totalErrors);
        return false;
    }
    
    printf("Verify successful\n");
    return true;
}

void
readFlash(Flash::Ptr& flash, const char* filename, long fsize)
{
    FILE* outfile;
    uint32_t pageSize = flash->pageSize();
    uint8_t buffer[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;

    if (fsize == 0)
        fsize = pageSize * flash->numPages();
    
    outfile = fopen(filename, "wb");
    if (!outfile)
        throw FileOpenError();

    try
    {
        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > flash->numPages())
            throw FileSizeError();

        printf("Read %ld bytes from flash\n", fsize);
        
        for (pageNum = 0; pageNum < numPages; pageNum++)
        {
            if (pageNum % 10 == 0)
                progressBar(pageNum, numPages);
                
            flash->readPage(pageNum, buffer);
            
            if (pageNum == numPages - 1 && fsize % pageSize > 0)
                pageSize = fsize % pageSize;
            if (fwrite(buffer, 1, pageSize, outfile) != pageSize)
                throw FileIoError();
        }
        progressBar(pageNum, numPages);
        printf("\n");
    }
    catch(exception& e)
    {
        fclose(outfile);
        throw;
    }
    fclose(outfile);
}

void
lockFlash(Flash::Ptr& flash, string& regionArg, bool enable)
{
    if (regionArg.empty())
    {
        printf("%s all regions\n", enable ? "Lock" : "Unlock");
        if (enable)
            flash->lockAll();
        else
            flash->unlockAll();
    }
    else
    {
        size_t pos = 0;
        size_t delim;
        uint32_t region;
        string sub;
        
        do
        {
            delim = regionArg.find(',', pos);
            sub = regionArg.substr(pos, delim < 0 ? -1 : delim - pos);
            region = strtol(sub.c_str(), NULL, 0);
            printf("%s region %d\n", enable ? "Lock" : "Unlock", region);
            flash->setLockRegion(region, enable);
            pos = delim + 1;
        } while (delim != string::npos);
    }
}

void
infoFlash(Samba& samba, Flash::Ptr& flash)
{
    bool first;

    printf("Device       : %s\n", flash->name().c_str());
    printf("Chip ID      : %08x\n", samba.chipId());
    printf("Version      : %s\n", samba.version().c_str());
    printf("Pages        : %d\n", flash->numPages());
    printf("Page Size    : %d bytes\n", flash->pageSize());
    printf("Total Size   : %dKB\n", flash->numPages() * flash->pageSize() / 1024);
    printf("Planes       : %d\n", flash->numPlanes());
    printf("Lock Regions : %d\n", flash->lockRegions());
    printf("Locked       : ");
    first = true;
    for (uint32_t region = 0; region < flash->lockRegions(); region++)
    {
        if (flash->getLockRegion(region))
        {
            printf("%s%d", first ? "" : ",", region);
            first = false;
        }
    }
    printf("%s\n", first ? "none" : "");
    printf("Security     : %s\n", flash->getSecurity() ? "true" : "false");
    if (flash->canBootFlash())
        printf("Boot Flash   : %s\n", flash->getBootFlash() ? "true" : "false");
    if (flash->canBod())
        printf("BOD          : %s\n", flash->getBod() ? "true" : "false");
    if (flash->canBor())
        printf("BOR          : %s\n", flash->getBor() ? "true" : "false");
}

bool
autoScan(Samba& samba, PortFactory& portFactory, string& port)
{
    for (port = portFactory.begin();
         port != portFactory.end();
         port = portFactory.next())
    {
        if (samba.connect(portFactory.create(port)))
            return true;
    }

    return false;
}

int
help(const char* program)
{
    fprintf(stderr, "Try '%s -h' or '%s --help' for more information\n", program, program);
    return 1;
}

int
main(int argc, char* argv[])
{
    int args;
    char* pos;
    CmdOpts cmd(argc, argv, sizeof(opts) / sizeof(opts[0]), opts);

    if ((pos = strrchr(argv[0], '/')) || (pos = strrchr(argv[0], '\\')))
        argv[0] = pos + 1;

    if (argc <= 1)
    {
        fprintf(stderr, "%s: you must specify at least one option\n", argv[0]);
        return help(argv[0]);
    }

    args = cmd.parse();
    if (args < 0)
        return help(argv[0]);

    if (config.read && (config.write || config.verify))
    {
        fprintf(stderr, "%s: read option is exclusive of write or verify\n", argv[0]);
        return help(argv[0]);
    }

    if (config.read || config.write || config.verify)
    {
        if (args == argc)
        {
            fprintf(stderr, "%s: missing file\n", argv[0]);
            return help(argv[0]);
        }
        argc--;
    }
    if (args != argc)
    {
        fprintf(stderr, "%s: extra arguments found\n", argv[0]);
        return help(argv[0]);
    }

    if (config.help)
    {
        printf("Usage: %s [OPTION...] [FILE]\n", argv[0]);
        printf("Basic Open Source SAM-BA Application (BOSSA) Version " VERSION "\n"
               "Flash programmer for Atmel SAM devices.\n"
               "Copyright (c) 2011 ShumaTech (http://www.shumatech.com)\n"
               "\n"
               "Examples:\n"
               "  bossac -e -w -v -b image.bin   # Erase flash, write flash with image.bin,\n"
               "                                 # verify the write, and set boot from flash\n"
               "  bossac -r0x10000 image.bin     # Read 64KB from flash and store in image.bin\n"
              );
        printf("\nOptions:\n");
        cmd.usage(stdout);
        printf("\nReport bugs to <bugs@shumatech.com>\n");
        return 1;
    }

    try
    {
        Samba samba;
        PortFactory portFactory;
        FlashFactory flashFactory;

        if (config.debug)
            samba.setDebug(true);
        
        if (config.port)
        {
            if (!samba.connect(portFactory.create(config.portArg)))
            {
                fprintf(stderr, "No device found on %s\n", config.portArg.c_str());
                return 1;
            }
        }
        else
        {
            string port;
            if (!autoScan(samba, portFactory, port))
            {
                fprintf(stderr, "Auto scan for device failed\n");
                fprintf(stderr, "Try specifying a serial port with the '-p' option\n");
                return 1;
            }
            printf("Device found on %s\n", port.c_str());
        }
        
        uint32_t chipId = samba.chipId();
        Flash::Ptr flash = flashFactory.create(samba, chipId);
        if (flash.get() == NULL)
        {
            fprintf(stderr, "Flash for chip ID %08x is not supported\n", chipId);
            return 1;
        }
            
        if (config.unlock)
            lockFlash(flash, config.unlockArg, false);
            
        if (config.erase)
            eraseFlash(flash);

        if (config.write)
            writeFlash(flash, argv[args]);

        if (config.verify)
            if  (!verifyFlash(flash, argv[args]))
                return 2;

        if (config.read)
            readFlash(flash, argv[args], config.readArg);

        if (config.boot)
        {
            printf("Set boot flash %s\n", config.bootArg ? "true" : "false");
            flash->setBootFlash(config.bootArg);
        }
        
        if (config.bod)
        {
            printf("Set brownout detect %s\n", config.bodArg ? "true" : "false");
            flash->setBod(config.bodArg);
        }

        if (config.bor)
        {
            printf("Set brownout reset %s\n", config.borArg ? "true" : "false");
            flash->setBor(config.borArg);
        }

        if (config.security)
        {
            printf("Set security\n");
            flash->setSecurity();
        }
        
        if (config.lock)
            lockFlash(flash, config.lockArg, true);

        if (config.info)
            infoFlash(samba, flash);
    }
    catch (exception& e)
    {
        fprintf(stderr, "\n%s\n", e.what());
        return 1;
    }
    catch(...)
    {
        fprintf(stderr, "\nUnhandled exception\n");
        return 1;
    }
    
    return 0;
}

