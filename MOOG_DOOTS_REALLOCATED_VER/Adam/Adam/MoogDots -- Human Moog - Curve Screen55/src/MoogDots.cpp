#include "StdAfx.h"
#include "MoogDots.h"
#include <wx/app.h>
#define wxUSE_GLCANVAS 1 //my2

CMainFrame::CMainFrame(const wxChar *title, int xpos, int ypos, int width, int height , Logger* logger) :
			wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height)),
			m_isRDXOpen(true), m_logger(logger)
{
	// Setup the menu bar.
	m_menuBar = new wxMenuBar();
	m_fileMenu = new wxMenu();
	m_toolsMenu = new wxMenu();
	m_fileMenu->Append(wxID_OPEN, "&Open...", "Open Grid file.");
	m_fileMenu->Append(wxID_SAVE, "&Save", "Save Grid in current file.");
	m_fileMenu->Append(wxID_SAVEAS, "Save &As...", "Save Grid in a new file.");
	m_fileMenu->Append(MENU_GRID_FILE_CLOSE, "&Close...", "Close Grid file.");
	m_fileMenu->Append(MENU_FILE_EXIT, "Exit", "Exits the Program");
	m_toolsMenu->Append(MENU_TOOLS_CONNECT, "Connect", "Connect to the MBC.");
	m_toolsMenu->Append(MENU_TOOLS_DISCONNECT, "Disconnect", "Disconnect from the MBC.");
	m_toolsMenu->AppendSeparator();
	m_toolsMenu->AppendCheckItem(MENU_TOOLS_TEMPO, "Listen Mode", "Toggles MoogDots to listen for external control.");
	m_toolsMenu->AppendCheckItem(MENU_TOOLS_TIMER, "Low Priority Mode", "Allows other OpenGL programs to run.");
	m_toolsMenu->AppendSeparator();
	m_toolsMenu->AppendCheckItem(MENU_TOOLS_VERBOSE, "Verbose Output");
	//m_toolsMenu->AppendCheckItem(MENU_TOOLS_VSYNC, "Enable VSync");
	m_toolsMenu->Append(MENU_SET_PACKET_RATE, "Set Packet Rate");
	m_menuBar->Append(m_fileMenu, "File");
	m_menuBar->Append(m_toolsMenu, "Tools");
	SetMenuBar(m_menuBar);

	// Disable the some of the options.
	m_toolsMenu->Enable(MENU_TOOLS_DISCONNECT, false);
	m_toolsMenu->Enable(MENU_TOOLS_TIMER, false);
	//m_toolsMenu->Enable(MENU_TOOLS_VSYNC, false);

	WRITE_LOG(m_logger->m_logger, "Main windows created....");

	// Create the Moog communications object.
	m_moogCom = new MoogDotsCom(
#if USE_LOCALHOST
								"127.0.0.1", 991,
								"127.0.0.1", 1978,
#else 
								"128.127.55.120", 991,
								//"128.127.55.121", 1978,
								"128.127.55.121", 992,
#endif							
								m_logger,
#if CUSTOM_TIMER
								true);
#else
								false);
#endif

	// We'll let the ReceiveCompute function run the whole time.
	m_moogCom->DoCompute(RECEIVE_COMPUTE);

#if SINGLE_CPU_MACHINE
	m_moogCom->SetComThreadPriority(THREAD_PRIORITY_NORMAL);
#endif

//avi : for interpolated version with matlab
#if USE_MATLAB | USE_MATLAB_INTERPOLATION
	// Start the Matlab engine.
	m_moogCom->StartMatlab();
#endif

	// Create the main panel of the program.
	m_mainPanel = new CMainPanel(this, -1, m_moogCom , m_logger);

	// Create default status bar to start with.
    CreateStatusBar(1);
    SetStatusText("Welcome to MoogDots!");
    m_statBar = GetStatusBar();

	//avi : deleted
	//wxString s = wxString::Format("If you will use curve screen, then you need open existing alignment file \nOr do both eyes alignement before using curve screen!");
	//wxMessageBox(s, "Confirm", wxOK, NULL);
}


CMainFrame::~CMainFrame()
{
}

/***************************************************************/
/*	Event Table												   */
/***************************************************************/
BEGIN_EVENT_TABLE(CMainFrame, wxFrame)
EVT_MENU(MENU_FILE_EXIT, CMainFrame::OnMenuFileExit)
EVT_MENU(wxID_OPEN, CMainFrame::OnOpenFile)
EVT_MENU(MENU_GRID_FILE_CLOSE, CMainFrame::OnCloseFile)
EVT_MENU(wxID_SAVE, CMainFrame::OnSaveFile)
EVT_MENU(wxID_SAVEAS, CMainFrame::OnSaveAsFile)
EVT_MENU(MENU_TOOLS_CONNECT, CMainFrame::OnMenuToolsConnect)
EVT_MENU(MENU_TOOLS_DISCONNECT, CMainFrame::OnMenuToolsDisconnect)
EVT_MENU(MENU_TOOLS_TEMPO, CMainFrame::OnMenuToolsTempo)
EVT_MENU(MENU_TOOLS_TIMER, CMainFrame::OnMenuToolsTimer)
EVT_MENU(MENU_TOOLS_VERBOSE, CMainFrame::OnMenuToolsVerboseMode)
//EVT_MENU(MENU_TOOLS_VSYNC, CMainFrame::OnMenuToolsVSync)
EVT_MENU(MENU_SET_PACKET_RATE, CMainFrame::OnMenuToolsPacketRate)
EVT_CLOSE(CMainFrame::OnFrameClose)
END_EVENT_TABLE()


void CMainFrame::OnMenuToolsVSync(wxCommandEvent &event)
{
	if (m_toolsMenu->IsChecked(MENU_TOOLS_VSYNC)) {
		m_moogCom->SetVSyncState(true);
	}
	else {
		m_moogCom->SetVSyncState(false);
	}
}


void CMainFrame::OnMenuToolsPacketRate(wxCommandEvent &event)
{
	// Create and show a dialog with the current packet rate.
	wxTextEntryDialog dlg(this, "Packet Rate (ms)", "Packet Rate", wxString::Format("%f", m_moogCom->GetPacketRate()));
	int ok = dlg.ShowModal();

	// If the use presses OK, then set the packet rate.
	if (ok == wxID_OK) {
		// Grab the value entered, convert it to a double, and set the packet rate
		// for the thread.
		wxString value = dlg.GetValue();
		double cval;
		if (value.ToDouble(&cval) == true) {
			// Make sure that the value is within an acceptable range.
			if (cval < 15.0 || cval > 18) {
				wxMessageDialog d(this, "Invalid range. Must be [15, 18].", "Range Error", wxOK | wxICON_ERROR);
				d.ShowModal();
			}
			else {
				m_moogCom->SetPacketRate(cval);
			}
		}
		else {
			// Complain
			wxMessageDialog d(this, "Failure to convert string to double.", "Conversion Error", wxOK | wxICON_ERROR);
			d.ShowModal();
		}
	}
}


void CMainFrame::OnMenuToolsVerboseMode(wxCommandEvent &event)
{
	if (m_toolsMenu->IsChecked(MENU_TOOLS_VERBOSE)) {
		m_moogCom->SetVerbosity(true);
	}
	else {
		m_moogCom->SetVerbosity(false);
	}
}


void CMainFrame::OnFrameClose(wxCloseEvent &event)
{
	// Disconnect from the MBC.
	m_moogCom->Disconnect();

#if USE_MATLAB
	// Kill Matlab.
	m_moogCom->CloseMatlab();
#endif
	
	Destroy();

	delete m_moogCom;
}


void CMainFrame::OnMenuToolsTimer(wxCommandEvent &event)
{
	if (m_toolsMenu->IsChecked(MENU_TOOLS_TIMER)) {
		// Use the built-in timer, lower the thread and process priority, and hide the OpenGL window.
		m_moogCom->ListenMode(false);
#if CUSTOM_TIMER
		m_moogCom->UseCustomTimer(false);
#endif
		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		m_moogCom->SetComThreadPriority(THREAD_PRIORITY_NORMAL);
		m_moogCom->ShowGLWindow(false);
	}
	else {
		// Switch to the custom timer, kick the thread and process priority up, and display the
		// OpenGL window.
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
		m_moogCom->SetComThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
		m_moogCom->ShowGLWindow(true);
#if CUSTOM_TIMER
		m_moogCom->UseCustomTimer(true);
#endif

		if (m_toolsMenu->IsChecked(MENU_TOOLS_TEMPO)) {
			m_moogCom->ListenMode(true);
		}
	}
}


void CMainFrame::OnMenuToolsTempo(wxCommandEvent &event)
{
	// Disable manual controls if Tempo mode is toggled.
	if (m_toolsMenu->IsChecked(MENU_TOOLS_TEMPO)) {
		m_mainPanel->Enable(FALSE);

		if (m_toolsMenu->IsChecked(MENU_TOOLS_TIMER) == false) {
			m_moogCom->ListenMode(true);
		}
	}
	else {
		m_mainPanel->Enable(TRUE);
		m_moogCom->ListenMode(false);
	}
}

void CMainFrame::OnMenuToolsConnect(wxCommandEvent &event)
{
	bool errorOccurred = true;
	wxString errorString = "";
	int errorCode,
		retCode;

	retCode = m_moogCom->Connect(errorCode);
	switch (retCode)
	{
		// Success
	case 0:
		// Turn off the Connect option and enable the
		// Disconnect option.
		m_toolsMenu->Enable(MENU_TOOLS_CONNECT, false);
		m_toolsMenu->Enable(MENU_TOOLS_DISCONNECT, true);

		// Turn on the built-in timer option.
		m_toolsMenu->Enable(MENU_TOOLS_TIMER, true);

		errorOccurred = false;
		break;

		// Error occurred
	case -1:
		errorString = wxString::Format("Connect Error: %d", errorCode);
		break;

		// Alread connected
	case 1:
		errorString = "Already Connected";
		break;
	}

	if (errorOccurred == true) {
		wxMessageDialog d(this, errorString);
		d.ShowModal();
	}
}


void CMainFrame::OnMenuToolsDisconnect(wxCommandEvent &event)
{
	// Turn off the Disconnect option and enable the
	// Connect option.
	m_toolsMenu->Enable(MENU_TOOLS_CONNECT, true);
	m_toolsMenu->Enable(MENU_TOOLS_DISCONNECT, false);

	// Uncheck and disable the built-in timer option.
	m_toolsMenu->Check(MENU_TOOLS_TIMER, false);
	m_toolsMenu->Enable(MENU_TOOLS_TIMER, false);

#if CUSTOM_TIMER
	// Switch to the custom timer, kick the thread and process priority up, and display the
	// OpenGL window.
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	m_moogCom->SetComThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
	m_moogCom->UseCustomTimer(true);
	m_moogCom->ShowGLWindow(true);
#endif

	// Disconnect
	m_moogCom->Disconnect();
}


void CMainFrame::OnMenuFileExit(wxCommandEvent &event)
{
	// Disconnect from the MBC.
	m_moogCom->Disconnect();

#if USE_MATLAB
	// Kill Matlab.
	m_moogCom->CloseMatlab();
#endif

	Destroy();

	delete m_moogCom;
}


/*******************************************************************************/
/*	Create the Application.													   */
/*******************************************************************************/
	IMPLEMENT_APP(MoogDots)

bool MoogDots::OnInit()
{
	int winLocX, winLocY = 250;

#if DUAL_MONITORS
#if FLIP_MONITORS
	winLocX = 250;
#else
	winLocX = 250 + SCREEN_WIDTH;
#endif
#else
	winLocX = 7;
#endif

	//the program main logger.
	m_logger = new Logger("C:\\MoogDots\\Logs\\", "MainLogFile");

	// Create the main window.
	CMainFrame *m_mainFrame = new CMainFrame("MoogDots", winLocX, winLocY, 435, 500 , m_logger);
	m_mainFrame->SetIcon(wxIcon("MAIN_ICO"));
	m_mainFrame->Show(true);
	SetTopWindow(m_mainFrame);

#if SINGLE_CPU_MACHINE
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#else
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif

	WRITE_LOG(m_logger->m_logger, "MoogDots initialization...");

	return true;
}


void CMainFrame::OnOpenFile(wxCommandEvent &event)
{
	gridFilename = wxFileSelector("Open a Grid file", GRID_FILE_DIR, 
		"", "", "Text files (*.txt)|*.txt", wxOPEN);
	if ( !gridFilename.empty() )
	{
		if( gridFile.Open(gridFilename) ) {
			readGridFile();
			gridFile.Close();

			wxStringTokenizer tkz(gridFilename, wxT("\\"));
			wxString token;
			while ( tkz.HasMoreTokens() ) token = tkz.GetNextToken();
			m_mainPanel->m_radioBox->SetLabel(wxString::Format("Screen and Alignment Control: %s",token));

		}
		else {
			wxString s = "Cannot open the file: " + gridFilename;
			wxMessageBox(s, "Confirm", wxOK, NULL);
			gridFilename.clear();
		}
	}
	//else: cancelled by user
}

void CMainFrame::OnCloseFile(wxCommandEvent &event)
{
	if ( !gridFilename.empty() ) gridFile.Close();
	gridFilename.clear();
	//m_mainPanel->m_radioBox->SetLabel(wxString::Format("Screen and Alignment Control"));
}

void CMainFrame::OnSaveFile(wxCommandEvent &event)
{
	if ( !gridFilename.empty() )
	{
		
		if( gridFile.Open(gridFilename) ) {
			WriteGridFile();
			gridFile.Close();
		}
		else {
			wxString s = "Cannot save the file: " + gridFilename;
			wxMessageBox(s, "Confirm", wxOK, NULL);
			gridFilename.clear();
		}
	}
	else 
	{
		OnSaveAsFile(event);
	}
}

void CMainFrame::OnSaveAsFile(wxCommandEvent &event)
{
	gridFilename = wxFileSelector("Save Grid file", GRID_FILE_DIR, 
		"", "", "Text files (*.txt)|*.txt", wxSAVE | wxOVERWRITE_PROMPT);
	if ( !gridFilename.empty() )
	{
		gridFile.Create(gridFilename);
		WriteGridFile();

		wxStringTokenizer tkz(gridFilename, wxT("\\"));
		wxString token;
		while ( tkz.HasMoreTokens() ) token = tkz.GetNextToken();
		m_mainPanel->m_radioBox->SetLabel(wxString::Format("Screen and Alignment Control: %s",token));

	}
}

void CMainFrame::WriteGridFile()
{
	gridFile.Clear();
	wxString s = "";
	int row, col;
	int r, c;	

	// save left eye grid information
	Grid *gr = & m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridLeft;
		
	gridFile.AddLine("Grid Left");
	s=wxString::Format("space %f", gr->space);
	gridFile.AddLine(s);
	s=wxString::Format("lineWidth %f", gr->lineWidth);
	gridFile.AddLine(s);
	s=wxString::Format("x_offset %f", gr->x_offset);
	gridFile.AddLine(s);
	s=wxString::Format("y_offset %f", gr->y_offset);
	gridFile.AddLine(s);
	s=wxString::Format("screenWidth %f", gr->screenWidth);
	gridFile.AddLine(s);
	s=wxString::Format("screenHeight %f", gr->screenHeight);
	gridFile.AddLine(s);
	s=wxString::Format("shiftDistance %f", gr->shiftDistance);
	gridFile.AddLine(s);
	s=wxString::Format("row %d", gr->GetRowNum());
	gridFile.AddLine(s);
	s=wxString::Format("col %d", gr->GetColNum());
	gridFile.AddLine(s);

	for(int i=0; i<10; i++){
		s=wxString::Format("coeff %d %.20f %.20f", i, gr->cubicEqCoeff_X[i], gr->cubicEqCoeff_Y[i]);
		gridFile.AddLine(s);
	}

	row = gr->GetRowNum();
	col = gr->GetColNum();
	for(r=0; r<=row; r++){	//y-direction
		for(c=0; c<=col; c++){	//x-direction
			s=wxString::Format("element %d %d %f %f %f %f", 
				r, c, gr->matrix[r][c].ox, gr->matrix[r][c].oy, gr->matrix[r][c].nx, gr->matrix[r][c].ny);
			gridFile.AddLine(s);
		}
	}

	// save right eye grid information
	gr = & m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridRight;

	gridFile.AddLine("Grid Right");	
	s=wxString::Format("space %f", gr->space);
	gridFile.AddLine(s);
	s=wxString::Format("lineWidth %f", gr->lineWidth);
	gridFile.AddLine(s);
	s=wxString::Format("x_offset %f", gr->x_offset);
	gridFile.AddLine(s);
	s=wxString::Format("y_offset %f", gr->y_offset);
	gridFile.AddLine(s);
	s=wxString::Format("screenWidth %f", gr->screenWidth);
	gridFile.AddLine(s);
	s=wxString::Format("screenHeight %f", gr->screenHeight);
	gridFile.AddLine(s);
	s=wxString::Format("shiftDistance %f", gr->shiftDistance);
	gridFile.AddLine(s);
	s=wxString::Format("row %d", gr->GetRowNum());
	gridFile.AddLine(s);
	s=wxString::Format("col %d", gr->GetColNum());
	gridFile.AddLine(s);


	for(int i=0; i<10; i++){
		s=wxString::Format("coeff %d %.20f %.20f", i, gr->cubicEqCoeff_X[i], gr->cubicEqCoeff_Y[i]);
		gridFile.AddLine(s);
	}

	row = gr->GetRowNum();
	col = gr->GetColNum();
	for(r=0; r<=row; r++){	//y-direction
		for(c=0; c<=col; c++){	//x-direction
			s=wxString::Format("element %d %d %f %f %f %f", 
				r, c, gr->matrix[r][c].ox, gr->matrix[r][c].oy, gr->matrix[r][c].nx, gr->matrix[r][c].ny);
			gridFile.AddLine(s);
		}
	}

	gridFile.Write();
}

void CMainFrame::readGridFile(void)
{
	wxString str;
	unsigned long r=0, c=0, coeffi=0;
	Grid *gr = & m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridLeft;
	//memset(gr->cubicEqCoeff_X, 0.0, 10 * sizeof(double));
	//memset(gr->cubicEqCoeff_Y, 0.0, 10 * sizeof(double));

	for ( str = gridFile.GetFirstLine(); !gridFile.Eof(); str = gridFile.GetNextLine() )
	{
		wxStringTokenizer tkz(str, wxT(" "));
		wxString token = tkz.GetNextToken();
		if(token == "Grid"){
			token = tkz.GetNextToken();
			if(token == "Left"){
				gr = & m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridLeft;
				//memset(gr->cubicEqCoeff_X, 0.0, 10 * sizeof(double));
				//memset(gr->cubicEqCoeff_Y, 0.0, 10 * sizeof(double));
			}
			else if (token == "Right"){
				gr = & m_moogCom->GetGLWindow()->GetGLPanel()->GetWorld()->gridRight;
				//memset(gr->cubicEqCoeff_X, 0.0, 10 * sizeof(double));
				//memset(gr->cubicEqCoeff_Y, 0.0, 10 * sizeof(double));
			}
		}
		else if(token == "space"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->space);
		}
		else if(token == "lineWidth"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->lineWidth);
		}
		else if(token == "x_offset"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->x_offset);
		}
		else if(token == "y_offset"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->y_offset);
		}
		else if(token == "screenWidth"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->screenWidth);
		}
		else if(token == "screenHeight"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->screenHeight);
		}
		else if(token == "shiftDistance"){
			token = tkz.GetNextToken();
			token.ToDouble(&gr->shiftDistance);
			// finished reading all useful information for setup matrix
			gr->SetupMatrix();
		}

		else if(token == "coeff"){
			token = tkz.GetNextToken();
			token.ToULong(&coeffi);
			token = tkz.GetNextToken();
			token.ToDouble(&gr->cubicEqCoeff_X[coeffi]);
			token = tkz.GetNextToken();
			token.ToDouble(&gr->cubicEqCoeff_Y[coeffi]);
		}

		else if(token == "element"){
			//break;
			token = tkz.GetNextToken();
			token.ToULong(&r);
			token = tkz.GetNextToken();
			token.ToULong(&c);
			token = tkz.GetNextToken();
			token.ToDouble(&gr->matrix[r][c].ox);
			token = tkz.GetNextToken();
			token.ToDouble(&gr->matrix[r][c].oy);
			token = tkz.GetNextToken();
			token.ToDouble(&gr->matrix[r][c].nx);
			token = tkz.GetNextToken();
			token.ToDouble(&gr->matrix[r][c].ny);
		}
	}

	//m_moogCom->UpdateGLScene(true);
	m_moogCom->GetGLWindow()->GetGLPanel()->Render();
	m_moogCom->GetGLWindow()->GetGLPanel()->SwapBuffers();

	m_mainPanel->OnModeSelected(wxCommandEvent());
}