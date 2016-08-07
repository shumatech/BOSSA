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
