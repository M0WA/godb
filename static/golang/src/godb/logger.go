package godb

// #include <logger.h>
import "C"

import (

)

func SetLogDebug() {
	C.set_loglevel(C.LOGLVL_DEBUG)
}