#pragma once

struct _DBColumnDef;

typedef struct _UniqKey {
	const struct _DBColumnDef **cols;
	size_t ncols;
} UniqKey;

typedef struct _IndexKey {
	const struct _DBColumnDef *col;
} IndexKey;

typedef struct _ForeignKey {
	const struct _DBColumnDef *col;
	const struct _DBColumnDef *ref;
} ForeignKey;
