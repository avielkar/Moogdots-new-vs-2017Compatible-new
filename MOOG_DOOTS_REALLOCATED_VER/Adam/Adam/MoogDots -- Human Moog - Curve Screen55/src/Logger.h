#pragma once

#ifndef LOGGER
#define LOGGER

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <string.h>

using namespace std;

#define CURRENT_TIME() (Logger::GetFormattedDate())

#define WRITE_LOG(file , string, param) file << CURRENT_TIME() <<	 \
	"@" << __FILE__ << " " << \
	"@" << __FUNCTION__ << " " << \
	string <<	\
		"\n";\
	file.flush();

#define WRITE_LOG_PARAMS(file , string, params , paramsLength) file << CURRENT_TIME() <<	 \
	"@" << __FILE__ << " " << \
	"@" << __FUNCTION__ << " " << \
	string <<	\
	"	PARAMS : ";\
	for(int i = 0; i < paramsLength ; i++)	\
		file << #params << "[" << i << "]=" << params[i] << " ";\
	file << "\n";\
	file.flush();

#define WRITE_LOG_PARAM(file , string, param) file << CURRENT_TIME() <<	 \
	"@" << __FILE__ << " " << \
	"@" << __FUNCTION__ << " " << \
	string <<	\
	"	PARAM : " << \
		#param  << "=" << param << \
		"\n";\
	file.flush();

class Logger
{
public:
	Logger(char* path , char *fileName);
	~Logger();
	static char* GetFormattedDate();
	void Close();
	void Write(char* string);
	void WriteParam(char* string, int value);
	void WriteParam(char* string, double value);
	void WriteParam(char* string, float value);
	void WriteParam(char* string, char value);
	void WriteParams(char* string, int* value, int length);
	void WriteParams(char* string, double* value, int length);
	void WriteParams(char* string, float* value, int length);
	void WriteParams(char* string, char* value, int length);

public:
	ofstream m_logger;
};

#endif