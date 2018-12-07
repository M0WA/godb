package godb

import (

)

type DBCredentials interface {
	ToNative()(*C.DBCredentials)
}

type DBCredentialsImpl struct {
	Host string
	Port int
	Name string
	User string
	Pass string
}

func (c *DBCredentialsImpl)ToNative()(*C.DBCredentials) {
	return nil
}