#ifndef MAINPANEL
#define MAINPANEL
#pragma once

//#include "ParameterList.h"
#include "GlobalDefs.h"
#include "MoogDotsCom.h"
#include "StdAfx.h"

class CMainPanel : public wxPanel
{
private:
	wxButton *m_EngageButton,
			 *m_StopButton,
			 *m_ParkButton,
			 *m_ResetButton;
			 
	wxStaticBox *m_generalBox;				// Surrounds the main control buttons.
	int m_width, m_height;					// Width, height of this CMainPanel.
	wxListBox *m_moogListBox;				// Displays all available parameter list items.
	wxStaticText *m_descriptionText;		// Shows the parameter description.
	wxTextCtrl *m_dataTextBox;				// Displays the data associated with a parameter list key.
	wxListBox *m_messageConsole;			// Displays info about what the program is doing.
	wxButton *m_goButton,					// Executes the parameter list.
			 *m_setButton,					// Sets the data for a parameter.
			 *m_goToZeroButton;				// Makes the motion base move to zero position.
	MoogDotsCom *m_moogCom;

 
	wxBoxSizer *m_topSizer,
			   *m_upperSizer,
			   *m_upperRightSizer,
			   *m_parameterSizer,
			   *m_otherButtonsSizer;
	wxStaticBoxSizer *m_buttonSizer;

	Logger* m_logger;						//the program main logger.

public:
	wxRadioBox *m_radioBox;
	wxStaticText *m_alignmentDescription;

public:
	CMainPanel(wxWindow *parent, wxWindowID id, MoogDotsCom *com , Logger* logger);

	// Engages the motion base.
	void OnEngageButtonClicked(wxCommandEvent &event);

	// Parks the motion base.
	void OnParkButtonClicked(wxCommandEvent &event);

	// Resets the MBC.
	void OnResetButtonClicked(wxCommandEvent &event);

	// Performs actions whenever a list item is selected.
	void OnItemSelected(wxCommandEvent &event);

	// Starts a trial based on the current parameter list.
	void OnGoButtonClicked(wxCommandEvent &event);

	// Sets the data for the selected parameter list key.
	void OnSetButtonClicked(wxCommandEvent &event);

	// Stops all motion base movement.
	void OnStopButtonClicked(wxCommandEvent &event);

	// Moves the motion base back to zero position.
	void OnGoToZeroButtonClicked(wxCommandEvent &event);

	// For radio box control
	void OnModeSelected(wxCommandEvent &event);

	// Find a equation for smoothing alignment
	double FindEquation(int whichEye);

	virtual bool Enable(bool enable = TRUE);

private:
	// Creates all buttons.
	void initButtons();

	// Creates the Parameter List stuff.
	void initParameterListStuff();

	enum
	{
		ENGAGE_BUTTON,
		STOP_BUTTON,
		MOOG_LISTBOX,
		MOOG_GO_BUTTON,
		MOOG_SET_BUTTON,
		MOOG_ZERO_BUTTON,
		PARK_BUTTON,
		RESET_BUTTON,
		MODE_RADIO_BOX
	};

private:
	DECLARE_EVENT_TABLE()

};

#endif
