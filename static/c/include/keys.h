#pragma once

struct _DBColumnDef;

typedef struct _UniqKey {
	const struct _DBColumnDef **cols;
	size_t ncols;
} UniqKey;
