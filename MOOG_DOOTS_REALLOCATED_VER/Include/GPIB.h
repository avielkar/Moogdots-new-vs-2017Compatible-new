#pragma once

//////////////////////////////////////////////////////////////////////////////////////////
//	GPIB.h																				//
//																						//
//	Header for for GPIB class.  The GPIB class aims to take care of most of the			//
//	tedium associated with connecting to and controlling the Acutorl.					//
//																						//
//	@Author:	Christopher Broussard													//
//	@Date:		November 2004															//
//////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <Decl-32.h>
#include <string>

#define GPIB0 0
#define NODEVID -1

using namespace std;

// We will throw this struct if an error occurs.
typedef struct GPIB_EXCPTN
{
	string message;
	bool isErrorCode;
	int errorCode;
} GPIB_Exception;

class CGPIB
{
private:
	int m_deviceID;		// GPIB device ID.

public:
	CGPIB();

	// Initializes and sets up communication with the Acutrol.  Throws a GPIB_Exception
	// if an error occurs.
	void Init() throw(...);

	// Writes a string out to the Acutrol.
	int WriteString(string command) throw(...);

	// Reads a string returned from the Acutrol.
	string ReadString() throw(...);

	// Closes the connection with the Acutrol.
	void Close() throw(...);

	// Makes the Acutrol beep.
	void Beep() throw(...);
};