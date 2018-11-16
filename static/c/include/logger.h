#pragma once

#include <stdio.h>

typedef enum _LogLevel {
	LOG_DEBUG = 0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,

	MAX_LOGLEVEL
} LogLevel;

#define LOGF_FATAL(exitcode,fmt, ...) \
	Logf(LOG_ERROR,fmt,__VA_ARGS__); \
	exit(exitcode);

#define LOG_FATAL(exitcode,msg) \
	Log(LOG_ERROR,msg); \
	exit(exitcode);

void SetLogFile(FILE* f);
void SetLogLevel(LogLevel lvl);

void Logf(LogLevel lvl,const char* __restrict fmt,...);
void Log(LogLevel lvl,const char* __restrict msg);