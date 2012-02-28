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
#include "BossaAbout.h"
#include "BossaApp.h"

#include "wx/version.h"

BossaAbout::BossaAbout(wxWindow* parent) : AboutDialog(parent)
{
    BossaApp& app = wxGetApp();

    _bossaBitmap->SetBitmap(app.bitmaps.getBossaLogo());
    _shumatechBitmap->SetBitmap(app.bitmaps.getShumaTechLogo());
    _versionStaticText->SetLabel(wxString::Format(wxT("Version: %s"), VERSION));
    _wxStaticText->SetLabel(wxString::Format(wxT("Built with %s"), wxVERSION_STRING));
    GetSizer()->Fit(this);
}
