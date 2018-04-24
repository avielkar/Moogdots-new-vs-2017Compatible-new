
#include "MainPanel.h"

// Parameter list -- Original declaration can be found in ParameterList.cpp
extern CParameterList g_pList;

CMainPanel::CMainPanel(wxWindow *parent, wxWindowID id, MoogDotsCom *com , Logger* logger) :
wxPanel(parent, id), m_moogCom(com), m_logger(logger)
{
	parent->GetClientSize(&m_width, &m_height);

	// Create all the sizer for the main panel.
	m_topSizer = new wxBoxSizer(wxVERTICAL);
	m_upperSizer = new wxBoxSizer(wxHORIZONTAL);
	m_upperRightSizer = new wxBoxSizer(wxVERTICAL);
	m_parameterSizer = new wxBoxSizer(wxHORIZONTAL);
	m_otherButtonsSizer = new wxBoxSizer(wxVERTICAL);

	m_generalBox = new wxStaticBox(this, -1, "General Controls", wxDefaultPosition, wxSize(m_width-10, 52));
	m_buttonSizer = new wxStaticBoxSizer(m_generalBox, wxHORIZONTAL);
	
	// Create all general buttons.
	initButtons();
    
	// Setup Alignment radio box chioces
	wxString choices[3] = {"Flat Screen Mode","Curve Screen Mode", "Alignment Mode"};
	m_radioBox = new wxRadioBox(this, MODE_RADIO_BOX, "Screen and Alignment Control", wxDefaultPosition, wxDefaultSize, 3, choices);
	//m_radioBox->Enable(false);

	m_alignmentDescription = new wxStaticText(this, -1, "Using Flat Screen.", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	

	// Create all the parameter list stuff.
	initParameterListStuff();

	// Create the message console.
	m_messageConsole = new wxListBox(this, -1, wxPoint(10, 185), wxSize(m_width-20, 100), 0, NULL, wxLB_HSCROLL);
	m_upperRightSizer->Add(m_messageConsole, 1, wxGROW | wxALL, 5);

	m_upperSizer->Add(m_moogListBox, 0, wxGROW | wxALL, 5);
	m_upperSizer->Add(m_upperRightSizer, 1, wxGROW);
	m_topSizer->Add(m_upperSizer, 1, wxGROW);
	m_topSizer->Add(m_buttonSizer, 0, wxGROW | wxLEFT | wxBOTTOM | wxRIGHT | wxALIGN_BOTTOM, 5);
	m_topSizer->Add(m_radioBox, 0, wxGROW | wxLEFT | wxBOTTOM | wxRIGHT | wxALIGN_BOTTOM, 5);
	m_topSizer->Add(m_alignmentDescription, 0, wxGROW | wxLEFT | wxBOTTOM | wxRIGHT | wxALIGN_BOTTOM, 5);


	// Tell the Moog communication class where the message console is, then initialize the Tempo crap.
	m_moogCom->SetConsolePointer(m_messageConsole);
	m_moogCom->InitTempo();

	SetSizer(m_topSizer);

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
}

void CMainPanel::initParameterListStuff()
{
	wxString *choices;
	string *keyList;
	int i, keyCount = 0;

	// Grabs the parameter list keys and puts them in an array of strings.  This allows us to display them
	// easily.
	keyList = g_pList.GetKeyList(keyCount);
	wxASSERT(keyCount);
	choices = new wxString[keyCount];
	for (i = 0; i < keyCount; i++) {
		choices[i] = keyList[i].c_str();
	}

	// Create the listbox that shows all of our parameters.
	m_moogListBox = new wxListBox(this, MOOG_LISTBOX, wxDefaultPosition, wxSize(135, 400),
								  g_pList.GetListSize(), choices, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
	m_moogListBox->SetFirstItem(0);  // Sets the first item to be selected.

	// Create the static text that will display the parameter's description.
	m_descriptionText = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_upperRightSizer->Add(m_descriptionText, 0, wxGROW | wxALL, 5);

	// Create the text box that will display a parameter's corresponding data.
	m_dataTextBox = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150, 100), wxALIGN_LEFT | wxTE_MULTILINE);
	m_parameterSizer->Add(m_dataTextBox, 1, wxGROW);
	m_parameterSizer->Add(m_otherButtonsSizer, 0, wxGROW | wxLEFT, 5);
	m_upperRightSizer->Add(m_parameterSizer, 0, wxGROW | wxALL, 5);

	// This makes sure that the information for the 1st item in the list shows up.
	OnItemSelected(wxCommandEvent(NULL));
}

void CMainPanel::initButtons()
{
	int buttonWidth = 75,
		buttonHeight = 31;

	// General control buttons.
	m_EngageButton = new wxButton(this, ENGAGE_BUTTON, "Engage", wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	m_EngageButton->SetToolTip("Engages the Motion Base");
	m_StopButton = new wxButton(this, STOP_BUTTON, "Stop", wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	m_StopButton->SetToolTip("Stops All Movement");
	m_ParkButton = new wxButton(this, PARK_BUTTON, "Park", wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	m_ParkButton->SetToolTip("Parks the Motion Base");
	m_ResetButton = new wxButton(this, RESET_BUTTON, "Reset", wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	m_ResetButton->SetToolTip("Resets the MBC");

	// Create the Go button.
	m_goButton = new wxButton(this, MOOG_GO_BUTTON, "Go!", wxPoint(305, 145-29), wxSize(75, 30));

	// Create the Set button.
	m_setButton = new wxButton(this, MOOG_SET_BUTTON, "Set Item", wxPoint(305, 45), wxSize(75, 30));

	// Create the Go to Zero button.
	m_goToZeroButton = new wxButton(this, MOOG_ZERO_BUTTON, "Go to Origin", wxPoint(305, 80), wxSize(75, 30));

	m_buttonSizer->Add(2, 0);
	m_buttonSizer->Add(m_EngageButton, 0);
	m_buttonSizer->Add(1, 1, 1);
	m_buttonSizer->Add(m_ParkButton, 0);
	m_buttonSizer->Add(1, 1, 1);
	m_buttonSizer->Add(m_ResetButton, 0);
	m_buttonSizer->Add(1, 1, 1);
	m_buttonSizer->Add(m_StopButton, 0);
	m_otherButtonsSizer->Add(m_setButton, 0);
	m_otherButtonsSizer->Add(1, 1, 1);
	m_otherButtonsSizer->Add(m_goToZeroButton, 0);
	m_otherButtonsSizer->Add(1, 1, 1);
	m_otherButtonsSizer->Add(m_goButton, 0);
}


/***************************************************************/
/*	Event Table												   */
/***************************************************************/
BEGIN_EVENT_TABLE(CMainPanel, wxPanel)
EVT_BUTTON(PARK_BUTTON, CMainPanel::OnParkButtonClicked)
EVT_BUTTON(RESET_BUTTON, CMainPanel::OnResetButtonClicked)
EVT_BUTTON(ENGAGE_BUTTON, CMainPanel::OnEngageButtonClicked)
EVT_LISTBOX(MOOG_LISTBOX, CMainPanel::OnItemSelected)
EVT_BUTTON(MOOG_GO_BUTTON, CMainPanel::OnGoButtonClicked)
EVT_BUTTON(MOOG_SET_BUTTON, CMainPanel::OnSetButtonClicked)
EVT_BUTTON(STOP_BUTTON, CMainPanel::OnStopButtonClicked)
EVT_BUTTON(MOOG_ZERO_BUTTON, CMainPanel::OnGoToZeroButtonClicked)
EVT_RADIOBOX(MODE_RADIO_BOX, CMainPanel::OnModeSelected)
END_EVENT_TABLE()


void CMainPanel::OnResetButtonClicked(wxCommandEvent &event)
{
	m_moogCom->Reset();
}


void CMainPanel::OnParkButtonClicked(wxCommandEvent &event)
{
	m_moogCom->Park();
}


void CMainPanel::OnGoToZeroButtonClicked(wxCommandEvent &event)
{
	m_moogCom->MovePlatformToOrigin();
	m_moogCom->DoCompute(COMPUTE | RECEIVE_COMPUTE);
}


void CMainPanel::OnEngageButtonClicked(wxCommandEvent &event)
{
	bool errorOccurred = false;
	wxString errorString = "";

	switch (m_moogCom->Engage())
	{
	case 0:		// Success
		break;
	case -1:	// Already engaged
		errorOccurred = true;
		errorString = "Already Engaged";
		break;
	case 1:		// Haven't connected yet
		errorOccurred = true;
		errorString = "Connect to Moog First";
		break;
	}

	if (errorOccurred == true) {
		wxMessageDialog d(this, errorString);
		d.ShowModal();
	}
}


void CMainPanel::OnStopButtonClicked(wxCommandEvent &event)
{
	// Stop the Compute() function but let ReceiveCompute() continue.
	m_moogCom->DoCompute(RECEIVE_COMPUTE);

#if USE_MATLAB
	m_moogCom->StuffMatlab();
#endif;
}


void CMainPanel::OnSetButtonClicked(wxCommandEvent &event)
{
	vector<double> value;
	int i;

	// Make sure that the correct number of data values is being entered for the
	// selected parameter.
	int sizeOfVector = static_cast<int>(g_pList.GetVectorData(m_moogListBox->GetStringSelection().c_str()).size());
	if (m_dataTextBox->GetNumberOfLines() != sizeOfVector) {
		wxMessageDialog d(this, wxString::Format("Parameter \"%s\" takes %d data values.", m_moogListBox->GetStringSelection(), sizeOfVector),
						  "Error", wxICON_ERROR);
		d.ShowModal();
		return;
	}
	// Grab the data from the data box and stuff it into a vector.
	for (i = 0; i < m_dataTextBox->GetNumberOfLines(); i++) {
		double d;

		// This converts the wxString to a double number.
		m_dataTextBox->GetLineText(i).ToDouble(&d);

		// Put the converted string into the vector.
		value.push_back(d);
	}

	// Set the vector data associated with the selected key.
	g_pList.SetVectorData(m_moogListBox->GetStringSelection().c_str(), value);

	// Update the Frustum and StarField data for the GL scene and re-render it.
    m_moogCom->UpdateGLScene(true);
}

void CMainPanel::OnGoButtonClicked(wxCommandEvent &event)
{
#if TRAJECTORY_SAFETY_CHECK
	// Check Moog Trajectory and make sure no bumping
	bool smooth = m_moogCom->CheckTrajectories();
	if(!smooth) return;
#endif

	// Make sure that the Moog isn't moving.
	m_moogCom->DoCompute(RECEIVE_COMPUTE);
	Sleep(50);


	// Turn on movement.
	vector<double> v;
	v.push_back(1.0);
	g_pList.SetVectorData("DO_MOVEMENT", v);

	m_moogCom->UpdateMovement();

	// Start the movement.
	m_moogCom->DoCompute(COMPUTE | RECEIVE_COMPUTE);

	if (g_pList.GetVectorData("ENABLE_COUNTER")[0] == 1){
		//char buffer[200];
		//Sleep(2000);
		int c = m_moogCom->GetGLWindow()->GetGLPanel()->GetCounter();
		m_messageConsole->Append(wxString::Format("%d frames per second", c));
	}
}


bool CMainPanel::Enable(bool enable)
{
	bool sumpinChanged = m_goButton->Enable(enable) | m_setButton->Enable(enable) | m_goToZeroButton->Enable(enable);

	return sumpinChanged;
}

void CMainPanel::OnItemSelected(wxCommandEvent &event)
{
	vector<double> value;

	// Set the description for the parameter data.
	m_descriptionText->SetLabel(g_pList.GetParamDescription(m_moogListBox->GetStringSelection().c_str()).c_str());

	// Grab the vector associated with the selected key.
	value = g_pList.GetVectorData(m_moogListBox->GetStringSelection().c_str());

	// If value is empty, then this function is being called from the initialization of the program.  We
	// return so that we don't throw an exception later.
	if (value.empty()) {
		return;
	}

	// Clear the data text box before we write to it.
	m_dataTextBox->Clear();

	// Write each value of the value vector to the data text box, each on its own line.
	// I write it out all weird so that it is formatted nicely in the box.
	int size = static_cast<int>(value.size());
	for (int i = 0; i < size - 1; i++) {
		m_dataTextBox->AppendText(wxString::Format("%f\n", value[i]));
	}
	wxString dataText = wxString::Format("%f", value.at(size-1));
	m_dataTextBox->AppendText(dataText);
}

void CMainPanel::OnModeSelected(wxCommandEvent &event)
{
	double coeffsLeftEye, coeffsRightEye;
	Grid *grid;
	wxString str = "";
	int select = m_radioBox->GetSelection();

	if(select == 0){ // selected flat screen
		m_moogCom->GetGLWindow()->GetGLPanel()->drawingMode = GLPanel::MODE_FLAT_SCREEN;
		m_alignmentDescription->SetLabel("Using Flat Screen!");
	}
	else if(select == 1){ // selected curve screen
#if USE_MATLAB
		coeffsLeftEye = FindEquation(LEFT_EYE);
		coeffsRightEye = FindEquation(RIGHT_EYE);
#else
		grid = &m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridLeft;
		coeffsLeftEye = 0.0;
		for (int i=0; i<10; i++){ 
			coeffsLeftEye += grid->cubicEqCoeff_X[i];
			coeffsLeftEye += grid->cubicEqCoeff_Y[i];
		}

		grid = &m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridRight;
		coeffsRightEye = 0.0;
		for (int i=0; i<10; i++){ 
			coeffsRightEye += grid->cubicEqCoeff_X[i];
			coeffsRightEye += grid->cubicEqCoeff_Y[i];
		}

		if (coeffsLeftEye == 0.0 || coeffsRightEye == 0.0)
			wxMessageBox("You need open an existing file to use curve screen!", "Confirm", wxOK, NULL);
#endif
		if (coeffsLeftEye == 0.0) str.append("Left eye need do alignment; ");
		if (coeffsRightEye == 0.0) str.append("Right eye need do alignment; ");
		if (coeffsLeftEye == 0.0 || coeffsRightEye == 0.0){
			str.append("Or open an existing file;");
			wxBell();
			m_alignmentDescription->SetLabel(str);
			m_radioBox->SetSelection(2);
			m_moogCom->GetGLWindow()->GetGLPanel()->drawingMode = GLPanel::MODE_ALIGNMENT;
			wxMessageBox("Please click Drawing Panel Screen to active the control of alignment!", "Confirm", wxOK, NULL);
		}
		else{
			m_moogCom->GetGLWindow()->GetGLPanel()->drawingMode = GLPanel::MODE_CURVE_SCREEN;
			m_alignmentDescription->SetLabel("Using Curve Screen!");
		}
	}
	else if(select == 2){ // selected manually alignment
		 m_moogCom->GetGLWindow()->GetGLPanel()->drawingMode = GLPanel::MODE_ALIGNMENT;
		 m_alignmentDescription->SetLabel("You can save to an alignment file anytime for future use!");
		 wxMessageBox("Please click Drawing Panel Screen to active the control of alignment!", "Confirm", wxOK, NULL);
	}
	

	// give a message to UpdateGlScene to make a new glCallList for TEXTURE
	m_moogCom->redrawTexture = true; 
	m_moogCom->UpdateGLScene(true);
}

double CMainPanel::FindEquation(int whichEye)
{
	double coeffs = 0.0;
#if USE_MATLAB
	Grid *grid;
	if(whichEye == LEFT_EYE)
		grid = &m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridLeft;
	else grid = &m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridRight;

	vector<double> OX, OY, NX, NY;
	int row = grid->GetRowNum();
	int col = grid->GetColNum();
	double upper = (double)grid->screenHeight * 0.85;
	double lower = (double)grid->screenHeight * 0.15;
	double left = (double)grid->screenWidth * 0.05;
	double right = (double)grid->screenWidth * 0.95;
	
	int r,c;	

	for(r=0; r<row; r++){
		for(c=0; c<=col; c++){
			// use all points in middle part of screen
			if ( grid->matrix[r][c].ox >= left && grid->matrix[r][c].ox <= right &&
				grid->matrix[r][c].oy >= lower && grid->matrix[r][c].oy <= upper)
			{
				OX.push_back(grid->matrix[r][c].ox);
				OY.push_back(grid->matrix[r][c].oy);
				NX.push_back(grid->matrix[r][c].nx);
				NY.push_back(grid->matrix[r][c].ny);
			}
		}
	}


	m_moogCom->stuffDoubleVector(OX, "ox");
	m_moogCom->stuffDoubleVector(OY, "oy");
	m_moogCom->stuffDoubleVector(NX, "nx");
	m_moogCom->stuffDoubleVector(NY, "ny");

	engEvalString(m_moogCom->m_engine, "ox = transpose(ox)");
	engEvalString(m_moogCom->m_engine, "oy = transpose(oy)");
	engEvalString(m_moogCom->m_engine, "nx = transpose(nx)");
	engEvalString(m_moogCom->m_engine, "ny = transpose(ny)");
	engEvalString(m_moogCom->m_engine, "diffx = nx-ox");
	engEvalString(m_moogCom->m_engine, "diffy = ny-oy");
	engEvalString(m_moogCom->m_engine, "X = [ones(length(ox),1) ox oy ox.^2 ox.*oy oy.^2 ox.^3 ox.^2.*oy ox.*oy.^2 oy.^3]");
	engEvalString(m_moogCom->m_engine, "bx = regress(diffx,X);");
	engEvalString(m_moogCom->m_engine, "by = regress(diffy,X)");
	mxArray *Xcoeff = engGetVariable(m_moogCom->m_engine, "bx");
	mxArray *Ycoeff = engGetVariable(m_moogCom->m_engine, "by");
	
	for (int i=0; i<10; i++){ 
		grid->cubicEqCoeff_X[i] = mxGetPr(Xcoeff)[i]; 
		coeffs += grid->cubicEqCoeff_X[i];
		grid->cubicEqCoeff_Y[i] = mxGetPr(Ycoeff)[i];
		coeffs += grid->cubicEqCoeff_Y[i];
	}

	// give a message to UpdateGlScene to make a new glCallList for TEXTURE
	// m_moogCom->redrawTexture = true; 
	// m_moogCom->UpdateGLScene(true);
#endif

	return coeffs;
}
