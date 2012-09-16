///////////////////////////////////////////////////////////////////////////////
// BOSSA
//
// Copyright (c) 2011-2012, ShumaTech
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
#include "BossaWindow.h"
#include "BossaApp.h"
#include "BossaAbout.h"
#include "BossaInfo.h"

#include "FlashFactory.h"

#include <string>

using namespace std;

BossaWindow::BossaWindow() : MainFrame(NULL)
{
    _bossaBitmap->SetBitmap(wxGetApp().bitmaps.getBossaLogo());

    _aboutButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler(BossaWindow::OnAbout),
                          NULL, this);

    _refreshButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(BossaWindow::OnRefresh),
                            NULL, this);

    _portComboBox->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED,
                           wxCommandEventHandler(BossaWindow::OnSerial),
                           NULL, this);

    _autoScanButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                             wxCommandEventHandler(BossaWindow::OnAutoScan),
                             NULL, this);

    _writeButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler(BossaWindow::OnWrite),
                          NULL, this);

    _verifyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                           wxCommandEventHandler(BossaWindow::OnVerify),
                           NULL, this);

    _readButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(BossaWindow::OnRead),
                         NULL, this);

    _infoButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(BossaWindow::OnInfo),
                         NULL, this);

    _exitButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                         wxCommandEventHandler(BossaWindow::OnExit),
                         NULL, this);

    Connect(wxEVT_THREAD_PROGRESS,
            wxCommandEventHandler(BossaWindow::OnThreadProgress),
            NULL, this);

    Connect(wxEVT_THREAD_SUCCESS,
            wxCommandEventHandler(BossaWindow::OnThreadSuccess),
            NULL, this);

    Connect(wxEVT_THREAD_WARNING,
            wxCommandEventHandler(BossaWindow::OnThreadWarning),
            NULL, this);

    Connect(wxEVT_THREAD_ERROR,
            wxCommandEventHandler(BossaWindow::OnThreadError),
            NULL, this);

    Connect(wxEVT_PROGRESS_CANCEL,
            wxCommandEventHandler(BossaWindow::OnProgressCancel),
            NULL, this);

    RefreshSerial();
    Disconnected();

    wxString port;
    wxConfig& config = wxGetApp().config;
    if (config.Read("Port", &port))
    {
        if (_portComboBox->FindString(port) >= 0)
        {
            PortFactory& portFactory = wxGetApp().portFactory;
            Samba& samba = wxGetApp().samba;

            if (samba.connect(portFactory.create(port.mb_str())))
            {
                CreateFlash();
            }
        }
    }

    wxString file;
    if (config.Read("File", &file))
    {
        _filePicker->SetPath(file);
    }

    wxIcon icon;
    icon.CopyFromBitmap(wxGetApp().bitmaps.getBossaIcon());
    SetIcon(icon);

    Iconize(false);
}

BossaWindow::~BossaWindow()
{
    wxConfig& config = wxGetApp().config;

    config.Write("Port", _portComboBox->GetStringSelection());
    config.Write("File", _filePicker->GetPath());
}

void
BossaWindow::RefreshSerial()
{
    string port;
    wxString selection = _portComboBox->GetStringSelection();
    PortFactory& portFactory = wxGetApp().portFactory;

    _portComboBox->Clear();
    for (port = portFactory.begin();
         port != portFactory.end();
         port = portFactory.next())
    {
        _portComboBox->Append(port.c_str());
    }

    if (!_portComboBox->SetStringSelection(selection))
        Disconnected();
}

void
BossaWindow::OnAbout(wxCommandEvent& event)
{
    BossaAbout* dialog = new BossaAbout(this);
    dialog->ShowModal();
    dialog->Destroy();
}

void
BossaWindow::OnRefresh(wxCommandEvent& event)
{
    RefreshSerial();
}

void
BossaWindow::Connected()
{
    Flash& flash = *wxGetApp().flash;
    Samba& samba = wxGetApp().samba;
    const SerialPort& port = samba.getSerialPort();

    _statusBar->SetStatusText(wxT("Connected"), 0);
    _statusBar->SetStatusText(wxString::Format(wxT("Device: %s"), flash.name().c_str()), 1);
    _portComboBox->SetStringSelection(port.name().c_str());
    _bootCheckBox->Enable(flash.canBootFlash());
    _bodCheckBox->Enable(flash.canBod());
    _borCheckBox->Enable(flash.canBor());
    _writeButton->Enable(true);
    _verifyButton->Enable(true);
    _readButton->Enable(true);
    _infoButton->Enable(true);
}

void
BossaWindow::Disconnected()
{
    _statusBar->SetStatusText(wxT("Not connected"), 0);
    _statusBar->SetStatusText("", 1);
    _portComboBox->SetSelection(wxNOT_FOUND);
    _writeButton->Enable(false);
    _verifyButton->Enable(false);
    _readButton->Enable(false);
    _infoButton->Enable(false);
}

void
BossaWindow::Error(const wxString& message)
{
    wxMessageDialog* dialog = new wxMessageDialog(
        this,
        message,
        "Error",
        wxOK | wxICON_ERROR
    );
    dialog->ShowModal();
    dialog->Destroy();
}

void
BossaWindow::Warning(const wxString& message)
{
    wxMessageDialog* dialog = new wxMessageDialog(
        this,
        message,
        "Warning",
        wxOK | wxICON_WARNING
    );
    dialog->ShowModal();
    dialog->Destroy();
}

void
BossaWindow::Info(const wxString& message)
{
    wxMessageDialog* dialog = new wxMessageDialog(
        this,
        message,
        "Info",
        wxOK | wxICON_INFORMATION
    );
    dialog->ShowModal();
    dialog->Destroy();
}

bool
BossaWindow::Question(const wxString& message)
{
    wxMessageDialog* dialog = new wxMessageDialog(
        this,
        message,
        "Question",
        wxYES_NO | wxICON_QUESTION
    );
    int resp = dialog->ShowModal();
    dialog->Destroy();

    return (resp == wxID_YES);
}

void
BossaWindow::CreateFlash()
{
    Samba& samba = wxGetApp().samba;
    Flash::Ptr& flash = wxGetApp().flash;
    FlashFactory flashFactory;
    uint32_t chipId;

    try
    {
        chipId = samba.chipId();
    }
    catch (exception& e)
    {
        Disconnected();
        Error(wxString(e.what()));
        return;
    }

    flash = flashFactory.create(samba, chipId);
    if (flash.get() == NULL)
    {
        Disconnected();
        Error(wxString::Format(wxT("Chip ID 0x%08x is not supported"), chipId));
        return;
    }

    _statusBar->SetStatusText(wxString::Format(wxT("Device: %s"), flash->name().c_str()), 1);
    Connected();
}

void
BossaWindow::OnSerial(wxCommandEvent& event)
{
    PortFactory& portFactory = wxGetApp().portFactory;
    Samba& samba = wxGetApp().samba;

    wxString port = _portComboBox->GetString(event.GetSelection());
    if (!samba.connect(portFactory.create(port.mb_str())))
    {
        Disconnected();
        Error(wxString::Format(wxT("Could not connect to device on %s"), port.mb_str()));
        return;
    }

    CreateFlash();
}

void
BossaWindow::OnAutoScan(wxCommandEvent& event)
{
    string port;
    PortFactory& portFactory = wxGetApp().portFactory;
    Samba& samba = wxGetApp().samba;

    RefreshSerial();

    for (port = portFactory.begin();
         port != portFactory.end();
         port = portFactory.next())
    {
        if (samba.connect(portFactory.create(port)))
        {
            CreateFlash();
            return;
        }
    }

    Disconnected();
    Error(wxString(wxT("Could not find a device")));
}

void
BossaWindow::OnWrite(wxCommandEvent& event)
{
    Flash& flash = *wxGetApp().flash;

    if (_filePicker->GetPath().IsEmpty())
    {
        Error(wxT("You must specify a file first"));
        return;
    }

    if (access(_filePicker->GetPath().mb_str(), F_OK))
    {
        Error(wxT("File does not exist"));
        return;
    }

    try
    {
        if (flash.isLocked())
        {
            if (!Question(wxT("The flash is currently locked. Do you want to unlock it and proceed with the write?")))
                return;

            flash.unlockAll();
        }
    }
    catch(exception& e)
    {
        Error(e.what());
        return;
    }

    _progress = new BossaProgress(this);
    _thread = new WriteThread(
        this,
        _filePicker->GetPath(),
        _eraseCheckBox->GetValue(),
        _bootCheckBox->GetValue(),
        _bodCheckBox->GetValue(),
        _borCheckBox->GetValue(),
        _lockCheckBox->GetValue(),
        _securityCheckBox->GetValue()
    );

    if (_thread->Create() != wxTHREAD_NO_ERROR ||
        _thread->Run() != wxTHREAD_NO_ERROR)
    {
        Error(wxT("Unable to start worker thread"));
        return;
    }

    _progress->ShowModal();
    _progress->Destroy();
}

void
BossaWindow::OnVerify(wxCommandEvent& event)
{
    if (_filePicker->GetPath().IsEmpty())
    {
        Error(wxT("You must specify a file first"));
        return;
    }

    if (access(_filePicker->GetPath().mb_str(), F_OK))
    {
        Error(wxT("File does not exist"));
        return;
    }

    _progress = new BossaProgress(this);
    _thread = new VerifyThread(
        this,
        _filePicker->GetPath()
    );

    if (_thread->Create() != wxTHREAD_NO_ERROR ||
        _thread->Run() != wxTHREAD_NO_ERROR)
    {
        Error(wxT("Unable to start worker thread"));
        return;
    }

    _progress->ShowModal();
    _progress->Destroy();
}

void
BossaWindow::OnRead(wxCommandEvent& event)
{
    size_t size;

    if (_filePicker->GetPath().IsEmpty())
    {
        Error(wxT("You must specify a file first"));
        return;
    }

    if (!access(_filePicker->GetPath().mb_str(), F_OK))
    {
        if (!Question(wxT("File already exists. Do you want to replace it?")))
            return;
    }

    if (_sizeTextCtrl->GetValue().IsEmpty())
    {
        size = 0;
    }
    else
    {
        size = strtol(_sizeTextCtrl->GetValue().mb_str(), NULL, 0);
        if (size == 0)
        {
            Error("Read size is invalid");
            return;
        }
    }

    _progress = new BossaProgress(this);
    _thread = new ReadThread(
        this,
        _filePicker->GetPath(),
        size
    );

    if (_thread->Create() != wxTHREAD_NO_ERROR ||
        _thread->Run() != wxTHREAD_NO_ERROR)
    {
        Error(wxT("Unable to start worker thread"));
        return;
    }

    _progress->ShowModal();
    _progress->Destroy();
}

void
BossaWindow::OnInfo(wxCommandEvent& event)
{
    BossaInfo* info;
    try
    {
        info = new BossaInfo(this);
    }
    catch (exception& e)
    {
        Error(wxString(e.what()));
        return;
    }

    info->ShowModal();
    info->Destroy();
}

void
BossaWindow::OnExit(wxCommandEvent& event)
{
    Destroy();
}

void
BossaWindow::OnThreadProgress(wxCommandEvent& event)
{
    _progress->SetValue(event.GetInt());
    _progress->SetLabel(event.GetString());
}

void
BossaWindow::OnThreadSuccess(wxCommandEvent& event)
{
    wxString message = event.GetString();
    if (!message.IsEmpty())
        Info(message);
    _progress->EndModal(0);
}

void
BossaWindow::OnThreadWarning(wxCommandEvent& event)
{
    wxString message = event.GetString();
    Warning(message);
    _progress->EndModal(0);
}

void
BossaWindow::OnThreadError(wxCommandEvent& event)
{
    wxString message = event.GetString();
    Error(message);
    _progress->EndModal(0);
}

void
BossaWindow::OnProgressCancel(wxCommandEvent& event)
{
    _thread->stop();
}
