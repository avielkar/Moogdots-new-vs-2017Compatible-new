#include "StdAfx.h"
#include "MatlabRDX.h"

CMatlabRDX::CMatlabRDX(int boardNum)
{
	m_boardNum = boardNum;
}

int CMatlabRDX::ReadString(double timeOut, int bufferSize, string *data , int inControlPort , int dataPort , int outControlPort)
{
	int errorCode;
	unsigned short recVal, dataVal;
	clock_t start, finish;

	// Clear the data string.
	data->clear();

	// Set the capacity of the string.  Ideally, this will be used to make
	// sure that we don't have a bunch of size reallocations if the string
	// is long.
	data->reserve(bufferSize);

	// Check to see if anything is available for us to read.
	errorCode = cbDIn(m_boardNum, inControlPort, &recVal);
	if (recVal) {
		// Loop through and read all data until we get a newline.
		do {
			// Make sure that the server is ready.
			start = clock();
			do {
				if (cbDIn(m_boardNum, inControlPort, &recVal) != 0) {
					//mexErrMsgTxt("*** Error reading server bit 1");
					return -1;
				}

				if (timeOut > 0) {
					// Make sure that we don't get stuck trying to read the
					// server bit.
					finish = clock();
					if (static_cast<double>(finish - start)/static_cast<double>(CLOCKS_PER_SEC) > 1.0) {
						//mexErrMsgTxt("*** Server bit read timeout 1");
						return -2;
					}
				}
			} while (!recVal);

			// Read the current character
			if (cbDIn(m_boardNum, dataPort, &dataVal) != 0) {
				//mexErrMsgTxt("*** Could not read the character");
				return -3;
			}

			// Let the server know we've read in the character.
			if (cbDOut(m_boardNum, outControlPort, static_cast<unsigned short>(1)) != 0) {
				//mexErrMsgTxt("*** Could not set read bit 1");
				return -4;
			}

			// Wait for confirmation that the server knows we've read
			// the bit.
			start = clock();
			do {
				if (cbDIn(m_boardNum, inControlPort, &recVal) != 0) {
					//mexErrMsgTxt("*** Error reading server bit 2");
					return -5;
				}

				if (timeOut > 0) {
					// Make sure that we don't get stuck trying to read the
					// server bit.
					finish = clock();
					if (static_cast<double>(finish - start)/static_cast<double>(CLOCKS_PER_SEC) > 1.0) {
						//mexErrMsgTxt("*** Server bit read timeout 2");
						return -6;
					}
				}
			} while (recVal & 1);

			// Let the server know that we're ready for another read.
			if (cbDOut(m_boardNum, outControlPort, (unsigned short)0) != 0) {
				//mexErrMsgTxt("*** Could not set read bit 2");
				return -7;
			}

			// If it's a newline character, don't bother putting it
			// into the buffer.
			if (static_cast<char>(dataVal) != '\n') {
				*data += static_cast<char>(dataVal);
			}
		} while (static_cast<char>(dataVal) != '\n');
	}

	// Return how many characters we've read.
	return static_cast<int>(data->size());
}


int CMatlabRDX::InitClient(int inControlPort, int dataPort, int outControlPort)
{
	int errorCode = 0;

	// Configure the port that we'll receive strings on.
	errorCode = cbDConfigPort(m_boardNum, dataPort, DIGITALIN);
	if (errorCode != 0) {
		return errorCode;
	}

	// Configure the receiver complete bit port.
	errorCode = cbDConfigPort(m_boardNum, outControlPort, DIGITALOUT);
	if (errorCode != 0) {
		return errorCode;
	}
	// Zero the port.
	cbDOut(m_boardNum, outControlPort, 0);

	// Configure the server send bit port.
	errorCode = cbDConfigPort(m_boardNum, inControlPort, DIGITALIN);
	if (errorCode != 0) {
		return errorCode;
	}

	return errorCode;
}