#include "BossaProgress.h"

DEFINE_EVENT_TYPE(wxEVT_PROGRESS_CANCEL)

BossaProgress::BossaProgress(wxWindow* parent) : ProgressDialog(parent), _parent(parent)
{
    _statusGauge->SetRange(100);
    SetValue(0);

    _sdbSizerCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                             wxCommandEventHandler(BossaProgress::OnCancel),
                             NULL, this);
}

void
BossaProgress::SetValue(int pos)
{
    _statusGauge->SetValue(pos);
#if __WIN32
    // Work around slow update on Windows
    _statusGauge->SetValue(pos - 1);
    _statusGauge->SetValue(pos);
#endif
}

void
BossaProgress::SetLabel(const wxString& label)
{
    _infoStaticText->SetLabel(label);
}

void
BossaProgress::OnCancel(wxCommandEvent& event)
{
    wxCommandEvent cmd(wxEVT_PROGRESS_CANCEL);
    _parent->AddPendingEvent(cmd);
}
