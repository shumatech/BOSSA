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
