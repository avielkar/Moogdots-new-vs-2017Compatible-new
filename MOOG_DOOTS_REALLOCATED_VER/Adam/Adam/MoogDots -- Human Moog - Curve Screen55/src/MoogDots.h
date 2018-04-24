#pragma once

#ifndef MOGDOTS
#define MOGDOTS


#include "GlobalDefs.h"
#include "MainPanel.h"
#include "MoogDotsCom.h"
#include "Grid.h"
#include <wx/textfile.h>
#include <wx/tokenzr.h>
//
#include <wx\msw\glcanvas.h>//my3


// application globals
RenderContext g_renderContext;
Application   g_application;
OculusVR      g_oculusVR;

// Describes the main windows which holds all the components for the
// application.
class CMainFrame : public wxFrame
{
private:
	wxMenuBar *m_menuBar;
	wxMenu *m_fileMenu,
		   *m_toolsMenu;
	CMainPanel *m_mainPanel;
	MoogDotsCom *m_moogCom;
	bool m_isRDXOpen;
	Grid gr;
	wxStatusBar *m_statBar;
	wxString gridFilename;
	wxTextFile gridFile;
	
	// Widget ID's
	enum
	{
		MENU_FILE_EXIT,
		MENU_GRID_FILE_CLOSE,
		MENU_TOOLS_CONNECT,
		MENU_TOOLS_DISCONNECT,
		MENU_TOOLS_TEMPO,
		MENU_TOOLS_TIMER,
		MENU_TOOLS_VERBOSE,
		MENU_TOOLS_VSYNC,
		MENU_SET_PACKET_RATE,
	};

	Logger* m_logger;										//used for making a log fie to trace the program(for bugs detection and for controlling).

public:

	// Default Constructor
	CMainFrame(const wxChar *title, int xpos, int ypos, int width, int height , Logger* m_logger);

	// Exits the program.
	void OnMenuFileExit(wxCommandEvent &event);

	// Connects to the MBC.
	void OnMenuToolsConnect(wxCommandEvent &event);

	// Disconnects from the MBC.
	void OnMenuToolsDisconnect(wxCommandEvent &event);

	// Toggles Tempo control mode.
	void OnMenuToolsTempo(wxCommandEvent &event);

	// Toggles the use of the built-in timer for the thread.
	void OnMenuToolsTimer(wxCommandEvent &event);

	// Toggles verbose mode for the message console.
	void OnMenuToolsVerboseMode(wxCommandEvent &event);

	// Opens a dialog to set the packet rate for the communication thread.
	void OnMenuToolsPacketRate(wxCommandEvent &event);

	// Toggles syncing to the vertical refresh.
	void OnMenuToolsVSync(wxCommandEvent &event);

	// Turn On/Off alignment tools of grid (on keybroad)
	void OnMenuToolsAlignment(wxCommandEvent &event);

	void OnFrameClose(wxCloseEvent &event);
	
	// open grid file
	void OnOpenFile(wxCommandEvent &event); 	
	// close grid file
	void OnCloseFile(wxCommandEvent &event);
	// save grid file to opened or new file
	void OnSaveFile(wxCommandEvent &event);
	// save grid file to new file
	void OnSaveAsFile(wxCommandEvent &event);

	~CMainFrame();

private:
	DECLARE_EVENT_TABLE()
public:
	void WriteGridFile();
	void readGridFile(void);
};

// The Application
class MoogDots : public wxGLApp
{
private:
	CMainFrame *m_mainFrame;

	Logger* m_logger;

public:
	virtual bool OnInit();
	
};

DECLARE_APP(MoogDots)

#endif
