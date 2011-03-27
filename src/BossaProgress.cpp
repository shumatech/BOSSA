#include "BossaProgress.h"

BossaProgress::BossaProgress(wxWindow* parent) : ProgressDialog(parent)
{
    _statusGauge->SetRange(100);
    SetValue(0);
}

void
BossaProgress::SetValue(int pos)
{
    // Work around slow update on Windows 7
    _statusGauge->SetValue(pos + 1);
    _statusGauge->SetValue(pos);
}

void
BossaProgress::SetLabel(const wxString& label)
{
    _infoStaticText->SetLabel(label);
}
