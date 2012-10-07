/*
 * Copyright (c) 2003-2012 Rony Shapiro <ronys@users.sourceforge.net>.
 * All rights reserved. Use of the code is allowed under the
 * Artistic License 2.0 terms, as specified in the LICENSE file
 * distributed with this code, or available from
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 */

/** \file safecombinationsetup.h
 * 
 */


// Generated by DialogBlocks, Wed 21 Jan 2009 09:07:57 PM IST

#ifndef _SAFECOMBINATIONSETUP_H_
#define _SAFECOMBINATIONSETUP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/valgen.h"
////@end includes
#include "YubiMixin.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations
class wxTimer;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SAFECOMBINATIONSETUP 10007
#define ID_PASSWORD 10008
#define ID_VERIFY 10009
#define ID_YUBIBTN 10229
#define ID_YUBISTATUS 10230
#define SYMBOL_CSAFECOMBINATIONSETUP_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#define SYMBOL_CSAFECOMBINATIONSETUP_TITLE _("Safe Combination Setup")
#define SYMBOL_CSAFECOMBINATIONSETUP_IDNAME ID_SAFECOMBINATIONSETUP
#define SYMBOL_CSAFECOMBINATIONSETUP_SIZE wxSize(400, 300)
#define SYMBOL_CSAFECOMBINATIONSETUP_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * CSafeCombinationSetup class declaration
 */

class CSafeCombinationSetup: public wxDialog, private CYubiMixin
{    
  DECLARE_DYNAMIC_CLASS( CSafeCombinationSetup )
  DECLARE_EVENT_TABLE()

public:
  /// Constructors
  CSafeCombinationSetup();
  CSafeCombinationSetup( wxWindow* parent, wxWindowID id = SYMBOL_CSAFECOMBINATIONSETUP_IDNAME, const wxString& caption = SYMBOL_CSAFECOMBINATIONSETUP_TITLE, const wxPoint& pos = SYMBOL_CSAFECOMBINATIONSETUP_POSITION, const wxSize& size = SYMBOL_CSAFECOMBINATIONSETUP_SIZE, long style = SYMBOL_CSAFECOMBINATIONSETUP_STYLE );

  /// Creation
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CSAFECOMBINATIONSETUP_IDNAME, const wxString& caption = SYMBOL_CSAFECOMBINATIONSETUP_TITLE, const wxPoint& pos = SYMBOL_CSAFECOMBINATIONSETUP_POSITION, const wxSize& size = SYMBOL_CSAFECOMBINATIONSETUP_SIZE, long style = SYMBOL_CSAFECOMBINATIONSETUP_STYLE );

  /// Destructor
  ~CSafeCombinationSetup();

  /// Initialises member variables
  void Init();

  /// Creates the controls and sizers
  void CreateControls();

  wxString GetPassword() const {return m_password;}

  ////@begin CSafeCombinationSetup event handler declarations

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_YUBIBTN
  void OnYubibtnClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
  void OnOkClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
  void OnCancelClick( wxCommandEvent& event );

////@end CSafeCombinationSetup event handler declarations
  void OnPollingTimer(wxTimerEvent& timerEvent);

////@begin CSafeCombinationSetup member function declarations

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource( const wxString& name );

  /// Retrieves icon resources
  wxIcon GetIconResource( const wxString& name );
////@end CSafeCombinationSetup member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

////@begin CSafeCombinationSetup member variables
  wxBitmapButton* m_YubiBtn;
  wxStaticText* m_yubiStatusCtrl;
////@end CSafeCombinationSetup member variables
 private:
  wxTimer* m_pollingTimer; // for Yubi, but can't go into mixin :-(
  wxString m_password;
  wxString m_verify;
};

#endif
  // _SAFECOMBINATIONSETUP_H_
