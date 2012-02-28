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
#include "BossaProgress.h"

DEFINE_EVENT_TYPE(wxEVT_PROGRESS_CANCEL)

BossaProgress::BossaProgress(wxWindow* parent) : ProgressDialog(parent), _parent(parent)
{
    _statusGauge->SetRange(100);
    SetValue(0);

    _sdbSizerCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                             wxCommandEventHandler(BossaProgress::OnCancel),
                             NULL, this);
}

void
BossaProgress::SetValue(int pos)
{
    _statusGauge->SetValue(pos);
#if __WIN32
    // Work around slow update on Windows
    _statusGauge->SetValue(pos - 1);
    _statusGauge->SetValue(pos);
#endif
}

void
BossaProgress::SetLabel(const wxString& label)
{
    _infoStaticText->SetLabel(label);
}

void
BossaProgress::OnCancel(wxCommandEvent& event)
{
    wxCommandEvent cmd(wxEVT_PROGRESS_CANCEL);
    _parent->AddPendingEvent(cmd);
}
