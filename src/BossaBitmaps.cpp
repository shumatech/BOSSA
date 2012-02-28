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
#include "BossaBitmaps.h"

#include <wx/mstream.h>

#include "BossaLogo.cpp"
#include "BossaIcon.cpp"
#include "ShumaTechLogo.cpp"

BossaBitmaps::BossaBitmaps()
{
    wxImage::AddHandler(new wxBMPHandler);
}

void
BossaBitmaps::init()
{
    _bossaLogo = GetBitmapFromMemory(BossaLogo_bmp, sizeof(BossaLogo_bmp));
    _bossaIcon = GetBitmapFromMemory(BossaIcon_bmp, sizeof(BossaIcon_bmp));
    _shumaTechLogo = GetBitmapFromMemory(ShumaTechLogo_bmp, sizeof(ShumaTechLogo_bmp));
}

wxBitmap
BossaBitmaps::GetBitmapFromMemory(const unsigned char *data, int length)
{
    wxMemoryInputStream is(data, length);
    return wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
}


