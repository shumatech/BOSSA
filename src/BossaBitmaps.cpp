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


