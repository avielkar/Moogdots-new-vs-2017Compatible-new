#ifndef MOGDOTSCOM
#define MOGDOTSCOM


#include "GlobalDefs.h"
#include "GLWindow.h"
#include "ParameterList.h"
#include "CB_Tools.h"
#include "Spline.h"
#include "LPTController.h"
#include "Logger.h"
#include <SDL.h>
#include <SDL_audio.h>



using namespace std;
using namespace LPTInterface;

#define TINY_NUMBER 0.0001f
#define HEAVE_OFFSET 69.3028
#define SPEED_BUFFER_SIZE 30	// Size of the stop buffer for fixation breaks.

// The base of the exponential set of values used to pad
// an abrupt stop when the monkey breaks fixation.
#define EXP_BASE 0.85
#define EXP_BASE2 0.85

// Parameters of the diffence function.
#define LATERAL_POLE 0.099064
#define LATERAL_ZERO 0.03508
#define HEAVE_POLE 0.092322
#define HEAVE_ZERO 0.028394
#define SURGE_POLE 0.097584
#define SURGE_ZERO 0.033943

#define CONTROL_LOOP_TIME 32.0	// Maximum time in ms for the control loop to read continuously from Tempo.

#define SCAN_SIZE 4		// Number of channels to scan for analog input.

#define VOLTS2DEGS 180.0/5.0		// Degs/Volts

#define LPT_PORT	0xb100		//the port to connect to and send dat with it to the EEG.
#define EEG_TRIAL_NUMBER_MSB_OFFSET	0x01
#define EEG_TRIAL_NUMBER_LSB_OFFSET	0x01

#define C_SOUND 343.0	//speed of sound in m/s.
#define MAIN_FREQ_AMPLITUDE_PERCENT 1.0f
#define ADDITIONAL_FREQ_AMPLITUDE_PERCENT 0.2f
#define MAIN_FREQ 500
#define ADDITIONAL_FREQ_0 600
#define ADDITIONAL_FREQ_1 700
#define ADDITIONAL_FREQ_2 400
#define ADDITIONAL_FREQ_3 300


enum CommandRecognitionType 
{
	Valid,

	UnknownType,

	Invalid
};


class MoogDotsCom : public CORE_CLASS
{
private:
	GLWindow *m_glWindow;				// Pointer to the OpenGL stimulus window.

	wxWindow* m_parentWindow;			//The main windows create this window.

	nmMovementData m_data,				// Motion base movement information.
				 m_glData,				// GL scene translation movement information.
				 m_glObjectData,		// GL object translation movement information.
				 m_rotData,				// Rotation data.
				 m_noise,				// Noise data.
				 m_filteredNoise,		// Filtered Noise.
				 m_fpData,
				 m_fpRotData;

	vector<double> m_soundVelocity;

	bool m_moveByMoogdotsTrajectory = false;	//Indicates if to move the MBC by the trajectory calculated by the Moogdots.

	//avi : interpolated version
	nmMovementData m_interpolatedData,				// Motion base interpolated movement information.
				 m_interpolatedRotData;				// Motion base (MBC) interpolated Rotation data.

	vector<bool> m_drawFlashingFrameSquareData;		//Data determines drawing the flashing squares during the frames.

	vector<double> m_sendStamp,			// Time stamp right before sending a UDP packet.
				   m_receiveStamp,		// Time stamp right after receiving a UDP packet.
				   m_recordedLateral,
				   m_recordedHeave,
				   m_recordedSurge,
				   m_interpLateral,
				   m_interpHeave,
				   m_interpSurge,
				   m_interpRotation,
				   m_glRotData,
				   m_glRotEle,
				   m_glRotAz,
				   m_recordedYaw,
				   m_recordedPitch,
				   m_recordedRoll,
				   m_recordedYawVel;
	vector<double> m_swapStamp;
	LARGE_INTEGER m_freq;				// Frequency of the high resolution timer.
	bool m_glWindowExists,				// Indicates if the GLWindow was created or not.
		 m_isLibLoaded,
		 m_drawRegularFeedback,
#if CUSTOM_TIMER
		 m_doSyncPulse,					// Flag to send the sync pulse.
#endif
		 m_verboseMode;					// Indicates if we have verbose output in the message console.
	HGLRC m_threadGLContext;			// Rendering context owned by the communication thread.
	double m_delay;
	int m_recordOffset,
		m_recordIndex;
	wxListBox *m_messageConsole;		// Message console where we print out useful runtime information.
	static const double m_speedBuffer[SPEED_BUFFER_SIZE];
	static const double m_speedBuffer2[SPEED_BUFFER_SIZE];
	DATA_FRAME m_previousPosition;
	nm3DDatum m_rotationVector;
	bool m_reloadCallLists;
	unsigned int m_objects2change;
	bool newRandomStars = false;							/*This is a boolean value indicating if should be a new random generating the stars field. 
																This would be true only before every new repetition.*/
	bool m_trial_finished = true;							// indicating if the trial rendering has finished or is rendered at the moment.
	bool m_waiting_a_little_after_finished = true;			//indicating if the "freezing" after the round time finished is also finshed.
	bool m_oculusIsOn = false;								//The oculus is on only after the 2nd trial and that is due to their bugs with no render at the first time.
	bool m_firstInOnlyFixationPoint = true;					//first in the step of rendering only the fixation point at the current trial.

	CRITICAL_SECTION m_CS;									//critical section for sending the frame for the MBC during the communication.
	const double INTERPOLATION_WIDE = 1;					//the interpolation range(x) wide - dont change this is not really matter.
	const double INTERPOLATION_UPSAMPLING_SIZE = 16.67;		//the interpolation size (the num of points to put for each points).
	bool m_forwardMovement = true;							//indicate if the MBC is now going to move forward or if it has finished the forward movement.
	DATA_FRAME m_finalForwardMovementPosition;				//the last forward position in the forward trajectory.
	
	ovrQuatf m_eyeOrientationQuaternion;					//avi : for the eyes orientation trace.
	unsigned short int* m_orientationsBytesArray = new unsigned short int[sizeof(ovrQuatf) / 2 * 500];			//avi : for the eyes orientation trace.
public:
#if USE_MATLAB | USE_MATLAB_INTERPOLATION
	Engine *m_engine;										// Matlab engine for matlab computation.
	CRITICAL_SECTION m_matlabInterpolation;
	Logger* m_logger;										//used for making a log fie to trace the program(for bugs detection and for controlling).
	int m_roundStartTime;									//the start time of the trial for logging.
	//bool m_receivedFirstSendingHeadCommandFromMatlab = false;
	bool m_finishedMovingBackward = false;					//indicate if the moving to origin (moving backward is finished).

#if USE_MATLAB_DEBUG_GRAPHS
	/*
		Note : this variables are only for debug propose to check ig the system is behaving correct by the graphs it would draw.
	*/
	vector<double> m_debugFrameTime;
	vector<double> m_debugPlaceTime;
	vector<double> m_debugPlace;
	vector<double> m_debugFramePlace;
#endif
#endif

private:
	// Tempo stuff.
	CCB_Tools m_PCI_DIO24_Object,
		      m_PCI_DIO48H_Object;
	int m_RDX_base_address;
	short m_tempoHandle,
		  m_tempoErr;
	char m_tempoBuffer[256];
	bool m_listenMode,
		 m_continuousMode;

	// Analog input variables.
	double m_previousAnalogPosition,	// Store the last position from the analog signal.
		   m_previousAnalogVelocity;	// Store the last velocity derived from the last position.
	CCB_Tools m_PCI_DAS6014_Object;		// Used to store basic info of the DAQ board.
	HGLOBAL m_memHandle;				// Handle to a memory location to store data from an analog input scan.

	bool m_previousBitLow;				// Keeps track of what the previous stop bit was.

	CMatlabRDX *m_matlabRDX;		//Long
	/*CMatlabRDX *m_matlabRDXHeave;
	CMatlabRDX *m_matlabRDXSurge;
	CMatlabRDX *m_matlabRDXLat;
	CMatlabRDX *m_matlabRDXRoll;
	CMatlabRDX *m_matlabRDXYaw;*/

	//the controller for the lpt port to send data with it to the EEG.
	LPTCOntroller* m_EEGLptContoller;
	//the current trial number
	int m_trialNumber;

public:
	MoogDotsCom(char *mbcIP, int mbcPort, char *localIP, int localPort, Logger* logger ,  bool useCustomTimer, wxWindow* parent);
	~MoogDotsCom();

#if USE_MATLAB | USE_MATLAB_INTERPOLATION
	// Creates a new Matlab engine.
	void StartMatlab();

	//Plots the trajevtory of ther current trial with the Matlab engine.
	void PlotTrajectoryGraph();

	// Closes the existing Matlab engine.
	void CloseMatlab();

	// Stuffs recorded data into Matlab.
	void StuffMatlab();
#endif

	// Moves the platform to the origin.
	void MovePlatformToOrigin();

	// ************************************************************************ //
	//	void UpdateGLScene(bool doSwapBuffers)									//
	//		Updates the GL scene to reflect parameter list changes.				//
	//																			//
	//	Inputs: doSwapBuffers -- If true, the OpenGL scene is rendered and		//
	//							 SwapBuffers() is called.						//
	// ************************************************************************ //
	void UpdateGLScene(bool doSwapBuffers);

	// Sets a pointer to the message console of the program.
	void SetConsolePointer(wxListBox *messageConsole);

	// ************************************************************************ //
	//	void InitTempo()														//
	//		Initializes Tempo stuff, like boards.								//
	// ************************************************************************ //
	void InitTempo();

	// ************************************************************************ //
	//	void ListenMode(bool value)												//
	//		Lets MoogDots listen for an external control source.				//
	//																			//
	//	Inputs: value -- true to use external control, false otherwise.			//
	// ************************************************************************ //
	void ListenMode(bool value);

	// ************************************************************************ //
	//	void ShowGLWindow(bool value)											//
	//		Shows or hides the GLWindow.										//
	//																			//
	//	Inputs: value == TRUE to show window, FALSE to hide.					//
	// ************************************************************************ //
	void ShowGLWindow(bool value);

	// ************************************************************************ //
	//	void SetVerbosity(bool value)											//
	//		Sets the verbosity of the message console.							//
	//																			//
	//	Inputs: value = true to turn verbosity on, false to turn off.			//
	// ************************************************************************ //
	void SetVerbosity(bool value);

	//Updates how the Moog should move, if at all.
	//
	void UpdateMovement();

	//Add an item to the message console.
	//
	void AddItemToConsole(string command);

	//Gran commands from the command string for a key with all commands params.
	void GrabCommand(string command, vector<double>& commandParamsOut, string& keywordOut);

	//Add the commands param with it's keyword to the params list if valid , and return command status validation.
	//
	CommandRecognitionType AddCommandParamsToCommandsList(string keyword, vector<double> commandParams);

	//Updates the console box with the command parameters and validation.
	//
	void ShowCommandStatusValidation(string command, string keyword, CommandRecognitionType commandRecognitionType);

	//Clear the console windows with the maximum num of items in the console.
	//
	void ClearMessageConsoleMaxItems();

	// Cues the thread to reload the call lists.
	//
	void ReloadCallLists(unsigned int objects);

	//Updates the statuses members of start trial , waiting , freezing rtc members.
	//
	void UpdateStatusesMembers();

// We only use this if we're using the built-in timer.
#if !CUSTOM_TIMER
public:
	// Returns true if vsync is enabled, false otherwise.
	bool VSyncEnabled();

	// Turns syncing to the SwapBuffer() call on and off.
	void SetVSyncState(bool enable);

private:
		// Sets up the pointers to the functions to modify the vsync for the program.
	void InitVSync();
#endif

private:
	// Overrides
	//

	//Defines the function for start the motion thread and the each frame rendering - making the transfrmations.
	//
	virtual void Compute();
	virtual void CustomTimer();
	virtual void ThreadInit();
	
	//Controls the input output and stuff changes , iclude the message console.
	//
	virtual void Control();
	virtual void Sync();
	virtual void ReceiveCompute();

	//Check if the Moog is at the given position with the given absolute maximum distance.
	//
	bool CheckMoogAtCorrectPosition(MoogFrame* position, double maxDistanceError);

	//Check if the Moog is at the origin with the givem max differential error.
	//
	bool CheckMoogAtOrigin(double maxDifferentialError);

	//Check if the Moog is at the final position with the givem max differential error.
	//
	bool CheckMoogAtFinal(double maxDifferentialError);

	//Check if the Moog is at the correct position (origin or final) with the givem max differential error.
	//
	bool CheckMoogAtCorrectPosition(double maxDifferentialError);

	void SendMBCFrame(int& dataIndex);
	void SendMBCFrameThread(int dataIndex);
	void MoveMBCThread(bool moveBtMoogdotsTraj = false);
	
	static void populate(void* data, Uint8 *stream, int len);
	void PlaySoundThread();
	void CalculateRotateTrajectory();
	void CalculateDistanceTrajectory();
	double CalculateITD(double azimuth, double frequency);
	int ITD2Offset(double ITD);

	void ResetEEGPins(short trialNumber);

	// Checks to see if the E-Stop bit has been set and takes any necessary actions.
	// Returns true if the E-Stop sequence was performed.
	bool CheckForEStop();

	// Generates a buffered stop movement.
	//
	void GenerateBufferedStop();

	// Moves the platform to the origin and loads the trajectory data received
	// from the server.
	void GenerateMovement();

	//Splitting 1 byte of data into 2 bytes for not sending indicator data never(see details in the implementation).
	//
	void ConvertUnsignedShortArrayToByteArrayDedicatedToCommunication(byte data , byte byteArray[2]);

	//Check Matlab ready to receive Oculus Motion and sending it.
	//
	void SendOculusHeadTrackingIfAckedTo();

	//Add the current fram oculus head orientation to the Oculus tracer.
	//
	void AddFrameOculusOrientationToCommulativeOculusOrientationTracer();

	//Sending to matlab after each trial the Oculus head motion points (for each frame , the place of the head).
	//
	void SendHeadMotionTrackToMatlab(unsigned short* orientationsBytesArray, int size);

	// ************************************************************************ //
	//	vector<double> convertPolar2Vector(double elevation, double azimuth,	//
	//									  double magnitude)						//
	//		Converts a movement defined by polar coordinates and a magnitude	//
	//		into a vector.  Angles need to be in radians.						//
	//																			//
	//	Inputs: elevation -- Elevation in radians.								//
	//			azimuth -- Azimuth in radians.									//
	//			magnitude -- Vector length.										//
	//	Returns: A double vector that contains the X, Y, and Z components.		//
	// ************************************************************************ //
	vector<double> convertPolar2Vector(double elevation, double azimuth,
									   double magnitude);

	// ************************************************************************ //
	//	double deg2rad(double deg)												//
	//		Converts a degree value into radians.								//
	//																			//
	//	Inputs: deg -- Degree value to convert.									//
	//	Returns: radian value.													//
	// ************************************************************************ //
	double deg2rad(double deg);

public:
#if USE_MATLAB
	// Puts a double vector into the Matlab workspace.
	void stuffDoubleVector(vector<double> data, const char *variable);
#endif

private:
	// Creates a Frustum object based on the parameter list.
	//
	Frustum createFrustum();

	// Creates a new StarField object based on the parameter list.
	//
	StarField createStarField();

	// Creates a Floor object based on the parameter list.
	//
	Floor createFloor();

	// Creates a Cylinders object based on the parameter list.
	//
	Cylinders createCylinders();

	// Compares two Frustums to see if they are equal.  Returns true if equal,
	// false otherwise.
	bool compareFrustums(Frustum a, Frustum b) const;

	// Compares two StarFields to see if they are equal.  Returns true if equal,
	// false otherwise.
	bool compareStarFields(StarField a, StarField b) const;

	//Compares two Floor objects.  Returns true if equal, false otherwise.
	//
	bool compareFloors(Floor a, Floor b) const;

	// Compares two Cylinders objects.  Returns true if equal, false otherwise.
	//
	bool compareCylinders(Cylinders a, Cylinders b) const;

	// Compares two Grid objects.  Returns true if equal, false otherwise.
	//
	bool compareGrid(Grid a, Grid b) const;


	// Replaces characters that are invalid in Matlab with regular characters.
	// This function is used to generate data structure names for our library.
	string replaceInvalidChars(string s);

	// Checks to see if a Tempo command has been sent.
	//
	string checkTempo();

	// Creates the movement to move the platform from its current
	// position to some specified position.
	void MovePlatform(DATA_FRAME *destination);
public:
	void createGrid(Grid& gr);
	//World world;
	bool redrawTexture;

	GLWindow* GetGLWindow(void) const;

	void RenderFrameInGlPanel();

	Cube createCube(void);

	void AddNoise();

#if TRAJECTORY_SAFETY_CHECK
	// check all trajectories from Matlab and find bumping inside
	//
	bool CheckTrajectories();
	bool FindBumping(vector<double> trajectory);
#endif
};
#endif
