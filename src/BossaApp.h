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
#ifndef _BOSSAAPP_H
#define _BOSSAAPP_H

#include <wx/wx.h>
#include <wx/config.h>

#include "PortFactory.h"
#include "Samba.h"
#include "Flash.h"
#include "BossaBitmaps.h"
#include "BossaWindow.h"

class BossaApp : public wxApp
{
public:
    BossaApp();

    PortFactory portFactory;
    Samba samba;
    BossaBitmaps bitmaps;
    Flash::Ptr flash;
    wxConfig config;

private:
    bool OnInit();

    BossaWindow* _window;
};

DECLARE_APP(BossaApp)

#endif // _BOSSAAPP_H
