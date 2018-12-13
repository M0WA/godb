package godb

/*
#include <logger.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

void GoLogFunc(LogLevel lvl,const char* file, unsigned long long line, const char* function, const char* message);

void logFunc(LogLevel lvl,const char *filename, const size_t line,const char *function,const char* __restrict fmt,...) {
	#define MAX_LOG_MESSAGE 2048
	char message[MAX_LOG_MESSAGE] = {0};	
	const char* ll = loglevelToString(lvl);

    time_t now = time(0);
    struct tm tmNow;
    gmtime_r(&now,&tmNow);

	char pszTimeString[80] = {0};
	strftime(pszTimeString, 80, "%Y-%m-%d %H:%M:%S", &tmNow);
	sprintf(message,"[%s][%s] ",pszTimeString,ll);

	va_list arglist;
	va_start( arglist, fmt );
	vsprintf( message, fmt, arglist );
	va_end( arglist );

	if(lvl >= LOGLVL_ERROR) {
		sprintf(message," [in %s() at %s:%zu]",function,filename,line); }

	sprintf(message,"\n");
	
	GoLogFunc(lvl,filename, line, function, message);
}

void set_golog() {
	set_logfunc(logFunc);
}
*/
import "C"

import (
	"fmt"
)

func GoLogFunc(lvl C.LogLevel,file *C.char, line uint64, function *C.char, message *C.char) {
	fmt.Print(message)
}

func SetLogFunc() {
	C.set_golog()
}

func SetLogDebug() {
	C.set_loglevel(C.LOGLVL_DEBUG)
}
