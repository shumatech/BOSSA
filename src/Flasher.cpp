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
#include <string>
#include <exception>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Flasher.h"

using namespace std;

void
Flasher::progressBar(int num, int div)
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
Flasher::erase()
{
    printf("Erase flash\n");
    _flash->eraseAll();
    _flash->eraseAuto(false);
}

void
Flasher::write(const char* filename)
{
    FILE* infile;
    uint32_t pageSize = _flash->pageSize();
    uint8_t buffer[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;
    long fsize;
    size_t fbytes;

    infile = fopen(filename, "rb");
    if (!infile)
        throw FileOpenError(errno);

    try
    {
        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError(errno);
        rewind(infile);

        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > _flash->numPages())
            throw FileSizeError();

        printf("Write %ld bytes to flash\n", fsize);

        while ((fbytes = fread(buffer, 1, pageSize, infile)) > 0)
        {
            if (pageNum % 10 == 0)
                progressBar(pageNum, numPages);

            _flash->loadBuffer(buffer);
            _flash->writePage(pageNum);

            pageNum++;
            if (pageNum == numPages || fbytes != pageSize)
                break;
        }
        if (fbytes < 0)
            throw FileIoError(errno);
        progressBar(pageNum, numPages);
        printf("\n");
    }
    catch(...)
    {
        fclose(infile);
        throw;
    }
    fclose(infile);
}

bool
Flasher::verify(const char* filename)
{
    FILE* infile;
    uint32_t pageSize = _flash->pageSize();
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
        throw FileOpenError(errno);

    try
    {
        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError(errno);
        rewind(infile);

        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > _flash->numPages())
            throw FileSizeError();

        printf("Verify %ld bytes of flash\n", fsize);

        while ((fbytes = fread(bufferA, 1, pageSize, infile)) > 0)
        {
            if (pageNum % 10 == 0)
                progressBar(pageNum, numPages);

            _flash->readPage(pageNum, bufferB);

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
            if (pageNum == numPages || fbytes != pageSize)
                break;
        }
        if (fbytes < 0)
            throw FileIoError(errno);
        progressBar(pageNum, numPages);
        printf("\n");
    }
    catch(...)
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
Flasher::read(const char* filename, long fsize)
{
    FILE* outfile;
    uint32_t pageSize = _flash->pageSize();
    uint8_t buffer[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;
    size_t fbytes;

    if (fsize == 0)
        fsize = pageSize * _flash->numPages();

    outfile = fopen(filename, "wb");
    if (!outfile)
        throw FileOpenError(errno);

    try
    {
        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > _flash->numPages())
            throw FileSizeError();

        printf("Read %ld bytes from flash\n", fsize);

        for (pageNum = 0; pageNum < numPages; pageNum++)
        {
            if (pageNum % 10 == 0)
                progressBar(pageNum, numPages);

            _flash->readPage(pageNum, buffer);

            if (pageNum == numPages - 1 && fsize % pageSize > 0)
                pageSize = fsize % pageSize;
            fbytes = fwrite(buffer, 1, pageSize, outfile);
            if (fbytes < 0)
                throw FileIoError(errno);
            if (fbytes != pageSize)
                throw FileShortError();
        }
        progressBar(pageNum, numPages);
        printf("\n");
    }
    catch(...)
    {
        fclose(outfile);
        throw;
    }
    fclose(outfile);
}

void
Flasher::lock(string& regionArg, bool enable)
{
    if (regionArg.empty())
    {
        printf("%s all regions\n", enable ? "Lock" : "Unlock");
        if (enable)
            _flash->lockAll();
        else
            _flash->unlockAll();
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
            _flash->setLockRegion(region, enable);
            pos = delim + 1;
        } while (delim != string::npos);
    }
}

void
Flasher::info(Samba& samba)
{
    bool first;

    printf("Device       : %s\n", _flash->name().c_str());
    printf("Chip ID      : %08x\n", samba.chipId());
    printf("Version      : %s\n", samba.version().c_str());
    printf("Address      : %d\n", _flash->address());
    printf("Pages        : %d\n", _flash->numPages());
    printf("Page Size    : %d bytes\n", _flash->pageSize());
    printf("Total Size   : %dKB\n", _flash->numPages() * _flash->pageSize() / 1024);
    printf("Planes       : %d\n", _flash->numPlanes());
    printf("Lock Regions : %d\n", _flash->lockRegions());
    printf("Locked       : ");
    first = true;
    for (uint32_t region = 0; region < _flash->lockRegions(); region++)
    {
        if (_flash->getLockRegion(region))
        {
            printf("%s%d", first ? "" : ",", region);
            first = false;
        }
    }
    printf("%s\n", first ? "none" : "");
    printf("Security     : %s\n", _flash->getSecurity() ? "true" : "false");
    if (_flash->canBootFlash())
        printf("Boot Flash   : %s\n", _flash->getBootFlash() ? "true" : "false");
    if (_flash->canBod())
        printf("BOD          : %s\n", _flash->getBod() ? "true" : "false");
    if (_flash->canBor())
        printf("BOR          : %s\n", _flash->getBor() ? "true" : "false");
}
