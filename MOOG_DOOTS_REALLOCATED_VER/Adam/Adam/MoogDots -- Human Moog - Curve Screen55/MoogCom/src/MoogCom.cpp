#include "stdafx.h"
#include "MoogCom.h"


MoogCom::MoogCom(string mbcIP, int mbcPort, string localIP, int localPort, bool useCustomTimer) :
// Initialization list.
m_continueSending(false), m_ExecutingGuiMBCCommand(false) , m_packetRate(16.6), m_computeCode(0x00),
m_isEngaged(false), m_doCustomTiming(useCustomTimer), m_doCompute(false),
m_doReceiveCompute(false), m_talker(NULL), m_moogCtrlTiming(false)
{
	// Set the MBC IP address and port.
	m_mbcIP = mbcIP;
	m_mbcPort = mbcPort;

	// Set the local IP address and port.
	m_localIP = localIP;
	m_localPort = localPort;

	// Determine the high performance counter frequency.
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	m_clockFrequency = (double)freq.QuadPart;

	// Initialize the Critical Sections.
	InitializeCriticalSection(&m_comCS);
	InitializeCriticalSection(&m_receiveCS);

	m_syncFrame = false;

	// Initialize current moog command posiion.
	m_currentMoogFrameCommand = new MoogFrame();
	m_currentMoogFrameCommand->heave = 0;
	m_currentMoogFrameCommand->lateral = 0;
	m_currentMoogFrameCommand->pitch = 0;
	m_currentMoogFrameCommand->roll = 0;
	m_currentMoogFrameCommand->surge = 0;
	m_currentMoogFrameCommand->yaw = 0;

	m_myfile.open("LogFile.txt");

	//The config object expects a full path name and file name eg. "C:/Moog/Motionbase.ini"
	char cFilePath[MAX_PATH + 1];
	strcpy_s(cFilePath, MAX_PATH, "C:\\Moog\\MotionBaseHost.ini");
	m_config = new CConfigFile(cFilePath , &m_myfile);
	if (NULL == m_config)
	{
		ExitProcess(-1);
	}
	else
	{
		m_pScriptFile = new CScriptFile(&m_MBCIF , m_myfile);
	}
	
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}


void MoogCom::ThreadInit()
{
}


void MoogCom::Control()
{
}


void MoogCom::talker(LPVOID lpParam)
{
	LARGE_INTEGER start, finish;			// Used to clock the thread.
	MoogCom *mcom = (MoogCom*)lpParam;		// Pointer to the parent class.
	SOCKET comSock;							// UDP socket
	ReverseTransform rt;

	// Custom thread initialization.
	mcom->ThreadInit();

	QueryPerformanceCounter(&finish);
	start = finish;

	while (mcom->m_continueSending)
	{
		//if nor executing MBC gui command only than listen to matlab and etc...
		if (!mcom->m_ExecutingGuiMBCCommand)
		{

			// Sync the built-in timer to an outside source.
			if (mcom->m_syncFrame == true)
			{
				mcom->Sync();
				mcom->m_syncFrame = false;
				QueryPerformanceCounter(&start);
			}

			while (((double)(finish.QuadPart - start.QuadPart) / mcom->m_clockFrequency * 1000.0) < mcom->m_packetRate)
			{
				QueryPerformanceCounter(&finish);

				// Grab the return packet.
			}
			start = finish;

			// Time stamp the send time.
			QueryPerformanceCounter(&finish);
			mcom->m_sendTime = (double)finish.QuadPart;
		}


		// Call the Control() function.
		EnterCriticalSection(&mcom->m_comCS);
		mcom->Control();
		LeaveCriticalSection(&mcom->m_comCS);

		// Set which compute functions are called.
		EnterCriticalSection(&mcom->m_comCS);
		if (mcom->m_computeCode & COMPUTE)
		{
			mcom->m_doCompute = true;
		}
		else
		{
			mcom->m_doCompute = false;
		}

		if (mcom->m_computeCode & RECEIVE_COMPUTE)
		{
			mcom->m_doReceiveCompute = true;
		}
		else
		{
			mcom->m_doReceiveCompute = false;
		}

		// Johnny - 12/13/07
		// 'goNextCommand' is used for 'mcom->m_moogCtrlTiming' that we wait for feedback and then send command.
		// If we don't recieve any feedback, we only send old command and keep communication with Moog.
		// First we don't update 'mcom->m_com' by 'mcom->m_commandBuffer' and
		// we have to stop call 'mcom->Compute()', because it will update next command by SET_DATA_FRAME (ThreadSetAxesPositions)
		// and 'm_data.index++; and m_grabIndex++;' in MoogDatsCom.cpp

		// Execute the Compute() function if needed.
		if (mcom->m_doCompute)
		{
			mcom->Compute();
		}


		LeaveCriticalSection(&mcom->m_comCS);

		QueryPerformanceCounter(&finish);
	}
}

void MoogCom::Sync()
{
}


void MoogCom::SyncNextFrame()
{
	m_syncFrame = true;
}


void MoogCom::UseCustomTimer(bool useCustomTimer)
{
	m_doCustomTiming = useCustomTimer;
}

void MoogCom::UseMoogCtrlTimer(bool useMoogCtrlTiming)
{
	m_moogCtrlTiming = useMoogCtrlTiming;
}


void MoogCom::ThreadSetAxisPosition(Axis axis, float value)
{
	switch (axis)
	{
	case Axis::Heave:
		// Make sure the Heave value is within acceptable range.
		if (value >= HEAVE_MAX && value <= 0.0f)
		{
			m_currentMoogFrameCommand->heave = value;
		}
		break;
	case Axis::Lateral:
		// Make sure the Lateral value is within acceptable range.
		if (value >= -LATERAL_MAX && value <= LATERAL_MAX)
		{
			m_currentMoogFrameCommand->lateral = value;
		}
		break;
	case Axis::Surge:
		// Make sure the Surge value is within acceptable range.
		if (value >= -SURGE_MAX && value <= SURGE_MAX)
		{
			m_currentMoogFrameCommand->surge = value;
		}
		break;
	case Axis::Yaw:
		// Make sure the Roll value is within acceptable range.
		if (value >= -YAW_MAX && value <= YAW_MAX)
		{
			m_currentMoogFrameCommand->yaw = value;
		}
		break;
	case Axis::Pitch:
		// Make sure the Roll value is within acceptable range.
		if (value >= -PITCH_MAX && value <= PITCH_MAX)
		{
			m_currentMoogFrameCommand->pitch = value;
		}
		break;
	case Axis::Roll:
		// Make sure the Roll value is within acceptable range.
		if (value >= -ROLL_MAX && value <= ROLL_MAX)
		{
			m_currentMoogFrameCommand->roll= value;
		}
		break;
	};
}

void MoogCom::SetCurrentExecutetCommand(MoogFrame* moogFrame)
{
	// Make sure the Lateral value is within acceptable range.
	if (moogFrame->lateral >= -LATERAL_MAX && moogFrame->lateral <= LATERAL_MAX)
	{
		m_currentMoogFrameCommand->lateral = moogFrame->lateral;
	}

	// Make sure the Surge value is within acceptable range.
	if (moogFrame->surge >= -SURGE_MAX && moogFrame->surge <= SURGE_MAX)
	{
		m_currentMoogFrameCommand->surge = moogFrame->surge;
	}

	// Make sure the Heave value is within acceptable range.
	if (moogFrame->heave >= HEAVE_MAX && moogFrame->heave <= 0.0f)
	{
		m_currentMoogFrameCommand->heave = moogFrame->heave;
	}

	// Make sure the Yaw value is within acceptable range.
	if (moogFrame->yaw >= -YAW_MAX && moogFrame->yaw <= YAW_MAX)
	{
		m_currentMoogFrameCommand->yaw = moogFrame->yaw;
	}

	// Make sure the Pitch value is within acceptable range.
	if (moogFrame->pitch >= -PITCH_MAX && moogFrame->pitch <= PITCH_MAX)
	{
		m_currentMoogFrameCommand->pitch = moogFrame->pitch;
	}

	// Make sure the Roll value is within acceptable range.
	if (moogFrame->roll >= -ROLL_MAX && moogFrame->roll <= ROLL_MAX)
	{
		m_currentMoogFrameCommand->roll = moogFrame->roll;
	}
}

#include <iostream>
void MoogCom::SendMBCAxesPositions(MoogFrame *moogFrame)
{
	//Step1: saving the current sent dof frame commands.
	SetCurrentExecutetCommand(moogFrame);	//m_currentMoogFrameCommand is set here to thw new command (if no limits problem).

	//Step2:Sending the dof command to the MBC.
	string commandString = ConvertFrameToCommand(m_currentMoogFrameCommand);
#pragma region LOGS-COMMAND
	m_myfile << "new command string";
	m_myfile << commandString;
	m_myfile.flush();
#pragma endregion LOGS-COMMAND
	m_pScriptFile->Load(commandString , m_myfile);
	m_pScriptFile->Execute(&m_MBCIF);
#pragma region LOGS_COMMAND
	m_myfile << "end command string\n\n";
	m_myfile.flush();
#pragma endregion LOGS_COMMAND
}

string MoogCom::ConvertFrameToCommand(MoogFrame* commandFrame)
{
	string commandString = "";

	commandString += DOF_LONG_COMMAND + to_string(commandFrame->surge) + "\n";
	commandString += DOF_HEAVE_COMMAND + to_string(commandFrame->heave) + "\n";
	commandString += DOF_LAT_COMMAND + to_string(commandFrame->lateral) + "\n";
	commandString += DOF_ROLL_COMMAND + to_string(commandFrame->roll) + "\n";
	commandString += DOF_PITCH_COMMAND + to_string(commandFrame->pitch) + "\n";
	commandString += DOF_YAW_COMMAND + to_string(commandFrame->yaw) + "\n";

	return commandString;
}

MoogFrame MoogCom::GetAxesFeedbackPosition()
{
	MoogFrame currentFrame;

	currentFrame.heave = m_MBCIF.GetDofPosition()->heaveFeedback;
	currentFrame.lateral = m_MBCIF.GetDofPosition()->lateralFeedback;
	currentFrame.pitch = m_MBCIF.GetDofPosition()->pitchFeedback;
	currentFrame.roll = m_MBCIF.GetDofPosition()->rollFeedback;
	currentFrame.surge = m_MBCIF.GetDofPosition()->longitudinalFeedback;
	currentFrame.yaw = m_MBCIF.GetDofPosition()->yawFeedback;

	return currentFrame;
}


double MoogCom::ExtractReturnData(int word)
{
	unsigned char arrayCopy[4];

	memcpy(arrayCopy, m_receiveBuffer + word * 4, 4);

	return (double)uc_hbo_f(arrayCopy);
}


void MoogCom::GetAxesCommandPosition(MoogFrame *moogFrame)
{
	/*
		avi : 
		This function is called after getting the current frame position and taking them from the commandBuffer here.
		After that , if there are high transition in the places values , there would be interpolation by the MovePLatform function.
	*/
	unsigned char currentPosition[4];

	// Pull the data for each axis out of the command buffer and convert it from and unsigned
	// char in network-byte-order into a float in host-byte-order.
	EnterCriticalSection(&m_comCS);
	moogFrame->heave = m_currentMoogFrameCommand->heave;
	moogFrame->lateral = m_currentMoogFrameCommand->lateral;
	moogFrame->surge = m_currentMoogFrameCommand->surge;
	moogFrame->yaw = m_currentMoogFrameCommand->yaw;
	moogFrame->pitch = m_currentMoogFrameCommand->pitch;
	moogFrame->roll = m_currentMoogFrameCommand->roll;
	LeaveCriticalSection(&m_comCS);
}


void MoogCom::ThreadGetAxesPositions(MoogFrame *moogFrame)
{
	unsigned char currentPosition[4];

	// Pull the data for each axis out of the command buffer and convert it from and unsigned
	// char in network-byte-order into a float in host-byte-order.
	moogFrame->heave = m_currentMoogFrameCommand->heave;
	moogFrame->lateral = m_currentMoogFrameCommand->lateral;
	moogFrame->surge = m_currentMoogFrameCommand->surge;
	moogFrame->yaw = m_currentMoogFrameCommand->yaw;
	moogFrame->pitch = m_currentMoogFrameCommand->pitch;
	moogFrame->roll = m_currentMoogFrameCommand->roll;
}


float MoogCom::GetAxisPosition(Axis axis)
{
	unsigned char currentPosition[4];
	float fcurrentPos;
	bool convert_rad2deg = false;

	EnterCriticalSection(&m_comCS);
	switch (axis)
	{
	case Axis::Heave:
		fcurrentPos = m_currentMoogFrameCommand->heave;
		break;
	case Axis::Lateral:
		fcurrentPos = m_currentMoogFrameCommand->lateral;
		break;
	case Axis::Surge:
		fcurrentPos = m_currentMoogFrameCommand->surge;
		break;
	case Axis::Yaw:
		fcurrentPos = m_currentMoogFrameCommand->yaw;
		convert_rad2deg = true;
		break;
	case Axis::Pitch:
		fcurrentPos = m_currentMoogFrameCommand->pitch;
		convert_rad2deg = true;
		break;
	case Axis::Roll:
		fcurrentPos = m_currentMoogFrameCommand->roll;
		convert_rad2deg = true;
		break;
	};
	LeaveCriticalSection(&m_comCS);

	return fcurrentPos;
}


void MoogCom::DoCompute(unsigned char code)
{
	EnterCriticalSection(&m_comCS);
	m_computeCode = code;
	LeaveCriticalSection(&m_comCS);
}


void MoogCom::ThreadDoCompute(unsigned char code)
{
	m_computeCode = code;
}


void MoogCom::SetPacketRate(double packetRate)
{
	m_packetRate = packetRate;
}


double MoogCom::GetPacketRate() const
{
	return m_packetRate;
}


BOOL MoogCom::SetComThreadPriority(int nPriority)
{
	BOOL x = FALSE;

	if (m_talker != NULL)
	{
		x = SetThreadPriority(m_talker, nPriority);
	}

	return x;
}


int MoogCom::Connect(int &errorCode)
{
	int wasError = 0;

	// Throw up an error if we've already connected.
	if (m_continueSending == false) 
	{
		//cobbect to the moog - open the socket and listen.
		m_MBCIF.Open(m_config);

		// Create the talker thread and make it realtime priority.
		m_continueSending = true;
		m_talker = (HANDLE)_beginthread(talker, 0, this);
		if ((uintptr_t)m_talker == -1L) 
		{
			// Thread failed to be created.
			errorCode = errno;
			return -1;
		}
		SetThreadPriority(m_talker, THREAD_PRIORITY_TIME_CRITICAL);
	}
	else
	{
		// Already connected.
		return 1;
	}

	// No errors!
	return 0;
}


void MoogCom::Disconnect()
{
	if (m_isEngaged)
	{
		Park();
	}

	m_MBCIF.Close();

	// Tell the communications thread to die.
	m_continueSending = false;

	// Sleeping 50ms allows the stop-sending flag to propogate through the
	// communications thread and gracefully terminate it in the event that
	// the parent process calls this function right before it exits.  Otherwise,
	// you will probably get some sort of memory access violation.
	Sleep(50);

	m_talker = NULL;
}

void MoogCom::ForceDisconnect()
{
	m_MBCIF.Close();

	// Tell the communications thread to die.
	m_continueSending = false;

	m_isEngaged = false;

	// Sleeping 50ms allows the stop-sending flag to propogate through the
	// communications thread and gracefully terminate it in the event that
	// the parent process calls this function right before it exits.  Otherwise,
	// you will probably get some sort of memory access violation.
	Sleep(50);

	m_talker = NULL;
}



void MoogCom::Reset()
{
	// Park the motion base before we reset it so that the user doesn't cause
	// it to make sudden movements.
	Park();

	// Get the command buffer lock and stuff the Reset command inside.
	EnterCriticalSection(&m_comCS);
	m_commandBuffer[3] = 0xA0;
	LeaveCriticalSection(&m_comCS);
}

int MoogCom::Engage()
{
	//for not listening to any matlab command during the engage.
	m_ExecutingGuiMBCCommand = true;

	SetThreadPriority(m_talker, THREAD_PRIORITY_NORMAL);
	m_MBCIF.SetSendThreadPriority(THREAD_PRIORITY_NORMAL);
	m_MBCIF.SetReceivehreadPriority(THREAD_PRIORITY_NORMAL);

	// Make sure we don't call the Engage command when the motion base is already
	// engaged.
	if (m_isEngaged)
	{
		return -1;	// Already engaged.
	}

	// Make sure that we're connected to the MBC.
	if (m_continueSending == false)
	{
		return 1;	// Need to connect first.
	}

	// Set the flag that tells us that we've engaged the MOOG.
	m_isEngaged = true;


	m_myfile << "Engaged by MoogDot!!!!!!!!\n";

	// Make sure the Compute() function is not called.
	m_doCompute = false;

	string s = "Mode Dof\nDOF heave ";
	s += to_string(MOTION_BASE_CENTER);
	s += "\nMDOF long 0";
	s += "\nMDOF yaw 0";
	s += "\nMDOF pitch 0";
	s += "\nMDOF roll 0";
	s += "\nMDOF lat 0";
	s += "\nReset\nWaitForState Ready 10 IgnoreFaults\nEngage\nWaitForState Engaged 20\n";

	m_pScriptFile->Load(s, m_myfile);

	m_pScriptFile->Execute(&m_MBCIF);

	EnterCriticalSection(&m_comCS);

	// Starts the motion base at the midpoint.
	ThreadSetAxisPosition(Axis::Heave, MOTION_BASE_CENTER);

	LeaveCriticalSection(&m_comCS);

	Sleep(50);		// Only send the Engage command a couple times.

	//if was listening before Engage command , continue to liten
	m_ExecutingGuiMBCCommand = false;

	SetThreadPriority(m_talker, THREAD_PRIORITY_TIME_CRITICAL);
	m_MBCIF.SetSendThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
	m_MBCIF.SetReceivehreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

	return 0;
}


void MoogCom::Park()
{
	//for not listening to any matlab command during the engage.
	m_ExecutingGuiMBCCommand = true;

	SetThreadPriority(m_talker, THREAD_PRIORITY_NORMAL);
	m_MBCIF.SetSendThreadPriority(THREAD_PRIORITY_NORMAL);
	m_MBCIF.SetReceivehreadPriority(THREAD_PRIORITY_NORMAL);

	// Make sure the Compute() function isn't called.
	m_doCompute = false;
	
	m_myfile << "Disengaged by MoogDot!!!!!!!!\n";

	//Load the command and execute them in the MBC interface.
	m_pScriptFile->Load("Disengage\nWaitForState Ready 30 IgnoreFaults\n" , m_myfile);
	m_pScriptFile->Execute(&m_MBCIF);

	EnterCriticalSection(&m_comCS);
	ThreadSetAxisPosition(Axis::Heave, 0);
	LeaveCriticalSection(&m_comCS);

	Sleep(50);

	// Flag that the motion base is not engaged now.
	m_isEngaged = false;

	//if was listening before Park command , continue to listen.
	m_ExecutingGuiMBCCommand = false;

	SetThreadPriority(m_talker, THREAD_PRIORITY_TIME_CRITICAL);
	m_MBCIF.SetSendThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
	m_MBCIF.SetReceivehreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
}


unsigned char * MoogCom::f_nbo_uc(float num)
{
	unsigned char flipper[4];
	unsigned char *newbuf;

	newbuf = new unsigned char[4];

	memcpy(flipper, &num, 4);
	memcpy(newbuf + 0, flipper + 3, 1);
	memcpy(newbuf + 1, flipper + 2, 1);
	memcpy(newbuf + 2, flipper + 1, 1);
	memcpy(newbuf + 3, flipper + 0, 1);

	return newbuf;
}


float MoogCom::uc_hbo_f(const unsigned char *arg)
{
	float f;
	unsigned char flipper[4];

	memcpy(flipper + 0, arg + 3, 1);
	memcpy(flipper + 1, arg + 2, 1);
	memcpy(flipper + 2, arg + 1, 1);
	memcpy(flipper + 3, arg + 0, 1);
	memcpy(&f, flipper, 4);

	return f;
}
