///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 17 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BossaForm__
#define __BossaForm__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/statbox.h>
#include <wx/filepicker.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/dialog.h>
#include <wx/statline.h>
#include <wx/hyperlink.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxStaticBitmap* _bossaBitmap;
		wxStaticText* _titleText;
		
		wxButton* _aboutButton;
		wxComboBox* _portComboBox;
		wxButton* _refreshButton;
		
		wxButton* _autoScanButton;
		wxFilePickerCtrl* _filePicker;
		wxCheckBox* _eraseCheckBox;
		wxCheckBox* _bodCheckBox;
		wxCheckBox* _lockCheckBox;
		wxCheckBox* _bootCheckBox;
		wxCheckBox* _borCheckBox;
		wxCheckBox* _securityCheckBox;
		wxStaticText* _sizeStaticText;
		wxTextCtrl* _sizeTextCtrl;
		
		wxButton* _writeButton;
		
		wxButton* _verifyButton;
		
		wxButton* _readButton;
		
		wxButton* _infoButton;
		
		wxButton* _exitButton;
		
		wxStatusBar* _statusBar;
	
	public:
		
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("BOSSA"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,400 ), long style = wxCAPTION|wxCLOSE_BOX|wxICONIZE|wxMINIMIZE|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		
		~MainFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProgressDialog
///////////////////////////////////////////////////////////////////////////////
class ProgressDialog : public wxDialog 
{
	private:
	
	protected:
		
		wxStaticText* _infoStaticText;
		wxGauge* _statusGauge;
		
		wxStdDialogButtonSizer* _sdbSizer;
		wxButton* _sdbSizerCancel;
		
	
	public:
		
		ProgressDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Progress"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,150 ), long style = wxCAPTION ); 
		~ProgressDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialog
///////////////////////////////////////////////////////////////////////////////
class AboutDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticBitmap* _bossaBitmap;
		wxStaticText* _titleStaticText;
		wxStaticText* _versionStaticText;
		wxStaticText* _wxStaticText;
		wxStaticLine* m_staticline1;
		wxStaticBitmap* _shumatechBitmap;
		wxStaticText* _copyrightStaticText;
		wxHyperlinkCtrl* _shumatechHyperlink;
		wxStaticLine* m_staticline2;
		wxStaticText* m_disclaimerStaticText;
		wxStaticLine* m_staticline3;
		wxStdDialogButtonSizer* _sdbSizer;
		wxButton* _sdbSizerOK;
	
	public:
		
		AboutDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About BOSSA"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~AboutDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class InfoDialog
///////////////////////////////////////////////////////////////////////////////
class InfoDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* _deviceStaticText;
		wxTextCtrl* _deviceTextCtrl;
		wxStaticText* _chipIdStaticText;
		wxTextCtrl* _chipIdTextCtrl;
		wxStaticText* _versionStaticText;
		wxTextCtrl* _versionTextCtrl;
		wxStaticText* _pagesStaticText;
		wxTextCtrl* _pagesTextCtrl;
		wxStaticText* _pageSizeStaticText;
		wxTextCtrl* _pageSizeTextCtrl;
		wxStaticText* _totalSizeStaticText;
		wxTextCtrl* _totalSizeTextCtrl;
		wxStaticText* _planesStaticText;
		wxTextCtrl* _planesTextCtrl;
		wxCheckBox* _bootCheckBox;
		wxCheckBox* _bodCheckBox;
		wxCheckBox* _securityCheckBox;
		wxCheckBox* _borCheckBox;
		wxStaticText* _lockStaticText;
		wxCheckBox* _lockCheckBox0;
		wxCheckBox* _lockCheckBox1;
		wxCheckBox* _lockCheckBox2;
		wxCheckBox* _lockCheckBox3;
		wxCheckBox* _lockCheckBox4;
		wxCheckBox* _lockCheckBox5;
		wxCheckBox* _lockCheckBox6;
		wxCheckBox* _lockCheckBox7;
		wxCheckBox* _lockCheckBox8;
		wxCheckBox* _lockCheckBox9;
		wxCheckBox* _lockCheckBox10;
		wxCheckBox* _lockCheckBox11;
		wxCheckBox* _lockCheckBox12;
		wxCheckBox* _lockCheckBox13;
		wxCheckBox* _lockCheckBox14;
		wxCheckBox* _lockCheckBox15;
		wxCheckBox* _lockCheckBox16;
		wxCheckBox* _lockCheckBox17;
		wxCheckBox* _lockCheckBox18;
		wxCheckBox* _lockCheckBox19;
		wxCheckBox* _lockCheckBox20;
		wxCheckBox* _lockCheckBox21;
		wxCheckBox* _lockCheckBox22;
		wxCheckBox* _lockCheckBox23;
		wxCheckBox* _lockCheckBox24;
		wxCheckBox* _lockCheckBox25;
		wxCheckBox* _lockCheckBox26;
		wxCheckBox* _lockCheckBox27;
		wxCheckBox* _lockCheckBox28;
		wxCheckBox* _lockCheckBox29;
		wxCheckBox* _lockCheckBox30;
		wxCheckBox* _lockCheckBox31;
		wxStdDialogButtonSizer* _sdbSizer;
		wxButton* _sdbSizerOK;
	
	public:
		
		InfoDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Info"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~InfoDialog();
	
};

#endif //__BossaForm__
