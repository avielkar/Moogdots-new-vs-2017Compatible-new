#pragma once

#include <windows.h>
//#include <afxwin.h>
#include <Vfw.h>		// AVI stuff.

// OpenGL stuff.
#include <GL\gl.h>
#include <GL\glu.h>

class CGLVidCapture
{
private:
	PAVIFILE m_aviFile;
	PAVISTREAM m_aviStream;
	PAVISTREAM m_compAviStream;

	int m_width, m_height,		// Width and height of the recorded area.
		m_frameCount;			// Let's us know which frame we're on.
	const char *m_fileName;		// Filename of the generated AVI file.
	BOOL m_aviReady;			// Lets us know if the AVI had any creation problems.
	UCHAR *m_glImage;
	DWORD m_frameRate;			// Frame rate of the captured AVI.

public:
	CGLVidCapture();
	~CGLVidCapture();

	// Creates the AVI file.  30's a good frameRate if you don't know what to use.
	BOOL CreateAVI(const char *fileName, int width, int height, DWORD frameRate, char *errorString);

	// Captures a frame.
	BOOL CaptureFrame(int &frameCount);

	// Closes the AVI file.
	BOOL CloseAVI();

private:
	BOOL initAVI(char *errorString);
};
