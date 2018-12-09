#include "golanghelper.h"

#include "dblimits.h"
#include "dbhandle.h"

#include <string.h>

const char *dbcreds_get_host(const DBCredentials *creds) {
	return creds->Host;
}

unsigned short dbcreds_get_port(const DBCredentials *creds) {
	return creds->Port;
}

const char *dbcreds_get_name(const DBCredentials *creds) {
	return creds->Name;
}

const char *dbcreds_get_user(const DBCredentials *creds) {
	return creds->User;
}

const char *dbcreds_get_pass(const DBCredentials *creds) {
	return creds->Pass;
}

void dbcreds_set_host(DBCredentials *creds, const char *host) {
	strncpy(creds->Host,host,MAX_DB_HOST);
	creds->Host[MAX_DB_HOST-1] = 0;
}

void dbcreds_set_port(DBCredentials *creds, unsigned short port) {
	creds->Port = port;
}

void dbcreds_set_name(DBCredentials *creds, const char *name) {
	strncpy(creds->Name,name,MAX_DB_NAME);
	creds->Name[MAX_DB_NAME-1] = 0;
}

void dbcreds_set_user(DBCredentials *creds, const char *user) {
	strncpy(creds->User,user,MAX_DB_USER);
	creds->User[MAX_DB_USER-1] = 0;
}

void dbcreds_set_pass(DBCredentials *creds, const char *pass) {
	strncpy(creds->Pass,pass,MAX_DB_PASS);
	creds->Pass[MAX_DB_PASS-1] = 0;
}

