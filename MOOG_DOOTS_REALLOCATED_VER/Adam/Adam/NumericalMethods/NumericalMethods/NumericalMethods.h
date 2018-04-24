////////////////////////////////////////////////////////////////////////////////////////////////
//	NumericalMethods.h
//
//	@Author:	Christopher Broussard
//	@Date:		April, 2003
//
//	These are C++ implementations of some of the functions from the book
//	"Numerical Recipes in C".  If you have problems with these functions, talk to the
//	authors of the book.
////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
using namespace std;

#ifndef PI
#define PI 3.14159265358979
#endif

#define DEG2RAD 0.017453292519943	// pi/180.0
#define RAD2DEG 57.295779513082323	// 180.0/pi
#define SQRT_PI 1.772453850905516   // Square root of pi

// Stores the x, y, and z components of a particular movement.  The index value
// is used as a self contained incrementer.
typedef struct NM_MOVEMENT_DATA
{
	int index;
	vector<double> X;	// Lateral
	vector<double> Y;	// Surge
	vector<double> Z;	// Heave
} nmMovementData;

// Defines a 3D datum.  This can be anything that has 3 core components, such as a point.
typedef struct NM_3D_DATUM
{
	double x, y, z;
} nm3DDatum;

// Returns the incomplete gamma function Q(a,x) evaluated by its continued fraction
// representation as "gammcf".
void gcf(double *gammcf, double a, double x, double *gln) throw (...);

// Returns the value ln[G(xx)] for xx > 0.
double gammln(double xx);

// Calculates the incomplete gamma function P(a, x) evaluated by its series representation
// as "gamser".  Also returns ln[G(a)] as "gln".
void gser(double *gamser, double a, double x, double *gln) throw (...);

// Returns the incomplete gamma function P(a, x).
double gammp(double a, double x) throw (...);

// Computes the error function erf(x).
double erff(double x) throw (...);

// Performs a linear interpolation of a set of data.
double * linear_interp(double *data, int data_length, int interp_factor, int &interp_len);

// Returns a normally distributed deviate with zero mean and unit variance, using ran1(idum)
// as the source of uniform deviates.
double nmGasdev(long *idum);

// “Minimal” random number generator of Park and Miller with Bays-Durham shuffle and added
// safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint
// values). Call with idum a negative integer to initialize; thereafter, do not alter idum between
// successive deviates in a sequence. RNMX should approximate the largest floating value that is
// less than 1.
double ran1(long *idum);

// Rotates a set of 3d data points first about the y axis, then the z axis.  Azimuth and elevation
// are in degrees not radians.  Returns true if the rotation worked, false otherwise.  Make sure
// that the lengths of each vector are the same.  It uses the following rotation matrix to do the
// rotation.
// |cos(e)cos(a)  sin(a) sin(e)cos(a) |
// |-cos(e)sin(a) cos(a) -sin(e)sin(a)|
// |-sin(e)		  0		 cos(e)		  |
// This is is a rotation of elevation then azimuth.
bool nmRotateDataYZ(nmMovementData *data, double azimuth, double elevation);

// Rotates a 3d point about an arbitrary 3d point.  This function generates an entire trajectory (x,y,z) based on a start
// angle and end angle of rotation.  The step parameter determines the increment of the trajectory.  rotElevation and
// rotAzimuth specify the axis of rotation that goes through rotPoint.  All angles are in degrees.  If clearData is set
// to true, then data will be cleared upon entry to the function.
bool nmRotatePointAboutPoint(nm3DDatum point, nm3DDatum rotPoint, double startAngle, double endAngle,
		   					 double step, double rotElevation, double rotAzimuth, nmMovementData *data,
							 bool clearData);

// Rotates a 3d point about an arbitrary 3d point, but uses a defined vector of angles to do each bit of the
// rotation.
void nmRotatePointAboutPoint(nm3DDatum point, nm3DDatum rotPoint,		// Point to rotate and point to rotate about.
							 double rotElevation, double rotAzimuth,	// Elevation and azimuth of rotation axis.
							 vector<double> *trajectory,				// Predefined position profile of the rotation.
							 nmMovementData *data,						// Stores the translation components of the trajectory.
							 nmMovementData *rotData,					// Stores the rotation components of the trajectory.
							 bool clearData,							// Flag to clear data and rotData.
							 bool inDegrees);							// Flag to indicate if angles are passed in degrees.

// Rotates a 3d point about an arbitrary 3d point in a sinusoidal fashion.  The axis of rotation is arbitrary and is
// defined by an elevation and azimuth.
void nmSinusoidPointAboutPoint(nm3DDatum point, nm3DDatum rotPoint,  // Point to rotate and point to rotate about.
							   double amplitude,		// Amplitude of the sinusoid.
							   double frequency,		// Frequency of the sinusoid.
							   double duration,			// Duration of the sinusoid
		   					   double phase,			// Phase of the sinusoid.
							   double centerAngle,		// Movement will be offset by this angle.
							   double step,				// Defines the granularity of the sinusoid in fractions of a second.
							   double rotElevation,		// Elevation of the axis of rotation.
							   double rotAzimuth,		// Azimuth of the axis of rotation.
							   nmMovementData *data,	// Stores the translation components of the trajectory.
							   nmMovementData *rotData);// Stores the rotation components of the trajectory.

// Generates a filtered vector of Gaussian noise.  The nmMovementData passed to it will be cleared inside the function.
// Returns true if everything worked, false otherwise.
bool nmGenerateFilteredNoise(long idum,							// Seed for the Gaussian distribution function.
							 int noiseLength,					// How many data points of noise we want.
							 double cutoffFreq,					// Cutoff frequency for the filter.
							 nm3DDatum noiseMagnitude,			// Magnitude of noise measured in standard deviations.
																// This should have exactly 3 elements for X, Y, and Z.
							 int numDimensions,					// Number of dimensions in which we want to generate noise.
																// Must be in the range [1,3].
							 bool fixNoise,						// Multiply noise by a high powered Gaussian.
							 bool fixFilter,					// Multiply the filter by a high powered Gaussian.
							 nmMovementData *noiseData,			// Holds the non filtered noise.
							 nmMovementData *filteredNoiseData);// Holds the filtered noise.

// Calculates a 3D trajectory with a Gaussian velocity profile.  Returns a 1 if everything went OK, -1 otherwise.
int nmGen3DVGaussTrajectory(nmMovementData *trajectory,	// Calculated trajectory is stored in this variable.
						   double elevation,			// Elevation of the movement.
						   double azimuth,				// Azimuth of the movement.
						   double distance,				// Total distance travelled.
						   double duration,				// Time of the movement in seconds.
						   double div,					// Time division. 60 would mean 60 steps per second.
						   double sigma,				// # sigmas = duration/2.0.  Increasing this makes the gaussian steaper.
						   nm3DDatum offsets,			// Offsets to add to the trajectory.
						   bool inDegrees,				// Indicates if angles passed in are in degrees.
						   bool clearData);				// If true, trajectory is cleared first.

// Calculates a 3D trajectory with a Gaussian velocity profile.  Different than the function above in that
// its trajectory follows the path from the start point to the end point rather than using elevation
// and azimuth.
int nmGen3DVGaussTrajectory(nmMovementData *trajectory,	// Calculated trajectory is stored in this variable.
						   nm3DDatum startPoint,		// Start point of the trajectory.
						   nm3DDatum endPoint,			// End point of the trajectory.
						   double duration,				// Time of the movement in seconds.
						   double div,					// Time division. 60 would mean 60 steps per second.
						   double sigma,				// # sigmas = duration/2.0.  Increasing this makes the gaussian steaper.
						   bool clearData);				// If true, trajectory is cleared first.

// Calculates a 1D trajectory with a Gaussian velocity profile.  The trajectory starts from zero plus
// the offset.  Returns 1 if everything went OK, -1 otherwise.
int nmGen1DVGaussTrajectory(vector<double> *trajectory,	// Holds the calculated trajectory.
						   double magnitude,			// Magnitude of movement.
						   double duration,				// Duration of movement.
						   double div,					// Time division.  60 would mean 60 steps per second.
						   double sigma,				// # sigmas = duration/2.0.  Increasing this makes the gaussian steaper.
						   double offset,				// Adds an offset to the result.
						   bool clearData);				// If true, trajectory is cleared first.

// Generates a Gaussian curve A*e^(-(x-u)^2/(2s^2)) with the domain [0,width] with an interval of div in
// in the range of [0,amplitude].  Returns 1 if everything went OK, -1 otherwise.
int nmGenGaussianCurve(vector<double> *curve,			// Holds the range values of the Gaussian.
					   double amplitude,				// Amplitude of the peak range value.
					   double width,					// Max domain value.
					   double div,						// Delta x
					   double sigma,					// # sigmas = width/2.0.  Increasing this makes the gaussian steaper.
					   int power,						// Power of the Gaussian.  Must be a multiple of 2 and non-zero.
					   bool clearData);					// If true, curve is cleared first.

// Uses the trapezoid method to integrate a function.  lowIndex must be >= 0 and highIndex must be < length of data.
// highIndex must also me >= lowIndex + 1.  data must contain at least 2 points.  Returns 0 if everything was OK, -1 if there was an error.  The integral
// result is stored in isum.
int nmTrapIntegrate(vector<double> *data,				// Holds all the data points of the function.
					vector<double> *integral,			// Holds all integrated data points.
					double &isum,						// The result of the integration.
					int lowIndex,						// Index from which to start integration.
					int highIndex,						// Index to end integration.
					double width);						// Distance between 2 points.

// Converts spherical coordinates into cartesian values.  The inDegrees flags whether the values passed
// to the function are in degrees or radians.
nm3DDatum nmSpherical2Cartesian(double elevation, double azimuth, double radius, bool inDegrees);

// Calculates derivative of a general curve.
int nmGenDerivativeCurve(vector<double> *curve,			// Holds output derived data.
		 			     vector<double> *y,				// Y value input.
						 double dx,						// Delta x
						 bool clearData);				// If true, curve is cleared first.


// Clears a nmMovementData structure.
void nmClearMovementData(nmMovementData *data);