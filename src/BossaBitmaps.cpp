#include "BossaBitmaps.h"

#include <wx/mstream.h>

#include "BossaLogo.cpp"
#include "BossaIcon.cpp"
#include "ShumaTechLogo.cpp"

BossaBitmaps::BossaBitmaps()
{
    wxImage::AddHandler(new wxPNGHandler);

    _bossaLogo = GetBitmapFromMemory(BossaLogo_png, sizeof(BossaLogo_png));
    _bossaIcon = GetBitmapFromMemory(BossaIcon_png, sizeof(BossaIcon_png));
    _shumaTechLogo = GetBitmapFromMemory(ShumaTechLogo_png, sizeof(ShumaTechLogo_png));
}

wxBitmap
BossaBitmaps::GetBitmapFromMemory(const unsigned char *data, int length)
{
    wxMemoryInputStream is(data, length);
    return wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
}

