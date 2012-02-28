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
#ifndef _BOSSATHREAD_H
#define _BOSSATHREAD_H

#include <wx/wx.h>

DECLARE_EVENT_TYPE(wxEVT_THREAD_PROGRESS, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_THREAD_SUCCESS, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_THREAD_WARNING, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_THREAD_ERROR, wxID_ANY)

class BossaThread : public wxThread
{
public:
    BossaThread(wxEvtHandler* parent);

    void stop() { _stopped = true; }

protected:
    wxEvtHandler* _parent;

    bool _stopped;

    void Progress(const wxString& message, int pos);
    void Success(const wxString& message);
    void Warning(const wxString& message);
    void Error(const wxString& message);
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
                bool security);
    wxThread::ExitCode Entry();

private:
    wxString _filename;
    bool _eraseAll;
    bool _bootFlash;
    bool _bod;
    bool _bor;
    bool _lock;
    bool _security;
};

class VerifyThread : public BossaThread
{
public:
    VerifyThread(wxEvtHandler* parent,
                 const wxString& filename);
    wxThread::ExitCode Entry();

private:
    wxString _filename;
};

class ReadThread : public BossaThread
{
public:
    ReadThread(wxEvtHandler* parent,
               const wxString& filename,
               size_t size);
    wxThread::ExitCode Entry();

private:
    wxString _filename;
    size_t _size;
};

#endif // _BOSSAINFO_H
