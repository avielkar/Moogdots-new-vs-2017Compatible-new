#include "MatSoundStimReader.h"


MatSoundStimReader::MatSoundStimReader(const char* dirPath)
{
	_dirPath = dirPath;

	for (const auto & entry : fs::directory_iterator(_dirPath))
	{
		string angleString = StringFromFileName(entry.path().filename().string());

		if (angleString != "none")
		{
			_convertAngleDictionary[stod(angleString)] = angleString;

		}
	}
}

MatSoundStimReader::~MatSoundStimReader()
{
}

void MatSoundStimReader::ReadStruct(double angle , char * structName, double * &leftChannelResult, double * &rightChannelResult , unsigned int& size , bool inverse)
{
	//MAT-file pointer
	MATFile *mfPtr;

	//mxArray pointer
	mxArray *aPtr;

	string fullPath;
	if (!inverse)
	{
		fullPath = _dirPath + string("/angle") + _convertAngleDictionary[angle] + string(".mat");

	}
	else
	{
		fullPath = _dirPath + string("/angle_inverse") + _convertAngleDictionary[angle] + string(".mat");
	}

	//string fullPath = "angles/angle90.mat";

	mfPtr = matOpen((fullPath).c_str(), "r");
	if (mfPtr == NULL)
	{
		//todo:throw exception
	}

	//get the pointer to the struct.
	aPtr = matGetVariable(mfPtr, structName);

	//if the number of fields is not 2 , throw exception
	if (mxGetNumberOfFields(aPtr))
	{
		//todo:throw exception.
	}

	//get the pointer to the fields in the struct.
	mxArray* leftChannelFieldData = mxGetField(aPtr, 0, "left");
	mxArray* rightChannelFieldData = mxGetField(aPtr, 0, "right");

	//if field sizes are different throw an  exception.
	if (mxGetElementSize(leftChannelFieldData) != mxGetElementSize(rightChannelFieldData))
	{
		//todo:throw exception.
	}

	//return the size of the fields.
	size = (unsigned int)mxGetNumberOfElements(leftChannelFieldData);

	//convert the mx-arrays to doubles.
	double *leftChannel = mxGetPr(leftChannelFieldData);
	double * rightChannel1 = mxGetPr(rightChannelFieldData); 

	leftChannelResult = new double[size];
	rightChannelResult = new double[size];

	for (int i = 0; i < size; i++)
	{
		leftChannelResult[i] = leftChannel[i];
		rightChannelResult[i] = rightChannel1[i];
	}

	mxDestroyArray(leftChannelFieldData);
	mxDestroyArray(rightChannelFieldData);

	matClose(mfPtr);
}

string MatSoundStimReader::StringFromFileName(string fileName)
{
	int fileNameLastIndex = fileName.find_last_of(".", fileName.size());

	int indexInverseWord = fileName.substr(0, fileNameLastIndex).find_first_of("_" , 0);

	if (indexInverseWord <= 0)
	{
		return fileName.substr(0, fileNameLastIndex).substr(string("angle").size(), fileNameLastIndex);
	}

	return "none";
}

