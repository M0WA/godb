package godb

// #include <db.h>
// #include <dbhandle.h>
// #include <golanghelper.h>
import "C"

type DBCredentials interface {
	GetHost()(string)
	GetPort()(uint16)
	GetUser()(string)
	GetPass()(string)
	GetName()(string)
	
	SetHost(string)
	SetPort(uint16)
	SetUser(string)
	SetPass(string)
	SetName(string)
	
	ToNative()(*C.DBCredentials)
}

type DBCredentialsImpl struct {
	native C.DBCredentials
}

func NewDBCredentials()(DBCredentials) {
	c := new(DBCredentialsImpl)
	return c
}

func (c *DBCredentialsImpl)ToNative()(*C.DBCredentials) {
	return &c.native
}
func (c *DBCredentialsImpl)GetHost()(string) {
	return C.GoString(C.dbcreds_get_host(&c.native))
}

func (c *DBCredentialsImpl)GetPort()(uint16) {
	return uint16(c.native.Port)
}

func (c *DBCredentialsImpl)GetUser()(string) {
	return C.GoString(C.dbcreds_get_user(&c.native))
}

func (c *DBCredentialsImpl)GetPass()(string) {
	return C.GoString(C.dbcreds_get_pass(&c.native))
}

func (c *DBCredentialsImpl)GetName()(string) {
	return C.GoString(C.dbcreds_get_name(&c.native))
}

func (c *DBCredentialsImpl)SetHost(host string) {
	C.dbcreds_set_name(&c.native,C.CString(host))
}

func (c *DBCredentialsImpl)SetPort(port uint16) {
	c.native.Port = C.ushort(port)
}

func (c *DBCredentialsImpl)SetUser(user string) {
	C.dbcreds_set_user(&c.native,C.CString(user))
}

func (c *DBCredentialsImpl)SetPass(pass string) {
	C.dbcreds_set_pass(&c.native,C.CString(pass))
}

func (c *DBCredentialsImpl)SetName(name string) {
	C.dbcreds_set_name(&c.native,C.CString(name))
}
