#include "StdAfx.h"
#include "NumericalMethods.h"

// Filter lookup tables.
extern double FIRS_TABLE[FIRS_TABLE_ROWS][FIRS_TABLE_COLS];
extern double FIRS_AREA[FIRS_TABLE_ROWS];
extern double STD_TABLE[FIRS_TABLE_ROWS];

bool nmGenerateFilteredNoise(long idum, int noiseLength, double cutoffFreq, nm3DDatum noiseMagnitude, int numDimensions,
							 bool fixNoise, bool fixFilter, nmMovementData *noiseData, nmMovementData *filteredNoiseData)
{
	int i, j;

	// Use this to store the high powered Gaussian we use to multiply the filtered noise signal.
	double *noiseMult;

	// Make sure that the noiseLength is a positive value.
	if (noiseLength <= 0) {
		return false;
	}

	// Also make sure that the number of dimensions is between 1 and 3.
	if (numDimensions < 1 || numDimensions > 3) {
		return false;
	}

	// Clear the movement data.
	noiseData->X.clear(); noiseData->Y.clear(); noiseData->Z.clear();
	filteredNoiseData->X.clear(); filteredNoiseData->Y.clear(); filteredNoiseData->Z.clear();

	// This is the seed to the Gaussian normal function.  We have to initialized the
	// normal function so to do that we pass it the negative value of idum.  Later
	// calls will use positive idum.
	long gseed = -idum;
	nmGasdev(&gseed);

	// Get noise magnitude parameters.
	double magnitudeX = noiseMagnitude.x,
		   magnitudeY = noiseMagnitude.y,
		   magnitudeZ = noiseMagnitude.z;

	// Index into the lookup table based on the cutoff frequency.
	int cutoffIndex = (int)floor(cutoffFreq*10.0 + 0.5) - 1;

	// This tells us how many points to the left and right of the current point
	// we need to look when using our filter.
	int pointBorder = (FIRS_TABLE_COLS - 1) / 2;

	// Make sure we don't go out of bounds in our FIRS lookup table.
	if (cutoffIndex < 0) {
		cutoffIndex = 0;
	}
	else if (cutoffIndex >= FIRS_TABLE_ROWS) {
		cutoffIndex = FIRS_TABLE_ROWS - 1;
	}

	// Pull a value off the normal distribution curve and store it.  Do this for the
	// selected dimensions.
	switch (numDimensions)
	{
	case 3: // 3 dimensions
		for (i = 0; i < noiseLength + FIRS_TABLE_COLS - 1; i++) {
			noiseData->Z.push_back(nmGasdev(&idum)*magnitudeZ);
		}
	case 2: // 2 dimensions
		for (i = 0; i < noiseLength + FIRS_TABLE_COLS - 1; i++) {
			noiseData->Y.push_back(nmGasdev(&idum)*magnitudeY);
		}
	case 1: // 1 dimension
		for (i = 0; i < noiseLength + FIRS_TABLE_COLS - 1; i++) {
			noiseData->X.push_back(nmGasdev(&idum)*magnitudeX);
		}
	}

	// Make sure unspecified dimensions, filtered and original, are filled with zeros.
	if (numDimensions == 1) {
		// Original noise
		for (i = 0; i < noiseLength + FIRS_TABLE_COLS - 1; i++) {
			noiseData->Y.push_back(0.0);
			noiseData->Z.push_back(0.0);
		}
		// Filtered noise
		for (i = 0; i < noiseLength; i++) {
			filteredNoiseData->Y.push_back(0.0);
			filteredNoiseData->Z.push_back(0.0);
		}
	}
	else if (numDimensions == 2) {
		// Original noise
		for (i = 0; i < noiseLength + FIRS_TABLE_COLS - 1; i++) {
			noiseData->Z.push_back(0.0); 
		}
		// Filtered noise
		for (i = 0; i < noiseLength; i++) {
			filteredNoiseData->Z.push_back(0.0);
		}

	}

	// Create a high powered Gaussian curve to multiply the filtered noise.  This will set the ends
	// of the noise to be zero.
	if (fixNoise) {
		noiseMult = new double[noiseLength];
		double t0 = ((double)noiseLength-1.0)/2.0,
			s = (t0/60.0)*55.0,
			exponent = 40.0;

		for (i = 0; i < noiseLength; i++) {
			noiseMult[i] = exp(-pow((i-t0),exponent)/(2*pow(s,exponent)));
		}
	}

	// Create a high powered Gaussian curve to multiply the filter.
	double filterMult[41];
	if (fixFilter) {
		double t0 = 40.0/2.0,
			s = (t0/60.0)*55.0,
			exponent = 22.0;
		for (i = 0; i < 41; i++) {
			filterMult[i] = exp(-pow((i-t0),exponent)/(2*pow(s,exponent)));
		}
	}

	// Filter the noise.  Iterate through every point in the noise
	// and run a FIRS noise filter.  Do this for each axis.
	for (i = 0; i < noiseLength; i++) {
		double noiseSumX = 0.0, noiseSumY = 0.0, noiseSumZ = 0.0;

		// This offsets us into the actual starting point for the filtered
		// noise.  At this value we won't get any distortion due to lack of
		// data points to the left of the point in question.
		int noiseIndex = i + pointBorder;

		// Sums the weighted points based around and including the i-th point.
		for (j = -pointBorder; j <= pointBorder; j++) {
			int tempIndex = noiseIndex + j;
			int jp = j+pointBorder;
			double noiseValue;

			switch (numDimensions)
			{
			case 3:	// 3 dimensions
				// Get Z value.
				noiseValue = noiseData->Z.at(tempIndex);
				if (fixFilter) {
					noiseSumZ += noiseValue*FIRS_TABLE[cutoffIndex][jp]*filterMult[jp];
				}
				else {
					noiseSumZ += noiseValue*FIRS_TABLE[cutoffIndex][jp];
				}
			case 2: // 2 dimensions
				// Get Y value.
				noiseValue = noiseData->Y.at(tempIndex);
				if (fixFilter) {
					noiseSumY += noiseValue*FIRS_TABLE[cutoffIndex][jp]*filterMult[jp];
				}
				else {
					noiseSumY += noiseValue*FIRS_TABLE[cutoffIndex][jp];
				}
			case 1: // 1 dimension
				// Get X value.
				noiseValue = noiseData->X.at(tempIndex);
				if (fixFilter) {
					noiseSumX += noiseValue*FIRS_TABLE[cutoffIndex][jp]*filterMult[jp];
				}
				else {
					noiseSumX += noiseValue*FIRS_TABLE[cutoffIndex][jp];
				}
			} // End switch (numDimensions)
		} // End for (j = -pointBorder; j <= pointBorder; j++)

		// Add the filtered data point to the filtered noise vector.
		switch (numDimensions)
		{
		case 3: // 3 dimensions
			if (fixNoise) {
				filteredNoiseData->Z.push_back(noiseSumZ/STD_TABLE[cutoffIndex]*noiseMult[i]);
			}
			else {
				filteredNoiseData->Z.push_back(noiseSumZ/STD_TABLE[cutoffIndex]);
			}
		case 2: // 2 dimensions
			if (fixNoise) {
				filteredNoiseData->Y.push_back(noiseSumY/STD_TABLE[cutoffIndex]*noiseMult[i]);
			}
			else {
				filteredNoiseData->Y.push_back(noiseSumY/STD_TABLE[cutoffIndex]);
			}
		case 1: // 1 dimension
			if (fixNoise) {
				filteredNoiseData->X.push_back(noiseSumX/STD_TABLE[cutoffIndex]*noiseMult[i]);
			}
			else {
				filteredNoiseData->X.push_back(noiseSumX/STD_TABLE[cutoffIndex]);
			}
		}
	} // End for (i = 0; i < noiseLength; i++)

	if (fixNoise) {
		delete [] noiseMult;
	}

	return true;
}


bool nmRotatePointAboutPoint(nm3DDatum point, nm3DDatum rotPoint, double startAngle, double endAngle,
							 double step, double rotElevation, double rotAzimuth, nmMovementData *data,
							 bool clearData)
{
	double b;

	// Convert the angles to radians.
	startAngle *= DEG2RAD;
	endAngle *= DEG2RAD;
	step *= DEG2RAD;
	rotElevation *= DEG2RAD;
	rotAzimuth *= DEG2RAD;

	if (clearData == true) {
		// Make sure the trajectory data is empty.
		data->X.clear(); data->Y.clear(); data->Z.clear();
	}

	// Precomput sines and cosines.
	double cosE = cos(rotElevation);
	double cosA = cos(rotAzimuth);
	double sinE = sin(rotElevation);
	double sinA = sin(rotAzimuth);

	for (b = startAngle; b <= endAngle; b += step) {
		// Precompute the sin and cosine of b, which is the angle of rotation.
		double sinB = sin(b), cosB = cos(b);

		double xval = ((cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*cosA+(sinA*cosB+sinE*cosA*sinB)*sinA)*point.x +
			(-(cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*sinA+(sinA*cosB+sinE*cosA*sinB)*cosA)*point.y +
			(-cosE*cosA*sinE+(-sinA*sinB+sinE*cosA*cosB)*cosE)*point.z +
			-((cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*cosA+(sinA*cosB+sinE*cosA*sinB)*sinA)*rotPoint.x-(-(cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*sinA+(sinA*cosB+sinE*cosA*sinB)*cosA)*rotPoint.y-(-cosE*cosA*sinE+(-sinA*sinB+sinE*cosA*cosB)*cosE)*rotPoint.z+rotPoint.x;
		data->X.push_back(xval);

		double yval = ((-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*cosA+(cosA*cosB-sinE*sinA*sinB)*sinA)*point.x +
			(-(-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*sinA+(cosA*cosB-sinE*sinA*sinB)*cosA)*point.y +
			(cosE*sinA*sinE+(-cosA*sinB-sinE*sinA*cosB)*cosE)*point.z +
			-((-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*cosA+(cosA*cosB-sinE*sinA*sinB)*sinA)*rotPoint.x-(-(-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*sinA+(cosA*cosB-sinE*sinA*sinB)*cosA)*rotPoint.y-(cosE*sinA*sinE+(-cosA*sinB-sinE*sinA*cosB)*cosE)*rotPoint.z+rotPoint.y;
		data->Y.push_back(yval);

		double zval = ((-sinE*cosE+cosE*cosB*sinE)*cosA+cosE*sinB*sinA)*point.x +
			(-(-sinE*cosE+cosE*cosB*sinE)*sinA+cosE*sinB*cosA)*point.y +
			(sinE*sinE+cosE*cosE*cosB)*point.z +
			-((-sinE*cosE+cosE*cosB*sinE)*cosA+cosE*sinB*sinA)*rotPoint.x-(-(-sinE*cosE+cosE*cosB*sinE)*sinA+cosE*sinB*cosA)*rotPoint.y-(sinE*sinE+cosE*cosE*cosB)*rotPoint.z+rotPoint.z;
		data->Z.push_back(zval);
	}

	return true;
}


void nmRotatePointAboutPoint(nm3DDatum point, nm3DDatum rotPoint,		// Point to rotate and point to rotate about.
							 double rotElevation, double rotAzimuth,	// Elevation and azimuth of rotation axis.
							 vector<double> *trajectory,				// Holds the trajectory of angles.
							 nmMovementData *data,						// Stores the translation components of the trajectory.
							 nmMovementData *rotData,					// Stores the rotation components of the trajectory.
							 bool clearData,							// Flag to clear data and rotData.
							 bool inDegrees)
{
	if (inDegrees == true) {
		// Convert angles from degrees to radians.
		rotElevation *= DEG2RAD;
		rotAzimuth *= DEG2RAD;
	}
	
	// Clear the storage data structures if flagged to do so.
	if (clearData == true) {
		data->X.clear(); data->Y.clear(); data->Z.clear();
		rotData->X.clear(); rotData->Y.clear(); rotData->Z.clear();
	}

	// We have to negate these 2 angles to make 90 degrees elevation point straight up and 90 degrees
	// azimuth point forward.
	rotAzimuth = -rotAzimuth;
	rotElevation = -rotElevation;

	// Precompute sines and cosines.
	double cosE = cos(rotElevation),
		   cosA = cos(rotAzimuth),
		   sinE = sin(rotElevation),
	   	   sinA = sin(rotAzimuth);

	// Calculate the rotation vector.
	nm3DDatum rotationVector = nmSpherical2Cartesian(rotElevation, rotAzimuth, 1.0, false);

	for (int i = 0; i < (int)trajectory->size(); i ++) {
		// Determine what the angle of rotation will be and precompute the sin and cosine of it.
		double b = trajectory->at(i);
		if (inDegrees == true) {
			b *= DEG2RAD;
		}
		double sinB = sin(b),
			   cosB = cos(b); 

		double xval = ((cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*cosA+(sinA*cosB+sinE*cosA*sinB)*sinA)*point.x +
			(-(cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*sinA+(sinA*cosB+sinE*cosA*sinB)*cosA)*point.y +
			(-cosE*cosA*sinE+(-sinA*sinB+sinE*cosA*cosB)*cosE)*point.z +
			-((cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*cosA+(sinA*cosB+sinE*cosA*sinB)*sinA)*rotPoint.x-(-(cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*sinA+(sinA*cosB+sinE*cosA*sinB)*cosA)*rotPoint.y-(-cosE*cosA*sinE+(-sinA*sinB+sinE*cosA*cosB)*cosE)*rotPoint.z+rotPoint.x;
		data->X.push_back(xval);

		double yval = ((-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*cosA+(cosA*cosB-sinE*sinA*sinB)*sinA)*point.x +
			(-(-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*sinA+(cosA*cosB-sinE*sinA*sinB)*cosA)*point.y +
			(cosE*sinA*sinE+(-cosA*sinB-sinE*sinA*cosB)*cosE)*point.z +
			-((-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*cosA+(cosA*cosB-sinE*sinA*sinB)*sinA)*rotPoint.x-(-(-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*sinA+(cosA*cosB-sinE*sinA*sinB)*cosA)*rotPoint.y-(cosE*sinA*sinE+(-cosA*sinB-sinE*sinA*cosB)*cosE)*rotPoint.z+rotPoint.y;
		data->Y.push_back(yval);

		double zval = ((-sinE*cosE+cosE*cosB*sinE)*cosA+cosE*sinB*sinA)*point.x +
			(-(-sinE*cosE+cosE*cosB*sinE)*sinA+cosE*sinB*cosA)*point.y +
			(sinE*sinE+cosE*cosE*cosB)*point.z +
			-((-sinE*cosE+cosE*cosB*sinE)*cosA+cosE*sinB*sinA)*rotPoint.x-(-(-sinE*cosE+cosE*cosB*sinE)*sinA+cosE*sinB*cosA)*rotPoint.y-(sinE*sinE+cosE*cosE*cosB)*rotPoint.z+rotPoint.z;
		data->Z.push_back(zval);

		// This calculates how much the point yaws, pitches, and rolls about the rotation axis given some theta b.
		double pitch  = -asin(rotationVector.y*rotationVector.z*(1-cos(b)) - sin(b)*rotationVector.x);
		double roll =  asin((rotationVector.x*rotationVector.z*(1-cos(b)) + sin(b)*rotationVector.y)/cos(pitch));
		double yaw  =  asin((rotationVector.y*rotationVector.x*(1-cos(b)) + sin(b)*rotationVector.z)/cos(pitch));
		rotData->X.push_back(yaw*RAD2DEG);
		rotData->Y.push_back(pitch*RAD2DEG);
		rotData->Z.push_back(roll*RAD2DEG);
	}
}


void nmSinusoidPointAboutPoint(nm3DDatum point, nm3DDatum rotPoint, double amplitude, double frequency,	double duration,
							   double phase, double centerAngle, double step, double rotElevation, double rotAzimuth,
							   nmMovementData *data, nmMovementData *rotData)
{
	// Convert angles from degrees to radians.
	rotElevation *= DEG2RAD;
	rotAzimuth *= DEG2RAD;
	phase *= DEG2RAD;

	// We have to negate these 2 angles to make 90 degrees elevation point straight up and 90 degrees
	// azimuth point forward.
	rotAzimuth = -rotAzimuth;
	rotElevation = -rotElevation;

	// Precompute sines and cosines.
	double cosE = cos(rotElevation),
		cosA = cos(rotAzimuth),
		sinE = sin(rotElevation),
		sinA = sin(rotAzimuth);

	// Calculate the rotation vector.
	nm3DDatum rotationVector = nmSpherical2Cartesian(rotElevation, rotAzimuth, 1.0, false);

	for (double i = 0.0; i < duration; i += step) {
		// Determine what the angle of rotation will be and precompute the sin and cosine of it.
		double b = (centerAngle + amplitude*sin(phase+i*2.0*PI*frequency))/180.0*PI,
			sinB = sin(b),
			cosB = cos(b); 

		double xval = ((cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*cosA+(sinA*cosB+sinE*cosA*sinB)*sinA)*point.x +
			(-(cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*sinA+(sinA*cosB+sinE*cosA*sinB)*cosA)*point.y +
			(-cosE*cosA*sinE+(-sinA*sinB+sinE*cosA*cosB)*cosE)*point.z +
			-((cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*cosA+(sinA*cosB+sinE*cosA*sinB)*sinA)*rotPoint.x-(-(cosE*cosE*cosA+(-sinA*sinB+sinE*cosA*cosB)*sinE)*sinA+(sinA*cosB+sinE*cosA*sinB)*cosA)*rotPoint.y-(-cosE*cosA*sinE+(-sinA*sinB+sinE*cosA*cosB)*cosE)*rotPoint.z+rotPoint.x;
		data->X.push_back(xval);

		double yval = ((-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*cosA+(cosA*cosB-sinE*sinA*sinB)*sinA)*point.x +
			(-(-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*sinA+(cosA*cosB-sinE*sinA*sinB)*cosA)*point.y +
			(cosE*sinA*sinE+(-cosA*sinB-sinE*sinA*cosB)*cosE)*point.z +
			-((-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*cosA+(cosA*cosB-sinE*sinA*sinB)*sinA)*rotPoint.x-(-(-cosE*cosE*sinA+(-cosA*sinB-sinE*sinA*cosB)*sinE)*sinA+(cosA*cosB-sinE*sinA*sinB)*cosA)*rotPoint.y-(cosE*sinA*sinE+(-cosA*sinB-sinE*sinA*cosB)*cosE)*rotPoint.z+rotPoint.y;
		data->Y.push_back(yval);

		double zval = ((-sinE*cosE+cosE*cosB*sinE)*cosA+cosE*sinB*sinA)*point.x +
			(-(-sinE*cosE+cosE*cosB*sinE)*sinA+cosE*sinB*cosA)*point.y +
			(sinE*sinE+cosE*cosE*cosB)*point.z +
			-((-sinE*cosE+cosE*cosB*sinE)*cosA+cosE*sinB*sinA)*rotPoint.x-(-(-sinE*cosE+cosE*cosB*sinE)*sinA+cosE*sinB*cosA)*rotPoint.y-(sinE*sinE+cosE*cosE*cosB)*rotPoint.z+rotPoint.z;
		data->Z.push_back(zval);

		// This calculates how much the point yaws, pitches, and rolls about the rotation axis given some theta b.
		double pitch  = -asin(rotationVector.y*rotationVector.z*(1-cosB) - sinB*rotationVector.x);
		double roll =  asin((rotationVector.x*rotationVector.z*(1-cosB) + sinB*rotationVector.y)/cos(pitch));
		double yaw  =  asin((rotationVector.y*rotationVector.x*(1-cosB) + sinB*rotationVector.z)/cos(pitch));
		rotData->X.push_back(yaw*RAD2DEG);
		rotData->Y.push_back(pitch*RAD2DEG);
		rotData->Z.push_back(roll*RAD2DEG);
	}
}


int nmTrapIntegrate(vector<double> *data,				// Holds all the data points of the function.
					vector<double> *integral,
					double &isum,						// The result of the integration.
					int lowIndex,						// Index from which to start integration.
					int highIndex,						// Index to end integration.
					double width)						// Distance between 2 points.
{
	int numPoints = (int)data->size();

	// Check for all the screwup conditions.
	if (numPoints < 2 || lowIndex < 0 || highIndex >= numPoints || highIndex <= lowIndex) {
		return -1;
	}

	// Clear the integral data structure and make 0 to be the 1st data point.
	integral->clear();
	integral->push_back(0.0);

	// Initialize isum;
	isum = 0.0;

	// Precompute width divided by 2.0;
	double w_over_2 = width/2.0;

	// Now iterate through the specified index range and sum up the trapezoids.
	for (int i = lowIndex; i < highIndex; i++) {
		isum += w_over_2*(data->at(i) + data->at(i+1));
		integral->push_back(isum);
	}

	return 0;
}


int nmGenGaussianCurve(vector<double> *curve,			// Holds the range values of the Gaussian.
		 			   double amplitude,				// Amplitude of the peak range value.
					   double width,					// Max domain value.
					   double div,						// Delta x
					   double sigma,					// # sigmas = width/2.0.  Increasing this makes the gaussian steaper.
					   int power,						// Power of the Gaussian.  Must be a multiple of 2 and non-zero.
					   bool clearData)					// If true, curve is cleared first.
{
	// Make sure that the power is a multiple of 2 greater than 0.
	if (power <= 0 || power%2 != 0) {
		return -1;
	}

	if (clearData == true) {
		curve->clear();
	}

	int trajectory_length = (int)(width*div) + 1;
	double w0 = width/2.0,
		   s = w0/sigma;

	for (int i = 0; i < trajectory_length; i++) {
		double w = (double)i / div;
		curve->push_back(exp(-pow(w-w0, (double)power)/2.0/pow(s, (double)power)));
	}

	return 1;
}


int nmGen1DVGaussTrajectory(vector<double> *trajectory,	// Holds the calculated trajectory.
						   double magnitude,			// Magnitude of movement.
						   double duration,				// Duration of movement.
						   double div,					// Time division.  60 would mean 60 steps per second.
						   double sigma,				// # sigmas = duration/2.0.  Increasing this makes the gaussian steaper.
						   double offset,
						   bool clearData)				// If true, trajectory is cleared first.
{
	double amplitude, xval, namp, area_half,
		   tdist = magnitude/2.0;

	if (clearData == true) {
		trajectory->clear();
	}

	// Determine the amplitude needed for the Gaussian.
	try {
		xval = sqrt(2.0) * (duration - duration/2.0)/(duration/sigma);
		namp = sqrt(2.0) / (sqrt(PI)*duration/2.0/sigma*erff(xval));
		amplitude = tdist*namp;
	}
	catch (...) {
		return -1;
	}

	// Calculate a position offset for the trajectory, otherwise we'll generate negative positions.
	// Basically we're just getting half the distance traveled during the gaussian.
	try {
		area_half = -amplitude/2.0 * sqrt(PI) * sqrt(2.0) * duration/2.0/sigma * erff((-duration/2.0) * sqrt(2.0) / (duration/sigma));
	}
	catch (...) {
		return -1;
	}

	// Now calculate the position trajectory.
	int trajectory_length = (int)(duration*div) + 1;
	// Precompute stuff.
	double pcalc = sqrt(PI)*duration/2.0/sigma;
	for (int i = 0; i < trajectory_length; i++) {
		double timeStep = (double)i / div,	// Time of the current Gaussian step.
			   t_i = 0.0;

		// I separated the Gaussian calculation into several steps.
		// It could have been done in one step, but it's easier to debug and read this way.
		try {
			xval = sqrt(2.0) * (timeStep - duration/2.0)/(duration/sigma);
			namp = sqrt(2.0) / (pcalc*erff(xval));
			t_i = amplitude/namp;
		}
		catch (...) {
			return -1;
		}

		// Store the trajectory point after adding half the area and the offset.
		trajectory->push_back(t_i + area_half + offset);
	}

	return 1;
}


int nmGen3DVGaussTrajectory(nmMovementData *trajectory, double elevation, double azimuth,
						   double distance, double duration, double div, double sigma,
						   nm3DDatum offsets, bool inDegrees, bool clearData)
{
	double amplitude, xval, namp, area_half,
		   tdist = distance/2.0;

	if (clearData == true) {
		nmClearMovementData(trajectory);
	}

	if (inDegrees == true) {
		elevation *= DEG2RAD;
		azimuth *= DEG2RAD;
	}

	// Determine the amplitude needed for the Gaussian.
	try {
		xval = sqrt(2.0) * (duration - duration/2.0)/(duration/sigma);
		namp = sqrt(2.0) / (sqrt(PI)*duration/2.0/sigma*erff(xval));
		amplitude = tdist*namp;
	}
	catch (...) {
		return -1;
	}

	// Calculate a position offset for the trajectory, otherwise we'll generate negative positions.
	// Basically we're just getting half the distance traveled during the gaussian.
	try {
		area_half = -amplitude/2.0 * sqrt(PI) * sqrt(2.0) * duration/2.0/sigma * erff((-duration/2.0) * sqrt(2.0) / (duration/sigma));
	}
	catch (...) {
		return -1;
	}

	// Now calculate the position trajectory.
	int trajectory_length = (int)(duration*div) + 1;
	// Precompute stuff.
	double pcalc = sqrt(PI)*duration/2.0/sigma,
		   sin_e = sin(elevation),
		   cos_e_sin_a = cos(elevation) * sin(azimuth),
		   cos_e_cos_a = cos(elevation) * cos(azimuth);
	for (int i = 0; i < trajectory_length; i++) {
		double timeStep = (double)i / div,	// Time of the current Gaussian step.
			   t_i = 0.0;

		// I separated the Gaussian calculation into several steps.
		// It could have been done in one step, but it's easier to debug and read this way.
		try {
			xval = sqrt(2.0) * (timeStep - duration/2.0)/(duration/sigma);
			namp = sqrt(2.0) / (pcalc*erff(xval));
			t_i = amplitude/namp;
		}
		catch (...) {
			return -1;
		}

		// This rotates the trajectory point using the elevation and azimuth.
		double tia = t_i + area_half;
		double zi = tia * sin_e + offsets.z;
		double yi = tia * cos_e_sin_a + offsets.y;
		double xi = tia * cos_e_cos_a + offsets.x;

		// Now store the point in the trajectory data structure.
		trajectory->Z.push_back(zi);
		trajectory->Y.push_back(yi);
		trajectory->X.push_back(xi);
	}

	return 1;
}


int nmGen3DVGaussTrajectory(nmMovementData *trajectory,	// Calculated trajectory is stored in this variable.
						   nm3DDatum startPoint,		// Start point of the trajectory.
						   nm3DDatum endPoint,			// End point of the trajectory.
						   double duration,				// Time of the movement in seconds.
						   double div,					// Time division. 60 would mean 60 steps per second.
						   double sigma,				// # sigmas = duration/2.0.  Increasing this makes the gaussian steaper.
						   bool clearData)				// If true, trajectory is cleared.
{
	double area_half, trajectory_length;
	double x, y, z;							// Component trajectory distance.
	double elevation, azimuth, tdist;
	double amplitude;						// Amplitude of the Gaussian.
	double xval, namp;

	// Find the total distance traveled from the start to end point.
	x = endPoint.x - startPoint.x;
	y = endPoint.y - startPoint.y;
	z = endPoint.z - startPoint.z;
	tdist = sqrt(x*x + y*y + z*z) / 2.0;

	// Calculate the elevation and azimuth.
	if (x == 0.0 && y != 0.0) {
		x = 1.0;
		azimuth = PI / 2.0;
	}
	else if (y == 0.0 && x != 0.0) {
		y = 1.0;
		azimuth = 0.0;
	}
	else if (y == 0.0 && x == 0.0) {
		x = 1.0; y = 1.0;
		azimuth = 0.0;
	}
	else {
		azimuth = atan(fabs(y/x));
	}

	// If tdist is 0.0 then set elevation manually, otherwise we'll get a divide by zero
	// error.
	if (tdist != 0.0) {
		elevation = asin(z / (tdist*2.0));
	}
	else {
		elevation = 0.0;
	}

	// Solve for the amplitude.
	try {
		xval = sqrt(2.0) * (duration - duration/2.0)/(duration/sigma);
		namp = sqrt(2.0) / (sqrt(PI)*duration/2.0/sigma*erff(xval));
		amplitude = tdist*namp;
	}
	catch (...) {
		return -1;
	}

	// Calculate a position offset for the trajectory, otherwise we'll generate negative positions.
	// Basically we're just getting half the distance traveled during the gaussian.
	try {
		area_half = -amplitude/2.0 * sqrt(PI) * sqrt(2.0) * duration/2.0/sigma * erff((-duration/2.0) * sqrt(2.0) / (duration/sigma));
	}
	catch (...) {
		return -1;
	}

	// Now calculate the position trajectory.
	double y_polarity = y/fabs(y);
	double x_polarity = x/fabs(x);
	trajectory_length = (int)(duration*div) + 1;
	// Precompute stuff.
	double pcalc = sqrt(PI)*duration/2.0/sigma,
		   sin_e = sin(elevation),
		   cos_e_sin_a = cos(elevation) * sin(azimuth),
		   cos_e_cos_a = cos(elevation) * cos(azimuth);
	for (int i = 0; i < trajectory_length; i++) {
		double timeStep = (double)i / div,	// Time of the current Gaussian step.
			   t_i = 0.0;

		// I separated the Gaussian calculation into several steps.
		// It could have been done in one step, but it's easier to debug and read this way.
		try {
			xval = sqrt(2.0) * (timeStep - duration/2.0)/(duration/sigma);
			namp = sqrt(2.0) / (pcalc*erff(xval));
			t_i = amplitude/namp;
		}
		catch (...) {
			return -1;
		}

		// This rotates the trajectory point using the elevation and azimuth.
		double tia = t_i + area_half;
		double zi = tia*sin_e + startPoint.z;
		double yi = tia*cos_e_sin_a*y_polarity + startPoint.y;
		double xi = tia*cos_e_cos_a*x_polarity + startPoint.x;

		// Now store the point in the trajectory data structure.
		trajectory->Z.push_back(zi);
		trajectory->Y.push_back(yi);
		trajectory->X.push_back(xi);
	}

	return 1;
}


void nmClearMovementData(nmMovementData *data)
{
	data->index = 0;
	data->X.clear();
	data->Y.clear();
	data->Z.clear();
}


nm3DDatum nmSpherical2Cartesian(double elevation, double azimuth, double radius, bool inDegrees)
{
	nm3DDatum val;

	// Convert the angle into radians if flagged to do so.
	if (inDegrees == true) {
		elevation *= DEG2RAD;
		azimuth *= DEG2RAD;
	}

	val.z = radius * sin(elevation);
	val.y = radius * cos(elevation) * sin(azimuth);
	val.x = radius * cos(elevation) * cos(azimuth);

	return val;
}


bool nmRotateDataYZ(nmMovementData *data, double azimuth, double elevation)
{
	int i;
	double a_cos, a_sin, e_cos, e_sin;

	// Precompute the sin and cos of the azimuth and elevation to speed up the following
	// calculations.
	azimuth *= DEG2RAD;
	elevation *= DEG2RAD;
	a_cos = cos(azimuth);
	a_sin = sin(azimuth);
	e_cos = cos(elevation);
	e_sin = sin(elevation);

	// Get the length of the x vector.
	int dataLength = (int)data->X.size();

	// Make sure that the lengths of each vector are the same.
	if (dataLength != (int)data->Y.size() || dataLength != (int)data->Z.size()) {
		return false;
	}

	// Run the data points through the rotation matrix.
	for (i = 0; i < dataLength; i++) {
		double xi = data->X.at(i),
			   yi = data->Y.at(i),
			   zi = data->Z.at(i);

		data->X.at(i) = xi*e_cos*a_cos + yi*a_sin + zi*e_sin*a_cos;
		data->Y.at(i) = -xi*e_cos*a_sin + yi*a_cos - zi*e_sin*a_sin;
		data->Z.at(i) = -xi*e_sin + zi*e_cos;
	}

	return true;
}

void gcf(double *gammcf, double a, double x, double *gln)
{
	int i;
	double an, b, c, d, del, h;

	// Setup for evaluating continued fraction by modified
	// Lentz's method, with b[0] = 0.
	*gln = gammln(a);
	b = x + 1.0 - a;
	c = 1.0/FPMIN;
	d = 1.0/b;
	h = d;

	// Iterate to convergance.
	for (i = 1; i <= ITMAX; i++) {
		an = -i*(i - a);
		b += 2.0;
		d = an*d + b;

		if (fabs(d) < FPMIN) {
			d = FPMIN;
		}

		c = b + an/c;

		if (fabs(c) < FPMIN) {
			c = FPMIN;
		}

		d = 1.0/d;
		del = d*c;
		h *= del;

		if (fabs(del - 1.0) < EPS) {
			break;
		}
	} // End for loop.

	if (i > ITMAX) {
		throw "gcf: a too large, ITMAX too small.";
	}

	// Put factors in front.
	*gammcf = exp(-x + a*log(x) - (*gln))*h;
}

double gammp(double a, double x)
{
	double gamser, gammcf, gln;

	if (x < 0.0 || a <= 0.0) {
		throw "gammp: Invalid arguments.";
	}

	if (x < (a + 1.0)) {
		try {
			gser(&gamser, a, x, &gln);
		}
		catch (...) {
			throw;
		}

		return gamser;
	}
	else {
		try {
			gcf(&gammcf, a, x, &gln);
		}
		catch (...) {
			throw;
		}

		return 1.0 - gammcf;
	}
}

void gser(double *gamser, double a, double x, double *gln)
{
	int n;
	double sum, del, ap;

	*gln = gammln(a);

	if (x <= 0.0) {
		// Make sure x isn't negative.
		if (x < 0.0) {
			throw "gser: x less than 0.";
		}

		*gamser = 0.0;

		return;
	}
	else {
		ap = a;
		del = sum = 1.0/a;

		for (n = 1; n <= ITMAX; n++) {
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser = sum * exp(-x + a*log(x) - (*gln));
				return;
			}
		}

		throw "gser: a too large, ITMAX too small.";
	}
}

double gammln(double xx)
{
	double x, y, tmp, ser;
	static double cof[6] = {76.18009172947146,
						    -86.50532032941677,
							24.01409824083091,
							-1.231739572450155,
							0.1208650973866179e-2,
							-0.5395239384953e-5};
	int j;

	y = x = xx;
	tmp = x + 5.5;
	tmp -= (x + 0.5)*log(tmp);
	ser = 1.000000000190015;

	for (j = 0; j <= 5; j++) {
		ser += cof[j]/++y;
	}

	return -tmp + log(2.5066282746310005*ser/x);
}

double erff(double x)
{
	double result;

	try {
		if (x < 0.0) {
			result = -gammp(0.5, x*x);
		}
		else {
			result = gammp(0.5, x*x);
		}
	}
	catch (...) {
		throw;
	}

	return result;
}

double * linear_interp(double *data, int data_length, int interp_factor, int &interp_len)
{
	int i, j, index = 0;;
	double pdiff;
	double *interpolatedData = NULL;

	// Only do the interpolation if we don't get bogus data lengths or
	// interpolation factors.
	if (data_length > 0 && interp_factor > 0) {
		interp_len = (data_length-1)*interp_factor + 1;
		interpolatedData = new double[interp_len];

		for (i = 1; i < data_length; i++) {
			pdiff = data[i] - data[i-1];
			
			interpolatedData[index++] = data[i-1];
			for (j = 0; j < interp_factor - 1; j++) {
				interpolatedData[index++] = pdiff/interp_factor*(j + 1) + data[i-1];
			}
		}
		interpolatedData[index] = data[i-1];
	}

	return interpolatedData;
}


double ran1(long *idum)
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];
	double temp;
	if (*idum <= 0 || !iy) {		// Initialize.
		if (-(*idum) < 1) *idum=1;  // Be sure to prevent idum = 0.
		else *idum = -(*idum);
		for (j = NTAB+7;j>=0;j--) {	// Load the shuffle table (after 8 warm-ups).
			k = (*idum)/IQ;
			*idum = IA*(*idum-k*IQ)-IR*k;
			if (*idum < 0) {
				*idum += IM;
			}
			if (j < NTAB) {
				iv[j] = *idum;
			}
		}
		iy = iv[0];
	}
	k = (*idum)/IQ; // Start here when not initializing.
	*idum = IA*(*idum-k*IQ)-IR*k; // Compute idum=(IA*idum) % IM without overflows by Schrage’s method.
	if (*idum < 0) {
		*idum += IM;
	}
	j = iy/NDIV;	// Will be in the range 0..NTAB-1.
	iy = iv[j];		// Output previously stored value and refill the
	iv[j] = *idum;	// shuffle table.
	if ((temp = AM*iy) > RNMX) {
		return RNMX; // Because users don’t expect endpoint values.
	}
	else {
		return temp;
	}
}


double nmGasdev(long *idum)
{
	static int iset = 0;
	static double gset;
	double fac, rsq, v1, v2;

	if (*idum < 0) iset = 0; //Reinitialize.
	if (iset == 0) { //We don’t have an extra deviate handy, so
		do {
			v1 = 2.0*ran1(idum)-1.0;	// pick two uniform numbers in the square extending
			v2 = 2.0*ran1(idum)-1.0;	// from -1 to +1 in each direction
			rsq = v1*v1+v2*v2;		// see if they are in the unit circle,
		} while (rsq >= 1.0 || rsq == 0.0); // and if they are not, try again.
		fac = sqrt(-2.0*log(rsq)/rsq);
		//Now make the Box-Muller transformation to get two normal deviates. Return one and
		//save the other for next time.
		gset = v1*fac;
		iset = 1; // Set flag.
		return v2*fac;
	} else { // We have an extra deviate handy,
		iset=0; // so unset the flag,
		return gset; // and return it.
	}
}


int nmGenDerivativeCurve(vector<double> *curve, vector<double> *y, double dx, bool clearData)
{	 
	// dx must be greater than zero to prevent a divide by zero error.
	if (dx <= 0) {
		return -1;
	}

	// Clear the date in the output vector if requested.
	if (clearData == true) {
		curve->clear();
	}

	// Initialize the 1st point to zero to make the derivative the same length
	// as the input.
	curve->push_back(0.0);

	// Loop through the input and calculate the dy/dx.
	int trajectory_length = (int)y->size();
	for (int i = 1; i < trajectory_length; i++) {
		curve->push_back((y->at(i) - y->at(i-1))/dx);
	}

	return 1;
}
