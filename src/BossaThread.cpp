///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2018, ShumaTech
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
#include "BossaThread.h"
#include "BossaApp.h"
#include "Flash.h"

#include <exception>
#include <stdio.h>

using namespace std;

DEFINE_LOCAL_EVENT_TYPE(wxEVT_THREAD_PROGRESS)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_THREAD_SUCCESS)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_THREAD_WARNING)
DEFINE_LOCAL_EVENT_TYPE(wxEVT_THREAD_ERROR)

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

void
ThreadObserver::onStatus(const char *message, ...)
{
}

void
ThreadObserver::onProgress(int num, int div)
{
    int percent = num * 100 / div;
    
    if (percent != _lastPercent)
    {
        _thread->Progress(wxString::Format(wxT("%s page %d (%d%%)"), _operation.mb_str(), num, percent), percent);
        _lastPercent = percent;
    }    
}

WriteThread::WriteThread(wxEvtHandler* parent,
                         const wxString& filename,
                         bool eraseAll,
                         bool bootFlash,
                         bool bod,
                         bool bor,
                         bool lock,
                         bool security,
                         uint32_t offset) :
    BossaThread(parent), _filename(filename), _eraseAll(eraseAll),
    _bootFlash(bootFlash), _bod(bod), _bor(bor), _lock(lock), _security(security), _offset(offset)

{
}

wxThread::ExitCode
WriteThread::Entry()
{
    Device& device = wxGetApp().device;
    Device::FlashPtr& flash = device.getFlash();
    Samba& samba = wxGetApp().samba;
    
    try
    {
        ThreadObserver observer(this, "Writing");
        Flasher flasher(samba, device, observer);
        
        if (_eraseAll)
        {
            flash->eraseAll(_offset);
            flash->eraseAuto(false);
        }
        else
        {
            flash->eraseAuto(true);
        }    
        
        flasher.write(_filename.mb_str(), _offset);

        if (flash->canBootFlash())
            flash->setBootFlash(_bootFlash);
        if (flash->canBod())
            flash->setBod(_bod);
        if (flash->canBor())
            flash->setBor(_bor);
        if (_lock)
            flash->setLockRegions(std::vector<bool>(flash->lockRegions(), true));
        if (_security)
            flash->setSecurity();
        flash->writeOptions();
    }
    catch(exception& e)
    {
        Error(wxString(e.what(), wxConvUTF8));
        return 0;
    }

    Success(_("Write completed successfully"));
    
    return 0;
}

VerifyThread::VerifyThread(wxEvtHandler* parent, const wxString& filename, uint32_t offset) :
    BossaThread(parent), _filename(filename), _offset(offset)
{
}

wxThread::ExitCode
VerifyThread::Entry()
{
    Device& device = wxGetApp().device;
    Samba& samba = wxGetApp().samba;
    uint32_t pageErrors;
    uint32_t totalErrors;
    
    try
    {
        ThreadObserver observer(this, "Verifying");
        Flasher flasher(samba, device, observer);
        
        if (!flasher.verify(_filename.mb_str(), pageErrors, totalErrors, _offset))
        {
            Warning(wxString::Format(_(
                "Verify failed\n"
                "Page errors: %d\n"
                "Byte errors: %d\n"),
                pageErrors, totalErrors));
            return 0;
        }
        
    }
    catch(exception& e)
    {
        Error(wxString(e.what(), wxConvUTF8));
        return 0;
    }

    Success(_("Verify successful\n"));

    return 0;
}

ReadThread::ReadThread(wxEvtHandler* parent, const wxString& filename, size_t size, uint32_t offset) :
    BossaThread(parent), _filename(filename), _size(size), _offset(offset)
{
}

wxThread::ExitCode
ReadThread::Entry()
{
    Device& device = wxGetApp().device;
    Samba& samba = wxGetApp().samba;
    
    try
    {
        ThreadObserver observer(this, "Reading");
        Flasher flasher(samba, device, observer);
        
        flasher.read(_filename.mb_str(), _size, _offset);
        
    }
    catch(exception& e)
    {
        Error(wxString(e.what(), wxConvUTF8));
        return 0;
    }

    Success(_("Read completed successfully"));

    return 0;
}
