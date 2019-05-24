/*
* Read a cell array CELL from MATFILE and
* display class for each element.
*
* Calling syntax:
*
*   matreadcellarray <matfile> <cell>
*
* See the MATLAB External Interfaces Guide for
* compiling information.
*
* Copyright 2012 The MathWorks, Inc.
*/

#include <stdlib.h>
#include "mat.h"
#include "MatSoundStimReader.h"




int main(int argc, char **argv)
{
	//get the pointer to the fields in the struct.
	double* leftChannel = nullptr;
	double* rightChannel = nullptr;
	unsigned int chennelSize;

	MatSoundStimReader* matSoundStimReader = new MatSoundStimReader("C:/MoogDots/angles");

	int i = 0;
	while (true)
	{
		i++;
		matSoundStimReader->ReadStruct(16, "stim", leftChannel, rightChannel, chennelSize, true);
		cout << i << endl;
		delete[] rightChannel;
		delete[] leftChannel;
	}
}
