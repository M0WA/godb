#pragma once

#include <stdio.h>

typedef enum _LogLevel {
	LOG_DEBUG = 0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,

	MAX_LOGLEVEL
} LogLevel;

void SetLogFile(FILE* f);
void SetLogLevel(LogLevel lvl);

void Logf(LogLevel lvl,const char* __restrict fmt,...);
void Log(LogLevel lvl,const char* __restrict msg);
