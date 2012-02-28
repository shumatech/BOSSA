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
#ifndef _BOSSAPROGRESS_H
#define _BOSSAPROGRESS_H

#include "BossaForm.h"

DECLARE_EVENT_TYPE(wxEVT_PROGRESS_CANCEL, wxID_ANY)

class BossaProgress : public ProgressDialog
{
public:
    BossaProgress(wxWindow* parent);

    void SetValue(int pos);
    void SetLabel(const wxString& label);

private:
    wxEvtHandler* _parent;

    void OnCancel(wxCommandEvent& event);
};

#endif // _BOSSAPROGRESS_H
