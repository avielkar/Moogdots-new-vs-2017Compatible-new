// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <math.h>
#include <windows.h>

#include <vector>

using namespace std;

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 3.0e-7
#define RNMX (1.0-EPS)
#define ITMAX 100
#define FPMIN 1.0e-30
#define XYPLANE 0
#define XZPLANE 1
#define YZPLANE 2

#define FIRS_TABLE_ROWS 500
#define FIRS_TABLE_COLS 41