#pragma once

typedef struct _UniqKey {
	size_t ncols;
	const char *const *const cols;
} UniqKey;
