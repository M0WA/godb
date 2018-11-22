#include "table.h"

#include "column.h"

size_t get_row_size(const struct _DBColumnDef *const defs,size_t ncols) {
	size_t s = 0;
	if(!defs||!ncols) {
		return s; }
	for(size_t i = 0; i < ncols; i++) {
		s += get_column_bufsize(&(defs[s])); }
	return s;
}
