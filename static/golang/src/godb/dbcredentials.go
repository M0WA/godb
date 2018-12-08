package godb

// #include <db.h>
// #include <dbhandle.h>
import "C"

type DBCredentials interface {
/*
	GetHost()(string)
	GetPort()(int)
	GetUser()(string)
	GetPass()(string)
	GetName()(string)
*/
	
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
/*
func (c *DBCredentialsImpl)GetHost()(string) {
	return C.GoString(c.native.Host)
}

func (c *DBCredentialsImpl)GetPort()(int) {
	return c.native.Port
}

func (c *DBCredentialsImpl)GetUser()(string) {
	return c.native.User
}

func (c *DBCredentialsImpl)GetPass()(string) {
	return c.native.Pass
}

func (c *DBCredentialsImpl)GetName()(string) {
	return c.native.Name
}
*/