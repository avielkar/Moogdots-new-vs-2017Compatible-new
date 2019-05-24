#pragma once
#ifndef READ_STRUCT

#include <map>
#include <string>
#include <stdlib.h>
#include "mat.h"
#include <filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;
using namespace std;

class MatSoundStimReader
{
public:
	MatSoundStimReader(const char* dirPath);
	~MatSoundStimReader();

	void ReadStruct(double angle , char* structName , double* &leftChannelResult , double* &rightChannelResult , unsigned int& size , bool inverse);

private:
	const char* _dirPath;
	map<double, string> _convertAngleDictionary;
	string StringFromFileName(string fileName);
};

#define READ_STRUCT
#endif