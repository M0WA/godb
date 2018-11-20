#pragma once

#ifndef _DISABLE_MYSQL

struct _DBHandle;

int mysql_init_dbh(struct _DBHandle*);
int mysql_init_dblib();
int mysql_exit_dblib();

#endif
