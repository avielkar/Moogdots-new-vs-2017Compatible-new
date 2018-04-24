// GlobalDefs.h -- Holds pound defines used throughout the program and a couple of header files that optionally get
// loaded depending on certain defines.  Put these things here instead on StdAfx.h to avoid lengthy recompiles.
#pragma once

#ifndef GLOBALDEFS
#define GLOBALDEFS

#define GL_OFFSET 22.86
#define DATA_FRAME MoogFrame
#define CORE_CLASS MoogCom
#define CORE_CONSTRUCTOR MoogCom(mbcIP, mbcPort, localIP, localPort, useCustomTimer)
#define SET_DATA_FRAME SendMBCAxesPositions
#define GET_FEEDBACK_DATA_FRAME GetAxesFeedbackPosition
#define THREAD_GET_DATA_FRAME ThreadGetAxesPositions

#define DEBUG_DEFAULTS 0

#define SHOW_GL_WINDOW 1	// Show the OpenGL rendering window.
#define SWAP_TIMER 0		// Time the speed of SwapBuffers().
#define CUSTOM_TIMER 0		// Have the thread use a custom defined timer.
#define INSERT_BUMP 0		// Insert a manually generated bump into the movement.
#define USE_MATLAB 0		// Load Matlab crap.
#define USE_LOCALHOST 1		// Only communicate with the localhost.
#define DUAL_MONITORS 1		// Dual monitor support.
#define WEIRD_MONITOR 0
#define FLIP_MONITORS 0
#define SINGLE_CPU_MACHINE 0
#define CIRCLE_TEST 0
#define ESTOP 0
//#define USE_STEREO 1
#define RECORD_MODE 0
#define USE_ANALOG_OUT_BOARD 1
#define PCI_DIO_24H_PRESENT 0
#define FIRST_PULSE_ONLY 0
#define SMALL_MONITOR 0
//USE_MATLAB_INTERPOLATION is for using a matlab engine for interpolation of the points from the oculus frequency tp the MBC frequency (can use the matlab but also the c++ spline.h).
#define USE_MATLAB_INTERPOLATION 1
//USE_MATLAB_DEBUG_GRAPHS is for plotting the graphs of the MBC place VS time and VS Oculus frame index.
#define USE_MATLAB_DEBUG_GRAPHS 0

// Set various digital in/out info depending on whether we
// have a 24h board.
#if PCI_DIO_24H_PRESENT
#define PULSE_OUT_BOARDNUM m_PCI_DIO24_Object.DIO_board_num
#define ESTOP_IN_BOARDNUM m_PCI_DIO24_Object.DIO_board_num
#else
#define PULSE_OUT_BOARDNUM m_PCI_DIO48H_Object.DIO_board_num
#define ESTOP_IN_BOARDNUM m_PCI_DIO48H_Object.DIO_board_num
#endif

#if SMALL_MONITOR
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#else
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#endif

#define FIRS_TABLE_ROWS 500
#define FIRS_TABLE_COLS 41

#if DEBUG_DEFAULTS
//#undef USE_STEREO
//#define USE_STEREO 0
#undef DUAL_MONITORS
#define DUAL_MONITORS 0
#undef USE_LOCALHOST
#define USE_LOCALHOST 1
#undef USE_MATLAB
#define USE_MATLAB 0
#undef WEIRD_MONITOR
#define WEIRD_MONITOR 0
#undef SWAP_TIMER
#define SWAP_TIMER 0
#undef ESTOP
#define ESTOP 0
#undef USE_ANALOG_OUT_BOARD
#define USE_ANALOG_OUT_BOARD 1
#undef SINGLE_CPU_MACHINE
#define SINGLE_CPU_MACHINE 0
#endif

#define MAX_CONSOLE_LENGTH 100

// Offsets for the center of rotation from the centroid of the
// platform in meters.
#define CENTROID_OFFSET_X 0.0
#define CENTROID_OFFSET_Y 0.0
#define CENTROID_OFFSET_Z 0.90

#define CENTER2SCREEN 100//37.5

#define FP_ORIGIN_ADJUST -2.5 //cm

/*
** Scales a distance (deg) value into an unsigned short to be sent to the PCI-DDA02/16 board.
**   Max unsigned short = 65535
**   Max magnitude of Moog movement = 50deg
**   Value that corresponds to "send 0 Volts" = 65535/2 = 32767.5
** End result is to map distance 50deg into unsigned short between 0 and 65535.
*/
#define DASCALE(val) (unsigned short)(((val)*32767.5/50)+32767.5)

// Conversion macros.
#define CMPERDEG  g_pList.GetVectorData("VIEW_DIST").at(0)*PI/180.0

// define grid file directory
#define GRID_FILE_DIR "C:"

// define trajectory bumping length (cm)
#define TRAJECTORY_SAFETY_CHECK 1
#define TRAJ_CHANGE_CHECK_SCALE 0.1
#define MAX_ACCELERATION 2 // cm/s^2

//define draw stars' center point
#define CENTER_POINT_DRAWING 0

//'1' for drawing the triangles at the last movement between the trials or '0' for drawing a black background with the fixation point.
#define DRAWING_BETWEEN_TRIALS 0

#endif
