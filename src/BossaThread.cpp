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
#include "BossaThread.h"
#include "BossaApp.h"
#include "Flash.h"

#include <exception>
#include <stdio.h>

using namespace std;

DEFINE_EVENT_TYPE(wxEVT_THREAD_PROGRESS)
DEFINE_EVENT_TYPE(wxEVT_THREAD_SUCCESS)
DEFINE_EVENT_TYPE(wxEVT_THREAD_WARNING)
DEFINE_EVENT_TYPE(wxEVT_THREAD_ERROR)

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

BossaThread::BossaThread(wxEvtHandler* parent) : wxThread(), _parent(parent), _stopped(false)
{
}

void
BossaThread::Progress(const wxString& message, int pos)
{
    wxCommandEvent event(wxEVT_THREAD_PROGRESS);
    event.SetString(message);
    event.SetInt(pos);
    _parent->AddPendingEvent(event);
}

void
BossaThread::Success(const wxString& message)
{
    wxCommandEvent event(wxEVT_THREAD_SUCCESS);
    event.SetString(message);
    _parent->AddPendingEvent(event);
}

void
BossaThread::Warning(const wxString& message)
{
    wxCommandEvent event(wxEVT_THREAD_WARNING);
    event.SetString(message);
    _parent->AddPendingEvent(event);
}

void
BossaThread::Error(const wxString& message)
{
    wxCommandEvent event(wxEVT_THREAD_ERROR);
    event.SetString(message);
    _parent->AddPendingEvent(event);
}

WriteThread::WriteThread(wxEvtHandler* parent,
                         const wxString& filename,
                         bool eraseAll,
                         bool bootFlash,
                         bool bod,
                         bool bor,
                         bool lock,
                         bool security) :
    BossaThread(parent), _filename(filename), _eraseAll(eraseAll),
    _bootFlash(bootFlash), _bod(bod), _bor(bor), _lock(lock), _security(security)

{
}

wxThread::ExitCode
WriteThread::Entry()
{
    FILE* infile = NULL;
    Flash& flash = *wxGetApp().flash;
    uint32_t pageSize = flash.pageSize();
    uint8_t buffer[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;
    long fsize;
    size_t fbytes;

    try
    {
        if (_eraseAll)
        {
            flash.eraseAll();
            flash.eraseAuto(false);
        }
        else
        {
            flash.eraseAuto(true);
        }

        infile = fopen(_filename.mb_str(), "rb");
        if (!infile)
            throw FileOpenError();

        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError();
        rewind(infile);

        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > flash.numPages())
            throw FileSizeError();

        while ((fbytes = fread(buffer, 1, pageSize, infile)) > 0)
        {
            if (_stopped)
            {
                fclose(infile);
                Warning(wxT("Write stopped"));
                return 0;
            }

            if (pageNum % 10 == 0 || pageNum == numPages - 1)
            {
                uint32_t percent = (pageNum + 1) * 100 / numPages;
                Progress(wxString::Format(wxT("Writing page %d (%d%%)"), pageNum, percent),
                         percent);
            }

            flash.loadBuffer(buffer);
            flash.writePage(pageNum);

            pageNum++;
            if (pageNum == numPages)
                break;
            if (fbytes != pageSize)
                throw FileIoError();
        }
        if (fbytes <= 0)
            throw FileIoError();

        flash.setBootFlash(_bootFlash);
        flash.setBod(_bod);
        flash.setBor(_bor);
        if (_lock)
            flash.lockAll();
        if (_security)
            flash.setSecurity();

        fclose(infile);
    }
    catch(exception& e)
    {
        if (infile)
            fclose(infile);
        Error(wxString(e.what(), wxConvUTF8));
        return 0;
    }

    Success(_("Write completed successfully"));
    return 0;
}

VerifyThread::VerifyThread(wxEvtHandler* parent, const wxString& filename) :
    BossaThread(parent), _filename(filename)
{
}

wxThread::ExitCode
VerifyThread::Entry()
{
    FILE* infile = NULL;
    Flash& flash = *wxGetApp().flash;
    uint32_t pageSize = flash.pageSize();
    uint8_t bufferA[pageSize];
    uint8_t bufferB[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;
    uint32_t byteErrors = 0;
    uint32_t pageErrors = 0;
    uint32_t totalErrors = 0;
    long fsize;
    size_t fbytes;

    try
    {
        infile = fopen(_filename.mb_str(), "rb");
        if (!infile)
            throw FileOpenError();

        if (fseek(infile, 0, SEEK_END) != 0 ||
            (fsize = ftell(infile)) < 0)
            throw FileIoError();
        rewind(infile);

        numPages = (fsize + pageSize - 1) / pageSize;
        if (numPages > flash.numPages())
            throw FileSizeError();

        while ((fbytes = fread(bufferA, 1, pageSize, infile)) > 0)
        {
            if (_stopped)
            {
                fclose(infile);
                Warning(wxT("Verify stopped"));
                return 0;
            }

            if (pageNum % 10 == 0 || pageNum == numPages - 1)
            {
                uint32_t percent = (pageNum + 1) * 100 / numPages;
                Progress(wxString::Format(wxT("Verifying page %d (%d%%)"), pageNum, percent),
                         percent);
            }

            flash.readPage(pageNum, bufferB);

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

        fclose(infile);
    }
    catch(exception& e)
    {
        if (infile)
            fclose(infile);
        Error(wxString(e.what(), wxConvUTF8));
        return 0;
    }

    if (byteErrors != 0)
    {
        Warning(wxString::Format(_(
            "Verify failed\n"
            "Page errors: %d\n"
            "Byte errors: %d\n"),
            pageErrors, totalErrors));
        return 0;
    }

    Success(_("Verify successful\n"));

    return 0;
}

ReadThread::ReadThread(wxEvtHandler* parent, const wxString& filename, size_t size) :
    BossaThread(parent), _filename(filename), _size(size)
{
}

wxThread::ExitCode
ReadThread::Entry()
{
    FILE* outfile = NULL;
    Flash& flash = *wxGetApp().flash;
    uint32_t pageSize = flash.pageSize();
    uint8_t buffer[pageSize];
    uint32_t pageNum = 0;
    uint32_t numPages;

    if (_size == 0)
        _size = pageSize * flash.numPages();

    outfile = fopen(_filename.mb_str(), "wb");
    if (!outfile)
        throw FileOpenError();

    try
    {
        numPages = (_size + pageSize - 1) / pageSize;
        if (numPages > flash.numPages())
            throw FileSizeError();

        for (pageNum = 0; pageNum < numPages; pageNum++)
        {
            if (_stopped)
            {
                fclose(outfile);
                Warning(wxT("Read stopped"));
                return 0;
            }

            if (pageNum % 10 == 0 || pageNum == numPages - 1)
            {
                uint32_t percent = (pageNum + 1) * 100 / numPages;
                Progress(wxString::Format(wxT("Reading page %d (%d%%)"), pageNum, percent),
                         percent);
            }

            flash.readPage(pageNum, buffer);

            if (pageNum == numPages - 1 && _size % pageSize > 0)
                pageSize = _size % pageSize;
            if (fwrite(buffer, 1, pageSize, outfile) != pageSize)
                throw FileIoError();
        }
        fclose(outfile);
    }
    catch(exception& e)
    {
        if (outfile)
            fclose(outfile);
        Error(wxString(e.what(), wxConvUTF8));
        return 0;
    }

    Success(_("Read completed successfully"));
    return 0;
}

