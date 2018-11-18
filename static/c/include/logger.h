#pragma once

#include <stdio.h>

typedef enum _LogLevel {
	LOGLVL_DEBUG = 0,
	LOGLVL_INFO,
	LOGLVL_WARN,
	LOGLVL_ERROR,

	MAX_LOGLEVEL
} LogLevel;

void SetLogFile(FILE* f);
void SetLogLevel(LogLevel lvl);

#define LOG_DEBUG(msg)       _log (LOGLVL_DEBUG,msg);
#define LOG_INFO(msg)        _log (LOGLVL_INFO,msg);
#define LOG_WARN(msg)        _log (LOGLVL_WARN,msg);
#define LOG_ERROR(msg)       _log (LOGLVL_ERROR,msg);
#define LOGF_DEBUG(fmt, ...) _logf(LOGLVL_DEBUG,fmt,__VA_ARGS__);
#define LOGF_INFO(fmt, ...)  _logf(LOGLVL_INFO,fmt,__VA_ARGS__);
#define LOGF_WARN(fmt, ...)  _logf(LOGLVL_WARN,fmt,__VA_ARGS__);
#define LOGF_ERROR(fmt, ...) _logf(LOGLVL_ERROR,fmt,__VA_ARGS__);

#define LOGF_FATAL(exitcode,fmt, ...) \
	LOGF_ERROR(fmt,#__VA_ARGS__); \
	exit(exitcode);

#define LOG_FATAL(exitcode,msg) \
	LOG_ERROR(msg); \
	exit(exitcode);

void _logf(LogLevel lvl,const char* __restrict fmt,...);
void _log(LogLevel lvl,const char* __restrict msg);
