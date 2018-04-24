#include "Logger.h";

char* GetFormattedDate()
{
	time_t rawTime;
	struct tm* timeInfo;

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	static char _retval[23];
	strftime(_retval, sizeof(_retval), "%H %M %S - %d %m %Y", timeInfo);

	return _retval;
}

Logger::Logger(char*path  , char* fileName)
{
	char* time = GetFormattedDate();

	string str;
	str.append(path);
	str.append(fileName);
	str.append(time);
	//str.append(".txt");

	const char* fullPath = const_cast<char*>(str.c_str());

	m_logger.open(fullPath);
}

char* Logger::GetFormattedDate()
{
	time_t rawTime;
	struct tm* timeInfo;

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	static char _retval[23];
	strftime(_retval, sizeof(_retval), "%H %M %S - %d %m %Y", timeInfo);

	return _retval;
}

void Logger::Close()
{
	m_logger.close();
}

void Logger::Write(char* string)
{
	WRITE_LOG(m_logger, string);
}

void Logger::WriteParam(char* string, int value)
{
	WRITE_LOG_PARAM(m_logger, string, value);
}

void Logger::WriteParam(char* string, double value)
{
	WRITE_LOG_PARAM(m_logger, string, value);
}

void Logger::WriteParam(char* string, float value)
{
	WRITE_LOG_PARAM(m_logger, string, value);
}

void Logger::WriteParam(char* string, char value)
{
	WRITE_LOG_PARAM(m_logger, string, value);
}

void Logger::WriteParams(char* string, int* value, int length)
{
	WRITE_LOG_PARAMS(m_logger, string, value, length);
}

void Logger::WriteParams(char* string, double* value, int length)
{
	WRITE_LOG_PARAMS(m_logger, string, value, length);
}

void Logger::WriteParams(char* string, float* value, int length)
{
	WRITE_LOG_PARAMS(m_logger, string, value, length);
}

void Logger::WriteParams(char* string, char* value, int length)
{
	WRITE_LOG_PARAMS(m_logger, string, value, length);
}

Logger::~Logger()
{
}