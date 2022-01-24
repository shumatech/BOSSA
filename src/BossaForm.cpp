///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 13 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "BossaForm.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	wxBoxSizer* _topBoxSizer;
	_topBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* _titleBoxSizer;
	_titleBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	_bossaBitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	_titleBoxSizer->Add( _bossaBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_titleText = new wxStaticText( this, wxID_ANY, wxT("Flash Programmer for Atmel SAM Devices"), wxDefaultPosition, wxDefaultSize, 0 );
	_titleText->Wrap( -1 );
	_titleBoxSizer->Add( _titleText, 0, wxALL|wxALIGN_BOTTOM, 5 );
	
	
	_titleBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_aboutButton = new wxButton( this, wxID_ANY, wxT("About"), wxDefaultPosition, wxDefaultSize, 0 );
	_aboutButton->SetToolTip( wxT("Display information about BOSSA") );
	
	_titleBoxSizer->Add( _aboutButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_topBoxSizer->Add( _titleBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* _portBoxSizer;
	_portBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Serial Port") ), wxHORIZONTAL );
	
	_portComboBox = new wxComboBox( _portBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	_portBoxSizer->Add( _portComboBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_portBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_refreshButton = new wxButton( _portBoxSizer->GetStaticBox(), wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	_refreshButton->SetToolTip( wxT("Refresh the list of available serial ports") );
	
	_portBoxSizer->Add( _refreshButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_topBoxSizer->Add( _portBoxSizer, 3, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* _fileBoxSizer;
	_fileBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("File") ), wxHORIZONTAL );
	
	_filePicker = new wxFilePickerCtrl( _fileBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	_filePicker->SetToolTip( wxT("Select the file to use for write/verify/read operations") );
	
	_fileBoxSizer->Add( _filePicker, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_topBoxSizer->Add( _fileBoxSizer, 3, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* _optionsBoxSizer;
	_optionsBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* _writeBoxSizer;
	_writeBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Write Options") ), wxHORIZONTAL );
	
	wxGridSizer* _writeGridSizer;
	_writeGridSizer = new wxGridSizer( 3, 2, 0, 0 );
	
	_eraseCheckBox = new wxCheckBox( _writeBoxSizer->GetStaticBox(), wxID_ANY, wxT("Erase all"), wxDefaultPosition, wxDefaultSize, 0 );
	_eraseCheckBox->SetToolTip( wxT("Erase entire flash before writing (recommended)") );
	
	_writeGridSizer->Add( _eraseCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_bootCheckBox = new wxCheckBox( _writeBoxSizer->GetStaticBox(), wxID_ANY, wxT("Boot to flash"), wxDefaultPosition, wxDefaultSize, 0 );
	_bootCheckBox->SetToolTip( wxT("Boot processor to flash instead of SAM-BA (if supported)") );
	
	_writeGridSizer->Add( _bootCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_bodCheckBox = new wxCheckBox( _writeBoxSizer->GetStaticBox(), wxID_ANY, wxT("Brownout detect"), wxDefaultPosition, wxDefaultSize, 0 );
	_bodCheckBox->SetToolTip( wxT("Enable the brownout detection circuitry") );
	
	_writeGridSizer->Add( _bodCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_borCheckBox = new wxCheckBox( _writeBoxSizer->GetStaticBox(), wxID_ANY, wxT("Brownout reset"), wxDefaultPosition, wxDefaultSize, 0 );
	_borCheckBox->SetToolTip( wxT("Enable processor reset on brownout detect") );
	
	_writeGridSizer->Add( _borCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_lockCheckBox = new wxCheckBox( _writeBoxSizer->GetStaticBox(), wxID_ANY, wxT("Lock"), wxDefaultPosition, wxDefaultSize, 0 );
	_lockCheckBox->SetToolTip( wxT("Lock access to the flash") );
	
	_writeGridSizer->Add( _lockCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_securityCheckBox = new wxCheckBox( _writeBoxSizer->GetStaticBox(), wxID_ANY, wxT("Security"), wxDefaultPosition, wxDefaultSize, 0 );
	_securityCheckBox->SetToolTip( wxT("Set the security bit") );
	
	_writeGridSizer->Add( _securityCheckBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_writeBoxSizer->Add( _writeGridSizer, 1, wxEXPAND, 5 );
	
	
	_optionsBoxSizer->Add( _writeBoxSizer, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* _readBoxSizer;
	_readBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Read Options") ), wxVERTICAL );
	
	wxBoxSizer* _sizeBoxSizer;
	_sizeBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	_sizeStaticText = new wxStaticText( _readBoxSizer->GetStaticBox(), wxID_ANY, wxT("Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	_sizeStaticText->Wrap( -1 );
	_sizeBoxSizer->Add( _sizeStaticText, 0, wxALL, 5 );
	
	_sizeTextCtrl = new wxTextCtrl( _readBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	_sizeTextCtrl->SetToolTip( wxT("Specify the amount of flash to read") );
	
	_sizeBoxSizer->Add( _sizeTextCtrl, 0, wxALL, 5 );
	
	
	_readBoxSizer->Add( _sizeBoxSizer, 1, wxEXPAND, 5 );
	
	
	_optionsBoxSizer->Add( _readBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* _generalBoxSizer;
	_generalBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("General Options") ), wxVERTICAL );
	
	wxBoxSizer* _offsetBoxSizer;
	_offsetBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	_offsetStaticText = new wxStaticText( _generalBoxSizer->GetStaticBox(), wxID_ANY, wxT("Flash Offset:"), wxDefaultPosition, wxDefaultSize, 0 );
	_offsetStaticText->Wrap( -1 );
	_offsetBoxSizer->Add( _offsetStaticText, 0, wxALL, 5 );
	
	_offsetTextCtrl = new wxTextCtrl( _generalBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	_offsetTextCtrl->SetToolTip( wxT("Specify the offset into the flash for operations") );
	
	_offsetBoxSizer->Add( _offsetTextCtrl, 0, wxALL, 5 );
	
	
	_generalBoxSizer->Add( _offsetBoxSizer, 1, wxEXPAND, 5 );
	
	
	_optionsBoxSizer->Add( _generalBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	
	_topBoxSizer->Add( _optionsBoxSizer, 4, wxEXPAND, 5 );
	
	wxBoxSizer* _buttonBoxSizer;
	_buttonBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	_buttonBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_writeButton = new wxButton( this, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	_writeButton->SetToolTip( wxT("Write the flash with the file above") );
	
	_buttonBoxSizer->Add( _writeButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_buttonBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_verifyButton = new wxButton( this, wxID_ANY, wxT("Verify"), wxDefaultPosition, wxDefaultSize, 0 );
	_verifyButton->SetToolTip( wxT("Verify the flash matches the file above") );
	
	_buttonBoxSizer->Add( _verifyButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_buttonBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_readButton = new wxButton( this, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	_readButton->SetToolTip( wxT("Read the flash into the file above") );
	
	_buttonBoxSizer->Add( _readButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_buttonBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_infoButton = new wxButton( this, wxID_ANY, wxT("Info"), wxDefaultPosition, wxDefaultSize, 0 );
	_infoButton->SetToolTip( wxT("Display inforamtion aboth the connected processor") );
	
	_buttonBoxSizer->Add( _infoButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_buttonBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_exitButton = new wxButton( this, wxID_ANY, wxT("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	_exitButton->SetToolTip( wxT("Exit and close BOSSA") );
	
	_buttonBoxSizer->Add( _exitButton, 0, wxALL, 5 );
	
	
	_buttonBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	_topBoxSizer->Add( _buttonBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( _topBoxSizer );
	this->Layout();
	_statusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
}

MainFrame::~MainFrame()
{
}

ProgressDialog::ProgressDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	wxBoxSizer* _progressBoxSizer;
	_progressBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	
	_progressBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_infoStaticText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	_infoStaticText->Wrap( -1 );
	_progressBoxSizer->Add( _infoStaticText, 0, wxALL|wxEXPAND, 5 );		// DC do not use wxALIGN_CENTER with wxEXPAND, it triggers an assertion failure in wxWidgets
	
	_statusGauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	_statusGauge->SetValue( 0 ); 
	_progressBoxSizer->Add( _statusGauge, 0, wxALL|wxEXPAND, 5 );
	
	
	_progressBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	_sdbSizer = new wxStdDialogButtonSizer();
	_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	_sdbSizer->AddButton( _sdbSizerCancel );
	_sdbSizer->Realize();
	
	_progressBoxSizer->Add( _sdbSizer, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	_progressBoxSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( _progressBoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

ProgressDialog::~ProgressDialog()
{
}

AboutDialog::AboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	wxBoxSizer* _topBoxSizer;
	_topBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	_bossaBitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	_topBoxSizer->Add( _bossaBitmap, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	_titleStaticText = new wxStaticText( this, wxID_ANY, wxT("Basic Open Source SAM-BA Application"), wxDefaultPosition, wxDefaultSize, 0 );
	_titleStaticText->Wrap( -1 );
	_topBoxSizer->Add( _titleStaticText, 0, wxALL, 5 );
	
	_versionStaticText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	_versionStaticText->Wrap( -1 );
	_topBoxSizer->Add( _versionStaticText, 0, wxALL, 5 );
	
	_wxStaticText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	_wxStaticText->Wrap( -1 );
	_topBoxSizer->Add( _wxStaticText, 0, wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	_topBoxSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	_shumatechBitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	_topBoxSizer->Add( _shumatechBitmap, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	_copyrightStaticText = new wxStaticText( this, wxID_ANY, wxT("(c) 2011-2018 ShumaTech. All rights reserved."), wxDefaultPosition, wxDefaultSize, 0 );
	_copyrightStaticText->Wrap( -1 );
	_topBoxSizer->Add( _copyrightStaticText, 0, wxALL, 5 );
	
	_shumatechHyperlink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("http://www.shumatech.com"), wxT("http://www.shumatech.com"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	_topBoxSizer->Add( _shumatechHyperlink, 0, wxALL, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	_topBoxSizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_disclaimerStaticText = new wxStaticText( this, wxID_ANY, wxT("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."), wxDefaultPosition, wxDefaultSize, 0 );
	m_disclaimerStaticText->Wrap( 280 );
	_topBoxSizer->Add( m_disclaimerStaticText, 0, wxALL, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	_topBoxSizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	_sdbSizer = new wxStdDialogButtonSizer();
	_sdbSizerOK = new wxButton( this, wxID_OK );
	_sdbSizer->AddButton( _sdbSizerOK );
	_sdbSizer->Realize();
	
	_topBoxSizer->Add( _sdbSizer, 1, wxALL|wxEXPAND, 10 );
	
	
	this->SetSizer( _topBoxSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

AboutDialog::~AboutDialog()
{
}

InfoDialog::InfoDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	wxBoxSizer* _topBoxSizer;
	_topBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* _deviceBoxSizer;
	_deviceBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	_deviceStaticText = new wxStaticText( this, wxID_ANY, wxT("Device:"), wxDefaultPosition, wxDefaultSize, 0 );
	_deviceStaticText->Wrap( -1 );
	_deviceBoxSizer->Add( _deviceStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_deviceTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	_deviceBoxSizer->Add( _deviceTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_topBoxSizer->Add( _deviceBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* _versionBoxSizer;
	_versionBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	_versionStaticText = new wxStaticText( this, wxID_ANY, wxT("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	_versionStaticText->Wrap( -1 );
	_versionBoxSizer->Add( _versionStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_versionTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	_versionBoxSizer->Add( _versionTextCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_topBoxSizer->Add( _versionBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* _middleBoxSizer;
	_middleBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* _flashBoxSizer;
	_flashBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Flash") ), wxVERTICAL );
	
	wxFlexGridSizer* _flashGridSizer;
	_flashGridSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	_flashGridSizer->SetFlexibleDirection( wxBOTH );
	_flashGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	_pagesStaticText = new wxStaticText( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxT("Pages:"), wxDefaultPosition, wxDefaultSize, 0 );
	_pagesStaticText->Wrap( -1 );
	_flashGridSizer->Add( _pagesStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_pagesTextCtrl = new wxTextCtrl( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	_flashGridSizer->Add( _pagesTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_pageSizeStaticText = new wxStaticText( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxT("Page Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	_pageSizeStaticText->Wrap( -1 );
	_flashGridSizer->Add( _pageSizeStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_pageSizeTextCtrl = new wxTextCtrl( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	_flashGridSizer->Add( _pageSizeTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_totalSizeStaticText = new wxStaticText( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxT("Total Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	_totalSizeStaticText->Wrap( -1 );
	_flashGridSizer->Add( _totalSizeStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_totalSizeTextCtrl = new wxTextCtrl( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	_flashGridSizer->Add( _totalSizeTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_planesStaticText = new wxStaticText( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxT("Planes:"), wxDefaultPosition, wxDefaultSize, 0 );
	_planesStaticText->Wrap( -1 );
	_flashGridSizer->Add( _planesStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_planesTextCtrl = new wxTextCtrl( _flashBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	_flashGridSizer->Add( _planesTextCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	_flashBoxSizer->Add( _flashGridSizer, 1, wxEXPAND, 5 );
	
	
	_middleBoxSizer->Add( _flashBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* _optionsBoxSizer;
	_optionsBoxSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Options") ), wxVERTICAL );
	
	wxFlexGridSizer* _gpnvGridSizer;
	_gpnvGridSizer = new wxFlexGridSizer( 0, 2, 0, 0 );
	_gpnvGridSizer->SetFlexibleDirection( wxBOTH );
	_gpnvGridSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	_bootCheckBox = new wxCheckBox( _optionsBoxSizer->GetStaticBox(), wxID_ANY, wxT("Boot to flash"), wxDefaultPosition, wxDefaultSize, 0 );
	_gpnvGridSizer->Add( _bootCheckBox, 0, wxALL, 5 );
	
	_bodCheckBox = new wxCheckBox( _optionsBoxSizer->GetStaticBox(), wxID_ANY, wxT("Brownout detect"), wxDefaultPosition, wxDefaultSize, 0 );
	_gpnvGridSizer->Add( _bodCheckBox, 0, wxALL, 5 );
	
	_securityCheckBox = new wxCheckBox( _optionsBoxSizer->GetStaticBox(), wxID_ANY, wxT("Security"), wxDefaultPosition, wxDefaultSize, 0 );
	_gpnvGridSizer->Add( _securityCheckBox, 0, wxALL, 5 );
	
	_borCheckBox = new wxCheckBox( _optionsBoxSizer->GetStaticBox(), wxID_ANY, wxT("Brownout reset"), wxDefaultPosition, wxDefaultSize, 0 );
	_gpnvGridSizer->Add( _borCheckBox, 0, wxALL, 5 );
	
	
	_optionsBoxSizer->Add( _gpnvGridSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	_lockStaticText = new wxStaticText( _optionsBoxSizer->GetStaticBox(), wxID_ANY, wxT("Lock Regions:"), wxDefaultPosition, wxDefaultSize, 0 );
	_lockStaticText->Wrap( -1 );
	bSizer13->Add( _lockStaticText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	_lockTextCtrl = new wxTextCtrl( _optionsBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer13->Add( _lockTextCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	_optionsBoxSizer->Add( bSizer13, 1, wxEXPAND, 5 );
	
	
	_middleBoxSizer->Add( _optionsBoxSizer, 0, wxALL|wxEXPAND, 5 );
	
	
	_topBoxSizer->Add( _middleBoxSizer, 0, wxEXPAND, 5 );
	
	_sdbSizer = new wxStdDialogButtonSizer();
	_sdbSizerOK = new wxButton( this, wxID_OK );
	_sdbSizer->AddButton( _sdbSizerOK );
	_sdbSizer->Realize();
	
	_topBoxSizer->Add( _sdbSizer, 1, wxALL|wxEXPAND, 10 );
	
	
	this->SetSizer( _topBoxSizer );
	this->Layout();
	_topBoxSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

InfoDialog::~InfoDialog()
{
}
