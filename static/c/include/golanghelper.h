#pragma once

typedef struct _DBCredentials DBCredentials;

const char *dbcreds_get_host(const DBCredentials *creds);
unsigned short dbcreds_get_port(const DBCredentials *creds);
const char *dbcreds_get_name(const DBCredentials *creds);
const char *dbcreds_get_user(const DBCredentials *creds);
const char *dbcreds_get_pass(const DBCredentials *creds);

void dbcreds_set_host(DBCredentials *creds, const char *host);
void dbcreds_set_port(DBCredentials *creds, unsigned short port);
void dbcreds_set_name(DBCredentials *creds, const char *name);
void dbcreds_set_user(DBCredentials *creds, const char *user);
void dbcreds_set_pass(DBCredentials *creds, const char *pass);
