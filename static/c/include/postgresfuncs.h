#pragma once

#ifndef _DISABLE_POSTGRES

struct _DBHandle;

int postgres_init_dbh(struct _DBHandle*);
int postgres_init_dblib();
int postgres_exit_dblib();

#endif
