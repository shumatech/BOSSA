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
#ifndef _BOSSATHREAD_H
#define _BOSSATHREAD_H

#include <wx/wx.h>

#include "Flasher.h"

DECLARE_LOCAL_EVENT_TYPE(wxEVT_THREAD_PROGRESS, wxID_ANY)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_THREAD_SUCCESS, wxID_ANY)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_THREAD_WARNING, wxID_ANY)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_THREAD_ERROR, wxID_ANY)

class BossaThread : public wxThread
{
public:
    BossaThread(wxEvtHandler* parent);

    void stop() { _stopped = true; }

    void Progress(const wxString& message, int pos);
    void Success(const wxString& message);
    void Warning(const wxString& message);
    void Error(const wxString& message);
    
protected:
    wxEvtHandler* _parent;

    bool _stopped;
};

class ThreadObserver : public FlasherObserver
{
public:
    ThreadObserver(BossaThread* thread, const wxString& operation) :
        _thread(thread), _operation(operation), _lastPercent(-1) {}
    virtual ~ThreadObserver() {}
    
    virtual void onStatus(const char *message, ...);
    virtual void onProgress(int num, int div);
    
private:
    BossaThread *_thread;
    wxString _operation;
    int _lastPercent;
};

class WriteThread : public BossaThread
{
public:
    WriteThread(wxEvtHandler* parent,
                const wxString& filename,
                bool eraseAll,
                bool bootFlash,
                bool bod,
                bool bor,
                bool lock,
                bool security,
                uint32_t offset);
    wxThread::ExitCode Entry();

private:
    wxString _filename;
    bool _eraseAll;
    bool _bootFlash;
    bool _bod;
    bool _bor;
    bool _lock;
    bool _security;
    uint32_t _offset;
};

class VerifyThread : public BossaThread
{
public:
    VerifyThread(wxEvtHandler* parent,
                 const wxString& filename,
                 uint32_t offset);
    wxThread::ExitCode Entry();

private:
    wxString _filename;
    uint32_t _offset;
};

class ReadThread : public BossaThread
{
public:
    ReadThread(wxEvtHandler* parent,
               const wxString& filename,
               size_t size,
               uint32_t offset);
    wxThread::ExitCode Entry();

private:
    wxString _filename;
    size_t _size;
    uint32_t _offset;
};

#endif // _BOSSAINFO_H
