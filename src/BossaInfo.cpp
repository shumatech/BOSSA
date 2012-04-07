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
#include "BossaInfo.h"
#include "BossaApp.h"

#include <vector>

BossaInfo::BossaInfo(wxWindow* parent) : InfoDialog(parent)
{
    Samba& samba = wxGetApp().samba;
    Flash::Ptr& flash = wxGetApp().flash;

    _deviceTextCtrl->SetValue(wxString(flash->name().c_str(), wxConvUTF8));
    _chipIdTextCtrl->SetValue(wxString::Format(wxT("%08x"), samba.chipId()));
    _versionTextCtrl->SetValue(wxString(samba.version().c_str(), wxConvUTF8));

    _pagesTextCtrl->SetValue(wxString::Format(wxT("%d"), flash->numPages()));
    _pageSizeTextCtrl->SetValue(wxString::Format(wxT("%d bytes"), flash->pageSize()));
    _totalSizeTextCtrl->SetValue(wxString::Format(wxT("%d KB"),flash->numPages() * flash->pageSize() / 1024));
    _planesTextCtrl->SetValue(wxString::Format(wxT("%d"),flash->numPlanes()));

    _bootCheckBox->Enable(false);
    _securityCheckBox->Enable(false);
    _bodCheckBox->Enable(false);
    _borCheckBox->Enable(false);

    _bootCheckBox->SetValue(flash->getBootFlash());
    _securityCheckBox->SetValue(flash->getSecurity());
    _bodCheckBox->SetValue(flash->getBod());
    _borCheckBox->SetValue(flash->getBor());

    uint32_t lockRegions = flash->lockRegions();
    wxCheckBox* lockCheckBox[32] = {
        _lockCheckBox0, _lockCheckBox1, _lockCheckBox2, _lockCheckBox3,
        _lockCheckBox4, _lockCheckBox5, _lockCheckBox6, _lockCheckBox7,
        _lockCheckBox8, _lockCheckBox9, _lockCheckBox10, _lockCheckBox11,
        _lockCheckBox12, _lockCheckBox13, _lockCheckBox14, _lockCheckBox15,
        _lockCheckBox16, _lockCheckBox17, _lockCheckBox18, _lockCheckBox19,
        _lockCheckBox20, _lockCheckBox21, _lockCheckBox22, _lockCheckBox23,
        _lockCheckBox24, _lockCheckBox25, _lockCheckBox26, _lockCheckBox27,
        _lockCheckBox28, _lockCheckBox29, _lockCheckBox30, _lockCheckBox31,
    };

    for (uint32_t i = 0; i < sizeof(lockCheckBox) / sizeof(lockCheckBox[0]); i++)
    {
        if (i >= lockRegions)
        {
            lockCheckBox[i]->Destroy();
        }
        else
        {
            lockCheckBox[i]->Enable(false);
            lockCheckBox[i]->SetValue(flash->getLockRegion(i));
        }
    }

    GetSizer()->Fit(this);
}
