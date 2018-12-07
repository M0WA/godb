#include "join.h"

#include "dblimits.h"
#include "stringbuf.h"
#include "logger.h"
#include "table.h"
#include "column.h"

#include <string.h>

int join_clause_string(const struct _JoinClause *join,struct _StringBuf *buf) {
	const char fmt[] = "%s JOIN %s ON %s.%s = %s.%s";
	char tmp[JOIN_BUF_SIZE] = {0};
	const char *left = "LEFT", *right = "RIGHT", *inner = "INNER", *outer = "OUTER";
	const char *typestr = 0;

	switch(join->type) {
	case DBJOIN_TYPE_LEFT:
		typestr = left;
		break;
	case DBJOIN_TYPE_RIGHT:
		typestr = right;
		break;
	case DBJOIN_TYPE_INNER:
		typestr = inner;
		break;
	case DBJOIN_TYPE_OUTER:
		typestr = outer;
		break;
	default:
		return 1;
	}

	if( snprintf(tmp,JOIN_BUF_SIZE,fmt,typestr,join->left->name,join->left->name,join->leftcol->name,join->right->name,join->rightcol->name) <= (JOIN_BUF_SIZE-1) ) {
		LOG_WARN("join clause too long");
		return 1; }

	if( stringbuf_append(buf,tmp) ) {
		return 1; }

	return 0;
}
