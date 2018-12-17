package godb

/*
#include <dbhandle.h>
#include <string.h>
#include <stdlib.h>

static const char *dbcreds_get_host(const DBCredentials *creds) {
	return creds->Host;
}

static unsigned short dbcreds_get_port(const DBCredentials *creds) {
	return creds->Port;
}

static const char *dbcreds_get_name(const DBCredentials *creds) {
	return creds->Name;
}

static const char *dbcreds_get_user(const DBCredentials *creds) {
	return creds->User;
}

static const char *dbcreds_get_pass(const DBCredentials *creds) {
	return creds->Pass;
}

static void dbcreds_set_host(DBCredentials *creds, const char *host) {
	strncpy(creds->Host,host,MAX_DB_HOST);
	creds->Host[MAX_DB_HOST-1] = 0;
}

static void dbcreds_set_port(DBCredentials *creds, unsigned short port) {
	creds->Port = port;
}

static void dbcreds_set_name(DBCredentials *creds, const char *name) {
	strncpy(creds->Name,name,MAX_DB_NAME);
	creds->Name[MAX_DB_NAME-1] = 0;
}

static void dbcreds_set_user(DBCredentials *creds, const char *user) {
	strncpy(creds->User,user,MAX_DB_USER);
	creds->User[MAX_DB_USER-1] = 0;
}

static void dbcreds_set_pass(DBCredentials *creds, const char *pass) {
	strncpy(creds->Pass,pass,MAX_DB_PASS);
	creds->Pass[MAX_DB_PASS-1] = 0;
}

static void dbcreds_free_string(char *str) {
	if(str) {
		free(str);
	}
}
*/
import "C"

import (
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
	C.dbcreds_free_string(h)
}

func (c *DBCredentialsImpl)SetPort(port uint16) {
	c.native.Port = C.ushort(port)
}

func (c *DBCredentialsImpl)SetUser(user string) {
	u := C.CString(user)
	C.dbcreds_set_user(&c.native,u)
	C.dbcreds_free_string(u)
}

func (c *DBCredentialsImpl)SetPass(pass string) {
	p := C.CString(pass)
	C.dbcreds_set_pass(&c.native,p)
	C.dbcreds_free_string(p)
}

func (c *DBCredentialsImpl)SetName(name string) {
	n := C.CString(name)
	C.dbcreds_set_name(&c.native,n)
	C.dbcreds_free_string(n)
}
