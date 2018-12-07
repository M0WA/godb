#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef enum _LogLevel {
	LOGLVL_DEBUG = 0,
	LOGLVL_INFO,
	LOGLVL_WARN,
	LOGLVL_ERROR,

	MAX_LOGLEVEL
} LogLevel;

void logger_end();
void set_logfile(FILE *f);
void set_loglevel(LogLevel lvl);

#define LOG_DEBUG(msg)       _log (LOGLVL_DEBUG,__FILE__,__LINE__,__func__,msg);
#define LOG_INFO(msg)        _log (LOGLVL_INFO,__FILE__,__LINE__,__func__,msg);
#define LOG_WARN(msg)        _log (LOGLVL_WARN,__FILE__,__LINE__,__func__,msg);
#define LOG_ERROR(msg)       _log (LOGLVL_ERROR,__FILE__,__LINE__,__func__,msg);
#define LOGF_DEBUG(fmt, ...) _logf(LOGLVL_DEBUG,__FILE__,__LINE__,__func__,fmt,__VA_ARGS__);
#define LOGF_INFO(fmt, ...)  _logf(LOGLVL_INFO,__FILE__,__LINE__,__func__,fmt,__VA_ARGS__);
#define LOGF_WARN(fmt, ...)  _logf(LOGLVL_WARN,__FILE__,__LINE__,__func__,fmt,__VA_ARGS__);
#define LOGF_ERROR(fmt, ...) _logf(LOGLVL_ERROR,__FILE__,__LINE__,__func__,fmt,__VA_ARGS__);

#define LOGF_FATAL(exitcode,fmt, ...) \
	LOGF_ERROR(fmt,#__VA_ARGS__); \
	exit(exitcode);

#define LOG_FATAL(exitcode,msg) \
	LOG_ERROR(msg); \
	exit(exitcode);

void _logf(LogLevel lvl,const char *filename, const size_t line,const char *function,const char* __restrict fmt,...);
void _log(LogLevel lvl,const char *filename, const size_t line,const char *function,const char* __restrict msg);
