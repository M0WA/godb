package godb

import (
	"fmt"
)

/*
#include <logger.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

void GoLogFunc(LogLevel lvl,char *file, uint64_t *line, char *function, char* message);

static void logFunc(LogLevel lvl,const char *filename, const size_t line,const char *function,const char *fmt,...) {
	#define MAX_LOG_MESSAGE 2048
	char message[MAX_LOG_MESSAGE] = {0};	
	//const char* ll = loglevelToString(lvl);

	va_list arglist;
	va_start( arglist, fmt );
	vsprintf( message, fmt, arglist );
	va_end( arglist );

	sprintf(message,"\n");
	
	uint64_t lineno = line;
	GoLogFunc(lvl,(char*)filename, &lineno, (char*)function, (char*)message);
}

static void set_golog() {
	set_logfunc(logFunc);
}
*/
import "C"

//export GoLogFunc
func GoLogFunc(lvl C.LogLevel, file *C.char, line *C.uint64_t, function *C.char, message *C.char) {
	fmt.Println(message)
}

func SetLogFunc() {
	C.set_golog()
}

func SetLogDebug() {
	C.set_loglevel(C.LOGLVL_DEBUG)
}
