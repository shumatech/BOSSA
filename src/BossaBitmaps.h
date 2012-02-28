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
#ifndef _BOSSABITMAPS_H
#define _BOSSABITMAPS_H

#include <wx/wx.h>

class BossaBitmaps
{
public:
    BossaBitmaps();

    void init();

    const wxBitmap& getBossaLogo() { return _bossaLogo; }
    const wxBitmap& getBossaIcon() { return _bossaIcon; }
    const wxBitmap& getShumaTechLogo() { return _shumaTechLogo; }

private:
    wxBitmap GetBitmapFromMemory(const unsigned char *data, int length);

    wxBitmap _bossaLogo;
    wxBitmap _bossaIcon;
    wxBitmap _shumaTechLogo;
};

#endif // _BOSSABITMAPS_H
