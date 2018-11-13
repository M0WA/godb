#pragma once

#ifndef _DISABLE_POSTGRES

struct _DBHandle;

int postgres_init_dbh(struct _DBHandle*);

#endif
