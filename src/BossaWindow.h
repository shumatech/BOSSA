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
#ifndef _BOSSAWINDOW_H
#define _BOSSAWINDOW_H

#include <wx/wx.h>

#include "BossaForm.h"
#include "BossaThread.h"
#include "BossaProgress.h"

class BossaWindow : public MainFrame
{
public:
    BossaWindow();
    virtual ~BossaWindow();

private:
    BossaThread* _thread;
    BossaProgress* _progress;

    void RefreshSerial();
    void CreateFlash();
    void Connected();
    void Disconnected();

    void Error(const wxString& message);
    void Warning(const wxString& message);
    void Info(const wxString& message);
    bool Question(const wxString& message);

    void OnAbout(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnSerial(wxCommandEvent& event);
    void OnAutoScan(wxCommandEvent& event);

    void OnWrite(wxCommandEvent& event);
    void OnVerify(wxCommandEvent& event);
    void OnRead(wxCommandEvent& event);
    void OnInfo(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    void OnThreadProgress(wxCommandEvent& event);
    void OnThreadSuccess(wxCommandEvent& event);
    void OnThreadWarning(wxCommandEvent& event);
    void OnThreadError(wxCommandEvent& event);

    void OnProgressCancel(wxCommandEvent& event);
};

#endif // _BOSSAWINDOW_H
