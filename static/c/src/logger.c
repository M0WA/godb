#include "logger.h"

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

typedef struct _Logger {
	FILE* fp;
	LogLevel lvl;
} Logger;

Logger* logger = 0;
int closed_logger = 0;

char debugLogLvl[]   = "DEBUG";
char infoLogLvl[]    = "INFO ";
char warnLogLvl[]    = "WARN ";
char errorLogLvl[]   = "ERROR";
char invalidLogLvl[] = "N/A  ";

static const char* loglevelToString(LogLevel lvl) {
	switch(lvl) {
	case LOGLVL_DEBUG:
		return debugLogLvl;
	case LOGLVL_INFO:
		return infoLogLvl;
	case LOGLVL_WARN:
		return warnLogLvl;
	case LOGLVL_ERROR:
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
		logger->fp = stderr;
		if(!logger->fp) {
			if (!closed_logger) {
				fprintf(stderr,"could not initialize logger");
				closed_logger++;
			}
			free(logger);
			logger = 0;
			return 1;
		}
		logger->lvl = LOGLVL_INFO;
	}
	return 0;
}

static void printPrefix(LogLevel lvl) {
	const char* ll = loglevelToString(lvl);

    time_t now = time(0);
    struct tm tmNow;
    gmtime_r(&now,&tmNow);

	char pszTimeString[80] = {0};
	strftime(pszTimeString, 80, "%Y-%m-%d %H:%M:%S", &tmNow);
	fprintf(logger->fp,"[%s][%s] ",pszTimeString,ll);
}

void set_logfile(FILE *f) {
	if(initLogger()) {
		return;	}
	if( f ) {
		logger->fp = f; }
}

void set_loglevel(LogLevel lvl) {
	if(initLogger()) {
		return;	}
	logger->lvl = lvl;
}

void _logf(LogLevel lvl,const char *filename, const size_t line,const char *function,const char* __restrict fmt,...) {
	if(initLogger()) {
		return;	}
	if(lvl < logger->lvl) {
		return; }

	printPrefix(lvl);

	va_list arglist;
	va_start( arglist, fmt );
	vfprintf( logger->fp, fmt, arglist );
	va_end( arglist );

	if(lvl >= LOGLVL_ERROR) {
		fprintf(logger->fp," [in %s() at %s:%zu]",function,filename,line); }

	fprintf(logger->fp,"\n");
	fflush(logger->fp);
}

void _log(LogLevel lvl,const char *filename, const size_t line,const char *function,const char* __restrict msg) {
	_logf(lvl,filename,line,function,msg);
}

void logger_end() {
	if(logger) {
		free(logger);
		logger =0;
	}
}
