// tabs=4
//************************************************************
//	COPYRIGHT 2010 Moog Inc. - ALL RIGHTS RESERVED
//
// This file is the product of Moog Inc. and cannot be 
// reproduced, copied, or used in any shape or form without 
// the express written consent of Moog Inc.
//************************************************************
//
//  Defines class that parses and executes a script file.
//  The script files drive the Motion-Base-Host interface
//  to send host commands to a Motion-Base-Computer.
//
//	Revision History: See end of file.
//
//*************************************************************
#include <string>
#include <iostream>
#include <fstream>

#include "MBC_Interface.h"

using namespace std;

class CScriptFile
{
public:
	// Script commands
	typedef enum
	{
		COMMAND_INVALID = 0,
		COMMAND_MODE,
		COMMAND_ENGAGE,
		COMMAND_DISENGAGE,
		COMMAND_PAUSE,
		COMMAND_SIGNALGEN,
		COMMAND_WAITFORSTATE,
		COMMAND_LENGTH,
		COMMAND_RESET,
		COMMAND_DIRECT_DISPLACEMENT_DOF,
		COMMAND_DIRECT_DISPLACEMENT_LENGTH,
		COMMAND_BUFFET_DISPLACEMENT,
		COMMAND_BUFFET_ACCELERATION,
		COMMAND_WHITENOISE_DISPLACEMENT,
		COMMAND_WHITENOISE_ACCELERATION,
		COMMAND_DOF,
		COMMAND_MDA,
		COMMAND_SKIP,           // skip (n) commands to MBC (for testing of dropped packets)
		COMMAND_PLAYSTART
	} TECommand;

private:
	// script boundaries
	enum
	{
		eMAX_NUM_COMMANDS = 20,                // max number of commands in a script
		eMAX_NUM_COMMAND_ARGUMENTS = 20,        // max number of arguments for a single command
		eMAX_NUM_COMMAND_ARGUMENT_LENGTH = 20   // max argument length
	};

	// maintains a single parsed command from the script file
	typedef struct
	{
		int         iLineNumber;    // script file line number
		TECommand   eCommand;       // command enumeration
		int         iNumArgs;       // number of arguments
		char        szArgs[eMAX_NUM_COMMAND_ARGUMENTS][eMAX_NUM_COMMAND_ARGUMENT_LENGTH + 1];
	} TCommand;

	int m_iCommandIndex;            // index of command being executed\

	int m_iNumCommands;             // number of commands in script
	TCommand m_stCommands[eMAX_NUM_COMMANDS];

public:
	CScriptFile(CMBCInterface* pMBC , ofstream& logFile);
	~CScriptFile();

	bool Load(const string& szFilePath, ofstream& logFile);
	bool Execute(CMBCInterface* pMBCInterface);
};

//-------------------------------------------------------------------
// END OF FILE
//-------------------------------------------------------------------
