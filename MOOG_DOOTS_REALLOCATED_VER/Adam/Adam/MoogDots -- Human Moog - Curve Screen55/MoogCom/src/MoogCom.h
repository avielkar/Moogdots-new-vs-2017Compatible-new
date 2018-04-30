#pragma once

#ifndef MOGCOM
#define MOGCOM

#include <windows.h>
#include <string>
#include "MBC_Interface.h"
#include "ScriptFile.h"

#include <iostream>
#include <fstream>

using namespace std;

// Defines
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#define DOF_MODE 1

// Starting byte in command buffer.
#if DOF_MODE
// DOF
#define ROLL_INDEX		4	// 4*1
#define PITCH_INDEX		8	// 4*2
#define HEAVE_INDEX		12	// 4*3
#define SURGE_INDEX		16	// 4*4
#define YAW_INDEX		20	// 4*5
#define LATERAL_INDEX	24	// 4*6
#else
// MDA
#define HEAVE_INDEX		60	// 4*15
#define SURGE_INDEX		64	// 4*16
#define LATERAL_INDEX	72	// 4*18
#endif

// Maximum Heave movement in meters.
#if DOF_MODE
//#define HEAVE_MAX -0.36f	// DOF
#define HEAVE_MAX -0.4572
#else
#define HEAVE_MAX 0.18f		// MDA	
#endif

// Number of bytes in a send packet.
#if DOF_MODE
#define PACKET_SIZE 32
#else
#define PACKET_SIZE 88
#endif

#define RETURNPACKET_SIZE 40		// Number of bytes in a return packet.
#define RECEIVE_COMPUTE 0x01
#define COMPUTE 0x02

#define MOTION_BASE_CENTER -0.22077500f
#define ACTUATOR_OFFSET 0.70866f

// Return packet
#define DOF_ROLL_INDEX 0			// Index into the DOF array for Roll.
#define DOF_PITCH_INDEX 1			// Index into the DOF array for Pitch.
#define DOF_HEAVE_INDEX 2			// Index into the DOF array for Heave.
#define DOF_SURGE_INDEX 3			// Index into the DOF array for Surge.
#define DOF_YAW_INDEX 4				// Index into the DOF array for Yaw.
#define DOF_LATERAL_INDEX 5			// Index into the DOF array for Lateral.

#define WORD_SIZE 4					// Number of bytes in a word.

#define SURGE_MAX 0.25f				// Maximum Surge movement in meters.
#define LATERAL_MAX 0.25f			// Maximum Lateral movement in meters.
#define ROLL_MAX 28.9945f			// Maximum Roll movement in degrees.
#define PITCH_MAX 32.9938f			// Maximum Pitch movement in degrees.
#define YAW_MAX 28.9945f				// Maximum Yaw movement in degrees.

#define DOF_ROLL_COMMAND  "DOF roll "		//used for the command name of the DOF roll for the MBC.
#define DOF_PITCH_COMMAND  "DOF pitch "		//used for the command name of the DOF pitch (surge) for the MBC.
#define DOF_HEAVE_COMMAND  "DOF heave "		//used for the command name of the DOF heave for the MBC.
#define DOF_YAW_COMMAND "DOF yaw "			//used for the command name of the DOF yaw for the MBC.
#define DOF_LONG_COMMAND  "DOF long "		//used for the command name of the DOF long for the MBC.
#define DOF_LAT_COMMAND "DOF lat "			//used for the command name of the DOF lateral for the MBC.

typedef struct STRUCT_MOOG_FRAME
{
	double heave;
	double surge;
	double lateral;
	double yaw;
	double pitch;
	double roll;
} MoogFrame;

using namespace std;


class MoogCom
{
private:
	string m_mbcIP,								// MBC IP Address
		m_localIP;							// Controller IP Address
	unsigned char m_commandBuffer[PACKET_SIZE],	// Command buffer
		m_receiveBuffer[RETURNPACKET_SIZE],
		m_com[PACKET_SIZE],
		m_prevCom[PACKET_SIZE];		// Save the previous data send to Moog
	int m_mbcPort,								// MBC port
		m_localPort;							// Controller port
	HANDLE m_talker;							// Communications thread
	bool m_continueSending,						// Determines if communication should continue.
		m_ExecutingGuiMBCCommand,
		m_doCompute,							// Indicates if Compute() is called.
		m_doReceiveCompute,					// Indicates if ReceiveCompute() is called.
		m_isEngaged,							// Indicates if the motion base is engaged.
		m_doCustomTiming,						// Indicates if we want to use SwapBuffers() as the timing constraint.
		m_moogCtrlTiming,						// Indicates if we receive moog feedback, then send data immediately.
		m_syncFrame;							// Indicates we need to call the SyncFrame() function to sync
	// a frame to an outside source.
	double m_packetRate,						// Number of miliseconds between packet sends.
		m_clockFrequency,					// High performance timer clock frequency.
		m_sendTime,							// Time right before a send.
		m_receiveTime;						// Time right after receiving.
	double m_actuatorData[6],
		m_dofValues[6];
	MoogFrame* m_currentMoogFrameCommand;
	unsigned char m_computeCode;				// Indicates which compute functions are called.
	CRITICAL_SECTION m_receiveCS,
		m_comCS;

	CMBCInterface m_MBCIF;    // interface to Motion Base Computer
	const char* m_pszParam = NULL;
	CScriptFile* m_pScriptFile = NULL;

	CConfigFile* m_config;		//The MBC config file.
	ofstream m_myfile;			//used for making a log fie to trace the program(for bugs detection and for controlling).


public:
	enum Axis
	{
		Heave, Surge, Lateral, Yaw, Pitch, Roll, Stimulus
	};

	//*******************************************************************************//
	//	Public member functions.													 //
	//*******************************************************************************//
public:
	// Default constructor
	MoogCom(string mbcIP,			// IP address of the MBC.
		int mbcPort,			// Port on MBC which received UDP packets.
		string localIP,			// IP address of the local computer.
		int localPort,			// Port which will data will be sent through.
		bool useCustomTimer);	// Use a user defined timer or the built in
	// one.

	// Connects to the MBC.  Returns a -1 if an error occurred, 0 if no error occurred, and.
	// 1 if Connect() was called before disconnecting.  errorCode holds the error code, such
	// as erno, if an error occurred.
	int Connect(int &errorCode);

	// Disconnects from the MBC.  This function MUST be called before
	// the parent process dies to make sure that the motion base moves safely to
	// home position and to prevent memory access violations by the communications
	// thread.
	void Disconnect();

	/**
	 * \Disconnect the MBC with no parking attempt (the Moog would park by it's self due to the disconnection.
	 */
	void ForceDisconnect();

	// Parks the motion platform, i.e. puts it in home position.
	void Park();

	// Engages the motion base.  Returns -1 if the base is already engaged, 0 if all is ok, and
	// 1 if we haven't connect to the MOOG yet.
	int Engage();

	// Resets the MBC.  This is used if the MBC goes into fault mode.  Basically,
	// it won't accept anymore commands until it receives the RESET command or is
	// manually rebooted.  Stupid Moog.
	void Reset();

	// Tells the talker thread which compute functions to run.
	void DoCompute(unsigned char code);

	// Sets/Gets the current packet rate.
	void SetPacketRate(double packetRate);
	double GetPacketRate() const;

	// Provides the ability to change whether the talker thread uses the custom timer
	// or the built in one during runtime.
	void UseCustomTimer(bool useCustomTimer);

	// We want to eliminate bumping, so we let moog to control the timing.
	// When we receive feedback from moog computer, we will send data to moog immediately.
	void UseMoogCtrlTimer(bool useMoogCtrlTiming);

	// Sets the thread priority of the talker thread.
	BOOL SetComThreadPriority(int nPriority);

	// Converts a host-byte order float to a 4-byte network-byte unsigned char array.
	unsigned char * f_nbo_uc(float num);

	// Converts a 4-byte network byte order unsigned char array into a host-byte float.
	float uc_hbo_f(const unsigned char *arg);

private:
	// This is the thread that does all communication with the MBC.
	static void __cdecl talker(LPVOID);

	// Returns the data value associated with the given word number in the
	// return UDP packet from the MBC.
	double ExtractReturnData(int word);


	//*******************************************************************************//
	//	These functions must be overridden in derived classes for the derived class	 //
	//	not to be abstract.															 //
	//*******************************************************************************//
protected:
	virtual void Compute() = 0;
	virtual void ReceiveCompute() = 0;
	virtual void CustomTimer() = 0;

	//*******************************************************************************//
	//	These functions may be overridden in derived classes.						 //
	//*******************************************************************************//
protected:
	// Does one time initialization when the thread is first created.
	virtual void ThreadInit();

	// Called before the sequence of compute, receive, send happens.  This
	// function is called every frame.  It's good to put stuff that has to
	// happen or may happen randomly in this function.
	virtual void Control();

	// Function that is called after SyncNextFrame() is called.  The idea is
	// to use whatever you put in this function to align the built-in timer
	// with some other signal.
	virtual void Sync();

	//*******************************************************************************//
	//	The following functions are thread safe, but are only intended to be called	 //
	//  outside of the control/compute functions.  They return the current value in	 //
	//  the command buffer.  Note that calling them within control/compute functions //
	//	won't do any real damage.													 //
	//*******************************************************************************//
protected:
	// Called when you want the next frame to align with some external sync signal.
	void SyncNextFrame();

	// Gets the current value of an axis in the command buffer.
	//
	float GetAxisPosition(Axis axis);

	// Get the current value of the command frame sent to the MBC.
	void GetAxesCommandPosition(MoogFrame *moogFrame);

	//*******************************************************************************//
	//	The following functions ARE NOT thread safe.  They are intended to be used   //
	//	from within the control/compute functions.  These functions are here to		 //
	//	basically prevent recursive calls to mutex resources.  I'm assuming that	 //
	//	everything being done in the thread is time critical.  In other situations,  //
	//	it may provide no benefit.													 //
	//*******************************************************************************//
protected:
	// Gets the last receive timestamp in ms.
	double ThreadGetReceiveTime() const;

	// Gets the last send timestamp in ms.
	double ThreadGetSendTime() const;

	// Gets the current value of every axis.
	//
	void ThreadGetAxesPositions(MoogFrame *moogFrame);

	//Sets the current executed MBC commands.
	//
	void SetCurrentExecutetCommand(MoogFrame* moogFrame);

	// Sends the MBC the moog frame to be executed.
	//
	void SendMBCAxesPositions(MoogFrame *moogFrame);

	//Convert the coomand moog frame to a MBC command string.
	//
	string ConvertFrameToCommand(MoogFrame* commandFrame);

	//Get the current feedback position frame given by the Moog controller.
	//
	MoogFrame GetAxesFeedbackPosition();

	// Gets the current value of an axis in the command buffer.
	//
	void ThreadSetAxisPosition(Axis axis, float value);

	// Gets the last returned Lateral, Heave, and Surge values in meters.
	double ThreadGetReturnedLateral() const;
	double ThreadGetReturnedHeave() const;
	double ThreadGetReturnedSurge() const;

	// Gets the last returned Roll, Pitch, and Yaw values in degrees.
	double ThreadGetReturnedRoll() const;
	double ThreadGetReturnedPitch() const;
	double ThreadGetReturnedYaw() const;

	// Sets which compute functions are called.
	void ThreadDoCompute(unsigned char code);
};

#include "MoogCom.inl"

#endif
