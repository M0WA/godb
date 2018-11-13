#include "logger.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _Logger {
	int fd;
	LogLevel lvl;
} Logger;

Logger* logger = 0;

char debugLogLvl[] = "DEBUG";
char infoLogLvl[]  = "INFO";
char warnLogLvl[]  = "WARN";
char errorLogLvl[] = "ERROR";
char invalidLogLvl[] = "N/A";

static const char* loglevelToString(LogLevel lvl) {
	switch(lvl) {
	case LOG_DEBUG:
		return debugLogLvl;
	case LOG_INFO:
		return debugLogLvl;
	case LOG_WARN:
		return warnLogLvl;
	case LOG_ERROR:
		return errorLogLvl;
	default:
		return invalidLogLvl;
	}
}

static int initLogger() {
	if( !logger ) {
		logger = malloc(sizeof(Logger));
		if( !logger ) {
			return 1;
		}
		logger->fd = STDOUT_FILENO;
		logger->lvl = LOG_INFO;
	}
	return 0;
}

void SetLogFile(int fd) {
	if(initLogger()) {
		return;	}
	logger->fd = fd;
}

void SetLogLevel(LogLevel lvl) {
	if(initLogger()) {
		return;	}
	logger->lvl = lvl;
}

void Logf(LogLevel lvl,const char* __restrict fmt,...) {
	if(initLogger()) {
		return;	}
	if(lvl < logger->lvl) {
		return; }
	const char* ll = loglevelToString(lvl);
}

void Log(LogLevel lvl,const char* __restrict msg) {
	Logf(lvl,msg);
}
