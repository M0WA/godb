package godb

// #include <db.h>
// #include <dbhandle.h>
// #include <golanghelper.h>
// #include <stdlib.h>
import "C"

import (
	"unsafe"
)

type DBCredentials interface {
	GetHost()string
	GetPort()uint16
	GetUser()string
	GetPass()string
	GetName()string
	
	SetHost(string)
	SetPort(uint16)
	SetUser(string)
	SetPass(string)
	SetName(string)
	
	ToNative()*C.DBCredentials
}

type DBCredentialsImpl struct {
	native C.DBCredentials
}

func NewDBCredentials()DBCredentials {
	c := new(DBCredentialsImpl)
	return c
}

func (c *DBCredentialsImpl)ToNative()*C.DBCredentials {
	return &c.native
}
func (c *DBCredentialsImpl)GetHost()string {
	return C.GoString(C.dbcreds_get_host(&c.native))
}

func (c *DBCredentialsImpl)GetPort()uint16 {
	return uint16(c.native.Port)
}

func (c *DBCredentialsImpl)GetUser()string {
	return C.GoString(C.dbcreds_get_user(&c.native))
}

func (c *DBCredentialsImpl)GetPass()string {
	return C.GoString(C.dbcreds_get_pass(&c.native))
}

func (c *DBCredentialsImpl)GetName()string {
	return C.GoString(C.dbcreds_get_name(&c.native))
}

func (c *DBCredentialsImpl)SetHost(host string) {
	h := C.CString(host)
	C.dbcreds_set_name(&c.native,h)
	C.free(unsafe.Pointer(h))
}

func (c *DBCredentialsImpl)SetPort(port uint16) {
	c.native.Port = C.ushort(port)
}

func (c *DBCredentialsImpl)SetUser(user string) {
	u := C.CString(user)
	C.dbcreds_set_user(&c.native,u)
	C.free(unsafe.Pointer(u))
}

func (c *DBCredentialsImpl)SetPass(pass string) {
	p := C.CString(pass)
	C.dbcreds_set_pass(&c.native,p)
	C.free(unsafe.Pointer(p))
}

func (c *DBCredentialsImpl)SetName(name string) {
	n := C.CString(name)
	C.dbcreds_set_name(&c.native,n)
	C.free(unsafe.Pointer(n))
}
